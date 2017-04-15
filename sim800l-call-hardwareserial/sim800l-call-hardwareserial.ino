/*
  Author: Dhanish
  Created on: 19-08-2014
  Company: Elementz Engineers Guild Pvt Ltd
  Run this code only after SIM800 Module has powered and connected to a Network.
  Please make a call to the module knowing whether the connection is established.
  If connected a ring will be heard at the caller end
*/

int onModulePin = 13;
int8_t answer;
void setup() {

  pinMode(onModulePin, OUTPUT);
  // initialize serial communications at 115200 bps:
  Serial.begin(115200);
  Serial.println("Starting...");
  power_on();

  delay(3000);

  Serial.println("Connecting to the network...");
  
  //while (sendATcommand2("AT+CREG?", "+CREG: 0,1", "+CREG: 0,5", 2000) == 0);
  answer = contacsOnSim("AT+CPBR=1,15", "OK", 1000);
  Serial.print(answer);
  delay(10000);

  //while ( (sendATcommand("AT+CREG?", "+CREG: 0,1", 500) || sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0 );

  //  delay(10000);
  //  // print the serial data to GSM
  //  Serial.print("\r\nATD04168262667;\r\n" ); // change here to call a number using SIM800
  //  // wait 10 seconds before the next loop
  //  delay(10000);
  //  Serial.print("\r\nATH\r\n" ); // hold the call
}

void loop() {
}
///////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////
int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout) {

  uint8_t x = 0,  answer = 0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);    // Initialize the string

  delay(100);

  //while ( Serial.available() > 0) Serial.read();   // Clean the input buffer

  while (Serial.available()) { //Cleans the input buffer
    Serial.read();
  }

  Serial.println(ATcommand);    // Send the AT command


  x = 0;
  previous = millis();

  // this loop waits for the answer
  do {
    if (Serial.available() != 0) {
      // if there are data in the UART input buffer, reads it and checks for the asnwer
      response[x] = Serial.read();
      x++;
      // check if the desired answer  is in the response of the module
      if (strstr(response, expected_answer) != NULL)
      {
        answer = 1;
      }
    }
    // Waits for the asnwer with time out
  } while ((answer == 0) && ((millis() - previous) < timeout));

  return answer;
}
//////////////////////////////////////////////////////////////////////////////////////////
int8_t sendATcommand2(char* ATcommand, char* expected_answer1, char* expected_answer2, unsigned int timeout) {

  uint8_t x = 0,  answer = 0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);    // Initialize the string

  delay(100);

  //while ( Serial.available() > 0) Serial.read();   // Clean the input buffer
  
  while (Serial.available()) { //Cleans the input buffer
    Serial.read();
  }

  Serial.println(ATcommand);    // Send the AT command


  x = 0;
  previous = millis();

  // this loop waits for the answer
  do {
    if (Serial.available() != 0) {
      response[x] = Serial.read();
      x++;
      // check if the desired answer 1 is in the response of the module
      if (strstr(response, expected_answer1) != NULL)
      {
        answer = 1;
      }
      // check if the desired answer 2 is in the response of the module
      if (strstr(response, expected_answer2) != NULL)
      {
        answer = 2;
      }
    }
    // Waits for the asnwer with time out
  } while ((answer == 0) && ((millis() - previous) < timeout));

  return answer;
}
//////////////////////////////////////////////////////////////////////////////////////////
int8_t contacsOnSim(char* ATcommand, char* expected_answer, unsigned int timeout) {

  uint8_t x = 0,  answer = 0;
  char response[512];
  unsigned long previous;

  memset(response, '\0', 512);    // Initialize the string

  delay(100);

  //while ( Serial.available() > 0) Serial.read();   // Clean the input buffer

  while (Serial.available()) { //Cleans the input buffer
    Serial.read();
  }

  Serial.println(ATcommand);    // Send the AT command


  x = 0;
  previous = millis();

  // this loop waits for the answer
  while (strstr(response, expected_answer) == NULL)
  {
    if (Serial.available() != 0) {
      char c = Serial.read();
      response[x] = c;
      x++;
      if (c == ':'){
       answer = answer + 1;
      }
    }
  }
  
//  do {
//    if (Serial.available() != 0) {
//      // if there are data in the UART input buffer, reads it and checks for the asnwer
//      response[x] = Serial.read();
//      x++;
//      // check if the desired answer  is in the response of the module
//      if (strstr(response, expected_answer) != NULL)
//      {
//        answer = 1;
//      }
//    }
//    // Waits for the asnwer with time out
//  } while ((answer == 0) && ((millis() - previous) < timeout));

  return answer;
}
