/*
    GPRS/GSM Quadband Module (SIM900)

    Copyright (C) Libelium Comunicaciones Distribuidas S.L.
    http://www.libelium.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    a
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see http://www.gnu.org/licenses/.

    Version:           2.0
    Design:            David Gascón
    Implementation:    Alejandro Gallego & Marcos Martinez
*/

#include <SoftwareSerial.h>

#define TIMEOUT  2000

//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 8

//SIM800 RX is connected to Arduino D7
#define SIM800_RX_PIN 7

//Create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN, SIM800_RX_PIN);

int8_t answer;
////int onModulePin = 2;
int onModulePin = 13;


char data[512];
int data_size;

char aux_str[100];
char aux;
int x = 0;

char pin[] = "*******";
////char apn[] = "*******";
char apn[] = "internet.movistar.ve";
char user_name[] = "*******";
char password[] = "*******";

char url[ ] = "test.libelium.com/test-get-post.php?a=1&b=2";


void setup() {

  pinMode(onModulePin, OUTPUT);

  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(115200);
  while (!Serial);

  //Being serial communication witj Arduino and SIM800
  serialSIM800.begin(115200);
  delay(1000);

  //    //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  //  Serial.begin(9600);
  //  while(!Serial);
  //
  //  //Being serial communication witj Arduino and SIM800
  //  serialSIM800.begin(9600);
  //  delay(1000);
  //
  //  Serial.println("Setup Complete!");

  Serial.println("Starting...");   //AT+SAPBR=3,1,"APN","internet.movistar.ve"
  power_on();

  delay(3000);

  //
  //  //sets the PIN code
  //  snprintf(aux_str, sizeof(aux_str), "AT+CPIN=%s", pin);
  //  sendATcommand(aux_str, "OK", 2000);
  //
  //  delay(3000);
  //
  //  while (sendATcommand2("AT+CREG?", "+CREG: 0,1", "+CREG: 0,5", 2000) == 0);
  //

  // sets APN , user name and password
  sendATcommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n", "OK\r\n", 2000);
  snprintf(aux_str, sizeof(aux_str), "AT+SAPBR=3,1,\"APN\",\"%s\"\r\n", apn);
  sendATcommand(aux_str, "OK\r\n", 2000);

  while (sendATcommand("AT+SAPBR=1,1\r\n", "OK\r\n", 60000) == 0)
  {
    Serial.println("Trying connection, please wait...");
    delay(5000);
  }


}
void loop() {
  // Initializes HTTP service
  answer = sendATcommand("AT+HTTPINIT\r\n", "OK\r\n", 10000);
}













//    // Sets CID parameter
//    answer = sendATcommand("AT+HTTPPARA=\"CID\",1", "OK", 5000);
//    if (answer == 1)
//    {
// Sets url
//
//      snprintf(aux_str, sizeof(aux_str), "AT+HTTPPARA=\"URL\",\"%s\"", url);
//
//      answer = sendATcommand(aux_str, "OK", 5000);
//      if (answer == 1)
//      {
//        // Starts GET action
//        answer = sendATcommand("AT+HTTPACTION=0", "+HTTPACTION:0,200", 10000);
//
//        if (answer == 1)
//        {
//          x = 0;
//          do {
//            sprintf(aux_str, "AT+HTTPREAD=%d,100", x);
//            if (sendATcommand2(aux_str, "+HTTPREAD:", "ERROR", 30000) == 1)
//            {
//              data_size = 0;
//              while (Serial.available() == 0);
//              aux = Serial.read();
//              do {
//                data_size *= 10;
//                data_size += (aux - 0x30);
//                while (Serial.available() == 0);
//                aux = Serial.read();
//              } while (aux != 0x0D);
//
//              Serial.print("Data received: ");
//              Serial.println(data_size);
//
//              if (data_size > 0)
//              {
//                while (Serial.available() < data_size);
//                Serial.read();
//
//                for (int y = 0; y < data_size; y++)
//                {
//                  data[x] = Serial.read();
//                  x++;
//                }
//                data[x] = '\0';
//              }
//              else
//              {
//                Serial.println("Download finished");
//              }
//            }
//            else if (answer == 2)
//            {
//              Serial.println("Error from HTTP");
//            }
//            else
//            {
//              Serial.println("No more data available");
//              data_size = 0;
//            }
//
//            sendATcommand("", "+HTTPACTION:0,200", 20000);
//          } while (data_size > 0);
//
//          Serial.print("Data received: ");
//          Serial.println(data);
//        }
//        else
//        {
//          Serial.println("Error getting the url");
//        }
//      }
//      else
//      {
//        Serial.println("Error setting the url");
//      }
//    }
//    else
//    {
//      Serial.println("Error setting the CID");
//    }
//  }
//  else
//  {
//    Serial.println("Error initializating");
//  }
//
//  sendATcommand("AT+HTTPTERM", "OK", 5000);
//
//  delay(5000);

