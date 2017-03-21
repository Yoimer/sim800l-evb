//---------Start-Libraries Section----------------------------------------------------//

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
//char currentLine[500] = "";
int currentLineIndex = 0;

//Boolean to be set to true if number is found on contact
bool isIncontact = false;

// Variable to hold contact from call
//char contact[13]; // changes this if needed (a contact name with more than 13 characters)

// Variable to hold number from call
//char phonenumber[13];

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
int firstDollar = -1;
int secondDollar = -1;
int thirdDollar = -1;
int forthDollar = -1;
int fifthDollar = -1;
int firstPound = -1;
int secondQuote = -1;
int len = -1;
int j = -1;
int i = -1;
int f = -1;
int r = -1;

// Contact Counters
int OldCounter = 0;
int NewCounter = 0;

// Temporal variable when using LoadWhiteList() ClearWhiteList() whitelist
String tmp = "";

// Where new phonenumbers will get saved
String BuildString = "";

char response [300];
//char whitelist [200];     // Might ge bigger if phone number is huge.

// List for commands to send to SIM800l
String commandlist[10];


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

  ////////////////////////////////////////////////////////////////////////////
  //  Serial.println("Processing commandlist...");
  //  commandlist[0] = "AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n";                                // Sets GPRS Context
  //  commandlist[1] = "AT+SAPBR=3,1,\"APN\",\"internet.movistar.ve\"\r\n";                   // Sets APN
  //  commandlist[2] = "AT+SAPBR=1,1\r\n";                                                   // Connects to internet
  //  commandlist[3] = "AT+SAPBR=2,1\r\n";                                                  //  Gets DHCP IP given by provider
  //  commandlist[4] = "AT+HTTPINIT\r\n";                                                  //  Establishes HTTP session
  //  commandlist[5] = "AT+HTTPPARA=\"URL\",\"www.castillolk.com.ve/WhiteList.txt\"\r\n"; //  Defines Cloud
  //  //commandlist[6] = "AT+HTTPACTION=0\r\n"; // GET function (has to response 200)
  //
  //  for (i = 0; i < 6; ++i)
  //  {
  //
  //    String actualCommand = commandlist[i];
  //    Serial.print("Actual Command: ");
  //    Serial.println(actualCommand.c_str());
  //
  //    // Start sending pre-configured commands
  //    if (0 != gprs.sendCmdAndWaitForResp(actualCommand.c_str(), "OK", 50000))
  //    {
  //      ERROR("ERROR:");
  //      return;
  //    }
  //    Serial.println("Passed!");
  //  }
  //
  //  // Reads whole buffer and takes ONLY what you needs and saves it on tmp
  //
  //  /////HTTP-GET///////////////////////////
  //  gprs.sendCmd("AT+HTTPACTION=0\r\n");                                         //HTTP GET
  //  Serial.print("Actual Command: ");
  //  Serial.println("AT+HTTPACTION=0\r\n");
  //  Serial.println("Getting data. It takes approximately 90 seconds, please wait...");
  //  delay(60000);                                            //If data gets incomplete, increase this value
  //  gprs.cleanBuffer(response, sizeof(response));
  //  gprs.readBuffer(response, sizeof(response));
  //
  //  for (i = 0; i < sizeof(response); ++i )
  //  {
  //    if (response[i] == ':')
  //    {
  //      j = i;
  //      //while (response[j] != '\r' &&  response[j] != '\n')
  //      while (response[j] != '$')
  //      {
  //        tmp += response[j];
  //        ++j;
  //      }
  //      Serial.println("Getting out of for loop...");
  //      Serial.print("Value of tmp: ");
  //      Serial.println(tmp);
  //      break;                                         // Exits for loop. There is no longer need to keep iterating
  //    }
  //  }
  //
  //  //Data received Error Checking
  //  if (tmp.indexOf("200") >= 0)     //string you are expecting
  //  {
  //    Serial.println("Data Succesfully Received!");
  //  }
  //  else
  //  {
  //    Serial.println("There was a network error");
  //    gprs.sendCmd("AT+CPOWD=1\r\n"); // Normal power off
  //    gprs.cleanBuffer(response, sizeof(response));
  //    gprs.readBuffer(response, sizeof(response));
  //    Serial.println("System will restart, please wait...");
  //    Serial.println(response);
  //    delay(25000);
  //  }
  //
  //  gprs.sendCmd("AT+HTTPREAD\r\n");
  //  Serial.print("Actual Command: ");
  //  Serial.println("AT+HTTPREAD\r\n");
  //  gprs.cleanBuffer(response, sizeof(response));
  //  gprs.readBuffer(response, sizeof(response));
  //
  ////  Serial.println("Printing response");
  ////  Serial.println(response);
  //
  //  tmp = "";   // Cleans old value of tmp
  //  for (i = 0; i < sizeof(response); ++i )
  //  {
  //    if (response[i] == ',')
  //    {
  //      j = i;
  //      j +=1;
  //      //while (response[j] != '\r' &&  response[j] != '\n')
  //      while (response[j] != '#')
  //      {
  //        tmp += response[j];
  //        ++j;
  //      }
  //      Serial.println("Getting out of for loop...");
  //      Serial.print("Value of tmp: ");
  //      Serial.println(tmp);
  //      break;                                         // Exits for loop. There is no longer need to keep iterating
  //    }
  //
  //    BuildString = tmp;                               //Assigns new phonenumbers to BuildString
  //  }


}
void loop() {
  // put your main code here, to run repeatedly:

  GetWhiteList();
  LoadWhiteList();
  Serial.println("Added...");
  delay(180000);


}

