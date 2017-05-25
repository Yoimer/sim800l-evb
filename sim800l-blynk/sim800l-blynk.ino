/* Piece of code for updating LEN ON OR LED OFF values to blynk
   when receiving SMS using SIM800L module */

#include <SoftwareSerial.h>
 
//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 8
 
//SIM800 RX is connected to Arduino D7
#define SIM800_RX_PIN 7
 
//Create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);

#define TIMEOUT 30000

int8_t answer;

int onModulePin                                                              = 13;

char aux_str[100];

int x                                                                        = 0;

int attempts;

char apn[]                                                                   = "internet.movistar.ve";

//char url[ ]="api.thingspeak.com/update?api_key=PHRFH37I50UK9MGF&field1=20.50";

unsigned long lastConnectionTime                                             = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval                                          = 15000L; // delay between updates, in milliseconds


//Boolean to be set to false if number is on contact or SMS content is LED OFF
bool ledStatus                                                               = HIGH;

//Variable to hold last line of serial output from SIM800
char currentLine[500]                                                        = "";
int currentLineIndex                                                         = 0;

//Boolean to be set to true if call notificaion was found and next line is NOT empty
//bool nextLineIsCall = false;
bool nextValidLineIsCall                                                     = false;

////Boolean to be set to true if message notificaion was found and next
////line of serial output is the actual SMS message content
bool nextLineIsMessage                                                       = false;

//Boolean to be set to true if number is found on phonebook
bool isInPhonebook                                                           = false;

// String which holds the last line read from Serial activitiy
String lastLine                                                              = "";

// To be saved phonemumber who sent SMS
String phonenum                                                              = "";


// Integer indexes
int firstComma                                                               = -1;
int secondComma                                                              = -1;
int thirdComma                                                               = -1;
int forthComma                                                               = -1;
int fifthComma                                                               = -1;

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
            sendATcommand("AT+CREG?\r\n", "+CREG: 0,5\r\n", 500)) == 0 ); //Waits for network registration

  connectToNetwork(); //Opens internet connectivity
  SMSandCallSettings();// Set SMS and Call configurations
  Serial.println("Passed!");
  
}
 
/*

PINOUT Connection:

///////////////////////////////////////////////////////////////////////////////

External 12VDC/2A Power Supply                                    MP1584 (Turn knot until a volmeter shows 5VDC)

Positive--------------------------------------------------------->Positive

Negative--------------------------------------------------------->Negative

///////////////////////////////////////////////////////////////////////////////

MP1584                                                            SIM800L-EVB

Positive--------------------------------------------------------->5V/4V

Negative--------------------------------------------------------->GNB

///////////////////////////////////////////////////////////////////////////////

Arduino UNO                                                        SIM800L-EVB

Digital 7--------------------------------------------------------->SIM_RXD

Digital 8--------------------------------------------------------->SIM_TX

RESET------------------------------------------------------------->RST

GND (POWER SECTION)----------------------------------------------->GND

///////////////////////////////////////////////////////////////////////////////
 
*/


void loop() 
{
	//Write current status to LED pin
  digitalWrite(onModulePin, ledStatus);
  
  //If there is serial output from SIM800
  if (serialSIM800.available())
     {
       char lastCharRead = serialSIM800.read();

    //Read each character from serial output until \r or \n is reached (which denotes end of line)
    if (lastCharRead == '\r' || lastCharRead == '\n')
       {
         endoflinereached();
       }
    else
       {
         currentLine[currentLineIndex++] = lastCharRead;
       }
  }
}

/////////////////////////////////////////////////////////
int8_t sendATcommand(const char* ATcommand, const char* expected_answer1, unsigned int timeout) {

  uint8_t x = 0,  answer = 0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);    // Initialize the string

  delay(100);

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
void restartPhoneActivity()
{
	do
	{
		sendATcommand("AT+CFUN=0\r\n", "OK\r\n", TIMEOUT);
		delay(2000);
		answer = sendATcommand("AT+CFUN=1\r\n", "Call Ready\r\n", TIMEOUT);
	}while(answer == 0);
}
/////////////////////////////////////////////////////////
void connectToNetwork()
{
	sendATcommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n", "OK\r\n", TIMEOUT);//sets Contype
	snprintf(aux_str, sizeof(aux_str), "AT+SAPBR=3,1,\"APN\",\"%s\"\r\n", apn);//sets APN
	sendATcommand(aux_str, "OK\r\n", TIMEOUT);
	attempts = 0;//tries 3 times or gets on the loop until sendATcommand != 0
	while (sendATcommand("AT+SAPBR=1,1\r\n", "OK\r\n", TIMEOUT) == 0)
          {
		    delay(5000);
		    attempts = attempts + 1;
		    if(attempts > 2)
		      {
			    restartPhoneActivity();
			    attempts = 0;
		      }
          } 
}
/////////////////////////////////////////////////////////
void initHTTPSession()
{
	while (sendATcommand("AT+HTTPINIT\r\n", "OK\r\n", TIMEOUT) == 0)
          {
		    restartPhoneActivity();
		    connectToNetwork();
          }
}
/////////////////////////////////////////////////////////
void HTTPRequest()
{
	char const * const url[]=
			{
	          "blynk-cloud.com/fdbaed2437134895a352412f0af64190/update/v0?value=0",
              "blynk-cloud.com/fdbaed2437134895a352412f0af64190/update/v0?value=255",
            };
			
	snprintf(aux_str, sizeof(aux_str), "AT+HTTPPARA=\"URL\",\"%s\"\r\n", url[ledStatus]);
	sendATcommand(aux_str, "OK\r\n", TIMEOUT);
	delay(3000);
	attempts = 0;//tries 3 times or gets on the loop until sendATcommand != 0
	while (sendATcommand("AT+HTTPACTION=0\r\n", "+HTTPACTION: 0,200,", TIMEOUT) == 0)
          {
		    delay(5000);
		    attempts = attempts + 1;
		    if(attempts > 2)
		      {
			    sendATcommand("AT+SAPBR=0,1\r\n", "OK\r\n", 2 * TIMEOUT);
			    sendATcommand("AT+HTTPTERM\r\n", "OK\r\n", TIMEOUT);
			    restartPhoneActivity();
			    connectToNetwork();
			    initHTTPSession();
			    snprintf(aux_str, sizeof(aux_str), "AT+HTTPPARA=\"URL\",\"%s\"\r\n", url[ledStatus]);
			    sendATcommand(aux_str, "OK\r\n", TIMEOUT);
			    attempts = 0;
		     }
          }
	Serial.println("Successfully uploaded");
	// note the time that the connection was made
    lastConnectionTime = millis();
	
}

