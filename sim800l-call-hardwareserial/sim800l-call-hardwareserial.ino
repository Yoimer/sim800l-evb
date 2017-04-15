/*
  Author: Yoimer David Román Figueroa
  Created on: 15/04/2017 (American Format)
  Company: NextWells
  This code checks SIM800 physical connection and determines
  amount on contact on sim card
  
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

  while (sendATcommand2("AT+CREG?", "+CREG: 0,1", "+CREG: 0,5", 2000) == 0);

  answer = contacsOnSim("AT+CPBR=1,250", "OK", ':', 10000); 
  Serial.print(answer);
  delay(10000);
  /*
   * answer == 0 could be caused by (not finding expected_answer and not finding expected_char) 
   * or not having even a simple contact on sim.
   *  Even when expected_answer is not found but expected_char is,
   *  function will leave by time expiration and answer will be
   *  the amount of contacts on sim
  */
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
int8_t contacsOnSim(char* ATcommand, char* expected_answer, char* expected_char, unsigned int timeout) {

  uint8_t x = 0,  answer = 0;
  char response[1024]; //Change to 512 if problems appear. The same applies to memset
                       // The smallest this number is, less amount of contacts can be proccessed.
                       // this is due to buffer size on hardwareserial.
  unsigned long previous;

  memset(response, '\0', 1024);    // Initialize the string

  delay(100);

  //while ( Serial.available() > 0) Serial.read();   // Clean the input buffer

  while (Serial.available()) { //Cleans the input buffer
    Serial.read();
  }

  Serial.println(ATcommand);    // Send the AT command


  x = 0;
  previous = millis();

  // this loop waits for the answer
  while ((strstr(response, expected_answer) == NULL) && ((millis() - previous) < timeout))
  {
    while (Serial.available() != 0)
    {
      // if there is data in the UART input buffer, reads it and checks for the answer
      char c = Serial.read();
      response[x] = c;
      x++;
      // checks for a colon in the UART the buffer, when found; it increments answer.
      // Every contact has a colon on the whole string
      //+CPBR: 1,"04168262645",129,"1"
      //+CPBR: 2,"02126421011",129,"2"
      if (c == expected_char) // Checks :
      {
        answer = answer + 1;
      }
    }
  }
  return answer;
}
