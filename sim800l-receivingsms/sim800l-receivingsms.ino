/* This piece of code receives SMS in order to turn on or off LED attached on PIN 13 on Arduino UNO board using
 SIM800L-EVB Module.

 If phone number is not on phonebook (contacts) the system rejects anything from them.

 This action is based on a specific command (LED ON) or (LED OFF) on SMS message.

 As mentioned before it will only take that action if the number was previously saved as a contact on SIMCard. This might be done by
 inserting SIMcard on a mobile phone and adding the contact or via AT commands.

 Check out the "Add contact" using AT commands code on: (Here comes the GitHub repo with that software, but ot has not been made yet)
 This only processes national numbers (ones that don't include the +  prefix).

 For example (AT+CPBW=1,"04161587896",129,"name") where 129 means national numbers.
 It is IMPERATIVE to save the number with a name or alias and not to leave it empty.
 If by mistake someone tries something like (AT+CPBW= 1,"+584161587896",129,"name") or (AT+CPBW= 1,"04161587896",145,"name")
 it won't work at all since it has a "+" sign in the first case and the 145 integer in the second.
 The 129 integer is taken as a flag for data processing in future releases. Feel free to change the logic at
 YOU OWN RISK. KEEP THIS IN MIND PLEASE!!

*
*
*

 We will use the GPRS library from SEEEDUINO. It has a lot of functions for the SIM800L board that will make the development process easier and faster.
 You can get it from https://github.com/Seeed-Studio/Seeeduino_GPRS.git

*
*
*
 In case you don't know or remember how to implement a library on Arduino IDE,
 go on https://www.arduino.cc/en/hacking/libraries. Everything is explanied there very detailed.

*
*
*
*/



/****************************************************************Start of the code********************************************************/

//--------------------------------Start-Libraries Section----------------------------------------------------//

#include <gprs.h>
#include <SoftwareSerial.h>

//--------------------------------End-Libraries Section----------------------------------------------------//


//--------------------------------Start-Definition Section--------------------------------------------------//

#define TIMEOUT    5000
#define LED_PIN    13

//--------------------------------End-Definition Section--------------------------------------------------//


//--------------------------------Start-Object Creation--------------------------------------------------//

GPRS gprs;

//--------------------------------End-Object Creation--------------------------------------------------//


//--------------------------------Start-Variable Declaration-------------------------------------------//

//Initial values of variables processing any character from serial

////Variable to hold last line of serial output from SIM800
char currentLine[500] = "";
int currentLineIndex = 0;

////Boolean to be set to true if message notificaion was found and next
////line of serial output is the actual SMS message content
bool nextLineIsMessage = false;

//Boolean to be set to true if message has "LED ON" on its content
bool ledStatus = LOW;

//Integers value to determine whether the number who sends SMS belongs the PhoneBook entry previously set.
int firstComma = -1;
int secondComma = -1;

//Boolean to be set to true if number is found on phonebook
bool isInPhonebook = false;

////char phonebook[13];

char *number = NULL;

//--------------------------------End-Variable Declaration-------------------------------------------//


//--------------------------------Start-Setup section-----------------------------------------------//

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // initialize digital pin 13 as an output.
  pinMode(LED_PIN, OUTPUT);


  Serial.println("Starting SIM800 SMS Command Processor");
  gprs.preInit();
  delay(1000);

  while (0 != gprs.init()) {
    delay(1000);
    Serial.print("init error\r\n");
  }

  //Set SMS mode to ASCII
  if (0 != gprs.sendCmdAndWaitForResp("AT+CMGF=1\r\n", "OK", TIMEOUT)) {
    ERROR("ERROR:CMGF");
    return;
  }

  //Start listening to New SMS Message Indications
  if (0 != gprs.sendCmdAndWaitForResp("AT+CNMI=1,2,0,0,0\r\n", "OK", TIMEOUT)) {
    ERROR("ERROR:CNMI");
    return;
  }

  Serial.println("Init success");
  Serial.println(ledStatus);

}

