/*
* Ultrasonic Sensor HC-SR04 and Arduino Tutorial
*
* Crated by Dejan Nedelkovski,
* www.HowToMechatronics.com
*
*/

#include <SoftwareSerial.h>
 
//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 8
 
//SIM800 RX is connected to Arduino D7
#define SIM800_RX_PIN 7
 
// create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);

#define TIMEOUT 30000

// defines pins numbers
const int trigPin = 9;
const int echoPin = 10;

// defines variables
long duration;
int distance;
int onModulePin = 13;
int8_t answer;
bool isIncontact                                                        = false;
bool isAuthorized                                                       = false;
int x;
unsigned long xprevious;
char currentLine[500]                                                   = "";
int currentLineIndex                                                    = 0;
bool nextLineIsMessage                                                  = false;
bool nextValidLineIsCall                                                = false;
String PhoneCallingIndex                                                = "";
String PhoneCalling                                                     = "";
String OldPhoneCalling                                                  = "";
String DateandHour                                                      = "";
String AnalogInput                                                      = "";
String lastLine                                                         = "";
String phonenum                                                         = "";
int firstComma                                                          = -1;
int prende                                                              =  0;
int secondComma                                                         = -1;
String Password                                                         = "";
int thirdComma                                                          = -1;
int forthComma                                                          = -1;
int fifthComma                                                          = -1;
int firstQuote                                                          = -1;
int secondQuote                                                         = -1;
int firstColon                                                          = -1;
int secondColon                                                         = -1;
int thirdColon                                                          = -1;
int swveces                                                             =  0;
int len                                                                 = -1;
int j                                                                   = -1;
int i                                                                   = -1;
int f                                                                   = -1;
int r                                                                   =  0;
bool isInPhonebook                                                      = false;
char contact[13];

void setup()
{
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    Serial.begin(9600); // Starts the serial communication

    //begin serial communication with Arduino and SIM800
    serialSIM800.begin(9600);
    delay(1000);

    Serial.println("Starting...");
    power_on();
    delay(3000);

    while( (sendATcommand("AT+CREG?\r\n", "+CREG: 0,1\r\n", 500, 0) || 
            sendATcommand("AT+CREG?\r\n", "+CREG: 0,5\r\n", 500, 0)) == 0 );

    sendATcommand("AT+CMGF=1\r\n", "OK\r\n", 5000, 0);
    sendATcommand("AT+CNMI=1,2,0,0,0\r\n", "OK\r\n", 5000, 0);
    sendATcommand("AT+CPBR=1,1\r\n", "OK\r\n", 5000, 1);
    Serial.println("Password:");
    Serial.println(Password);

    //sendSMS("04168262667", "YOIMER");

}

void loop() {

    // check movement
    //CheckUltrasoundSensor();
    if (serialSIM800.available() > 0)
    {
        char lastCharRead = serialSIM800.read();
        if (lastCharRead == '\r' || lastCharRead == '\n')
        {
            endOfLineReached();
        }
        else
        {
            currentLine[currentLineIndex++] = lastCharRead;
        }
    }
}

////////////////////////////////////////////////////
void CheckUltrasoundSensor()
{
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);

    // Calculating the distance
    distance= (duration*0.034)/(2);

    // Prints the distance on the Serial Monitor
    Serial.print("Distance is: ");
    Serial.println(distance);
}

////////////////////////////////////////////////////
int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout, int xpassword)
////////////////////////////////////////////////////
{
  uint8_t x = 0,  answer = 0;
  char response[100];
  unsigned long previous;
  memset(response, '\0', 100);
  delay(100);
  while (serialSIM800.available())
  {
    serialSIM800.read();
  }

  Serial.println(ATcommand);
  serialSIM800.write(ATcommand); // Sends the AT command

  x = 0;
  previous = millis();
  do
  {
    if (serialSIM800.available() != 0)
       {
         response[x] = serialSIM800.read();
         x++;
      if (strstr(response, expected_answer) != NULL)
         {
           answer = 1;
           String numbFromSim = String(response);
           numbFromSim = numbFromSim.substring(numbFromSim.indexOf(":"),
                                            numbFromSim.indexOf(",129,"));
           numbFromSim = numbFromSim.substring((numbFromSim.indexOf(34) + 1),
                                            numbFromSim.indexOf(34, numbFromSim.indexOf(34) + 1));
        if ( xpassword == 1)
           {
             numbFromSim = numbFromSim.substring( 0, 4);
             Password = numbFromSim ;
             return 0;
           }
        else
           {
             numbFromSim = numbFromSim.substring( 0, 11 );
           }
         }
       }
  }
  while ((answer == 0) && ((millis() - previous) < timeout));
  return answer;
}

