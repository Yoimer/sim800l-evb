/*  
 *  GPRS/GSM Quadband Module (SIM900)
 *  
 *  Copyright (C) Libelium Comunicaciones Distribuidas S.L. 
 *  http://www.libelium.com 
 *  
 *  This program is free software: you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation, either version 3 of the License, or 
 *  (at your option) any later version. 
 *  
 *  This program is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License 
 *  along with this program.  If not, see http://www.gnu.org/licenses/. 
 *  
 *  Version:           1.0
 *  Design:            David Gascón 
 *  Implementation:    Luis Miguel Martí
 */


// #define HEATING 9
// #define COOLING 10

#define LED 13

//Temperature control
//int cnt; //counter
//float tdegrees; //
//int sensorPin = A0; // input for the thermistor
//int sensorValue = 0; // voltage value from voltage divider

int8_t answer;
int x;
int onModulePin = 2;
char SMS[200];
char aux_string[30];
char phone_number[] = "*********"; //phone number to send SMS
char received[200];
char message1[] = {"LED ON"};
char message2[] = {"LED OFF"};
//char message3[] = {"HEATING ON"};
//char message4[] = {"HEATING OFF"};
//char message5[] = {"TEMP"};
//char message6[] = {"Temp: "};
//char coma[] = {","};
//char deg[] = {" Celsius degrees"};


void setup()   {
  
  pinMode(onModulePin, OUTPUT);
  Serial.begin(115200);

  Serial.println("Starting...");
  power_on();
  delay(3000);
  memset(SMS, 0, sizeof(SMS));
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  Serial.println("Setting SMS mode...");
  sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
  delay(500);
  sendATcommand("AT+CNMI=3,2,0,0", "OK", 1000); //configure new message reception

  digitalWrite(LED, HIGH);

}


void loop()
{
  //wait for the start character
  if (Serial.read() == '+') {
    //read all data
    for (byte i = 0; i < 200; i++) {
      delay(1);
      received[i] = Serial.read();
    }
	
	
	 byte i = 0;
      //SMS comes after quotes ('\n') so discard all bytes until find it
     while (received[i] != '\n') i++;
     i++;
	 byte z = 0;
	 while (char(received[i]) != '\n') {
      SMS[z] = received[i];
      Serial.print(SMS[z]);
	  i++;
      z++;
	 }
	 
	Serial.println(); //prints space
    byte message = checkSMS();

    switch (message) {

      case 0:
        Serial.println("Unknown message!!");
        break;

      case 1:
        Serial.println("LED on!");
        digitalWrite(LED, HIGH);
        break;

      case 2:
        Serial.println("LED off!");
        digitalWrite(LED, LOW);
        break;
    break;

    }
  }

}


void power_on() {

  uint8_t answer = 0;

  // checks if the module is started
  answer = sendATcommand("AT", "OK", 2000);
  if (answer == 0)
  {
    // power on pulse
    digitalWrite(onModulePin, HIGH);
    delay(3000);
    digitalWrite(onModulePin, LOW);

    // waits for an answer from the module
    while (answer == 0) {   // Send AT every two seconds and wait for the answer
      answer = sendATcommand("AT", "OK", 2000);
    }
  }

}

int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout) {

  uint8_t x = 0,  answer = 0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);    // Initialice the string

  delay(100);

  while ( Serial.available() > 0) Serial.read();   // Clean the input buffer

  Serial.println(ATcommand);    // Send the AT command


  x = 0;
  previous = millis();

  // this loop waits for the answer
  do {
    // if there are data in the UART input buffer, reads it and checks for the asnwer
    if (Serial.available() != 0) {
      response[x] = Serial.read();
      x++;
      // check if the desired answer is in the response of the module
      if (strstr(response, expected_answer) != NULL)
      {
        answer = 1;
      }
    }
    // Waits for the asnwer with time out
  } while ((answer == 0) && ((millis() - previous) < timeout));

  return answer;
}

//this function compares the SMS received with messages declared to know what to do
byte checkSMS() {

  for (byte i = 0; i < sizeof(message1) - 1; i++) {
    if (message1[i] == SMS[i]) {
      if (i == sizeof(message1) - 2) return 1;
    } else i = sizeof(message1);
  }

  for (byte i = 0; i < sizeof(message2) - 1; i++) {
    if (message2[i] == SMS[i]) {
      if (i == sizeof(message2) - 2) return 2;
    } else i = sizeof(message2);
  }

  return 0;

}
