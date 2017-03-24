//---------Start-Libraries Section----------------------------------------------------//

#include <gprs.h>
#include <SoftwareSerial.h>

//--------------------------------End-Libraries Section----------------------------------------------------//


//--------------------------------Start-Definition Section--------------------------------------------------//

#define TIMEOUT    60000
#define LED_PIN    13
//#define TIMEOUTINTERNET 30000

//--------------------------------End-Definition Section--------------------------------------------------//


//--------------------------------Start-Object Creation--------------------------------------------------//

GPRS gprs;

//--------------------------------End-Object Creation--------------------------------------------------//


//--------------------------------Start-Variable Declaration-------------------------------------------//


// Integer indexes

int j = -1;
int i = -1;
int kk = -1;
int OldCounter = -1;
int NewCounter = -1;

// Temporal variable when manipulating char arrays
//char tmp = "";

//String BuildString = "";

char response [350];
//char whitelist [200];     // Might ge bigger if phone number is huge.

// List for commands to send to SIM800l
//String commandlist[10];


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
  //digitalWrite(LED_PIN, ledStatus);

  Serial.println("Starting SIM800 Http Development");
  gprs.preInit();
  delay(1000);

  while (0 != gprs.init())
  {
    delay(1000);
    Serial.print("init error\r\n");
  }

  Serial.println("Init success");
}
void loop() {
  // put your main code here, to run repeatedly:

  GetWhiteList();
  Serial.println("Added...");
  delay(180000);

}

///////////////////////////////////////////////////////////////////////////
void GetWhiteList()
{
  Serial.println("On GetWhiteList...");
  const int MAX = 6;
  char const * const commandlist[] =
  {
    //command 1  Sets GPRS Context
    "AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n",
    //command 2  Sets APN
    "AT+SAPBR=3,1,\"APN\",\"internet.movistar.ve\"\r\n",
    //command 3  Connects to internet
    "AT+SAPBR=1,1\r\n",
    //command 4  Gets DHCP IP given by provider
    "AT+SAPBR=2,1\r\n",
    //command5   Establishes HTTP session
    "AT+HTTPINIT\r\n",
    //command 6 Defines Cloud
    "AT+HTTPPARA=\"URL\",\"www.castillolk.com.ve/WhiteList.txt\"\r\n",
  };

  for ( i = 0; i < MAX; i++)
  {
    Serial.println("Actual command:");
    Serial.println(commandlist[i]);
    if (0 != gprs.sendCmdAndWaitForResp(commandlist[i], "OK", TIMEOUT)) //Expects OK
    {
      ERROR("ERROR:");
      Serial.println("There was a network problem. System will restart, please wait...");
      RestartSystem();
      delay(TIMEOUT); // Waits for system to restart
    }
    Serial.println("Passed!");
  }

    //GET response command.
    Serial.println("Actual command:");
    Serial.println("AT+HTTPACTION=0\r\n");
    if (0 != gprs.sendCmdAndWaitForResp("AT+HTTPACTION=0\r\n", "200", TIMEOUT)) //Expects 200
    {
      ERROR("ERROR:");
      Serial.println("There was a network problem. System will restart, please wait...");
      RestartSystem();
      delay(TIMEOUT); // Waits for system to restart
    }
    Serial.println("Passed!");

    gprs.sendCmd("AT+HTTPREAD\r\n");
    Serial.print("Actual Command: ");
    Serial.println("AT+HTTPREAD\r\n");
    gprs.cleanBuffer(response, sizeof(response));
    readData(response, sizeof(response), 60000);      
    ///gprs.readBuffer(response, sizeof(response));
    Serial.print("Printing response:");
    Serial.println(response);
}

////////////////////////////////////////////////////////////////////////////////////////////////
void RestartSystem()
{
  if (0 != gprs.sendCmdAndWaitForResp("AT+CPOWD=1\r\n", "NORMAL POWER DOWN", TIMEOUT))
  {
    ERROR("ERROR:CPOWD");
    //return;
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////
int readData(char *buffer, int count, unsigned int timeOut)
{
  Serial.println("On readData...");
  int sw1;
  unsigned long timerStart, timerEnd;
  timerStart = millis();
  while (1)
  {
    sw1 = 0;
    while (gprs.serialSIM800.available())
    {
      char c = gprs.serialSIM800.read();
      if (c == '#') {
        sw1 = sw1 + 1;
        Buscar();
        break;
      }
    }
    if (sw1 == 1) {
      break;
    }
    timerEnd = millis();
    if (timerEnd - timerStart > 1000 * timeOut) {
      break;
    }
  }
  delay(500);
  while (gprs.serialSIM800.available()) {
    gprs.serialSIM800.read();
  }
  return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////
void Buscar()
{
  Serial.println("On Buscar...");
  memset(response, '\0', sizeof(response));  // Initialize the char array as string
  i = 0;
  char c = gprs.serialSIM800.read();
  ////tmp = tmp + c;
  response[i] = c;
  i = i + 1;
  c = gprs.serialSIM800.read();
  ////tmp = tmp + c;
  response[i] = c;
  i = i + 1;
  response[i] = '\0';
  ///OldCounter = tmp.toInt();
  OldCounter = atoi(response);
  Serial.println("OldCounter: ");
  Serial.println(OldCounter);
  i = 0;
  c = gprs.serialSIM800.read();
  ///tmp = tmp + c;
  response[i] = c;
  i = i + 1;
  c = gprs.serialSIM800.read();
  ////tmp = tmp + c;
  response[i] = c;
  i = i + 1;
  response[i] = '\0';
  NewCounter = atoi(response);
  Serial.println("NewCounter: ");
  Serial.println(NewCounter);
  ////NewCounter = tmp.toInt();
//  i = 0;
//  j = 1;
//  while ( j <= NewCounter)
//  {
//    jj = 1;
//    tmp = '';
//    while (jj <= 11)
//    {
//      c = gprs.serialSIM800.read();
//      tmp = tmp + c;
//      jj = jj + 1;
//    }
//    PhoneNumber[j] = c;
//    j = j + 1;
//  }
//  sw1 = 1;
}
