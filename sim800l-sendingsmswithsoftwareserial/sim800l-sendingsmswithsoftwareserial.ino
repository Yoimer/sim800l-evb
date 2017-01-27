/* Sending SMS with SoftwareSerial

In below code delay of 1 second is used after each command to give necessary time for SIM800 to respond to each command.
With this approach it is not possible to clearly identify any ERROR conditions because program will not ready the responses sent. 
Proper method of doing this is by checking each response against an expected value. 
This is already handled in most of the Arduino libraries including “Seeeduino” library we’ll be using below.
Hence, for this stage, 1 second delay is used for the sake of simplicity. 

/* Note : Replace 07194XXXXX with mobile number SMS should be sent to.  */

#include <SoftwareSerial.h>

//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 8
 
//SIM800 RX is connected to Arduino D7
#define SIM800_RX_PIN 7
 
//Create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);
 
void setup() {
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while(!Serial);
   
  //Being serial communication witj Arduino and SIM800
  serialSIM800.begin(9600);
  delay(1000);
   
  Serial.println("Setup Complete!");
  Serial.println("Sending SMS...");
   
  //Set SMS format to ASCII
  serialSIM800.write("AT+CMGF=1\r\n");
  delay(1000);
 
  //Send new SMS command and message number
  serialSIM800.write("AT+CMGS=\"07194XXXXX\"\r\n");
  delay(1000);
   
  //Send SMS content
  serialSIM800.write("SMS from SIM800l");
  delay(1000);
   
  //Send Ctrl+Z / ESC to denote SMS message is complete
  serialSIM800.write((char)26);
  delay(1000);
     
  Serial.println("SMS Sent!");
}
 
void loop() {
}