void SMSandCallSettings()
{
	sendATcommand("AT+CMGF=1\r\n", "OK\r\n", TIMEOUT); // SMS Text mode
    sendATcommand("AT+CNMI=1,2,0,0,0\r\n", "OK\r\n", TIMEOUT); // Receive SMS on UART
	sendATcommand("AT+CLIP=1\r\n","OK\r\n", TIMEOUT);// Receive Call Identifier on UART
}
/////////////////////////////////////////////////////////
void LastLineIsCMT()
{
  if (nextLineIsMessage)
     {
       Serial.println(lastLine);

      // If exists on Phonebook
      //if (secondComma > 22)   // Only works with Movilnet and Digitel Venezuela
      //if (secondComma > 24)    // Only works with for TelefÃ³nica Movistar Venezuela
    if ((secondComma - firstComma) > 3)
       {
         Serial.println("In Phonebook"); //For debugging
         isInPhonebook = true;
         Serial.println(isInPhonebook);
       }
    else
       {
         Serial.println("Not in Phonebook"); //For debugging
         isInPhonebook = false;
       }
    //if on phonebook ---------------------------
    if (isInPhonebook)
       {
         // If SMS contains LED ON or LED OFF or #WhiteList
        if (lastLine.indexOf("LED ON") >= 0)
           {
             ledStatus = 1;   // Turns ON LED
		     digitalWrite(onModulePin, ledStatus);
		     HTTPRequest();
           }
        else if (lastLine.indexOf("LED OFF") >= 0)
                {
                  ledStatus = 0;  // Turns OFF LED
		          digitalWrite(onModulePin, ledStatus);
		          HTTPRequest();
                }
       }
    CleanCurrentLine();
    nextLineIsMessage = false;
  }
}
/////////////////////////////////////////////////////////
void endoflinereached()
{
  lastLine = String(currentLine);

  //-----------------------Call---------------------------//
  // If lastLine reads RING, New Call Indication was received.
  // Hence, the THIRD LINE is the caller information.

  // The whole string will be something like this if caller IS registered on SIM Card:
  //RING (First Line)
  //     (Second Line is empty)
  //+CLIP: "04168262667",129,"",0,"Yoimer",0 (Third Line)

  // The whole string will be something like this if caller is NOT registered on SIM Card
  //RING (First Line)
  //     (Second Line is empty)
  //+CLIP: "04168262667",129,"",0,"",0 (Third Line)

  //----------------------SMS-------------------------------------//
  //If lastLine reads +CMT, New SMS Message Indications was received.
  //Hence, next line is the message content.

  if (lastLine.startsWith("RING"))                                   // New incoming call
     {
       Serial.println(lastLine);
       nextValidLineIsCall = true;
     }
  else if ((lastLine.length() > 0) && (nextValidLineIsCall))        // Rejects any empty line
          {
    //LastLineIsCLIP();
          }
  else if (lastLine.startsWith("+CMT:"))                           // New incoming SMS
          {
             Serial.println(lastLine);
             phonenum = lastLine.substring((lastLine.indexOf(34) + 1),   //Parser to extract phonenumber
                       lastLine.indexOf(34, lastLine.indexOf(34) + 1));
             nextLineIsMessage = true;

             // Parsing lastLine to determine registration on SIM card
             firstComma = lastLine.indexOf(',');
             secondComma = lastLine.indexOf(',', firstComma + 1); 
          }
  else if ((lastLine.length() > 0) && (nextLineIsMessage))       // Rejects any empty line
          {
            LastLineIsCMT();
          }
  CleanCurrentLine();
}
/////////////////////////////////////////////////////////
void CleanCurrentLine()
{
  //Clear char array for next line of read
  for ( int i = 0; i < sizeof(currentLine);  ++i )
      {
        currentLine[i] = (char)0;
      }
  currentLineIndex = 0;
}