/////////////////////////////////////////////////////////
void power_on()
{

  uint8_t answer = 0;

  Serial.println("On Power_on...");

  // check if the module is started
  answer = sendATcommand("AT\r\n", "OK\r\n", TIMEOUT, 0);
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
      answer = sendATcommand("AT\r\n", "OK\r\n", TIMEOUT, 0);
      Serial.println("Trying connection with module...");
    }
  }
}

/////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
int sendSMS(char *phone_number, char *sms_text)

{
  char aux_string[30];
  //char phone_number[] = "04168262667"; // ********* is the number to call
  //char sms_text[] = "Test-Arduino-Hello World";
  Serial.println("Setting SMS mode...");
  sendATcommand("AT+CMGF=1\r\n", "OK\r\n", 5000, 0);   // sets the SMS mode to text
  Serial.println("Sending SMS");

  sprintf(aux_string, "AT+CMGS=\"%s\"\r\n", phone_number);
  answer = sendATcommand(aux_string, ">", 20000, 0);   // send the SMS number
  if (answer == 1)
     {
       //Serial.println(sms_text);
       serialSIM800.write(sms_text);
       serialSIM800.write(0x1A);
       answer = sendATcommand("", "OK\r\n", 20000, 0);
       if (answer == 1)
          {
            Serial.println("Sent ");
          }
       else
          {
            Serial.println("error ");
          }
     }
  else
     {
       Serial.println("error ");
       Serial.println(answer, DEC);
     }
  return answer;
}

void endOfLineReached()
/////////////////////////////////////////////////////////////////////////////
{
  lastLine = String(currentLine);

  if (lastLine.startsWith("RING"))
  {
    Serial.println(lastLine);
    nextValidLineIsCall = true;
  }
  else
  {
    if ((lastLine.length() > 0) && (nextValidLineIsCall))
    {
      //LastLineIsCLIP();
    }
    else if (lastLine.startsWith("+CMT:"))
    {
      Serial.println(lastLine);
      phonenum = lastLine.substring((lastLine.indexOf(34) + 1),
                                    lastLine.indexOf(34, lastLine.indexOf(34) + 1));
      nextLineIsMessage = true;
      firstComma        = lastLine.indexOf(',');
      secondComma       = lastLine.indexOf(',', firstComma  + 1);
      thirdComma        = lastLine.indexOf(',', secondComma + 1);
      forthComma        = lastLine.indexOf(',', thirdComma  + 1);
      fifthComma        = lastLine.indexOf(',', forthComma  + 1);
      PhoneCalling      = lastLine.substring((firstComma - 12), (firstComma - 1));
      PhoneCallingIndex = lastLine.substring((firstComma + 2), (secondComma - 1));
      Serial.println(phonenum);
      //Serial.println(PhoneCalling);
      //Serial.println(PhoneCallingIndex);
      j            = PhoneCallingIndex.toInt();
      isIncontact  = false;
      isAuthorized = false;
      if (j > 0)
      {
        isIncontact = true;
        Serial.println("en phonebook");
        if (j <= 5 )
        {
          Serial.println("autorizada");
          isAuthorized = true;
        }
      }
      else
      {
      }
    }
    else if ((lastLine.length() > 0) && (nextLineIsMessage))
    {
      LastLineIsCMT();
    }
  }
  CleanCurrentLine();
}
/////////////////////////////////////////////////////////////////////

void LastLineIsCMT()
/////////////////////////////////////////////////////////////////////
{
  Serial.println(lastLine);
  clearBuffer();
  if (isIncontact)
  {
    if (lastLine.indexOf("LED ON") >= 0)
    {
      //prendeapaga(0);
    }
    else if (lastLine.indexOf("LED OFF") >= 0)
    {
      //prendeapaga(1);
    }
    else if (lastLine.indexOf("ADD") >= 0)
    {
      //DelAdd(1);
    }
    else if (lastLine.indexOf("DEL") >= 0)
    {
      //DelAdd(2);
    }
    else
    {
      clearBuffer();
    }
  }
  CleanCurrentLine();
  nextLineIsMessage = false;

}
////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void CleanCurrentLine()
{
  for ( int i = 0; i < sizeof(currentLine);  ++i )
  {
    currentLine[i] = (char)0;
  }
  currentLineIndex = 0;
}
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void clearBuffer()
/////////////////////////////////////////////////////////////////////////////
{
  byte w = 0;
  for (int i = 0; i < 10; i++)
  {
    while (serialSIM800.available() > 0)
    {
      char k = serialSIM800.read();
      w++;
      delay(1);
    }
    delay(1);
  }
}
/////////////////////////////////////////////////////////////////////////////
