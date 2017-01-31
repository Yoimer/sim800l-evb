/* Processing Received SMS through white list. 
If phone number is not on phonebook (contacts) rejects anything from them.
If so, turns ON LED on pin 13 on Arduino UNO board      */

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
   
  //Being serial communication with Arduino and SIM800
  serialSIM800.begin(9600);
  delay(1000);
   
  Serial.println("Boot Complete!");
  Serial.println("Listing current phonebook on SIM card ...");

  //Fetch currently phonebook
  
   serialSIM800.write("AT+CPBS=?\r\n");
  delay(2000);
  
  serialSIM800.write("AT+CPBS=\"SM\"\r\n");
  delay(2000);

  serialSIM800.write("AT+CPBS=?\r\n");
  delay(2000);

  serialSIM800.write("AT+CPBR=1,10\r\n");
  

  
}
   
  /*//Set SMS format to ASCII
  serialSIM800.write("AT+CMGF=1\r\n");
  delay(1000);
 
  //Send new SMS command and message number
  ////serialSIM800.write("AT+CMGS=\"07194XXXXX\"\r\n");
  serialSIM800.write("AT+CMGS=\"04168262667\"\r\n");  //Working
  delay(1000);
   
  //Send SMS content
  serialSIM800.write("SMS from SIM800l Prueba");
  delay(1000);
   
  //Send Ctrl+Z / ESC to denote SMS message is complete
  serialSIM800.write((char)26);
  delay(1000);
     
  Serial.println("SMS Sent!");    "+CMTI: \"SM\"" */

void loop() {

  if(serialSIM800.available()){
    Serial.write(serialSIM800.read());
   }

}






