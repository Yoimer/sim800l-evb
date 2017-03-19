//--------------------------------Start-Libraries Section----------------------------------------------------//

#include <gprs.h>
#include <SoftwareSerial.h>

//--------------------------------End-Libraries Section----------------------------------------------------//


//--------------------------------Start-Definition Section--------------------------------------------------//

#define TIMEOUT    15000
#define LED_PIN    13
#define TIMEOUTINTERNET 30000

//--------------------------------End-Definition Section--------------------------------------------------//


//--------------------------------Start-Object Creation--------------------------------------------------//

GPRS gprs;

//--------------------------------End-Object Creation--------------------------------------------------//


//--------------------------------Start-Variable Declaration-------------------------------------------//

//Boolean to be set to false if number is on contact
bool ledStatus = HIGH;

////Variable to hold last line of serial output from SIM800
char currentLine[500] = "";
int currentLineIndex = 0;

//Boolean to be set to true if number is found on contact
bool isIncontact = false;

// Variable to hold contact from call
char contact[13]; // changes this if needed (a contact name with more than 13 characters)

// Variable to hold number from call
char phonenumber[13];

// Variable to send SMS
char *number = NULL;

////Boolean to be set to true if call notificaion was found and next line is NOT empty
//bool nextLineIsCall = false;
bool nextValidLineIsCall = false;

////Boolean to be set to true if message notificaion was found and next
////line of serial output is the actual SMS message content
bool nextLineIsMessage = false;

//Boolean to be set to true if number is found on phonebook
bool isInPhonebook = false;

// String which holds the last line read from Serial activitiy
String lastLine = "";

// Integer indexes
int firstComma = -1;
int secondComma = -1;
int thirdComma = -1;
int forthComma = -1;
int fifthComma = -1;
int firstQuote = -1;
int secondQuote = -1;
int len = -1;
int j = -1;
int i = -1;
int f = -1;
int r = -1;

// Contact Counters
int OldCounter = 0;
int NewCounter = 0;

// Temporal variable when using LoadWhiteList() ClearWhiteList()
String tmp = "";


int out = false;

char response [200];


//--------------------------------End-Variable Declaration-------------------------------------------//



//------------------------------End-Libraries Section--------------------------------------------------------//


//--------------------------------Start-Setup Section-----------------------------------------------//

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  while (!Serial);

  // initialize digital pin 13 as an output.
  pinMode(LED_PIN, OUTPUT);

  //Write current status to LED pin
  digitalWrite(LED_PIN, ledStatus);

  Serial.println("Starting SIM800 Http Development");
  gprs.preInit();
  delay(1000);

  while (0 != gprs.init())
  {
    delay(1000);
    Serial.print("init error\r\n");
  }

  Serial.println("Init success");
  number = NULL;
  Serial.print("Initial value of number: ");
  Serial.println(strlen(number));
  number = "OKAA";
  Serial.print("Actual value of number: ");
  Serial.println(strlen(number));
  gprs.sendCmd("AT\r\n");
  //  if (0 != gprs.sendCmdAndWaitForResp("AT\r\n", "OK", 50000))  //ALWAYS CLEANS BUFFERRRRRRRRRRRRRRRRRRRR
  //    {
  //      ERROR("ERROR:");
  //      return;
  //    }
  gprs.cleanBuffer(response, sizeof(response));
  gprs.readBuffer(response, sizeof(response));
  Serial.println("Printing Response");
  Serial.println(response);
  gprs.sendCmd("AT+CREG?\r\n");
  gprs.cleanBuffer(response, sizeof(response));
  gprs.readBuffer(response, sizeof(response));
  Serial.println("Printing Response");
  Serial.println(response);
  gprs.sendCmd("AT+SAPBR=3,1,\"ConType\",\"GPRS\"\r\n");
  gprs.cleanBuffer(response, sizeof(response));
  gprs.readBuffer(response, sizeof(response));
  Serial.println("Printing Response");
  Serial.println(response);
  gprs.sendCmd("AT+SAPBR=3,1,\"APN\",\"internet.movistar.ve\"\r\n");
  gprs.cleanBuffer(response, sizeof(response));
  gprs.readBuffer(response, sizeof(response));
  Serial.println("Printing Response");
  Serial.println(response);
  if (0 != gprs.sendCmdAndWaitForResp("AT+SAPBR=1,1\r\n", "OK", 50000)) //("AT+CMGF=1\r\n", "OK", TIMEOUT))
  {
    ERROR("ERROR:SAPBR-3");
    return;
  }
  Serial.println("3-Passed!");
  gprs.sendCmd("AT+SAPBR=2,1\r\n");
  gprs.cleanBuffer(response, sizeof(response));
  gprs.readBuffer(response, sizeof(response));
  Serial.println("Printing Response");
  Serial.println(response);
  gprs.sendCmd("AT+HTTPINIT\r\n");
  gprs.cleanBuffer(response, sizeof(response));
  gprs.readBuffer(response, sizeof(response));
  Serial.println("Printing Response");
  Serial.println(response);
  gprs.sendCmd("AT+HTTPPARA=\"URL\",\"www.castillolk.com.ve/WhiteList.txt\"\r\n");
  gprs.cleanBuffer(response, sizeof(response));
  gprs.readBuffer(response, sizeof(response));
  Serial.println("Printing Response");
  Serial.println(response);
  gprs.sendCmd("AT+HTTPACTION=0\r\n");
  delay(5000);
  gprs.cleanBuffer(response, sizeof(response));
  gprs.readBuffer(response, sizeof(response));
  Serial.println("Printing Response");
  Serial.println(response);
  //"AT+HTTPREAD\r\n"
  gprs.sendCmd("AT+HTTPREAD\r\n");
  //delay(5000);
  gprs.cleanBuffer(response, sizeof(response));
  gprs.readBuffer(response, sizeof(response));
  Serial.println("Printing Response");
  Serial.println(response);
  tmp = String(response);
  Serial.println("Printing tmp...");
  Serial.println(tmp);
  //AT+HTTPTERM
  gprs.sendCmd("AT+HTTPTERM\r\n");
  gprs.cleanBuffer(response, sizeof(response));
  gprs.readBuffer(response, sizeof(response));
  Serial.println("Printing Response");
  Serial.println(response);
  //AT+SAPBR=0,1
  if (0 != gprs.sendCmdAndWaitForResp("AT+SAPBR=0,1\r\n", "OK", 50000)) //("AT+CMGF=1\r\n", "OK", TIMEOUT))
  {
    ERROR("ERROR:SAPBR-4");
    return;
  }
  Serial.println("4-Passed!");




  //"AT+HTTPPARA=\"URL\",\"www.castillolk.com.ve/WhiteList.txt\"\r\n";

  //  gprs.sendCmd("AT+SAPBR=1,1\r\n");
  //  gprs.cleanBuffer(response, sizeof(response));
  //  gprs.readBuffer(response, sizeof(response));
  //  Serial.println("Printing Response");
  //  Serial.println(response);
  //

}
void loop() {
  // put your main code here, to run repeatedly:

}
