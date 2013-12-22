#include "mbed.h"
#include <stdlib.h>     // srand, rand
#include <time.h>       // time
#include<map>
#include<string.h>
#include <vector>
#include "rtos.h"
#include "OSCmsg.h"
#include "MRF24J40.h"


Serial pc(USBTX, USBRX);
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


// Used for sending and receiving
char txBuffer[128];
char rxBuffer[128];
int rxLen;
float r_value,b_value,g_value;
map<int,vector<float> > color_table;
vector<float> values;

typedef enum { RED, GREEN, BLUE } color;
typedef enum {ON1,STROBE1,WAVE1,FLOW1,NILL1,FLASH1,DEFAULT,OFF} stat;
stat status = DEFAULT;
typedef enum { SELECT, STROBE, FLOW,FADE,RAINBOW,SELECT_MIX} mode;

PwmOut red(p23);
PwmOut green(p24);
PwmOut blue(p25);

//CAN INIT


mode m_select = SELECT_MIX; // start with SELECT mode selected
color c_select = RED;   // start with RED color selected
float latch_red = 1.0f, latch_green = 0.0f, latch_blue =0.0f;   // for storing previous rgb values temporarily
float red_goal = 0, green_goal = 0, blue_goal = 0;  // for FLOW mode, the goal rgb values
float r = 0, g = 0, b = 0;  // the current rgb values
int count = 0;

void switch_mode() {
    latch_red = r;
    latch_green = g;
    latch_blue = b;
    count = 0;
    
    // switch mode
    if (m_select == SELECT) {
        // switch to STROBE mode if currently in SELECT mode
        pc.printf("STROBE\n");
        m_select = STROBE;
    } else if (m_select == STROBE) {
        // switch to FLOW mode if currently in STROBE mode
        pc.printf("FLOW\n");
        m_select = FLOW;
    } else if (m_select == FLOW) {
        // switch to SELECt mode if currently in FLOW mode
        pc.printf("SELECT\n");
        m_select = SELECT;
    }
}

void switch_color() {
    float temp_r = red, temp_g = green, temp_b = blue;
    
    // switch color selected and flash purely the new color selected for half a second
    if (c_select == RED) {   
        c_select = GREEN;
        red = 0.0f;
        green = 1.0f;
        blue = 0.0f;
    } else if (c_select == GREEN) {
        c_select = BLUE;
        red = 0.0f;
        green = 0.0f;
        blue = 1.0f;
    } else if (c_select == BLUE) {
        c_select = RED;
        red = 1.0f;
        green = 0.0f;
        blue = 0.0f;
    }
    wait(0.5f);
    
    // return to original rgb color
    red = temp_r;
    green = temp_g;
    blue = temp_b;
}

void select(float r1,float g1,float b1){

    red = r1;
    green = g1;
    blue = b1;

}
void select_mix(float r1,float b1,float g1){

    red = r1;
    blue = b1;
    green = g1;
    wait(0.1f);

}
void fade(float r1,float g1,float b1,int step){
    int count = step;
    float red1,green1,blue1;
    while(step){
            
            red1 = r1*step*1.0/count;
            blue1 = b1*step*1.0/count;
            green1 = g1*step*1.0/count;
            step--;
            red = red1;
            blue = blue1;
            green = green1;
            pc.printf("%f %f %f\r\n",red1,blue1,green1);
            wait(0.01);
     }
   }
            
    
  void strobe(float red1,float green1, float blue1,float delay){        

            
            
            if (count == 0) {
                count = 1;
                // turn the LEDs off for time delay
                red = 0;
                green = 0;
                blue =0;
            } else {
                count = 0;
                // turn them back on again for time delay
                r = red1;
                g = green1;
                b = blue1;
            }
            
            wait(0.1);
            red = r;
            green = g;
            blue = b;
  }

void flow(){
            int flow_steps = 25;
            // if count is zero, generate new goal color
            if (count == 0) {
                latch_red = r, latch_green = g, latch_blue = b;
                // set new random goal color to flow to
                red_goal = 0.25f * (rand() % 5);
                green_goal = 0.25f * (rand() % 5 );
                blue_goal = 0.25f * (rand() % 5);
                pc.printf("\nr %1.2f g %1.2f b %1.2f\n", red_goal, green_goal, blue_goal);
            }
            
            // count the number of steps toward goal color taken
            count++;
            
            // if goal color has been reached, reset count (causes new goal color to be generated)
            if (count > flow_steps) count = 0;
            
            // increment/decrement rgb values toward goal color
            r += (red_goal) / flow_steps;
            g += (green_goal) / flow_steps;
            b += (blue_goal) / flow_steps;
            
            // rate of flow is determined by slider
            //wait(0.1);
            red = r;
            green = g;
            blue = b;

}


