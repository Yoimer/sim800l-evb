/*
Author: Dhanish
Created on: 19-08-2014
Company: Elementz Engineers Guild Pvt Ltd
Run this code only after SIM800 Module has powered and connected to a Network.
Please make a call to the module knowing whether the connection is established.
If connected a ring will be heard at the caller end
*/

void setup() {
// initialize serial communications at 9600 bps:
Serial.begin(9600);

delay(10000);
// print the serial data to GSM
Serial.print("\r\nATD04168262667;\r\n" ); // change here to call a number using SIM800
// wait 10 seconds before the next loop
delay(10000);
Serial.print("\r\nATH\r\n" ); // hold the call
}

void loop() {
}
