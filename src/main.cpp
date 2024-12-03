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

// Circular buffer pointers
// volatile int rx_read_pos = 0;
// volatile int rx_write_pos = 0;
// volatile int tx_read_pos = 0;
// volatile int tx_write_pos = 0;
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
// EventQueue queue(32 * EVENTS_EVENT_SIZE);

void write_string(const char *str) {
    while (*str) {
        debuger.write(str++,1);  // Write each character one by one
    }
}

// Function to handle reading from pc and writing to uart1
void passthrough_read_from_slave1() {
    while (slave1.readable() && (current_slave == 1)) {
        unsigned char c;
        slave1.read(&c, 1);  // Read 1 byte from USB serial
        
        // Store received byte in the circular buffer (rx_buffer)
        // int next_pos = (rx_write_pos + 1) % BUFFER_SIZE;
        // if (next_pos != rx_read_pos) {  // Check buffer overflow
        //     rx_buffer[rx_write_pos] = c;
        //     rx_write_pos = next_pos;
        // }
        // printf("I got '%s' from pc\n", c);
        // write_string("pc: ");
        // debuger.write(&c,1);
        // write_string("\n");
        master.write(&c,1);
    }
}

void passthrough_read_from_slave2() {
    while (slave2.readable() && (current_slave == 2)) {
        unsigned char c;
        slave2.read(&c, 1);  // Read 1 byte from USB serial
        
        // Store received byte in the circular buffer (rx_buffer)
        // int next_pos = (rx_write_pos + 1) % BUFFER_SIZE;
        // if (next_pos != rx_read_pos) {  // Check buffer overflow
        //     rx_buffer[rx_write_pos] = c;
        //     rx_write_pos = next_pos;
        // }
        // printf("I got '%s' from pc\n", c);
        // write_string("pc: ");
        // debuger.write(&c,1);
        // write_string("\n");
        master.write(&c,1);
    }
}

// Function to handle reading from uart1 and writing to pc
void passthrough_read_from_master() {
    while (master.readable()) {
        unsigned char c;
        master.read(&c, 1);  // Read 1 byte from UART1
        
        // Store received byte in the circular buffer (tx_buffer)
        // int next_pos = (tx_write_pos + 1) % BUFFER_SIZE;
        // if (next_pos != tx_read_pos) {  // Check buffer overflow
        //     tx_buffer[tx_write_pos] = c;
        //     tx_write_pos = next_pos;
        // }
        // printf("I got '%s' from uart1\n", c);
        // write_string("master: ");
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
            slave1.write(&c,1);
        }
        
    }
}

// Function to transmit data from rx_buffer to uart1
// void passthrough_write_to_uart1() {
//     while (tx_read_pos != tx_write_pos) {
//         if (uart1.writeable()) {
//             uart1.write(&tx_buffer[tx_read_pos], 1);  // Write 1 byte to UART1
//             tx_read_pos = (tx_read_pos + 1) % BUFFER_SIZE;
//         }
//     }
// }

// Function to transmit data from tx_buffer to pc
// void passthrough_write_to_pc() {
//     while (rx_read_pos != rx_write_pos) {
//         if (pc.writeable()) {
//             pc.write(&rx_buffer[rx_read_pos], 1);  // Write 1 byte to USB Serial
//             rx_read_pos = (rx_read_pos + 1) % BUFFER_SIZE;
//         }
//     }
// }

int main() {
    // Attach the interrupt functions to monitor readable serial data
    slave1.attach(&passthrough_read_from_slave1, SerialBase::RxIrq); // Trigger on USB Serial read
    slave2.attach(&passthrough_read_from_slave2, SerialBase::RxIrq); // Trigger on USB Serial read
    master.attach(&passthrough_read_from_master, SerialBase::RxIrq); // Trigger on UART1 read

    // Use EventQueue to manage the passthrough
    // queue.call_every(10ms, passthrough_write_to_uart1);  // Check every 10 ms for data to write to UART1
    // queue.call_every(10ms, passthrough_write_to_pc);    // Check every 10 ms for data to write to PC

    // Main loop is empty; all the work is done by interrupts and EventQueue
    while (true) {
        // queue.dispatch();  // Execute events in the queue
    }
}
