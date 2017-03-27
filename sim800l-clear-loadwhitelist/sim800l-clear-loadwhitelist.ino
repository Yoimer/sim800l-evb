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


// Integer indexes

int j = -1;
int i = -1;
//int jj = -1;

// Contact Counters
int OldCounter = 0;
int NewCounter = 0;


const byte numChars = 512;
const byte indexRespuesta = 30;
const byte indexjj = 101;
char response [numChars];
char respuesta[indexRespuesta];

/////char jj[101];
char jj[indexjj];

//char tmp[numChars];


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
  //char jj   = 'A';
  //  Serial.println("Printing jj");
  //  Serial.println(jj);
  j = 3;
  //jj = j;
  Serial.println("Printing jj as char");  //char* someCharPointer = (char*)someInteger;
  //char jj[3]; 
  itoa(j, jj, 10);
  Serial.println(jj);
  char*tmp = join3Strings("AT+CPBW=", jj, "\r\n");
  /////char*tmp = join3Strings("AT+CPBW=", "AT+CPBW=", "AT+CPBW="); WORKS
  Serial.println("Printing tmp");
  Serial.println(tmp);
}
void loop() {
  // put your main code here, to run repeatedly:
  //  OldCounter = 5;
  //  ClearWhiteList();

}

/////////////////////////////////////////////////////////////////////
void ClearWhiteList()
{
  char jj   = "";
  //strcat(to,from)
  j = 1 ;         // lleva la cuenta de los nros a borrar  (char)0
  while ( j  <= OldCounter)
  {
    Serial.println("Deleting Contacts.");
    jj = j;
    // tmp = "AT+CPBW=" + jj + "\r\n"; numChars
    char *tmp = join3Strings("AT+CPBW=", jj, "\r\n");
    //    if (0 != gprs.sendCmdAndWaitForResp(tmp.c_str(), "OK", TIMEOUT))
    //    {
    //      ERROR("ERROR:CPBW");
    //      return;
    //    }
    Serial.println(tmp);       // comando AT a ejecutar ??
    j   = j + 1;
  }
}
/////////////////////////////////////////////////////////////////

char* join3Strings(char* string1, char* string2, char* string3)
{
  gprs.cleanBuffer(respuesta, indexRespuesta);
  strcat(respuesta, string1);
  strcat (respuesta, string2);
  strcat (respuesta, string3);
  Serial.println("respuesta:");
  Serial.println(respuesta);
  return respuesta;
}




//char bigstring[40];  // enough room for all strings together
//
//char* join3Strings(char* string1, char* string2, char* string3) {
//  bigstring[0] = 0;          // start with a null string:
//  strcat(bigstring, string1);   // add first string
//  strcat (bigstring, string2);
//  strcat (bigstring, string3);
//
//  return bigstring;
//}


//char* stringToBeJoined = makeString(userSelectionMode);
//char* lcdOutput = join3Strings("Mode ",stringToBeJoined , " selected");
//  This does work:
//  char* lcdOutput = join3Strings("Mode ", "1" , " selected");


//The itoa() stdlib C library function can be used to convert a number into a string, 
//in a variety of bases (e.g. decimal, binary). The buffer must be large enough to hold the largest number, 
//plus sign and terminating null: e.g. 32 bit base-10: "-2147483648\0" = 12 characters.
//
//The stdlib itoa() library routine adds around 600 bytes, the roll-your-own K&R implementation, 
//which includes string.h, adds around 670 bytes. Using snprintf() from stdio.h adds just over 2200 bytes.
//
//e.g.
//
//char buf[12]; // "-2147483648\0"
//lcd.printIn(itoa(random(1024)-512, buf, 10));

//too few arguments to function 'char* itoa(int, char*, int)'

