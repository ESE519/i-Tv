#include "mbed.h"
#include "OSCmsg.h"


//CAN can1(p30, p29);


OSCmsg::OSCmsg(){
    type=0;
    msg=0;
    length=0;

}

int OSCmsg::setAddr(char* addr){

        strncpy(address,addr,5);
        address[5]=0;
        return 1;
        
 }
 
 char* OSCmsg::getAddr(){
        return address;
 }
 
 int OSCmsg::setType(char mtype){
        type = mtype;
         return 1;
  }
  
  char OSCmsg::getType(){
        return type;
  }
  
  int OSCmsg::setArgs(char msgs){ 
  
        msg = msgs;
        return 1;
    
  }
  
  char OSCmsg::getArgs(){
        return msg;
  }
  

  /****************functions for OSC class***********************/
  
OSCclass::OSCclass(){}
 
 void OSCclass::sendOSCmsg(OSCmsg *sendmsg){
  /*          
        can1.frequency(250000);
        
        if(sendmsg->getType() == 0)
            ttype = 'n';
         else ttype = sendmsg->getType();
         
         strncpy(addr,sendmsg->getAddr(),5);
        //int len = strlen(sendmsg->getArgs())
        //strncpy(msg,sendmsg->getArgs(),len);
        
        strncpy(packet,sendmsg->getAddr(),5);
        packet[5] = ',';
        packet[6]=ttype;
        packet[7] = sendmsg->getArgs();
        packet[8] = 0;

        printf("OSc mesage %s\r\n",packet);
        //Send this message on the can bus
       
       if(can1.write(CANMessage(1,packet,8))) {
            printf("sent %s %d \r\n",packet,strlen(packet));
        }*/
        
  }
  
  OSCmsg* OSCclass::getOSCmsg(char *recv){

                
                temp = new OSCmsg;
                
                //strncpy(addr,recv,5);
                addr[0] = recv[0];
                addr[1] = recv[1];
                addr[2] = recv[2];
                addr[3] = recv[3];
                addr[4] = recv[4];
                
                temp->setAddr(addr);
                temp->setType(recv[6]);
                temp->setArgs(recv[7]);

        
                return temp;
        
        
  
  
  }
        
  
  
  
  
  