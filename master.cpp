//Commands used:
//RGB - C
//Haptics - H
//Vibe Pad - V
//SCoreboard - S
//Light Tower - L

// RGB Commands
// C <type> <parameters>
// C I RRR GGG BBB - Instant change
// C F RRR GGG BBB TTT - Fade over TTT milliseconds
// C P RRR GGG BBB TTT - Pulse for TTT milliseconds, then turn off 
// C L RRR GGG BBB TTT X - Flash for TTT milliseconds X times
// C S <script> - Run a predetermined script (unimplemented)

//Light Tower commands:
// L <type> <parameters>
// L C 1/0 1/0 1/0 1/0 - Configure each pin to be either forwards or backwards
// L I 111 222 333 444 - Instant change
// L F 111 222 333 444 TTT - Fade over TTT milliseconds
// L P 111 222 333 444 TTT - Pulse for TTT milliseconds, then turn off
// L L 111 222 333 444 TTT X - Flash for TTT milliseconds X times
// L S <script> - Run a predetermined script (unimplemented)

//Haptics vest: H <top-left> <top-right> <bot-left> <bot-right> <back-left> <back-right>

#include "mbed.h"
#include "OSCmsg.h"
#include "MRF24J40.h"
//nclude "rtos.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>

char   rcvBuff[8]; 

int   buflength;

//CAN can1(p30, p29);

// Serial port for showing RX data. Will be changed to Input Stream from USB!!!
Serial pc(USBTX,USBRX);

DigitalOut led1(LED1);
DigitalOut song(p5);
PwmOut red(p23);
PwmOut green(p24);
PwmOut blue(p25);

// Used for sending
char inputBuffer[128];
char previousInput[128];

// Sports Data
short sportsflag = 0; //sports data needs updating
char *team1=0;
char *prev_team1=0;
char *team2 =0;
char *prev_team2 =0;
int score1=0;
int prev_score1=0;
int score2=0;
int prev_score2=0;
char *event=0;
char *prev_event=0;

//Action data
char *movie = 0;
char *mood = 0;
char *impact = 0;
char *shot = 0;

//RGB data
short rgbflag = 0;    //rgb data needs updating


//Vest map:



