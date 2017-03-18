/* This piece of code receives phonecalls in order to turn off LED attached on PIN 13 on Arduino UNO board,
   Turns ON or OFF PIN 13 via SMS if previously registered on SIM Card,
   and adds a new contact on SIM Card via SMS command and establishing a HTTP connection
   to www.castillolk.com.ve/WhiteList.txt using SIM800L-EVB Module.

  If phone number is not on contact (contacts) the system rejects anything from them.

  if on contact turns off LED.

  //////////////////////////QUITAR
  Check out how to add a contact on the SIM Card as mentioned in the Spanish version of manual
  "InstalaciÃ¯Â¿Â½n de software Control de Encendido y Apagado por SMS .Prueba de Concepto del SIM800L-EVB en Arduino UNO_Finalizado"
  on page 2 from https://drive.google.com/folderview?id=0BxtBNyHdFnkkUnl0RGt0RTFOZzQ
  This only processes national numbers (ones that don't include the +  prefix).

  For example (AT+CPBW=1,"04161587896",129,"name") where 129 means national numbers.
  It is IMPERATIVE to save the number with a name or alias and not to leave it empty.
  If by mistake someone tries something like (AT+CPBW= 1,"+584161587896",129,"name") or (AT+CPBW= 1,"04161587896",145,"name")
  it won't work at all since it has a "+" sign in the first case and the 145 integer in the second.
  //////////////////////////QUITAR



  We will use the GPRS library from SEEEDUINO. It has a lot of functions for the SIM800L board that will make the development process easier and faster.
  You can get it from https://github.com/Seeed-Studio/Seeeduino_GPRS.git

  In case you don't know or remember how to implement a library on Arduino IDE,
  go on https://www.arduino.cc/en/hacking/libraries. Everything is explanied there very detailed.

*/


/****************************************************************Start of the code********************************************************/



//--------------------------------Start-Libraries Section----------------------------------------------------//

#include <gprs.h>
#include <SoftwareSerial.h>



//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 8

//SIM800 RX is connected to Arduino D7
#define SIM800_RX_PIN 7

//Create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN, SIM800_RX_PIN);


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
//char currentLine[250] = "";
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
bool nextLine = false;

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
String tmp2 = "";

int out = false;

char sim800buffer[300];
String commandlist[10];
String actualCommand = "";
String process = "";


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

  Serial.println("Starting SIM800 ConnectToInternet Function");
  gprs.preInit();
  delay(1000);

  while (0 != gprs.init())
  {
    delay(1000);
    Serial.print("init error\r\n");
  }


  //Check Call Availability
  if (0 != gprs.sendCmdAndWaitForResp("AT+CCALR?\r\n", "1", TIMEOUT))
  {
    ERROR("ERROR:CCALR");
    return;
  }

  ProcessCommandList();


  //  Serial.println("Processing commandlist...");
  //  commandlist[0] = "AT+SAPBR=3,1,\"ConType\",\"GPRS\"\r\n";
  //  commandlist[1] = "AT+SAPBR=3,1,\"APN\",\"internet.movistar.ve\"\r\n";
  //  commandlist[2] = "AT+SAPBR=1,1\r\n";
  //  commandlist[3] = "AT+SAPBR=2,1\r\n";
  //  commandlist[4] = "AT+HTTPINIT\r\n";
  //  commandlist[5] = "AT+HTTPPARA=\"URL\",\"www.castillolk.com.ve/WhiteList.txt\"\r\n";
  //  commandlist[6] = "AT+HTTPACTION=0\r\n";
  //
  //  for (i = 0; i < 7; ++i)
  //  {
  //    Serial.print("Value of commandlist[i]: ");
  //    Serial.println(commandlist[i]);
  //    String actualCommand = commandlist[i];
  //    Serial.print("Value of actualComand: ");
  //    Serial.println(actualCommand);
  //    if (0 != gprs.sendCmdAndWaitForResp(actualCommand.c_str(), "OK", 50000))
  //    {
  //      ERROR("ERROR:");
  //      return;
  //    }
  //    Serial.println("Passed!");
  //
  //  }
  //  Serial.println("End of commandlist...");


  //
  //
  //  Serial.println("AT+SAPBR=3,1,\"ConType\",\"GPRS\"\r\n");
  //  if (0 != gprs.sendCmdAndWaitForResp("AT+SAPBR=3,1,\"ConType\",\"GPRS\"\r\n", "OK", TIMEOUT))
  //  {
  //    ERROR("ERROR:SAPBR-1");
  //    return;
  //  }
  //  Serial.println("1-Passed!");
  //
  //  Serial.println("AT+SAPBR=3,1,\"APN\",\"internet.movistar.ve\"\r\n");
  //  if (0 != gprs.sendCmdAndWaitForResp("AT+SAPBR=3,1,\"APN\",\"internet.movistar.ve\"\r\n", "OK", TIMEOUT))
  //  {
  //    ERROR("ERROR:SAPBR-2");
  //    return;
  //  }
  //  Serial.println("2-Passed!");
  //
  //  Serial.println("AT+SAPBR=1,1\r\n");
  //  if (0 != gprs.sendCmdAndWaitForResp("AT+SAPBR=1,1\r\n", "OK", 50000)) //("AT+CMGF=1\r\n", "OK", TIMEOUT))
  //  {
  //    ERROR("ERROR:SAPBR-3");
  //    return;
  //  }
  //  Serial.println("3-Passed!");
  //
  //  Serial.println("Printing AT...");
  //  //gprs.sendCmd("AT\r\n");  //"AT+SAPBR=1,1\r\n"
  //  gprs.sendCmd("AT+SAPBR=2,1\r\n");
  //  ///
  //  int x = 0;
  //  while (gprs.serialSIM800.available())
  //  {
  //    //     Serial.println("On infinite loop...");
  //    delay(1500);
  //    if (gprs.serialSIM800.available())
  //    {
  //      sim800buffer[x] = gprs.serialSIM800.read();
  //      //      char c = gprs.serialSIM800.read();
  //      //      int numb = c;
  //      //Serial.println(c);
  //      Serial.println(sim800buffer[x]);
  //      ++x;
  //    }
  //  }
  //
  //  Serial.println("Out of loop");
  //  String process = String(sim800buffer);
  //  Serial.println(process);
  //  if (process.indexOf("OK") >= 0)
  //  {
  //    Serial.println("YESSS!!");
  //  }
  //  delay(50000);



  //Check Call Availability
  if (0 != gprs.sendCmdAndWaitForResp("AT+CCALR?\r\n", "1", TIMEOUT))
  {
    ERROR("ERROR:CCALR");
    return;
  }


  //  //Set call notification
  //  if (0 != gprs.sendCmdAndWaitForResp("AT+CLIP=1\r\n", "OK", TIMEOUT))
  //  {
  //    ERROR("ERROR:CLIP");
  //    return;
  //  }
  //
  //  //Set SMS mode to ASCII
  //  if (0 != gprs.sendCmdAndWaitForResp("AT+CMGF=1\r\n", "OK", TIMEOUT))
  //  {
  //    ERROR("ERROR:CMGF");
  //    return;
  //  }
  //
  //  //Start listening to New SMS Message Indications
  //  if (0 != gprs.sendCmdAndWaitForResp("AT+CNMI=1,2,0,0,0\r\n", "OK", TIMEOUT))
  //  {
  //    ERROR("ERROR:CNMI");
  //    return;
  //  }

  Serial.println("Init success");

}