void rainbow(){


  float rise_time = 1000;
  float fall_time = .01;
  float wait_time = .005;
  
  for(float i = 0; i<=1; i+=0.01)
  {
    red=i;
    wait_us(rise_time);
  }
  //wait(wait_time);
  for(float i = 1; i>=0; i-=0.01)
  {
    blue=i; 
   wait_us(fall_time);
  }
  //wait(wait_time);
  for(float i = 0; i<=1; i+=0.01)
  {
    green=i;
    wait_us(rise_time);
    //wait(rise_time);
  }
  //wait(wait_time);
  for(float i = 1; i>=0; i-=0.01)
  {
    red=i;
    wait_us(rise_time);
    //wait(fall_time);
  }
  //wait(wait_time);
  for(float i = 0; i<=1; i+=0.01)
  {
    blue=i;
    wait_us(rise_time);
    //wait(rise_time);
  }
  //wait(wait_time);
  for(float i = 1; i>=0; i-=0.01)
  {
    green=i;
    wait_us(rise_time);
    //wait(fall_time);
  }
  //wait(wait_time);

}
void switch_off(){

    red = 0.0f;
    green=0.0f;
    blue=0.0f;


}
void flash(){
    
    red=r_value;
    blue=b_value;
    green=g_value;
    wait_ms(100);
    red=0;
    blue=0;
    green=0;
    wait_ms(100);
    


}

void led_control(void const* args){
    while(1){
        if(status == ON1){
           //pc.printf("on\r\n");
           select(r_value,g_value, b_value);
        }
         if(status == STROBE1){
           //pc.printf("strobe\r\n");
           strobe(r_value,g_value,b_value,0.2);
        }
         if(status == WAVE1){
           //pc.printf("wave\r\n");
           //fade(r_value,g_value,b_value,25);
           flash();
        }
         if(status == FLOW1){
          // pc.printf("flow\r\n");
            //flow();
         rainbow();
        }
         if(status == NILL1){
           //pc.printf("nill\r\n");
        }
         if(status == FLASH1){
            flash();
           //pc.printf("flash\r\n");
        }
        if(status == OFF){
           switch_off();
           //pc.printf("flash\r\n");
        }
            
}
}
void init_map(){
    vector<float> color;
    float c[3];
     //red
    c[0]=1.0f;c[1]=0.0f,c[2]=0.0f;
    color.insert(color.begin(),c,c+3);
    
    color_table[1] = color;
    
    //green
    c[0]=0.0f;c[1]=1.0f,c[2]=0.0f;
    color.insert(color.begin(),c,c+3);
    color_table[2] = color;
    
    //blue
    c[0]=0.0f;c[1]=0.0f,c[2]=1.0f;
    color.insert(color.begin(),c,c+3);
    color_table[3] = color;  
   
   //yellow
    c[0]=1.0f;c[1]=1.0f,c[2]=0.0f;
    color.insert(color.begin(),c,c+3);
    color_table[4] = color;  
    
    //brown
    c[0]=0.2f;c[1]=0.0f,c[2]=0.0f;
    color.insert(color.begin(),c,c+3);
    color_table[5] = color; 
    
    //light blue
    c[0]=0.0f;c[1]=1.0f,c[2]=1.0f;
    color.insert(color.begin(),c,c+3);
    color_table[6] = color;   
    
    //pink
    c[0]=1.0f;c[1]=0.0f,c[2]=1.0f;
    color.insert(color.begin(),c,c+3);
    color_table[10] = color;  
    
    //purple
    c[0]=0.5f;c[1]=0.0f,c[2]=0.5f;
    color.insert(color.begin(),c,c+3);
    color_table[8] = color;  
    
    //white
    c[0]=1.0f;c[1]=1.0f,c[2]=1.0f;
    color.insert(color.begin(),c,c+3);
    color_table[9] = color;  
    
    //black
    c[0]=0.0f;c[1]=0.0f,c[2]=0.0f;
    color.insert(color.begin(),c,c+3);
    color_table[7] = color;  
    
    //maroon
    c[0]=0.5f;c[1]=0.0f,c[2]=0.0f;
    color.insert(color.begin(),c,c+3);
    color_table[11] = color;
    
    // dark green
    c[0]=0.0f;c[1]=0.5f,c[2]=0.0f;
    color.insert(color.begin(),c,c+3);
    color_table[12] = color;  
    
    //dark blue
    c[0]=0.0f;c[1]=0.0f,c[2]=0.5f;
    color.insert(color.begin(),c,c+3);
    color_table[13] = color;  
    
    //olive
    c[0]=0.5f;c[1]=0.5f,c[2]=0.0f;
    color.insert(color.begin(),c,c+3);
    color_table[14] = color;  
    
    //teal
    c[0]=0.0f;c[1]=0.5f,c[2]=0.5f;
    color.insert(color.begin(),c,c+3);
    color_table[15] = color;  
    
    
}

