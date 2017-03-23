#include "gprs.h"
#include <softwareserial.h>
#define TIMEOUT    5000
String phonenum;
char phone[21];  // a global buffer to hold phone number
bool ledStatus;
uint8_t LED1 = 4;
uint8_t LED2 = 5;
uint8_t LED3 = 6;
uint8_t LED4 = 7;
uint8_t LED5 = 8;
uint8_t LED6 = 9;
uint8_t LED7 = 10;
uint8_t LED8 = 11;
GPRS gprs;
void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);
  pinMode(LED7, OUTPUT);
  pinMode(LED8, OUTPUT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Starting SIM800 SMS Command Processor");
  gprs.preInit();
  delay(1000);
  while (0 != gprs.init()) {
    delay(1000);
    Serial.print("init error\r\n");
  }
  if (0 != gprs.sendCmdAndWaitForResp("AT+CMGF=1\r\n", "OK", TIMEOUT)) {
    ERROR("ERROR:CNMI");
    return;
  }
  if (0 != gprs.sendCmdAndWaitForResp("AT+CNMI=1,2,0,0,0\r\n", "OK", TIMEOUT)) {
    ERROR("ERROR:CNMI");
    return;
  }
  Serial.println("Init success");
}
char currentLine[50] = "";
int currentLineIndex = 0;
bool nextLineIsMessage = false;
void loop() {
  if (gprs.serialSIM800.available()) {
    char lastCharRead = gprs.serialSIM800.read();
    if (lastCharRead == '\r' || lastCharRead == '\n') {
      String lastLine = String(currentLine);
      if (lastLine.startsWith("+CMT:")) {
        Serial.println(lastLine);
        String phonenum = lastLine.substring(7, 19);
        Serial.println(phonenum);
        phonenum.toCharArray(phone, 21);  // copy String object directly to global buffer
        nextLineIsMessage = true;
      }
      else if (lastLine.length() > 0)
      {
        if (nextLineIsMessage)
        {
          Serial.println(lastLine);
          if (lastLine.indexOf("on1") >= 0)
          {
            digitalWrite(LED1, 1);
            gprs.sendSMS(phone, "Device 1 is now ON"); // phone out of scope
          }
          else if (lastLine.indexOf("off1") >= 0) {
            digitalWrite(LED1, 0);
            gprs.sendSMS(phone, "Device 1 is now OFF"); // phone out of scope
          }
          if (lastLine.indexOf("on2") >= 0) {
            digitalWrite(LED2, 1);
            gprs.sendSMS(phone, "Device 2 is now ON");
          }
          else if (lastLine.indexOf("off2") >= 0) {
            digitalWrite(LED2, 0);
            gprs.sendSMS(phone, "Device 2 is now OFF");
          }
          if (lastLine.indexOf("on3") >= 0) {
            digitalWrite(LED3, 1);
            gprs.sendSMS(phone, "Device 3 is now ON");
          }
          else if (lastLine.indexOf("off3") >= 0) {
            digitalWrite(LED3, 0);
            gprs.sendSMS(phone, "Device 3 is now OFF");
          }
          if (lastLine.indexOf("on4") >= 0) {
            digitalWrite(LED4, 1);
            gprs.sendSMS(phone, "Device 4 is now ON");
          }
          else if (lastLine.indexOf("off4") >= 0) {
            digitalWrite(LED4, 0);
            gprs.sendSMS(phone, "Device 4 is now OFF");
          }
          if (lastLine.indexOf("on5") >= 0) {
            digitalWrite(LED5, 1);
            gprs.sendSMS(phone, "Device 5 is now ON");
          }
          else if (lastLine.indexOf("off5") >= 0) {
            digitalWrite(LED5, 0);
            gprs.sendSMS(phone, "Device 5 is now OFF");
          }
          if (lastLine.indexOf("on6") >= 0) {
            digitalWrite(LED6, 1);
            gprs.sendSMS(phone, "Device 6 is now ON");
          }
          else if (lastLine.indexOf("off6") >= 0) {
            digitalWrite(LED6, 0);
            gprs.sendSMS(phone, "Device 6 is now OFF");
          }
          if (lastLine.indexOf("on7") >= 0) {
            digitalWrite(LED7, 1);
            gprs.sendSMS(phone, "Device 7 is now ON");
          } else if (lastLine.indexOf("off7") >= 0) {
            digitalWrite(LED7, 0);
            gprs.sendSMS(phone, "Device 7 is now OFF");
          }
          if (lastLine.indexOf("on8") >= 0) {
            digitalWrite(LED8, 1);
            gprs.sendSMS(phone, "Device 8 is now ON");
          }
          else if (lastLine.indexOf("off8") >= 0) {
            digitalWrite(LED8, 0);
            gprs.sendSMS(phone, "Device 8 is now OFF");
          }
          nextLineIsMessage = false;
        }
      }
      for ( int i = 0; i < sizeof(currentLine);  ++i ) {
        currentLine[i] = (char)0;
      }
      currentLineIndex = 0;
    } else {
      currentLine[currentLineIndex++] = lastCharRead;
    }
  }
}
 
