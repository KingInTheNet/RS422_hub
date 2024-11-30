#include "mbed.h"

// Thread masterThread;
// Thread slaveThread;
// // Create a serial object
// static BufferedSerial pc(PC_12, PD_2);
// // static BufferedSerial pc(PE_8, PE_7);
// static BufferedSerial pcmaster(PD_5, PD_6);
// static UnbufferedSerial debuger(USBTX, USBRX,38400);



// Create UnbufferedSerial objects for two serial interfaces
UnbufferedSerial pc(PC_12, PD_2, 38400); // USB Serial
UnbufferedSerial uart1(PD_5, PD_6, 38400);  // UART1 Serial (example pins)

// Buffers for serial passthrough
#define BUFFER_SIZE 1
char rx_buffer[BUFFER_SIZE];
char tx_buffer[BUFFER_SIZE];
const char * pc_str = "pc: ";
const char * uart1_str = "uart1: ";
const char * newline_str = "\n";

// Circular buffer pointers
volatile int rx_read_pos = 0;
volatile int rx_write_pos = 0;
volatile int tx_read_pos = 0;
volatile int tx_write_pos = 0;

EventQueue queue(32 * EVENTS_EVENT_SIZE);

// void write_string(const char *str) {
//     while (*str) {
//         debuger.write(str++,1);  // Write each character one by one
//     }
// }

// Function to handle reading from pc and writing to uart1
void passthrough_read_from_pc() {
    while (pc.readable()) {
        char c;
        pc.read(&c, 1);  // Read 1 byte from USB serial
        
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
        uart1.write(&c,1);
    }
}

// Function to handle reading from uart1 and writing to pc
void passthrough_read_from_uart1() {
    while (uart1.readable()) {
        char c;
        uart1.read(&c, 1);  // Read 1 byte from UART1
        
        // Store received byte in the circular buffer (tx_buffer)
        // int next_pos = (tx_write_pos + 1) % BUFFER_SIZE;
        // if (next_pos != tx_read_pos) {  // Check buffer overflow
        //     tx_buffer[tx_write_pos] = c;
        //     tx_write_pos = next_pos;
        // }
        // printf("I got '%s' from uart1\n", c);
        // write_string("uart1: ");
        // debuger.write(&c,1);
        // write_string("\n");
        pc.write(&c,1);
    }
}

// Function to transmit data from rx_buffer to uart1
void passthrough_write_to_uart1() {
    while (tx_read_pos != tx_write_pos) {
        if (uart1.writeable()) {
            uart1.write(&tx_buffer[tx_read_pos], 1);  // Write 1 byte to UART1
            tx_read_pos = (tx_read_pos + 1) % BUFFER_SIZE;
        }
    }
}

// Function to transmit data from tx_buffer to pc
void passthrough_write_to_pc() {
    while (rx_read_pos != rx_write_pos) {
        if (pc.writeable()) {
            pc.write(&rx_buffer[rx_read_pos], 1);  // Write 1 byte to USB Serial
            rx_read_pos = (rx_read_pos + 1) % BUFFER_SIZE;
        }
    }
}

int main() {
    // Attach the interrupt functions to monitor readable serial data
    pc.attach(&passthrough_read_from_pc, SerialBase::RxIrq); // Trigger on USB Serial read
    uart1.attach(&passthrough_read_from_uart1, SerialBase::RxIrq); // Trigger on UART1 read

    // Use EventQueue to manage the passthrough
    // queue.call_every(10ms, passthrough_write_to_uart1);  // Check every 10 ms for data to write to UART1
    // queue.call_every(10ms, passthrough_write_to_pc);    // Check every 10 ms for data to write to PC

    // Main loop is empty; all the work is done by interrupts and EventQueue
    while (true) {
        // queue.dispatch();  // Execute events in the queue
    }
}
