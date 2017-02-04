//Parser for name associated with a number from SIM card phonebook.
//This version only works with String sim = "+CPBR: 1,'121',129,'AtencionCliente'";
//Next Update  will take only the number associated in an standarized way, which means that any global world number should work.


// Define 
String  str = "hola"; 

//String from Serial
String sim = "+CPBR: 1,'121',129,'AtencionCliente'";

//Srtring to compare with received SMS
String compare = "";

//const int len = strq.length();

//char char_array[20];


void setup() {
  // put your setup code here, to run once:

 
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while(!Serial);

  Serial.println("Starting SIM800 SMS Command Processor");
  delay(1000);


  int value = str.indexOf('z');
  int valor = sim.indexOf("AtencionCliente");  // Starts in 20
  Serial.println(value);
  Serial.println(valor);
  if(sim.charAt(20) == 'A'){
    Serial.println("Yes");
  }

  //Parser for name associated with a number (it is not standar, works only with "+CPBR: 1,'121',129,'AtencionCliente'";)
  // Next update will take only the number in any scenario.
  // Serial output is +CPBR: 1,"121",129,"AtencionCliente"
  // It starts on A which is sim.charAt(20) (position 20 of the whole string) and finishes when there are no characters left. 
  // The carriage returns character is the end character indicator '\r'
  
  for(int i = 20; i < (sim.length()-1); ++i){
    if(sim[i] != '\r'){
      compare += sim[i];
    }else{
      break;
    }
  }

 Serial.print(compare);   
  //int secondComma = sim.indexOf(',' + firstComma);
 // Serial.println(secondComma);

}

void loop() {
  
  // put your main code here, to run repeatedly:

}
