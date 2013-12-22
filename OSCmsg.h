/*
 
    Transport Type: 
      CAN
 
    Features: 
     Packet Parsing (Slave)
     Packet Construction (Master)
     Bundle NOT Support
 
    Type Support: 
     i: int32
     b: blob
     s: string
     f: float32
     m: MIDI message(port id, status byte, data1, data2) // I don't know the detail
 
*/   
    
class OSCmsg{

    char address[6];
    char type;
    char msg;
    int length;
    public:
         OSCmsg();
        
        //Setter and getter for the address field. Limiting it to just one character for now.
        
        int setAddr(char* addr);
        char* getAddr();
        
        //Setter and Getter for the type field.
        
        int setType(char mtype);
        char getType();
        
        //Setter and Getter for the actual message. Limiting it to just 2 bytesfor now.
        
        int setArgs(char msgs);
        char getArgs();
        
        //Get the legth of the message
        int getLen();
        
        friend class OSCclass;
        
        
};

/***************class to send and receive the OSC message ***********/

class OSCclass{

    char   rcvBuff[8]; 
    char packet[9];
    int   buflength;
    char msgs;
    char ttype;
    char addr[5];
    OSCmsg *temp;
    CANMessage msg;
    
    OSCmsg *receive_msg;
    OSCmsg *send_msg;
     
    public:
        OSCclass();
        void sendOSCmsg(OSCmsg *msg);
        OSCmsg* getOSCmsg(char *recv);
        
};
       
        
        
        
        
        
        
        
        
        
        
        
        
        
        