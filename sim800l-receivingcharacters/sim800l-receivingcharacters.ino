/* Processing Received SMS through white list using SIM800L

If phone number is not on phonebook (contacts) rejects anything from them.
If so, turns ON LED on pin 13 on Arduino UNO board or adds a new contact on SIM card. These options will depend
on the command (SMS) sent by the user.
*
*
* We will use the GPRS library from SEEEDUINO. It has a lot  of function for the SIM800L board that will make the development process easier and faster.
  You can get it from https://github.com/Seeed-Studio/Seeeduino_GPRS.git
* 
* 
* In case you don't know or remember how to implement a library on Arduino IDE,
  go on https://www.arduino.cc/en/hacking/libraries. Everything is explanied there very detailed.
*
*
* For this code version to work the user has to add the contacts on the SIMCARD first.
* This might be done by inserting the SIM card on their mobile phone and do so as usual.
* Or by using the AT command as explained in "Write Phonebook Entry" from SIM800 Series_AT Command Manual_V1.09.pdf.
* This manual can be found at http://simcomm2m.com/En/
* For just testing AT commands and their response on Arduino IDE Serial Monitor user might upload the simpleserialcommunication code on Arduino UNO hosted on
  https://github.com/Yoimer/sim800l-evb/blob/master/sim800l-simpleserialcommunication/sim800l-simpleserialcommunication.ino.
  With the above code user can test any AT command and check their responde. If decided adding contacts with this one and the response was OK; user can upload the code below.
  NOTE: This version only works with national numbers. If user adds international code (numbers with the '+' prefix wont work)
  For example a mobile number in Venezuela starts by 0412, 0416, 0424, 0414, 0426. If user adds something like +58 426 wont work at all.
  The reason for this; is because this application is meant to work just in our region. No one from abroad will have any posibility to take action in our system.

 * Our GitHub repo will grow almost dialy until the first realease which will be called "First Release V1.0". We recommend issuing the "git fetch" command 
   if using Git client from terminal on Windows, Linux or MacOSx in order the get any updates.
   When cloning our repo issue "git clone https://github.com/Yoimer/sim800l-evb.git".
   If user prefers the web version of GitHub feel free to use any browser and visit our website at  https://github.com/Yoimer/sim800l-evb.
*



/****************************************************************Start of the code********************************************************/


//--------------------------------Start-Libraries Section----------------------------------------------------//

#include <SoftwareSerial.h>
#include <gprs.h>

//--------------------------------End-Libraries Section----------------------------------------------------//


//--------------------------------Start-Definition Section--------------------------------------------------//

// Timeout SIMCard to respond
#define TIMEOUT    5000

//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 8
 
//SIM800 RX is connected to Arduino D7
#define SIM800_RX_PIN 7

//--------------------------------End-Definition Section--------------------------------------------------//


//--------------------------------Start-Object Creation--------------------------------------------------//
 
//Create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);

// Create GPRS object to apply SIM commands with  SIM800
GPRS gprs;

//--------------------------------End-Object Creation--------------------------------------------------//




//--------------------------------Start-Variable Declaration-------------------------------------------//
 
//Receive data with an end-marker. In this case will be the '\n' character.

//Initial values of variables processing any character from serial

const byte numChars = 500;
char receivedChars[numChars];   // an array to store the received data 
boolean newData = false;


//Initial values of variables to take just the phone number from phonebook

// When listing phonebook from SIM800L module it responses something like this:
// +CPBR: 1,"121",129,"AtencionCliente"
// +CPBR: 2,"04268888881",129,"Consulta saldo"

//We want to to take the number phone.

// First comma on String from phonebook
int firstComma = -1;

// Stopper on String from phonebook
int stopper = -1;


// Variable where phone number will be saved

String phoneNumber = "";

//--------------------------------End-Variable Declaration-------------------------------------------//



//--------------------------------Start-Setup section-----------------------------------------------//

void setup() {

  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while(!Serial);

 // Checks SIM800 board integrity.
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

//--------------------------------End-Setup Section----------------------------------------------//


//--------------------------------Start-Loop Section--------------------------------------------//

void loop() {

  recvWithEndMarker();   //Function that takes any character from serial until a new line has arrived.
  showNewData();         //Function that shows all the data saved on receivedChars
  
  }

//--------------------------------End-Loop Section-----------------------------------------------//


//--------------------------------Start-Function Section-------------------------------------------//

void recvWithEndMarker() {    
  static byte ndx = 0;    
  char endMarker = '\n';    //terminate line
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
      receivedChars[ndx] = '\0';              // terminate the string
      
      //Processes only phone number
      String lastLine = String(receivedChars);
      firstComma = lastLine.indexOf(',');
      stopper = lastLine.indexOf("129");
      if(lastLine.startsWith("+CPBR:")){
        //Serial.println("Yes");
         //Serial.println(firstComma );
         //Serial.println(stopper);
         for(int i = (firstComma+1); i <=(stopper-2); ++i){
          phoneNumber += lastLine[i];
         }
         Serial.println(phoneNumber);
         
         phoneNumber = "\r\n";  // Cleans string for new line
         
        //Clear char array for next line of read
       /*for( int i = 0; i < sizeof(receivedChars);  ++i ) {
       receivedChars[i] = (char)0;
      }*/
     } 
      ndx = 0;
      newData = true;
    }
  }
 }


void showNewData() {    
  if (newData == true) {        
    ////Serial.println(receivedChars);          
    newData = false;    
   }
 }

//--------------------------------End-Function Section-------------------------------------------//


/****************************************************************End of the code********************************************************/

