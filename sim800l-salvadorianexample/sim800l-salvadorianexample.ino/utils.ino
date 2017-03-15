// Limpia el buffer de datos
void clearBuffer() {
  for(int i=0;i<BUFFSIZE;i++) {
    buffer[i] = 0;
  }
  buffSize = 0;
}

//Agrega un caracter al bufer para comparar luego
void appendToBuffer(char c) {
  if(buffSize<BUFFSIZE) {
    buffer[buffSize++] = c;
  } else {
    Serial.print(buffer);
    clearBuffer();
    buffer[buffSize++] = c;
  }
}

// Envia datos por el SoftSerial
// lentamente
void pushSlow(char* command,int charaterDelay,int endLineDelay) {
  for(int i=0; i<strlen(command); i++) {
    mySerial.write(command[i]);
    if(command[i]=='\n') {
      delay(endLineDelay);
    } else {
      delay(charaterDelay);
    }
  }
}

// Espera por una cadena de texto sobre el serial
bool waitForString(char* string, long waitForMilliseconds) {
  clearBuffer();
  long startTime = millis();
  boolean found = false;
  Serial.print("Waiting for string: \"");
  Serial.print(string);
  Serial.print("\"\r\n");
  while((millis()-startTime)<waitForMilliseconds) {
    if (mySerial.available()) {
      chr = mySerial.read();
      Serial.write((char)chr);
      appendToBuffer((char)chr);
      if((char)chr=='\n') {
        if(strstr(buffer,string)!=NULL) {
          found = true;
          break;
        } else if(strstr(buffer,"ERROR")!=NULL) {
          found = false;
          break;
        }
        clearBuffer();
      }
    }
    if (Serial.available())
      mySerial.write(Serial.read());
  }
  return found;
}

bool waitForString(char *string, int waitForMilliseconds) {
  return waitForString(string,(long)waitForMilliseconds);
}


void waitForData(int wait_delay) {
  //Espera por datos en el puerto serial por
  // el timpo especificado
  long startTime = millis();
  while((millis()-startTime)<wait_delay) {
    if (mySerial.available())
      Serial.write(mySerial.read());
    if (Serial.available()) {
      mySerial.write(Serial.read());
    }
  }
}

void bridge() {
  if (mySerial.available())
    Serial.write(mySerial.read());
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
}
