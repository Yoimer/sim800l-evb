/* Piece of code for updating analogue values to ThingSpeak using SIM800L module */

#include <SoftwareSerial.h>
 
//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 8
 
//SIM800 RX is connected to Arduino D7
#define SIM800_RX_PIN 7
 
//Create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);

#define TIMEOUT 30000

int8_t answer;

int onModulePin = 13;

char aux_str[100];

int x = 0;

int attempts;

char apn[] = "internet.movistar.ve";
//char apn[] = "www";

 
void setup()
{
  
  pinMode(onModulePin, OUTPUT);
  
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while(!Serial);
   
  //Being serial communication with Arduino and SIM800
  serialSIM800.begin(9600);
  delay(1000);
   
  Serial.println("Starting...");
  power_on();

  delay(3000);
  
  while( (sendATcommand("AT+CREG?\r\n", "+CREG: 0,1\r\n", 500) || 
            sendATcommand("AT+CREG?\r\n", "+CREG: 0,5\r\n", 500)) == 0 );
			
  sendATcommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n", "OK\r\n", TIMEOUT);
  snprintf(aux_str, sizeof(aux_str), "AT+SAPBR=3,1,\"APN\",\"%s\"\r\n", apn);
  sendATcommand(aux_str, "OK\r\n", TIMEOUT);
  
  attempts = 0;
  while (sendATcommand("AT+SAPBR=1,1\r\n", "OK\r\n", TIMEOUT) == 0)
  {
	delay(5000);
	attempts = attempts + 1;
	if(attempts > 2)
	{
		restartGPRS();
		attempts = 0;
	}
  }
  
  
  Serial.println("Passed");
}
 
void loop() 
{
	
}

/////////////////////////////////////////////////////////
int8_t sendATcommand(const char* ATcommand, const char* expected_answer1, unsigned int timeout) {

  uint8_t x = 0,  answer = 0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);    // Initialize the string

  delay(100);

  //while ( Serial.available() > 0) Serial.read();   // Clean the input buffer

  //  while(Serial.available()) { //Cleans the input buffer
  //        Serial.read();
  //    }

  while (serialSIM800.available())
  { //Cleans the input buffer
    serialSIM800.read();
  }



  Serial.println(ATcommand);    // Prints the AT command
  serialSIM800.write(ATcommand); // Sends the AT command


  x = 0;
  previous = millis();

  // this loop waits for the answer
  do
  {
    ////if (Serial.available() != 0) {
    if (serialSIM800.available() != 0)
	{
      ////response[x] = Serial.read();
      response[x] = serialSIM800.read();
      x++;
      // check if the desired answer is in the response of the module
      if (strstr(response, expected_answer1) != NULL)
      {
        answer = 1;
      }
    }
    // Waits for the asnwer with time out
  }
  while ((answer == 0) && ((millis() - previous) < timeout));

  return answer;
}

/////////////////////////////////////////////////////////
void power_on()
{

  uint8_t answer = 0;

  Serial.println("On Power_on...");

  // checks if the module is started
  answer = sendATcommand("AT\r\n", "OK\r\n", TIMEOUT);
  if (answer == 0)
  {
    // power on pulse
    digitalWrite(onModulePin, HIGH);
    delay(3000);
    digitalWrite(onModulePin, LOW);

    // waits for an answer from the module
    while (answer == 0)
	{
      // Send AT every two seconds and wait for the answer
      answer = sendATcommand("AT\r\n", "OK\r\n", TIMEOUT);
      Serial.println("Trying connection with module...");
    }
  }
}

/////////////////////////////////////////////////////////
void restartGPRS()
{
	sendATcommand("AT+CFUN=0\r\n", "OK\r\n", TIMEOUT);
	delay(5000);
	while(sendATcommand("AT+CFUN=1\r\n", "Call Ready\r\n", 2*TIMEOUT) == 0);
} 