//}

void power_on() {

  uint8_t answer = 0;

  Serial.println("On Power_on...");

  // checks if the module is started
  answer = sendATcommand("AT\r\n", "OK\r\n", TIMEOUT);
  if (answer == 0)
  {
    // power on pulse
    digitalWrite(onModulePin, HIGH);
    delay(3000);
    digitalWrite(onModulePin, LOW);

    // waits for an answer from the module
    while (answer == 0) {
      // Send AT every two seconds and wait for the answer
      answer = sendATcommand("AT\r\n", "OK\r\n", TIMEOUT);
      Serial.println("Trying connection with module...");
    }
  }
}


int8_t sendATcommand(const char* ATcommand, const char* expected_answer1, unsigned int timeout) {

  uint8_t x = 0,  answer = 0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);    // Initialize the string

  delay(100);

  //while ( Serial.available() > 0) Serial.read();   // Clean the input buffer

  //  while(Serial.available()) { //Cleans the input buffer
  //        Serial.read();
  //    }

  while (serialSIM800.available()) { //Cleans the input buffer
    serialSIM800.read();
  }



  Serial.println(ATcommand);    // Prints the AT command
  serialSIM800.write(ATcommand); // Sends the AT command


  x = 0;
  previous = millis();

  // this loop waits for the answer
  do {
    ////if (Serial.available() != 0) {
    if (serialSIM800.available() != 0) {
      ////response[x] = Serial.read();
      response[x] = serialSIM800.read();
      x++;
      // check if the desired answer is in the response of the module
      if (strstr(response, expected_answer1) != NULL)
      {
        answer = 1;
      }
    }
    // Waits for the asnwer with time out
  }
  while ((answer == 0) && ((millis() - previous) < timeout));

  return answer;
}


//
//int8_t sendATcommand2(char* ATcommand, char* expected_answer1,
//                      char* expected_answer2, unsigned int timeout) {
//
//  uint8_t x = 0,  answer = 0;
//  char response[100];
//  unsigned long previous;
//
//  memset(response, '\0', 100);    // Initialize the string
//
//  delay(100);
//
//  while ( Serial.available() > 0) Serial.read();   // Clean the input buffer
//
//  Serial.println(ATcommand);    // Send the AT command
//
//
//  x = 0;
//  previous = millis();
//
//  // this loop waits for the answer
//  do {
//    if (Serial.available() != 0) {
//      response[x] = Serial.read();
//      x++;
//      // check if the desired answer 1 is in the response of the module
//      if (strstr(response, expected_answer1) != NULL)
//      {
//        answer = 1;
//      }
//      // check if the desired answer 2 is in the response of the module
//      if ((response, expected_answer2) != NULL)
//      {
//        answer = 2;
//      }
//    }
//    // Waits for the asnwer with time out
//  } while ((answer == 0) && ((millis() - previous) < timeout));
//
//  return answer;
//}