///////////////////////////////////////////////////////////////////////////
void GetWhiteList()
{
  Serial.println("Processing commandlist...");
  commandlist[0] = "AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n";                                // Sets GPRS Context
  commandlist[1] = "AT+SAPBR=3,1,\"APN\",\"internet.movistar.ve\"\r\n";                   // Sets APN
  commandlist[2] = "AT+SAPBR=1,1\r\n";                                                   // Connects to internet
  commandlist[3] = "AT+SAPBR=2,1\r\n";                                                  //  Gets DHCP IP given by provider
  commandlist[4] = "AT+HTTPINIT\r\n";                                                  //  Establishes HTTP session
  commandlist[5] = "AT+HTTPPARA=\"URL\",\"www.castillolk.com.ve/WhiteList.txt\"\r\n"; //  Defines Cloud
  commandlist[6] = "AT+HTTPACTION=0\r\n"; // GET function (has to response 200)

  for (i = 0; i < 7; ++i)
  {

    String actualCommand = commandlist[i];
//    Serial.print("Actual Command: ");
//    Serial.println(actualCommand.c_str());

    // Start sending pre-configured commands
    if (i < 6)
    {
      Serial.print("Actual Command: ");
      Serial.println(actualCommand.c_str());
      if (0 != gprs.sendCmdAndWaitForResp(actualCommand.c_str(), "OK", 50000)) //Expects OK
      {
        ERROR("ERROR:");
        return;
      }
      Serial.println("Passed!");
    }
    else
    {
      Serial.print("Value of i: ");
      Serial.println(i);
      //"AT+HTTPACTION=0\r\n"
      Serial.print("Actual Command: ");
      Serial.println(actualCommand.c_str());
      if (0 != gprs.sendCmdAndWaitForResp(actualCommand.c_str(), "200", 50000))  //Expects 200
      {
        ERROR("ERROR:");
        return;
      }
      Serial.println("Passed!");
    }
  }

  delay(50000);


  // Reads whole buffer and takes ONLY what you needs and saves it on tmp

//  /////HTTP-GET///////////////////////////
//  gprs.sendCmd("AT+HTTPACTION=0\r\n");                                         //HTTP GET
//  Serial.print("Actual Command: ");
//  Serial.println("AT+HTTPACTION=0\r\n");
//  Serial.println("Getting data. It takes approximately 90 seconds, please wait...");
//  delay(60000);                                            //If data gets incomplete, increase this value
//  gprs.cleanBuffer(response, sizeof(response));
//  gprs.readBuffer(response, sizeof(response));
//
//  for (i = 0; i < sizeof(response); ++i )
//  {
//    if (response[i] == ':')
//    {
//      j = i;
//      //while (response[j] != '\r' &&  response[j] != '\n')
//      while (response[j] != '$')
//      {
//        tmp += response[j];
//        ++j;
//      }
//      Serial.println("Getting out of for loop...");
//      Serial.print("Value of tmp: ");
//      Serial.println(tmp);
//      break;                                         // Exits for loop. There is no longer need to keep iterating
//    }
//  }
//
//  //Data received Error Checking
//  if (tmp.indexOf("200") >= 0)     //string you are expecting
//  {
//    Serial.println("Data Succesfully Received!");
//  }
//  else
//  {
//    Serial.println("There was a network error");
//    gprs.sendCmd("AT+CPOWD=1\r\n"); // Normal power off
//    gprs.cleanBuffer(response, sizeof(response));
//    gprs.readBuffer(response, sizeof(response));
//    Serial.println("System will restart, please wait...");
//    Serial.println(response);
//    delay(25000);
//  }

  gprs.sendCmd("AT+HTTPREAD\r\n");
  Serial.print("Actual Command: ");
  Serial.println("AT+HTTPREAD\r\n");
  gprs.cleanBuffer(response, sizeof(response));
  gprs.readBuffer(response, sizeof(response));

  //  Serial.println("Printing response");
  //  Serial.println(response);

  tmp = "";   // Cleans old value of tmp
  for (i = 0; i < sizeof(response); ++i )
  {
    if (response[i] == ',')
    {
      j = i;
      j += 1;
      //while (response[j] != '\r' &&  response[j] != '\n')
      while (response[j] != '#')
      {
        //tmp += response[j];
        BuildString += response[j];
        ++j;
      }

      Serial.println("Getting out of for loop...");
      Serial.print("Value of BuildString: ");
      Serial.println(BuildString);
      break;                                         // Exits for loop. There is no longer need to keep iterating
    }

  }

}