//--------------------------------End-Setup Section----------------------------------------------//


////Variable to hold last line of serial output from SIM800
//char currentLine[500] = "";
//int currentLineIndex = 0;
//
////Boolean to be set to true if message notificaion was found and next
////line of serial output is the actual SMS message content
//bool nextLineIsMessage = false;
//
//
//// Integers value to determine whether the number who sends SMS belongs the PhoneBook entry previously set.
//int firstComma = -1;
//int secondComma = -1;
//
////Boolean to be set to true if number is found on phonebook
//bool isInPhonebook = false;

//--------------------------------Start-Loop Section--------------------------------------------//

void loop() {
  //Write current status to LED pin
  digitalWrite(LED_PIN, ledStatus);
  static char phonebook[13];
  int firstQuote = -1;
  int secondQuote = -1;
  int j = 0;
  ////char *number = NULL;

  //If there is serial output from SIM800
  if (gprs.serialSIM800.available()) {
    char lastCharRead = gprs.serialSIM800.read();
    //Read each character from serial output until \r or \n is reached (which denotes end of line)
    if (lastCharRead == '\r' || lastCharRead == '\n') {
      String lastLine = String(currentLine);

      //If last line read +CMT, New SMS Message Indications was received.
      //Hence, next line is the message content.
      if (lastLine.startsWith("+CMT:")) {

       firstComma = lastLine.indexOf(',');
       Serial.println(firstComma);  //For debugging
       secondComma = lastLine.indexOf(',', firstComma + 1);
       Serial.println(secondComma); //For debugging

       firstQuote = lastLine.indexOf(34); //ASCII code for ""
//       Serial.println(firstQuote);  //For debugging
       secondQuote = lastLine.indexOf(34, firstQuote + 1);
//       Serial.println(secondQuote); //For debugging
//
       for (int i = firstQuote+1; i < secondQuote; ++i) {
        phonebook[j] = lastLine[i];
          ++j;
        }
       phonebook[j] = '\0';
       Serial.println(phonebook);
       number = phonebook;
       //gprs.sendSMS(number, "Hi"); Works OK (THANK YOU GOD!)
       //Serial.println(number); // This messes the received message. It guess it is cos its way or printing out

        /*If secondComma position is greater than 22 it means that the number was previously entered on PhoneBook
           When not; secondComma is just 22.

           Example:
           +CMT: "04161587896","Yoimer","17/02/11,16:41:41-16"  secondComma position is 28
           +CMT: "04161587896","","17/02/11,16:41:41-16"        secondComma position is 22 which means this number is not on SIMCard PhoneBook
         */

        // If exists on Phonebook
        if (secondComma > 22) {
          Serial.println("In Phonebook"); //For debugging
          isInPhonebook = true;
          Serial.println(isInPhonebook);
        } else {
          Serial.println("Not in Phonebook"); //For debugging
        }

        Serial.println(lastLine);
        nextLineIsMessage = true;

      } else if ((lastLine.length() > 0) && (isInPhonebook)) {
        if (nextLineIsMessage) {
          Serial.println(lastLine);

          //Read message content and set status according to SMS content
          if (lastLine.indexOf("LED ON") >= 0) {
            ledStatus = 1;
            Serial.println(phonebook);
            gprs.sendSMS(number, "Command Successfully Applied"); //define phone number and text
          } else if (lastLine.indexOf("LED OFF") >= 0) {
            ledStatus = 0;
          }

          nextLineIsMessage = false;
          isInPhonebook = false;
        }

      }/*else{
          Serial.println("Not Allowed");
        }*/

      //Clear char array for next line of read
      for ( int i = 0; i < sizeof(currentLine);  ++i ) {
        currentLine[i] = (char)0;
      }
      currentLineIndex = 0;
    } else {
      currentLine[currentLineIndex++] = lastCharRead;
    }
  }
}

//--------------------------------End-Function Section-------------------------------------------//


/****************************************************************End of the code********************************************************/
