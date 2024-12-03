#include "mbed.h"

// Thread masterThread;
// Thread slaveThread;
// // Create a serial object
// static BufferedSerial pc(PC_12, PD_2);
// // static BufferedSerial pc(PE_8, PE_7);
// static BufferedSerial pcmaster(PD_5, PD_6);
#define NO_SLAVE 2
static UnbufferedSerial debuger(USBTX, USBRX,38400);

#define TYPE_A 2
#define TYPE_B 1

// Create UnbufferedSerial objects for two serial interfaces
UnbufferedSerial slave1(PC_12, PD_2, 38400); // USB Serial
UnbufferedSerial slave2(PE_8, PE_7,38400);
UnbufferedSerial master(PD_5, PD_6, 38400);  // UART1 Serial (example pins)

// Buffers for serial passthrough
#define BUFFER_SIZE 1
char rx_buffer[BUFFER_SIZE];
char tx_buffer[BUFFER_SIZE];
static char cmd_keys[255];
const char * pc_str = "pc: ";
const char * uart1_str = "uart1: ";
const char * newline_str = "\n";
char temp_str[5];
int current_slave = 1;

void init_map() {
    cmd_keys[(uint8_t)0xA1] = TYPE_A;
    cmd_keys[(uint8_t)0xA2] = TYPE_A;
    cmd_keys[(uint8_t)0xA3] = TYPE_A;
    cmd_keys[(uint8_t)0xA4] = TYPE_A;
    cmd_keys[(uint8_t)0xA5] = TYPE_A;
    cmd_keys[(uint8_t)0xA6] = TYPE_A;
    cmd_keys[(uint8_t)0xA7] = TYPE_A;
    cmd_keys[(uint8_t)0xA8] = TYPE_A;
    cmd_keys[(uint8_t)0xA9] = TYPE_A;
    cmd_keys[(uint8_t)0xAA] = TYPE_A;
    cmd_keys[(uint8_t)0xAD] = TYPE_B;
    cmd_keys[(uint8_t)0xAE] = TYPE_B;
    cmd_keys[(uint8_t)0xD1] = TYPE_A;
    cmd_keys[(uint8_t)0xD2] = TYPE_A;
    cmd_keys[(uint8_t)0xD3] = TYPE_A;
    cmd_keys[(uint8_t)0xDC] = TYPE_A;
    cmd_keys[(uint8_t)0xDE] = TYPE_A;
    cmd_keys[(uint8_t)0xDD] = TYPE_B;
    cmd_keys[(uint8_t)0xE0] = TYPE_B;
    cmd_keys[(uint8_t)0xED] = TYPE_A;
    cmd_keys[(uint8_t)0xED] = TYPE_B;
}


void write_string(const char *str) {
    while (*str) {
        debuger.write(str++,1);  // Write each character one by one
    }
}

// Function to handle reading from pc and writing to uart1
void passthrough_read_from_slave1() {
    while (slave1.readable() && (current_slave == 1)) {
        unsigned char c;
        slave1.read(&c, 1);  // Read 1 byte from slave1
        master.write(&c,1);
    }
}

void passthrough_read_from_slave2() {
    while (slave2.readable() && (current_slave == 2)) {
        unsigned char c;
        slave2.read(&c, 1);  // Read 1 byte from slave2
        master.write(&c,1);
    }
}

// Function to handle reading from master and writing to slave
void passthrough_read_from_master() {
    while (master.readable()) {
        unsigned char c;
        master.read(&c, 1);  // Read 1 byte from Master
        
        sprintf(temp_str,"%i\n",c);
        // i = (uint8_t) c;
        debuger.write(temp_str,4);
        // write_string("\n");
        if ((uint8_t)c == 0xFA){
            write_string("Switch detected\n ");
            current_slave++;
            if (current_slave > NO_SLAVE){
                current_slave = 1;
            }
        }
        else{
            switch (current_slave){
                case 1:
                    slave1.write(&c,1);
                    break;
                case 2:
                    slave2.write(&c,1);
                    break;
                default:
                    slave1.write(&c,1);
                    break;
            }
            
        }
        
    }
}



int main() {
    // Attach the interrupt functions to monitor readable serial data
    slave1.attach(&passthrough_read_from_slave1, SerialBase::RxIrq); // Trigger on slave1 read
    slave2.attach(&passthrough_read_from_slave2, SerialBase::RxIrq); // Trigger on slave2 read
    master.attach(&passthrough_read_from_master, SerialBase::RxIrq); // Trigger on master read

    // Main loop is empty; all the work is done by Interupts
    while (true) {
        
    }
}
