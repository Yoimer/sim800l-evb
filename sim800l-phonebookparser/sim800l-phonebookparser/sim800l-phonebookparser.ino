//Parser for name associated with a number from SIM card phonebook.
//This version only works with String sim = "+CPBR: 1,'121',129,'AtencionCliente'";
//Next Update  will take only the number associated in an standarized way, which means that any global world number should work.


// Define 
String  str = "hola"; 

//String from Serial
////String sim = "+CPBR: 1,'121',129,'AtencionCliente'";

///String sim = "+CPBR: 4,'2582',129,'Club Digitel'";

String sim = "+CPBR: 5,'04168262667',129,'Yoimer'";

//Srtring to compare with received SMS
String compare = "";


// Number registered on sim card

String phoneNumber = "";


//const int len = strq.length();

//char char_array[20];


void setup() {
  // put your setup code here, to run once:

 
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while(!Serial);

  Serial.println("Starting SIM800 SMS Command Processor");
  delay(1000);


  int firstComma = sim.indexOf(',');
  int valor = sim.indexOf("AtencionCliente");  // Starts in 20
  Serial.println(firstComma);
  Serial.println(valor);
  if(sim.charAt(20) == 'A'){
    Serial.println("Yes");
  }

  int stopper = sim.indexOf("129");
  Serial.println(stopper);


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

 Serial.println(compare);   
  //int secondComma = sim.indexOf(',' + firstComma);
 // Serial.println(secondComma);


// Take only the phone number registed on the Sim Card.   //(firstComma+1); i <=(stopper-2);

for(int i = (firstComma+2); i <=(stopper-3); ++i){
  phoneNumber += sim[i];
 }
 Serial.println(phoneNumber);


 /*if (phoneNumber == "04168262667"){
  Serial.println("Yes");
 }else{
  Serial.println("No");
 }*/

}






void loop() {
  
  // put your main code here, to run repeatedly:

}
