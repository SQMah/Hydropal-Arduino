//  Basic bluetooth communication sketch using AltSoftSerial (ASS). 
//  Uses hardware serial to talk to the host computer and ASS for communication with the bluetooth module
//
//  When a command is entered in the serial monitor on the computer 
//  the Arduino will relay it to the bluetooth module and display the result in the serial monitor.
//
//  Pins
//    BT VCC to Arduino 5V out. 
//    BT GND to GND
//    Connect Bluetooth RXD to pin D9
//    Connect Bluetooth TXD to pin D8
//  Commands to write in order - REMEMBER TO TURN ON CR AND NL IN THE SERIAL MONITOR
//    AT (reply should be OK)
//    BT is working at this point
 
#include <AltSoftSerial.h>
AltSoftSerial BTSerial; 

#define SOP '<'
#define EOP '>'

bool started = false;
bool ended = false;
 
 
char c=' ';
boolean NL = true;
 
void setup() 
{
    Serial.begin(115200);
    Serial.print("Sketch:   ");   Serial.println(__FILE__);
    Serial.print("Uploaded: ");   Serial.println(__DATE__);
    Serial.println(" ");
 
    BTSerial.begin(115200);  
    Serial.println("BTserial started at 115200");
    Serial.println("");
}
 
void loop()
{
 
    // Read from the Bluetooth module and send to the Arduino Serial Monitor
    if (BTSerial.available())
    {
        c = BTSerial.read();
        Serial.write(c);
    }
 
 
    // Read from the Serial Monitor and send to the Bluetooth module
    if (Serial.available())
    {
        c = Serial.read();
        BTSerial.write(c);   
 
        // Echo the user input to the main window. The ">" character indicates the user entered text.
        if (NL) { Serial.print(">");  NL = false; }
        Serial.write(c);
        if (c==10) { NL = true; }
    }
 
}