///////////////////////////////////////////////////////////////////////////
void ClearWhiteList()
{
  Serial.println("On ClearWhiteList()");

  String jj = "";
  j = 1 ;     // lleva la cuenta de los nros a borrar
  while ( j <= OldCounter)
  {
    jj = j;
    tmp   = "AT+CPBW=" + jj + "\r\n";
    if (0 != gprs.sendCmdAndWaitForResp(tmp.c_str(), "OK", TIMEOUT))
    {
      ERROR("ERROR:CPBW");
      return;
    }
    Serial.println(tmp);       // comando AT a ejecutar ??
    j = j + 1;
  }
}

////////////////////////////////////////////////////////////////////
void LoadWhiteList()
{
  Serial.println("On LoadWhiteList()");

  // *************************
  // *************************
  // ojo  colocar  las intrucciones get  del sim800l

  //HttpClient client;

  //  String BuildString = "";   //Initialize string
  //  BuildString = tmp;

  ////////////// Here GOES HTTP GET ROUTINE ////////////

  //client.get("http://castillolk.com.ve/WhiteList.txt");
  //
  // ejemplo WhiteList.txt
  // 10,05,04265860622,04275860622,04285860622,04295860622,04305860622,####
  //
  //while (client.available())
  //{
  //char      c = client.read();
  //BuildString = BuildString + c;
  //}

  ////////////// Here GOES NEW IMPLEMENTATION ////////////

  // This is what I will receive from "http://castillolk.com.ve/WhiteList.txt"
  // and will save in BuildString 10,05,04265860622,04275860622,04285860622,04295860622,04305860622,####

  ////BuildString = "03,05,04168262667,04275860622,04285860622,04295860622,04305860622,####";  //Deletes 3 contacts
  //05,10,04168262667,02126426913,04285860622,04295860622,04295899999,04168262667,02126426913,04285860622,04295860622,04295899999,

  String jj   = "";
  tmp = BuildString.substring(0, 2); // Saves Old number of contacts in SIM in tmp
  Serial.println(tmp);
  OldCounter  = tmp.toInt();         // Converts String to Integer
  //Serial.println(OldCounter);
  tmp = BuildString.substring(3, 5); // Saves New number of contacts in SIM
  NewCounter = tmp.toInt();          // Converts String to Integer
  ClearWhiteList();

  ////////////// Here Adds New Contacts ////////////

  f = 6;         // aqui comienzan los nros de telefono
  j = 1;         // lleva la cuenta de los nros a cargar

  while (j <= NewCounter)
  {
    r = f + 11; //  nros son de largo 11 ejm 04265860622
    tmp = BuildString.substring(f, r);
    jj = j;
    tmp   = "AT+CPBW=" + jj + ",\"" + tmp + "\",129,\"" + jj + "\"\r\n";
    if (0 != gprs.sendCmdAndWaitForResp(tmp.c_str(), "OK", TIMEOUT))
    {
      ERROR("ERROR:CPBW");
      return;
    }
    Serial.println(tmp);
    f = f + 12; //  12 para saltar la coma ,
    j = j + 1;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////

