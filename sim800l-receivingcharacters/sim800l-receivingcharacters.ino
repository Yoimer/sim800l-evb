/* Processing Received SMS through white list. 
If phone number is not on phonebook (contacts) rejects anything from them.
If so, turns ON LED on pin 13 on Arduino UNO board      */

#include <SoftwareSerial.h>
#include <gprs.h>

// Timeout SIMCard to respond
#define TIMEOUT    5000

//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 8
 
//SIM800 RX is connected to Arduino D7
#define SIM800_RX_PIN 7
 
//Create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);


// Create GPRS object to apply SIM commands with  SIM800
GPRS gprs;


//                                                          Example 2 - Receive with an end-marker
const byte numChars = 500;
char receivedChars[numChars];   // an array to store the received data 
boolean newData = false;

//Initial values of variables to take just the phone number from phonebook

// First comma on String from phonebook
int firstComma = -1;

// Stopper on String from phonebook
int stopper = -1;


// Variable where phone number will be saved

String phoneNumber = "";



void setup() {

  /*****************************************************************Begining--Initialization Module***********************************************************************/
  
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while(!Serial);

  Serial.println("Starting SIM800 SMS Command Processor");
  gprs.preInit();
  delay(1000);
 
  while(0 != gprs.init()) {
      delay(1000);
      Serial.print("init error\r\n");
  } 
  Serial.print("Initialization Succesfull!\r\n");


 //Check current phonebook status
 if(0 != gprs.sendCmdAndWaitForResp("AT+CPBR=1,10\r\n", "OK", TIMEOUT)) {
    ERROR("ERROR:CPBR");
    return;
    } else {
      Serial.print("Phonebook is ok\r\n");
     }
     
// Send "list the content from phonebook command"

gprs.sendCmd("AT+CPBR=1,10\r\n");

////gprs.sendCmd("AT+CMGL=?\r\n");


}

/*****************************************************************End--Initialization Module***********************************************************************/

void loop() {

  recvWithEndMarker();   
  showNewData();
  
  }


void recvWithEndMarker() {    
  static byte ndx = 0;    
  char endMarker = '\n';    
  char rc;
         
  while (gprs.serialSIM800.available() > 0 && newData == false) {        
    rc = gprs.serialSIM800.read();
    
    if (rc != endMarker) {            
      receivedChars[ndx] = rc;            
      ndx++;            
      if (ndx >= numChars) {                
        ndx = numChars - 1;            
      }        
    }
    else {
      receivedChars[ndx] = '\0'; // terminate the string
      //Processes only phone number
      String lastLine = String(receivedChars);
      firstComma = lastLine.indexOf(',');
      stopper = lastLine.indexOf("129");
      if(lastLine.startsWith("+CPBR:")){
        //Serial.println("Yes");
        Serial.println(firstComma );
        Serial.println(stopper);
        for(int i = (firstComma+1); i <=(stopper-2); ++i){
         phoneNumber += lastLine[i];
        }
        Serial.println(phoneNumber);
      }/*else{
        Serial.print("No");
      }*/
      //Clear char array for next line of read
      /*for( int i = 0; i < sizeof(receivedChars);  ++i ) {
       receivedChars[i] = (char)0;
      }*/

      
      ndx = 0;
      newData = true;
    }
  }
 }

void showNewData() {    
  if (newData == true) {        
    //Serial.print("This just in ... ");
    ////Serial.println(receivedChars);          
    newData = false;    
   }
 }



