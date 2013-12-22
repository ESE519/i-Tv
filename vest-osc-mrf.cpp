#include "mbed.h"
#include "OSCmsg.h"
#include "MRF24J40.h"


#include <string>

Timeout to1;
Timeout to2;
Timeout to3;
Timeout to4;
Timeout to5;
Timeout to6;

DigitalOut motor1(p23);
DigitalOut motor2(p28);
DigitalOut motor3(p24);
DigitalOut motor4(p30);
DigitalOut motor6(p25);
DigitalOut solenoid1(p29);   
DigitalOut solenoid2(p26);   
                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
DigitalOut led1(LED1);

int motorStatus;

// Serial port for showing RX data.
Serial pc(USBTX,USBRX);

MRF24J40 mrf(p11,p12,p13,p14,p21);
/**
* Receive data from the MRF24J40.
*
* @param data A pointer to a char array to hold the data
* @param maxLength The max amount of data to read.
*/
int rf_receive(char *data, uint8_t maxLength)
{
    uint8_t len = mrf.Receive((uint8_t *)data, maxLength);
    uint8_t header[8]= {1, 8, 0, 0xA1, 0xB2, 0xC3, 0xD4, 0x00};
    if(len > 10) {
        //Remove the header and footer of the message
        for(uint8_t i = 0; i < len-2; i++) {
            if(i<8) {
                //Make sure our header is valid first
                if(data[i] != header[i])
                    return 0;
            } else {
                data[i-8] = data[i];
            }
        }

        //pc.printf("Received: %s length:%d\r\n", data, ((int)len)-10);
    }
    return ((int)len)-10;
}


//Haptics vest: H <top-left> <top-right> <bot-left> <bot-right> <back-left> <back-right>

// Used for sending and receiving










char txBuffer[128];
char rxBuffer[128];
int rxLen;





// Timeout for vibration
void shot1() {

     motor1 = !motor1;
       //solenoid1=!solenoid1;
}
void shot2() {
     motor2 = !motor2;
}
void shot3() {
     motor3 = !motor3;
     //solenoid2=!solenoid2;
}
void shot4() {
     motor4 = !motor4;
}
/*void shot5() {
     motor5 = !motor5;
}*/
void shot6() {
     motor6 = !motor6;
}

void updateMotors() {
    
    if(motor1) to1.attach(&shot1,0.2);
    if(motor2) to2.attach(&shot2,.2);
    if(motor3) to3.attach(&shot3,.2);
    if(motor4) to4.attach(&shot4,.2);
    //if(motor5) to5.attach(&shot5,.2);
    if(motor6) to6.attach(&shot6,.2);
}

int find_status(char *add){
    int motorstatus; 
    if(strncmp(add,"/B/LT",5)==0){
            motorstatus = 10000;
    }
    else if(strncmp(add,"/B/LB",5)==0){
        motorstatus = 1000;
    }
    else if(strncmp(add,"/B/RT",5)==0){
        motorstatus = 100;
    }
    else if(strncmp(add,"/B/RB",5)==0){
            motorstatus = 10;
    }
    else if(strncmp(add,"/B/FR",5)==0){
        motorstatus = 11110;
    }
     else if(strncmp(add,"/B/HS",5)==0){
        motorstatus = 11111;
     }
    else if(strncmp(add,"/B/BA",5)==0){
         motorstatus = 1;
     }
    else motorstatus = 0;
    
   
   return motorstatus;
                   
        
        
}
/********Motor Mappings*****************/
/*
    motor1 : LT
    motor2 : LB
    motor3 : RT
    motor4 : RB
    motor5 : BACK
    motor6 : BACK

*/
int main() {

    OSCclass *c=new OSCclass;
    OSCmsg *recv;

    
    uint8_t channel = 7;
    // Set the Channel. 0 is the default, 15 is max
    //mrf.SetChannel(channel);
    char add[5];
    char *command;
    
    pc.printf("Start----- Haptic Vest!\r\n");
    
    
    while(1) {

        rxLen = rf_receive(rxBuffer, 128);
       // pc.printf("RxLen %d", rxLen);
        if(rxLen > 0) {
                    
            recv= c->getOSCmsg(rxBuffer);

            printf("Address is %s with type %c and msg %c \r\n",recv->getAddr(),recv->getType(),recv->getArgs());
            
            strncpy(add,recv->getAddr(),5);
           
            if(add[1] == 'B'){
            
                    
                        motorStatus = find_status(add);
        
                        pc.printf("motor status: %d\r\n", motorStatus);
        
                        motor6 = motorStatus%10;//BACK
                        //motor5 = motor6;
                        motor4 = (motorStatus / 10) % 10;//RB
                        motor3 = (motorStatus / 100) % 10;//RT
                        //solenoid2=motor3;
                        motor2 = (motorStatus / 1000) % 10;//LB
                        motor1 = (motorStatus / 10000) % 10;//LT
                        //solenoid1=motor1;
                        //printf("%d %d %d %d %d %d \r\n",motor1,motor2,motor3,motor4,motor5,motor6);
                        
                        updateMotors();

                }
        }

    }
}
