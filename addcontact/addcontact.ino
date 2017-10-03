/*
	Recibe y procesa SMS basados en strings predefinidos
	incluyendo una contraseña para tomar acciones de control,
	registro y eliminación de usuarios.
	Solo los 5 primeros contactos en el simcard
	pueden agregar y eliminar usuarios.
	Solo lo usuarios registrados en la "whitelist"
	pueden tomar acciones de control.
	Cualquier SMS distinto a los strings predefinidos
	no serán tomados en cuenta por el sistema
	Para control el SMS debe ser:
	LED ON,0416, si el el número guardado en posición 1 del simcard
	empieza por 0416
	LED OFF,0416, si el el número guardado en posición 1 del simcard
	empieza por 0416
	Para registro y eliminación de usuarios:
	Registro
	ADD,posicion a guardar en sim,numero a guardar,
	Ejemplo: ADD,6,04168262667,
	DEL,posicion a borrar,
	Ejemplo: DEL,85,
*/

int8_t answer;
bool isIncontact                          = false;
bool isAuthorized                         = false;
int x;
unsigned long xprevious;
char currentLine[500]                     = "";
int currentLineIndex                      = 0;
bool nextLineIsMessage                    = false;
bool nextValidLineIsCall                  = false;
String PhoneCallingIndex                  = "";
String PhoneCalling                       = "";
String OldPhoneCalling                    = "";
String lastLine                           = "";
String phonenum                           = "";
int firstComma                            = -1;
int prende                                =  0;
int secondComma                           = -1;
String Password                           = "";
String indexAndName                       = "";
String newContact                         = "";
String trama                              = "";
int SMSerror                                 = -1;
int thirdComma                            = -1;
int forthComma                            = -1;
int fifthComma                            = -1;
int firstQuote                            = -1;
int secondQuote                           = -1;
int swveces                               = 0;
int len                                   = -1;
int j                                     = -1;
int i                                     = -1;
int f                                     = -1;
int r                                     = 0;
bool isInPhonebook = false;
char contact[13];
char phone[21]; // a global buffer to hold phone number
char message[100];
int onModulePin = 13;

void setup() {

  pinMode(onModulePin, OUTPUT);
  // initialize serial communications at 115200 bps:
  Serial.begin(115200);
  Serial.println("Starting...");
  // Comprueba conexión con red celular
  while ( (sendATcommand("AT+CREG?", "+CREG: 0,1", 5000, 0) ||
           sendATcommand("AT+CREG?", "+CREG: 0,5", 5000, 0)) == 0 );

  // Configura lectura de SMS como caracteres ASCII
  sendATcommand("AT+CMGF=1", "OK", 5000, 0);
  
  // Configura lectura de SMS solo en el buffer serial
  // y no guarda los SMS en la SIMcard
  sendATcommand("AT+CNMI=1,2,0,0,0", "OK", 5000, 0);
  
  // Lee primer contacto guardado previamente en el SIMcard
  // para ser ultilizado como contraseña en los SMS
  sendATcommand("AT+CPBR=1,1", "OK\r\n", 5000, 1);
  
  Serial.println("Password:");
  
  // Imprime la contraseña en la consola
  Serial.println(Password);
  
}

void loop() {
}

//**********************************************************

// Función que comprueba conexión física con el SIM800L
void power_on()
{
  uint8_t answer = 0;
  answer = sendATcommand("AT", "OK", 5000, 0);
  if (answer == 0)
  {
    while (answer == 0)
    {
      answer = sendATcommand("AT", "OK", 2000, 0);
    }
  }
}

//**********************************************************

// Función que envía comandos AT al SIM800L
// Cuando int xpassword tiene el valor de 0 no consulta contraseña
// Cuando int xpassword tiene el valor de 1 consulta contraseña

int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout, int xpassword)
{
  uint8_t x = 0,  answer = 0;
  char response[100];
  unsigned long previous;
  memset(response, '\0', 100);
  delay(100);
  while (Serial.available())
  {
    Serial.read();
  }
  Serial.println(ATcommand);
  x = 0;
  previous = millis();
  do
  {
    if (Serial.available() != 0)
    {
      response[x] = Serial.read();
      x++;
      if (strstr(response, expected_answer) != NULL)
      {
        answer = 1;
        String numbFromSim = String(response);
        numbFromSim = numbFromSim.substring(numbFromSim.indexOf(":"),
                                            numbFromSim.indexOf(",129,"));
        numbFromSim = numbFromSim.substring((numbFromSim.indexOf(34) + 1),
                                            numbFromSim.indexOf(34, numbFromSim.indexOf(34) + 1));
        if ( xpassword == 1)
        {
          numbFromSim = numbFromSim.substring( 0, 4);
          Password = numbFromSim ;
          return 0;
        }
        else
        {
          numbFromSim = numbFromSim.substring( 0, 11 );
        }
      }
    }

  }
  while ((answer == 0) && ((millis() - previous) < timeout));
  return answer;
}
