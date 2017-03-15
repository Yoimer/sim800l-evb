//#define BUFFSIZE 1024
#define BUFFSIZE 768
#include <SoftwareSerial.h>
#include <string.h>
#include "enums.h"
#include "utils.h"

// Buffer de datos serial
char buffer[BUFFSIZE];
int buffSize = 0;
volatile byte chr;

// Eliminador de rebote por software
volatile long lastInt = 0;

// Definimos un SoftSerial donde conectaremos
// el modulo GSM
////SoftwareSerial mySerial(10, 11); // RX, TX  Commented


///////////////Added by me/////////////////////
//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 8
 
//SIM800 RX is connected to Arduino D7
#define SIM800_RX_PIN 7
///////////////Added by me/////////////////////
 
//Create software serial object to communicate with SIM800
SoftwareSerial mySerial(SIM800_TX_PIN,SIM800_RX_PIN);
 

// Estas enumeraciones serviran para detectar
// Si el modulo esta conectado y si esta activa
// La red de datos
gsmStatus currentStatus = OFFLINE;
gprsStatus currentGPRS = DISCONNECTED;

// Banderas para las interrupciones
// Estas variables indicaran al loop
// principal que debera de realizar una accion
volatile boolean sendMessage = false;
volatile boolean startShutDown = false;

void setup()  
{
  // Abrir el puerto serie
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  // Inicializar el SoftSerial
  mySerial.begin(9600);

  // Inicializamos la ultima interrupcion para evitar que
  // se activen al encender
  lastInt = millis();
  
  // Asignamos las interrupciones 0 y 1
  pinMode(3,INPUT_PULLUP); // Maneja el apagado del modulo GSM
  attachInterrupt(0, pin3OnFalling, FALLING);
  
  pinMode(2,INPUT_PULLUP); // Maneja la conexion al GPRS y envio de datos
  attachInterrupt(1, pin2OnFalling, FALLING);

  Serial.print("Arduino: Setup finished.\r\n");
}

void pin3OnFalling() {
  if((millis()-lastInt)>500) {
    startShutDown = true;
    lastInt = millis();
  }
}

void pin2OnFalling() {
  if((millis()-lastInt)>500) {
    sendMessage = true;
    lastInt = millis();
  }
}

void shutDown() {
  // Envia el comando de apagado
  pushSlow("\r\n");
  pushSlow("AT+CPOWD=1\r\n");
}

void shutdownGRPS() {
  // Desactiva el GPRS del modulo
  pushSlow("AT+CIPSHUT\r\n");
  waitForData(3000);
  pushSlow("AT+CGATT=0\r\n");
  waitForData(3000);
  currentGPRS = DISCONNECTED;
}

void setupGPRS() {
  // Enviamos [Enter] para limpiar cualquier
  // comando previo
  pushSlow("\r\n");
  waitForData(1000);
  // Desconectamos el GPRS
  // para evitar conflictos.
  shutdownGRPS();
  // Conectamos nuevamente
  pushSlow("AT+CGATT=1\r\n");
  if(!waitForString("OK",10000)) {
    shutdownGRPS();
    return;
  }
  //pushSlow("AT+CSTT=\"internet.tigo.sv\",\"\",\"\"\r\n");  internet.movistar.ve
  pushSlow("AT+CSTT=\"internet.movistar.ve\",\"\",\"\"\r\n");
  if(!waitForString("OK",10000)) {
    shutdownGRPS();
    return;
  }
  pushSlow("AT+CIICR\r\n"); // Habilitar conexiones Wireless
  if(!waitForString("OK",10000)) {
    shutdownGRPS();
    return;
  }
  pushSlow("AT+CIFSR\r\n"); // Obtener IP local
  waitForData(1000);
  currentGPRS = CONNECTED;
}

void GET() {
  // Enviamos [Enter] para limpiar cualquier
  // comando previo
  pushSlow("\r\n");
  waitForData(1000);
  pushSlow("AT+CIPSTART=\"TCP\",\"dev.teubi.co\",80\r\n");
  if(!waitForString("CONNECT OK",30000)) {
    return;
  }
  pushSlow("AT+CIPSEND\r\n");
  waitForData(1000);
  pushSlow("GET /hola.php HTTP/1.1\r\n");
  pushSlow("Host: dev.teubi.co\r\n");
  pushSlow("Connection: Keep-Alive\r\n");
  pushSlow("\r\n");
  pushSlow("\x1A");
  if(!waitForString("SEND OK",15000)) {
    return;
  }
  waitForData(15000);
  pushSlow("AT+CIPCLOSE\r\n");
  if(!waitForString("CLOSE OK",5000)) {
    return;
  }
}


boolean checkForSIM900() {
  // Figure out if already connected
  pushSlow("AT\r\n");
  if(!waitForString("OK",5000))
    return false;
  pushSlow("AT+CPIN?\r\n");
  if(!waitForString("+CPIN: READY",5000))
    return false;
  pushSlow("AT+CFUN?\r\n");
  if(!waitForString("+CFUN: 1",5000))
    return false;
  pushSlow("AT+CCALR?\r\n");
  if(!waitForString("+CCALR: 1",5000))
    return false;
  return true;
}

void loop() // run over and over
{
  if(currentStatus==OFFLINE) {
    // Si el estatus es OFFLINE intentar detectar
    // el modulo por dos maneras
    Serial.write("Trying to detect SIM900 module\r\n");
    // Esperando a Call ready
    if(waitForString("Call Ready",5000)) {
      Serial.write("GSM Module detected. Current status: ONLINE\r\n");
      currentStatus=ONLINE;
      // O verificando mediante comandos AT
    } else if(checkForSIM900()) {
      Serial.write("GSM Module already connected. Changing status to: ONLINE\r\n");
      currentStatus=ONLINE;
    }
  }
  if(currentStatus==ONLINE) {
    // Si esta ONLINE procesara el comando de la interrupcion
    if(sendMessage) {
      // La primera vez que reciba la interrupcion intentara conectar
      // Este codigo ignorara el envio del mensaje
      if(currentGPRS==DISCONNECTED) {
        Serial.print("GPRS is disconnected. Starting up GPRS data.\r\n");
        Serial.print("Current request is going to be ignored.\r\n");
        // try to SETUP GPRS before connect
        setupGPRS();
        sendMessage = false;
      // Una vez este conectado
      // Intentara realizar la conexion
      } else if(currentGPRS==CONNECTED) {
        Serial.print("DATA Interrupt received\r\n");
        GET();
        sendMessage = false;
      }
    }
    bridge();
  }
  if(startShutDown) {
    Serial.print("Shutdown interrupt received\r\n");
    shutDown();
    startShutDown = false;
    currentStatus = OFFLINE;
    currentGPRS = DISCONNECTED;
  }
}