int main(){

    OSCmsg m;
    OSCmsg *recv;

    OSCclass *c=new OSCclass;
   //pc.baud(115200);
    
    Thread t1(led_control);
    char *command=(char *)malloc(128);
    char mode;
    char color[2];
    float delay,s=0.0f;
    init_map();
    //initialize table
    pc.printf("Start----- Light COntroller!\r\n");
    red=0;blue=0;green=0;
    
    /****OSC message temp***/
    char add[5];
    
    while(1) {
        rxLen = rf_receive(rxBuffer, 128);
     
        if(rxLen > 0) {
                    
            recv= c->getOSCmsg(rxBuffer);
            printf("Address is %s with type %c and msg %c \r\n",recv->getAddr(),recv->getType(),recv->getArgs());
            strncpy(add,recv->getAddr(),5);
           
           //check if the message is for LED Control
           printf("add 2 is %c\r\n",add[1]);
           if(add[1] == 'A'){
                //check for the mode-strobe,on,flash,wave
                mode = add[3];
               
               switch(mode){
                    
                    case'O':  //switch on LED
                               color[0] = recv->getArgs();
                               pc.printf("color is: %s and \r\n",color);
                              values = color_table.find(atoi(color))->second;
                              r_value=values[0];
                              g_value=values[1];
                              b_value=values[2];
                              pc.printf("%f %f %f",r_value,g_value,b_value);
                              status = ON1;
                               break;
                     case'S':  //switch on LED
                               color[0] = recv->getArgs();
                               pc.printf("color is: %s and S\r\n",color);
                              values = color_table.find(atoi(color))->second;
                              r_value=values[0];
                              g_value=values[1];
                              b_value=values[2];
                              pc.printf("%f %f %f",r_value,g_value,b_value);
                              status = STROBE1;
                               break;
                     case'W':  //switch on LED
                               color[0] = recv->getArgs();
                               pc.printf("color is: %s and W\r\n",color);
                              values = color_table.find(atoi(color))->second;
                              r_value=values[0];
                              g_value=values[1];
                              b_value=values[2];
                              pc.printf("%f %f %f",r_value,g_value,b_value);
                              status = WAVE1;
                               break;
                    case'R':  //switch on LED
                                color[0] = recv->getArgs();
                               pc.printf("color is: %s and F\r\n",color);
                               values = color_table.find(atoi(color))->second;
                               status = FLOW1;
                               r_value=values[0];
                               g_value=values[1];
                               b_value=values[2];
                           
                               break;
                      case'K':  //switch on LED
                               color[0] = recv->getArgs();
                               pc.printf("color is: %s and K\r\n",color);
                              values = color_table.find(atoi(color))->second;
                              r_value=values[0];
                              g_value=values[1];
                              b_value=values[2];
                              status = FLOW1;
                               break;
                     case 'N':  //switch on LED
                                color[0] = recv->getArgs();
                               //pc.printf("color is: %s and K\r\n",color);
                              //values = color_table.find(atoi(color))->second;
                              r_value=0.0f;
                              g_value=0.0f;
                              b_value=0.0f;
                              status = OFF;
                               break;
                    default:  break;
                }
            
            }
            
      }
    }             
}  
          