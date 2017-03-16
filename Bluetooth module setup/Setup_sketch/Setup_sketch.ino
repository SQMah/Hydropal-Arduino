//**HYDROPAL SETUP SKETCH**
// Leave newline and carriage return ON
// Set serial baud to 9600
// Connections:
//  Connect Bluetooth RXD to pin D9
//  Connect Bluetooth TXD to pin D8
//  PWM D10 as a result will not be usable

#include <AltSoftSerial.h>
AltSoftSerial BTSerial; 

String serialString = "";
long baudRate = 0;
long baudtoCheck = 0;
unsigned long baudRates[ ] = {9600, 19200, 38400, 57600, 115200};
bool validated = false;

char c=' ';
boolean NL = true;

void setup() {
  Serial.begin(9600);
  Serial.println("This is the setup script for the Hydropal water bottle");
  Serial.println("Copyright Hydropal 2016");
  Serial.println("Type YES when the Bluetooth module is attached and ready");
  String str = "";
  str = Serial.readString();
  Serial.println(str);
  while (validated == false) {
    if (Serial.available()>0) {
      String str = "";
      str = Serial.readString();
      if (str == "YES\r\n") {
        validated = true;
        break;
      } else {
        // No yes detected
      }
    }
  }

  // Detect baud rate
  int baudIndex = 0;
    while (baudRate == 0 && baudIndex < 5){
      baudtoCheck = baudRates[baudIndex];
      baudRate = checkBaud(baudtoCheck);
      baudIndex++;
    }

    if (baudRate == 0) {
      Serial.print("Baud rate not found!");
    }
    // Serial number
    // Run commands
    // Change baud
}

void loop() {
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

int checkBaud(long baudChecking) {
  int baud = 0;
  BTSerial.begin(baudChecking);
  Serial.println("Checking " + String(baudChecking) + " baud rate...");
  BTSerial.print("AT\r\n");
  if (BTSerial.available() > 0)
  { 
     // Received something
     Serial.println("Received something at " + String(baudChecking));
  } else {
     // Not correct baud rate, baud stays at 0
     // Reset
     serialString = "";
     BTSerial.end();
  }
  return baud;
}

