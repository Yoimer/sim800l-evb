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

// Temporal variable when manipulating char arrays
//String tmp = "";

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
//  readData(response, sizeof(response), 60000);      
    gprs.readBuffer(response, sizeof(response));
    Serial.print("Printing response:");
    Serial.println(response);


}



//  Serial.println("Processing commandlist...");
//  commandlist[0] = "AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n";                                // Sets GPRS Context
//  commandlist[1] = "AT+SAPBR=3,1,\"APN\",\"internet.movistar.ve\"\r\n";                   // Sets APN
//  commandlist[2] = "AT+SAPBR=1,1\r\n";                                                   // Connects to internet
//  commandlist[3] = "AT+SAPBR=2,1\r\n";                                                  //  Gets DHCP IP given by provider
//  commandlist[4] = "AT+HTTPINIT\r\n";                                                  //  Establishes HTTP session
//  commandlist[5] = "AT+HTTPPARA=\"URL\",\"www.castillolk.com.ve/WhiteList.txt\"\r\n"; //  Defines Cloud
//  commandlist[6] = "AT+HTTPACTION=0\r\n"; // GET function (has to response 200)
//
//  for (i = 0; i < 7; ++i)
//  {
//
//    String actualCommand = commandlist[i];
//    //    Serial.print("Actual Command: ");
//    //    Serial.println(actualCommand.c_str());
//
//    // Start sending pre-configured commands
//    if (i < 6)
//    {
//      Serial.print("Actual Command: ");
//      Serial.println(actualCommand.c_str());
//      if (0 != gprs.sendCmdAndWaitForResp(actualCommand.c_str(), "OK", 60000)) //Expects OK
//      {
//        ERROR("ERROR:");
//        Serial.println("There was a network problem. System will restart, please wait...");
//        RestartSystem();
//
//        //return;
//      }
//      Serial.println("Passed!");
//    }
//    else
//    {
//      Serial.print("Value of i: ");
//      Serial.println(i);
//      //"AT+HTTPACTION=0\r\n"
//      Serial.print("Actual Command: ");
//      Serial.println(actualCommand.c_str());
//      if (0 != gprs.sendCmdAndWaitForResp(actualCommand.c_str(), "200", 60000))  //Expects 200
//      {
//        ERROR("ERROR:");
//        Serial.println("There was a network problem. System will restart, please wait...");
//        RestartSystem();
//        //return;
//      }
//      Serial.println("Passed!");
//    }
//  }

////delay(50000);


// Reads whole buffer and takes ONLY what you needs and saves it on response


//  gprs.sendCmd("\r\n");
//
//  gprs.sendCmd("AT+HTTPREAD\r\n");
//  Serial.print("Actual Command: ");
//  Serial.println("AT+HTTPREAD\r\n");
//  gprs.cleanBuffer(response, sizeof(response));
//  readData(response, sizeof(response), 60000);       //WORKS OK. NEVER USE DELAY ON 297
//  ///gprs.readBuffer(response, sizeof(response));   //WORKS OK NEVER USE DELAY ON 297
//  Serial.print("Printing response:");
//  Serial.println(response);
//
//  kk = i - 2;   // priinting kk
//  Serial.println("Printing kk: ");
//  Serial.println(kk);
//  Serial.println("Printing tmp: ");
//  Serial.println(tmp);
//  BuildString = String (tmp); //BuildString = String(response);
//  Serial.println( BuildString);
//  tmp = BuildString.substring(2, 4);
//  Serial.println("Printing tmp");
//  Serial.println(tmp);
//
//  // Getting part of BuildString and assign it to tmp (Sometimes might work but not always)
//  tmp = BuildString.substring(5, 7);
//  Serial.println("Printing tmp");
//  Serial.println(tmp);
//
//  // Trying to get  part of BuildString and reassign it to BuildString (It has never worked)
//  kk = i - 2;   // new
//  tmp = BuildString.substring(2, kk);  // new
//  Serial.print("Printing tmp");
//  Serial.println(tmp);
//
//  delay(50000);    // Stay here. Ignore anything else.
//}
//
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
//
//int readData(char *buffer, int count, unsigned int timeOut)
//{
//  Serial.println("On readData...");
//  //int i = 0; removed
//  i = 0; tmp = ""; // added
//  unsigned long timerStart, timerEnd;
//  timerStart = millis();
//  int sw1 = 0;
//  while (1)
//  {
//    while (gprs.serialSIM800.available())
//    {
//      char c = gprs.serialSIM800.read();
//      if (c == '#')
//      {
//        sw1 = sw1 + 1;
//      }
//      if (sw1 >= 2)
//      {
//        //buffer[i++] = c;
//        tmp = tmp + c;
//      }
//
//      if (i > count - 1 || sw1 == 3)
//      {
//        // buffer[i++] = '\0';
//        break;
//      }
//    }
//    if (i > count - 1 || sw1 == 3)
//    {
//      break;
//    }
//    timerEnd = millis();
//    if (timerEnd - timerStart > 1000 * timeOut)
//    {
//      break;
//    }
//  }
//  delay(500);
//  while (gprs.serialSIM800.available()) {  // Cleans buffer
//    gprs.serialSIM800.read();
//  }
//  return 0;
//}
