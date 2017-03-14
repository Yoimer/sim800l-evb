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

  Serial.println("Starting SIM800 SMS-Call Command Processor");
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

 
  //Set call notification
  if (0 != gprs.sendCmdAndWaitForResp("AT+CLIP=1\r\n", "OK", TIMEOUT))
  {
    ERROR("ERROR:CLIP");
    return;
  }

  //Set SMS mode to ASCII
  if (0 != gprs.sendCmdAndWaitForResp("AT+CMGF=1\r\n", "OK", TIMEOUT))
  {
    ERROR("ERROR:CMGF");
    return;
  }

  //Start listening to New SMS Message Indications
  if (0 != gprs.sendCmdAndWaitForResp("AT+CNMI=1,2,0,0,0\r\n", "OK", TIMEOUT))
  {
    ERROR("ERROR:CNMI");
    return;
  }

  //AT+SAPBR=3,1,"Contype", "GPRS"
  //AT+CPBW=1,"04168262667",129,"Yoimer"
   //String jj;
   //int j;
  //jj = j;
  //tmp = "AT+CPBW=" + jj + ",\"" + tmp + "\",129,\"" + jj + "\"\r\n";
  
  // f (0 != gprs.sendCmdAndWaitForResp(tmp.c_str(), "OK", TIMEOUT))

//   int numb1 = 3;
//   int numb2 = 1;
//   String n1;
//   String n2;
//   n1 = numb1;
//   n2 = numb2;
//   tmp = "AT+SAPBR=" + n1 + "," + n2 + "," +  "\"Contype\"" + "," + " " + "\"GPRS\"" + "\r\n";
//   Serial.println(tmp);
//   
//   if (0 != gprs.sendCmdAndWaitForResp(tmp.c_str(), "OK", TIMEOUT))
//    {
//      ERROR("ERROR:SAPBR");
//      return;
//    }
//
//   //AT+SAPBR=3,1,"APN","int.movilnet.com.ve"
//
//   tmp = "AT+SAPBR=" + n1 + "," + n2 + "," + "\"APN\"" + "," + "\"internet.movistar.ve\"" + "\r\n"; 
//
//   Serial.println(tmp);
//
//   if (0 != gprs.sendCmdAndWaitForResp(tmp.c_str(), "OK", TIMEOUT))
//    {
//      ERROR("ERROR:SAPBR");
//      return;
//    }
//
//   //AT+SAPBR=1,1
//   numb1 = 1;
//   numb2 = 1;
//   n1 = numb1;
//   n2 = numb2;
//   tmp = "AT+SAPBR=" + n1 + "," + n2 + "\r\n";
//   Serial.println(tmp);
//   if (0 != gprs.sendCmdAndWaitForResp(tmp.c_str(), "OK", TIMEOUTINTERNET))
//   {
//      ERROR("ERROR:SAPBR");
//      return;
//   }
//
// 
    
   
  Serial.println("Init success");

}

//--------------------------------End-Setup Section----------------------------------------------//


//--------------------------------Start loop Section----------------------------------------------//