//--------------------------------End-Setup Section----------------------------------------------//


//--------------------------------Start loop Section----------------------------------------------//


void loop()
{

  ConnectToInternet();
}


//--------------------------------End-loop Section----------------------------------------------//

////////////////////////////////////////////////////////////////////
void ConnectToInternet()
{

}

////////////////////////////////////////////////////////////////////
void ProcessCommandList()
{
  Serial.println("Processing commandlist...");
  commandlist[0] = "AT+SAPBR=3,1,\"ConType\",\"GPRS\"\r\n";                                // Sets GPRS Context
  commandlist[1] = "AT+SAPBR=3,1,\"APN\",\"internet.movistar.ve\"\r\n";                   // Sets APN
  commandlist[2] = "AT+SAPBR=1,1\r\n";                                                   // Connects to internet
  commandlist[3] = "AT+SAPBR=2,1\r\n";                                                  //  Gets DHCP IP given by provider
  commandlist[4] = "AT+HTTPINIT\r\n";                                                  //  Establishes HTTP session
  commandlist[5] = "AT+HTTPPARA=\"URL\",\"www.castillolk.com.ve/WhiteList.txt\"\r\n"; //   Connects to Cloud
  commandlist[6] = "AT+HTTPACTION=0\r\n";                                             // GET function (has to response 200)

  for (i = 0; i < 7; ++i)
  {

    //    if(i == 3)
    //    {
    //      Serial.println("Going to CharByCharProcessesor function...");
    //      CharByCharProcessesor();
    //    }
    //Serial.print("Value of commandlist[i]: ");
    //Serial.println(commandlist[i]);
    actualCommand = commandlist[i];
    if (i == 6)
    {
      Serial.println("Going to CharByCharProcessesor function...");
      CharByCharProcessesor();
    }
    Serial.print("Value of actualComand: ");
    Serial.println(actualCommand);
    // Sends all the commands (saves a lottttt of memory)
    if (0 != gprs.sendCmdAndWaitForResp(actualCommand.c_str(), "OK", 50000))
    {
      ERROR("ERROR:");
      return;
    }
    Serial.println("Passed!");
  }
  Serial.println("End of commandlist...");
}

////////////////////////////////////////////////////////////////////
void CharByCharProcessesor()
{
  Serial.println("Printing actualCommand on CharByCharProcessesor...");
  Serial.println(actualCommand);
  Serial.println("Starting char processing...");
  gprs.sendCmd(actualCommand.c_str());
  ///
  int x = 0;
  while (gprs.serialSIM800.available())
  {
    //     Serial.println("On infinite loop...");
    delay(2500);
    if (gprs.serialSIM800.available())
    {
      char c = gprs.serialSIM800.read();
      sim800buffer[x] = c;
      Serial.println(sim800buffer[x]);
      ++x;




      //       if (c == ':')
      //       {
      //         Serial.println(": char passed!");


      //         while(c != '\r')
      //         {
      //          Serial.println("Starting process String...");
      //          process = process + c;
      //          char c = gprs.serialSIM800.read();
      //          Serial.print(process);
      //         }
      //int processFlag =
      //process += c;
    }
    //sim800buffer[x] = gprs.serialSIM800.read();
    //Serial.println(sim800buffer[x]);
    //++x;
    //    }
  }

  Serial.println("Out of loop");
  String process = String(sim800buffer);
  Serial.println(process);
  //  if (process.indexOf("OK") >= 0)
  //  {
  //    Serial.println("YESSS!!");
  //  }
}


////////////////////////////////////////////////////////////////////
//void CleanCurrentLine()
//{
//  //Clear char array for next line of read
//  for ( int i = 0; i < sizeof(currentLine);  ++i )
//  {
//    currentLine[i] = (char)0;
//  }
//  currentLineIndex = 0;
//}
////////////////////////////////////////////////////////////////////