int main()
{
    OSCmsg m;

    OSCclass *c=new OSCclass;
    
    
    uint8_t channel = 7;
    // Set the Channel. 0 is the default, 15 is max
    //mrf.SetChannel(channel);
    //Thread updater(update, NULL, osPriorityNormal, DEFAULT_STACK_SIZE, NULL);

    pc.baud(115200);
    //sb.baud(115200);

    pc.printf("Start----- MASTER!\r\n");

    pc.printf("Vibrator Command: 'V' AAA where AAA is target duty cycle \r\n");
    pc.printf("RGB Command: 'C' RRR GGG BBB where each are the respective duty cycles\r\n");
    pc.printf("Haptic Vest Command: 'H' XXXXXX where XXXXXX represents which motors are on (binary)\r\n");
    pc.printf("Scoreboard Command: 'S' XX where XX represent the two scores in order- 1 digit each\r\n");

    char *inputToken;
 
    while(1) {

        char buffer[30] = "V I 60\r\n";

        if (pc.readable()) {
            
            pc.gets(inputBuffer,128);

            if (strcmp(previousInput,inputBuffer) != 0) {

                sprintf(previousInput,"%s",inputBuffer);

                inputToken = strtok(inputBuffer, " ,");

                // Differentiate between different game modes: sports, action, etc....
                // Sports:
                if(strncmp(inputToken, "sports",6) == 0) {
                    team1 = strtok(NULL," ,"); //printf("team1 = %s\r\n", team1);
                    team2 = strtok(NULL," ,");//printf("team2 = %s\r\n", team2);
                    score1 = atoi(strtok(NULL," ,"));
                    score2 = atoi(strtok(NULL," ,"));
                    event = strtok(NULL," \n");//printf("event = %s, %d\r\n", event, strcmp(team1,event));

                    char buffer[14];
                    sprintf(buffer, "S %d\r\n", (score1 * 10 + score2));

                    
                    printf("%s\r\n",event);

                    // Updates the event
                    if(strcmp(event, team1) == 0) {
                            m.setAddr("/A/WA");
                            m.setType('s');
                            m.setArgs('1');
                            c->sendOSCmsg(&m);
                            red = 1.0;
                            blue = 0.0;
                            green = 0.0f;
                            

                    } else if(strcmp(event, team2) == 0) {
                            m.setAddr("/A/WA");
                            m.setType('s');
                            m.setArgs('3');
                            c->sendOSCmsg(&m);
                            red = 0.0;
                            blue = 1.0;
                            green = 0.0f;

                    } else if(strncmp(event, "red",5) == 0) {
                            m.setAddr("/A/ST");
                            m.setType('s');
                            m.setArgs('1');
                            c->sendOSCmsg(&m);
                            

                    } else if(strncmp(event, "end",3) == 0) {
                            m.setAddr("/A/WA");
                            m.setType('s');
                            m.setArgs('2');
                            c->sendOSCmsg(&m);
                        
                    } 
                    else if(strncmp(event, "opening",7) == 0) {
                            m.setAddr("/A/RA");
                            m.setType('t');
                            m.setArgs('1');
                            c->sendOSCmsg(&m);
                            //sound reactive leds
                            song = 1;
                            red=0.0f;
                            blue=0.0f;
                            green=1.0f;
                            
                    }
                    else if(strncmp(event, "fight",5) == 0) {
                            m.setAddr("/A/ST");
                            m.setType('t');
                            m.setArgs('2');
                            c->sendOSCmsg(&m);

                    }
                    
                    else if(strncmp(event, "shootout",8) == 0) {
                            m.setAddr("/A/WA");
                            m.setType('s');
                            m.setArgs('2');
                            c->sendOSCmsg(&m);
                    }
                    
                    else if(strncmp(event, "miss",4) == 0) {
                            m.setAddr("/A/ST");
                            m.setType('s');
                            m.setArgs('4');
                            c->sendOSCmsg(&m);

                    }
                    else if(strncmp(event, "nil",3) == 0) {
                            m.setAddr("/A/NI");
                            m.setType('t');
                            m.setArgs('1');
                            c->sendOSCmsg(&m);
                            red = 0.0f;
                            blue=0.0f;
                            green=0.5f;
                            song = 0;

                    }
                    else if(strncmp(event,"vibrate",7)==0){
                            //send to the jacket mbed
                            m.setAddr("/B/HS");
                            m.setType('t');
                            m.setArgs('1');
                            c->sendOSCmsg(&m);
                    }
                    else if(strncmp(event,"music",7)==0){
                            //sound reactive led
                            song = 1;
                    }
                    else if(strncmp(event,"ends",7)==0){
                            //switch off sound reactive led
                            song = 0;
                    }
                } 
                if(strncmp(inputToken, "action",6) == 0) {
                    //action BoondockSaints <mood> <impact> <shot>
                    //moods: bright, indoor, dark, action
                    //impact: left-right, back, nil
                    //shot: right-top, right-bot, left-top, left-bot, nil
                    
                    movie = strtok(NULL, " ");
                    mood = strtok(NULL, " ");
                    impact = strtok(NULL, " ");
                    shot = strtok(NULL, " ");

                       if (strncmp(mood,"bright",6)==0){

                             m.setAddr("/A/ON");
                             m.setType('s');
                             m.setArgs('4');
                             c->sendOSCmsg(&m);
                             red = 1.0f;
                             green=1.0f;
                             blue=0.0f;
                             
                         
                        } 
                        else if(strncmp(mood,"indoor",6)==0){//TODO

                            m.setAddr("/A/ON");
                            m.setType('s');
                            m.setArgs('5');
                            c->sendOSCmsg(&m);
                         }
                         else if(strncmp(mood,"music",5)==0){//TODO

                            song=1;
                         }
                         else if(strncmp(mood,"dark",4)==0){//TODO

                            m.setAddr("/A/NI");
                            m.setType('t');
                            m.setArgs('7');
                            c->sendOSCmsg(&m); 
                             red = 0.0f;
                             green=0.0f;
                             blue=0.0f;
                        } 
                        else if(strncmp(mood,"action",6)==0){

                            m.setAddr("/A/ST");
                            m.setType('s');
                            m.setArgs('1');
                            c->sendOSCmsg(&m);
                        } 
                        else if(strncmp(mood,"strobe",6)==0){

                            m.setAddr("/A/ST");
                            m.setType('s');
                            m.setArgs('4');
                            c->sendOSCmsg(&m);
                        } 
                         else if(strncmp(mood,"done",6)==0){

                            m.setAddr("/A/NI");
                            m.setType('s');
                            m.setArgs('4');
                            c->sendOSCmsg(&m);
                            red = 1.0f;
                             green=1.0f;
                             blue=1.0f;
                        } 
                     
                        else if(strncmp(mood,"nil",3)==0){
                       // printf("mood %s\r\n",mood);
                             //rf_send("Nil\r\n",6);
                        }
                        
                        
                       if(strncmp(shot,"right-top",9)==0){

                             m.setAddr("/B/RT");
                            m.setType('t');
                            m.setArgs('1');
                            c->sendOSCmsg(&m);
                         }
                        else if(strncmp(shot,"right-bot",9)==0){

                            m.setAddr("/B/RB");
                            m.setType('t');
                            m.setArgs('1');
                            c->sendOSCmsg(&m);
                        }
                        else if(strncmp(shot,"left-top",8)==0){

                            m.setAddr("/B/LT");
                            m.setType('t');
                            m.setArgs('1');
                            c->sendOSCmsg(&m);
                        }
                        else if(strncmp(shot,"left-bot",8)==0){

                            m.setAddr("/B/LB");
                            m.setType('t');
                            m.setArgs('1');
                            c->sendOSCmsg(&m);
                        }
                        else if(strncmp(impact,"back",4)==0){

                            m.setAddr("/B/BA");
                            m.setType('t');
                            m.setArgs('1');
                            c->sendOSCmsg(&m);  
                        }
                        else if(strncmp(shot,"head",4)==0){

                            m.setAddr("/B/HS");
                            m.setType('t');
                            m.setArgs('1');
                            c->sendOSCmsg(&m);     
                        }
                        else if(strncmp(shot,"front",5)==0){

                            m.setAddr("/B/FR");
                            m.setType('t');
                            m.setArgs('1');
                            c->sendOSCmsg(&m);    
                        }
                        else if(strncmp(shot,"back",4)==0){

                            m.setAddr("/B/BA");
                            m.setType('t');
                            m.setArgs('1');
                            c->sendOSCmsg(&m);    
                        }

            
            }

        }
    }
    
}
}
