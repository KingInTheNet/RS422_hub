#include "mbed.h"

Thread masterThread;
Thread slaveThread;
// Create a serial object
static BufferedSerial pc(PC_12, PD_2);
// static BufferedSerial pc(PE_8, PE_7);
static BufferedSerial pcmaster(PD_5, PD_6);
static BufferedSerial debuger(USBTX, USBRX);

// static UnbufferedSerial pcmaster(PD_5, PD_6);
// static UnbufferedSerial debugger(USBTX, USBRX);
// static UnbufferedSerial pc(PE_8, PE_7);
bool processing = 0;
bool big_data = 0;
bool transfer = 0;
static char cmd_keys[255];
static uint8_t buff;
char masterBuffer[1] = {};
char slaveBuffer[1] = {};
char big_buffer[6]={};
int transfer_phase = 0;


void init_map() {
    cmd_keys[(uint8_t)0xA1] = 0;
    cmd_keys[(uint8_t)0xA2] = 0;
    cmd_keys[(uint8_t)0xA3] = 0;
    cmd_keys[(uint8_t)0xA4] = 0;
    cmd_keys[(uint8_t)0xA5] = 0;
    cmd_keys[(uint8_t)0xA6] = 0;
    cmd_keys[(uint8_t)0xA7] = 0;
    cmd_keys[(uint8_t)0xA8] = 0;
    cmd_keys[(uint8_t)0xA9] = 0;
    cmd_keys[(uint8_t)0xAA] = 0;
    cmd_keys[(uint8_t)0xAD] = 1;
    cmd_keys[(uint8_t)0xAE] = 1;
    cmd_keys[(uint8_t)0xD1] = 0;
    cmd_keys[(uint8_t)0xD2] = 0;
    cmd_keys[(uint8_t)0xD3] = 0;
    cmd_keys[(uint8_t)0xDC] = 0;
    cmd_keys[(uint8_t)0xDE] = 0;
    cmd_keys[(uint8_t)0xDD] = 1;
    cmd_keys[(uint8_t)0xE0] = 1;
    cmd_keys[(uint8_t)0xED] = 0;
    cmd_keys[(uint8_t)0xED] = 1;
}
void master_recv()
{
    // led1 = !led1;
    
    // while(pc.readable()) {
    //     pc.read(buff,1);
    //     pcmaster.write(buff,1);
    // }
    if(pc.read(&buff,1)){
        pcmaster.write(&buff,1);
    }
}

void pc_recv()
{
    // led4 = !led4;
    // while(pcmaster.readable()) {
    //     pcmaster.read(buff,1);
    //     pc.write(buff,1);
    // }
    if(pcmaster.read(&buff,1)){
        pc.write(&buff,1);
    }
}

void master_thread() {
    printf("Start master thread\n");
    while(1){
        if (pcmaster.readable()) {
            // ThisThread::sleep_for(100);
            // sleep();
            // ThisThread::sleep_for(100);
            // ThisThread::sleep_for(1);
            pcmaster.read(masterBuffer, 1);
            // switch(buffer[0]){
            // if(!processing) {
                
            //     int i = 0;
            //     i = (uint8_t)masterBuffer[0];
            //     // printf("%i    %i\n",i,cmd_keys[0]);
            //     processing = 1;
            //     big_data = cmd_keys[i];
            //     // printf("Processing start\n");
            //     // watchdog.start(500);
                
            // }
            // if (big_data && (masterBuffer[0] == 0xAC)) {
            //     transfer = 1;
            // }
            
            // }
            printf("I got '%02x' from master\n", masterBuffer[0]);
            // pcslave1.write("I got ",6);
            // pcslave1.write(buffer,sizeof(buffer));
            // // pc1.write(" \n",2);
            // pc.write("I have ",7);
            pc.write(masterBuffer,1);
            // // pc2.write(" \n",2);
            // // pcslave2.write("I lick ",7);
            // // printf("I got '%s'\n", buffer);
            // // pcslave2.write(buffer,sizeof(buffer));
            // // pc3.write(" \n",2);
            // memset(buffer,'\0',sizeof(buffer));
            // pc.sync();
        }
    }
}

void slave_thread() {
    printf("Start slave thread\n");
    while(1) {
        if (pc.readable()) {
            // ThisThread::sleep_for(100);
            // sleep();
            // ThisThread::sleep_for(100);
            // ThisThread::sleep_for(1);
            pc.read(slaveBuffer, sizeof(slaveBuffer));

            // printf("I got '%02x' from slave\n", slaveBuffer[0]);
            // for(int j = 0; j < 24; j++) {
            //     printf("%i \n", big_buffer[j]);
            // }
            // if (processing) {
            //     if (transfer) {
            //         while(transfer_phase < 4){
            //             // ThisThread::sleep_for(5);
            //             pc.read(big_buffer, sizeof(big_buffer));

            //             // printf("I got '%i' from slave\n", sizeof(big_buffer));
            //             // for(int j = 0; j < 24; j++) {
            //             //     printf("%i \n", big_buffer[j]);
            //             // }
            //             pcmaster.write(big_buffer,sizeof(big_buffer));
                        
            //             transfer_phase++;
            //         }
            //         processing = 0;
            //         transfer = 0;
            //         big_data = 0;
            //         transfer_phase = 0;
            //         // watchdog.stop();
            //         // printf("processing end with big data\n");
                    
            //     }
            //     else{
            //         pc.read(slaveBuffer, sizeof(slaveBuffer));
            //         if (slaveBuffer[0] == 0xAC) {
            //             processing = 0;
                        
            //             // watchdog.stop();
            //             // printf("processing end\n");
            //         }
            //         // printf("I got '%02x' from slave\n", slaveBuffer[0]);
            //         pcmaster.write(slaveBuffer,sizeof(slaveBuffer));
            //     }
            // }
            // pc.read(slaveBuffer, sizeof(slaveBuffer));
            // switch(buffer[0]){

            // }
            printf("I got '%02x' from slave\n", slaveBuffer[0]);
            // pcslave1.write("I got ",6);
            // pcslave1.write(buffer,sizeof(buffer));
            // // pc1.write(" \n",2);
            // pc.write("I have ",7);
            pcmaster.write(slaveBuffer,sizeof(slaveBuffer));

        }
    }
    
}

int main(void)
{
    
    // pcmaster.baud(38400);
    // pc.baud(38400);
    pcmaster.set_baud(38400);
    pc.set_baud(38400);
    debuger.set_baud(38400);
    // Watchdog &watchdog = Watchdog::get_instance();
    // pc.format(
    //     /* bits */ 8,
    //     /* parity */ SerialBase::None,
    //     /* stop bit */ 1
    // );
    init_map();
    // ThisThread::sleep_for(100);
    // printf("Restart");
    // pc.attach(&pc_recv);
    // pcmaster.attach(&master_recv);
    masterThread.start(master_thread);
    slaveThread.start(slave_thread);
    masterThread.join();
    slaveThread.join();
    // while (1) {
    //     // sleep();
        
        
        
    // }
}