void loop()
{
  //Write current status to LED pin
  digitalWrite(LED_PIN, ledStatus);

  //If there is serial output from SIM800
  if (gprs.serialSIM800.available())
  {
    char lastCharRead = gprs.serialSIM800.read();

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

//--------------------------------End-loop Section----------------------------------------------//


//--------------------------------Start Functions Section----------------------------------------------//

////////////////////////////////////////////////////////////////////////////////////////////////////
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
    LastLineIsCLIP();
  }
  else if (lastLine.startsWith("+CMT:"))                           // New incoming SMS
  {
    Serial.println(lastLine);
    nextLineIsMessage = true;

    // Parsing lastLine to determine registration on SIM card
    firstComma = lastLine.indexOf(',');
    Serial.println(firstComma);  //For debugging
    secondComma = lastLine.indexOf(',', firstComma + 1);
    Serial.println(secondComma); //For debugging

    //If secondComma position is greater than 22,
    //it means that the number was previously entered on PhoneBook
    // When not; secondComma is just 22.

    //Example:
    //+CMT: "04161587896","Yoimer","17/02/11,16:41:41-16"  secondComma position is 28



    // HERE GOES the extraction of mobile number code
    ExtractPhoneNumber();
    //Serial.println(number);


  }
  else if ((lastLine.length() > 0) && (nextLineIsMessage))       // Rejects any empty line
  {
    //LastLineIsCLIP();
    LastLineIsCMT();
    //Serial.println("Ready to process");
  }

  CleanCurrentLine();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void LastLineIsCLIP()
{
  if (nextValidLineIsCall)
  {
    Serial.println(lastLine);

    // Parsing lastLine to determine registration on SIM card
    firstComma = lastLine.indexOf(',');
    Serial.println(firstComma);  //For debugging
    secondComma = lastLine.indexOf(',', firstComma + 1);
    Serial.println(secondComma); //For debugging
    thirdComma = lastLine.indexOf(',', secondComma + 1);
    Serial.println(thirdComma);  //For debugging
    forthComma = lastLine.indexOf(',', thirdComma + 1);
    Serial.println(forthComma); //For debugging
    fifthComma = lastLine.indexOf(',', forthComma + 1);
    Serial.println(fifthComma); //For debugging

    //Extracts contact
    j = 0;
    for (int i = forthComma + 1; i < fifthComma; ++i) {
      contact[j] = lastLine[i];
      ++j;
    }
    contact[j] = '\0'; // Contact as a full string
    Serial.println(contact); //For Debugging

    len = strlen(contact); //lenght of contact string
    Serial.println(len);  // For Debugging

    // HERE GOES the extraction of mobile number code

    ExtractPhoneNumber();
    //Serial.println(number);

    // If exists on contact
    if (len > 2)
    {
      Serial.println("In contact"); //For debugging
      isIncontact = true;
      Serial.println(isIncontact);
    }
    else
    {
      Serial.println("Not in contact"); //For debugging
      isIncontact = false;
    }

    // If registered turns off led on pin 13.
    //If not, just do nothing. (In a later release the action of turning off the led will notify the caller via SMS
    // that the action was committed succesfully)

    if (isIncontact)
    {
      ledStatus = 0;
      digitalWrite(LED_PIN, ledStatus);
    }

    CleanContactArray();
    nextValidLineIsCall = false;
  }

}

//////////////////////////////////////////////////////////////////////////////////////////////

void CleanCurrentLine()
{
  //Clear char array for next line of read
  for ( int i = 0; i < sizeof(currentLine);  ++i )
  {
    currentLine[i] = (char)0;
  }
  currentLineIndex = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////

void CleanContactArray()
{
  //Clear char array for next reading
  for ( int i = 0; i < sizeof(contact);  ++i )
  {
    contact[i] = (char)0;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
void LastLineIsCMT()
{
  if (nextLineIsMessage)
  {
    Serial.println(lastLine);


    // If exists on Phonebook
    if (secondComma > 22)
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
        //Serial.println(number);
        ////gprs.sendSMS(number, "LED has been turned ON"); //define phone number and text BUG +58 Movistar
        ////CleanPhoneNumber();
      }
      else if (lastLine.indexOf("LED OFF") >= 0)
      {
        ledStatus = 0;  // Turns OFF LED
        ////gprs.sendSMS(number, "LED has been turned OFF"); //define phone number and text BUG +58 Movistar
        ////CleanPhoneNumber();
      }
      else if (lastLine.indexOf("#WhiteList") >= 0)
      {
        // Go to WhiteList Routine
        Serial.println("Go to WhiteList Routine");
        //LoadWhiteList();
      }
    }

    // If SMS contains LED ON or LED OFF or #WhiteList
    //if (lastLine.indexOf("LED ON") >= 0)
    //{
    //ledStatus = 1;   // Turns ON LED
    //Serial.println(number);
    //gprs.sendSMS(number,"LED has been turned ON"); //define phone number and text
    //}
    //else if (lastLine.indexOf("LED OFF") >= 0)
    //{
    // ledStatus = 0;  // Turns OFF LED
    // gprs.sendSMS(number,"LED has been turned OFF"); //define phone number and text
    //}
    //else if (lastLine.indexOf("#WhiteList"))
    //{
    // Go to WhiteList Routine
    //Serial.println("Go to WhiteList Routine");
    //}

    CleanCurrentLine();
    nextLineIsMessage = false;

  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
void ExtractPhoneNumber()
{
  // Parse phone number
  firstQuote = lastLine.indexOf(34); // ASCII character for quote "
  Serial.println(firstQuote);  //For debugging
  secondQuote = lastLine.indexOf(34, firstQuote + 1);
  Serial.println(secondQuote); //For debugging

  // Extracts phone number
  j = 0;
  for (int i = firstQuote + 1; i < secondQuote; ++i)
  {
    phonenumber[j] = lastLine[i];
    ++j;
  }

  phonenumber[j] = '\0'; // phone number as a full string
  number = phonenumber;

  ////Serial.println(number); //For Debugging // PHONENUMBER HAS TO BE CLEANED LATER
}

//////////////////////////////////////////////////////////////////////////////////////////////
void CleanPhoneNumber()
{
  //Clear char array for next line of read
  for ( int i = 0; i < sizeof(phonenumber);  ++i )
  {
    phonenumber[i] = (char)0;
  }

  number = NULL;

}

//////////////////////////////////////////////////////////////////////////////////////////////
void LoadWhiteList()
{
  // *************************
  // *************************
  // ojo  colocar  las intrucciones get  del sim800l

  //HttpClient client;

  String BuildString = "";   //Initialize string

  ////////////// Here GOES HTTP GET ROUTINE ////////////

  ConnectToInternet();


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

  BuildString = "03,05,04168262667,04275860622,04285860622,04295860622,04305860622,####";  //Deletes 3 contacts

  String jj   = "";
  tmp = BuildString.substring(0, 2); // Saves 2 in tmp (Old number of contacts in SIM)
  Serial.println(tmp);
  OldCounter  = tmp.toInt();         // Converts String to Integer
  //Serial.println(OldCounter);
  tmp = BuildString.substring(3, 5); // Saves 5 in temp (New number of contacts in SIM)
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
void ClearWhiteList()
{
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
    Serial.println(tmp);
    j = j + 1;
  }
}

void ConnectToInternet()
{
  tmp = "AT+SAPBR=3,1,'Contype', 'GPRS'";
  if (0 != gprs.sendCmdAndWaitForResp(tmp.c_str(), "OK", TIMEOUT))
  {
    ERROR("ERROR:SAPBR");
    return;
  }
  // AT+SAPBR=3,1,"Contype", "GPRS"
}


//--------------------------------End Functions Section----------------------------------------------//


