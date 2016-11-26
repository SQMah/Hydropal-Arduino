// Connect Bluetooth RXD to pin D9
// Connect Bluetooth TXD to pin D8
// PWM D10 as a result will not be usable
// To-do:
// - Day rollover save into array
// - Possible fade in and out LED to signal to sync
// - Use sleep and wake up time information to enable/disable LEDs, write to EEPROM?

#include <Time.h>
#include <TimeAlarms.h>
#include <AltSoftSerial.h>

AltSoftSerial BTSerial;

#define SOP '<'
#define EOP '>'

bool started = false;
bool ended = false;
 
char inData[80]; // creates an 80 character array called "inData"
byte index; //creates a variable type=byte called "index"

int waterConsumption[ ] = {0,0,0,0};

// Flowrate and water variables
volatile int flow_frequency; // Measures flow sensor pulses
float ml_sec; // Calculated ml/sec
float total_water = 0.0; // Total water consumed, in ml
unsigned char flowsensor = 2; // Sensor input [PIN NUMBER UPDATE AS REQUIRED]

// Loop timers variables
unsigned long currentTime;
unsigned long cloopTime;

// LED variables
int ledPins[] = {3,4,5,6,7}; // LED Pins [PIN NUMBERS UPDATE AS REQUIRED, WRITE IN ORDER]
int ledState = LOW;
int ledsOn = 0;

int reminderLed = 11; // Pin number of reminderLed

String reminderState = "ON"; // Is LED reminder preference transmitted by phone, defaults to ON
int reminderMinutes = 60; // Defaults to 60 minutes before reminder on LED

bool ledShouldOn = true;

// Alarm
int wakeHour = 8;
int wakeMin = 15;
int sleepHour = 20;
int sleepMin = 50;

// Checks if bottle is synced
bool needSync = true;

void flow () // Interrupt function
{
   flow_frequency++;
}
void setup()
{
  // Flow sensor
   pinMode(flowsensor, INPUT);
   digitalWrite(flowsensor, HIGH); // Optional Internal Pull-Up
   Serial.begin(115200);
   BTSerial.begin(115200);
   attachInterrupt(0, flow, RISING); // Setup Interrupt
   sei(); // Enable interrupts

   // Loop
   currentTime = millis();
   cloopTime = currentTime;

   
   // Set all LED pins to output.
   for (int i = 0; i < 5; i++) {
    pinMode(ledPins[i], OUTPUT);
   }   
}

void loop ()
{
   // Read all serial data available, as fast as possible
  while(BTSerial.available() > 0)
  {
    char inChar = BTSerial.read();
    if(inChar == SOP)
    {
       index = 0;
       inData[index] = '\0';
       started = true;
       ended = false;
    }
    else if(inChar == EOP)
    {
       ended = true;
       break;
    }
    else
    {
      if(index < 79)
      {
        inData[index] = inChar;
        index++;
        inData[index] = '\0';
      }
    }
  }

  // We are here either because all pending serial
  // data has been read OR because an end of
  // packet marker arrived. Which is it?
  if(started && ended)
  {
    // The end of packet marker arrived. Process the packet

    // Send packet of water consumption
    BTSerial.print("<");
    BTSerial.print(waterConsumption[0]);
    BTSerial.print(",");
    BTSerial.print(waterConsumption[1]);
    BTSerial.print(",");
    BTSerial.print(waterConsumption[2]);
    BTSerial.print(",");
    BTSerial.print(waterConsumption[3]);
    BTSerial.print(">");

    // Set needSync to false
    needSync = false;
    
    // Find index of commas
    String dataString = String(inData);
    int commaIndex = dataString.indexOf(',');
    //  Search for the next comma just after the first
    int secondCommaIndex = dataString.indexOf(',', commaIndex+1);
    int thirdCommaIndex = dataString.indexOf(',', secondCommaIndex+1);
    int fourthCommaIndex = dataString.indexOf(',', thirdCommaIndex+1);
    int fifthCommaIndex = dataString.indexOf(',', fourthCommaIndex+1);
    int sixthCommaIndex = dataString.indexOf(',', fifthCommaIndex+1);
    int seventhCommaIndex = dataString.indexOf(',', sixthCommaIndex+1);
    int eigthCommaIndex = dataString.indexOf(',', seventhCommaIndex+1);
    int ninthCommaIndex = dataString.indexOf(',', eigthCommaIndex+1);
    int tenthCommaIndex = dataString.indexOf(',', ninthCommaIndex+1);
    int eleventhCommaIndex = dataString.indexOf(',', tenthCommaIndex+1);
    

    // Parse data, turns time related strings into integers
    int hoursInt = dataString.substring(0, commaIndex).toInt();
    int minutesInt = dataString.substring(commaIndex+1, secondCommaIndex).toInt();
    int secondsInt = dataString.substring(secondCommaIndex +1, thirdCommaIndex).toInt();
    int daysInt = dataString.substring(thirdCommaIndex +1, fourthCommaIndex).toInt();
    int monthsInt = dataString.substring(fourthCommaIndex +1, fifthCommaIndex).toInt();
    int yearsInt = dataString.substring(fifthCommaIndex +1, sixthCommaIndex).toInt();
    reminderState = dataString.substring(sixthCommaIndex +1, seventhCommaIndex);
    reminderMinutes = dataString.substring(seventhCommaIndex+1, eigthCommaIndex).toInt();
    wakeHour = dataString.substring(eigthCommaIndex+1, ninthCommaIndex).toInt();
    wakeMin = dataString.substring(ninthCommaIndex+1, tenthCommaIndex).toInt();
    sleepHour = dataString.substring(tenthCommaIndex+1, eleventhCommaIndex).toInt();
    sleepMin = dataString.substring(eleventhCommaIndex+1).toInt();

    // Set time based on Bluetooth data
    setTime(hoursInt, minutesInt, secondsInt, daysInt, monthsInt, yearsInt);

    // Create the alarms 
    Alarm.alarmRepeat(0,0,0, waterReset);  // 12:00 am every day, reset total_water consumed
    
    // Reset for the next packet
    started = false;
    ended = false;
    index = 0;
    inData[index] = '\0';
  }
   currentTime = millis();
    
   // Every second, calculate and print litres/hour
   if(currentTime >= (cloopTime + 500)) {
      cloopTime = currentTime; // Updates cloopTime

      // Checks for any alarms, i.e. resets total_water at 12 am
      Alarm.delay(0);
      
      // Pulse frequency (Hz) = 8.1, Q is flow rate in L/min.
      ml_sec = (flow_frequency * 3.3); // (Pulse frequency x 60) / 5 Q = flowrate in L/hour, therefore (Pulse frequency x 1000 / 5Q x 60) is flowrate in ml/sec
      total_water = total_water + ml_sec;
      flow_frequency = 0; // Reset Counter
      
      // Add total_water to waterConsumption array
      waterConsumption[3] = total_water;

      // Set up number of non-blinking LEDs that are on
      if (total_water < 1000) {
        ledsOn = 0;
      } else if (total_water < 2000) {
        ledsOn = 1;
      } else if (total_water < 3000) {
        ledsOn = 2;
      } else if (total_water < 4000) {
        ledsOn = 3;
      } else if (total_water < 5000) {
        ledsOn = 4;
      } else {    //Only fires when total_water is above 5000
        ledsOn = 5;
      }

      // If the LED is off turn it on and vice-versa:
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }

      // Since LED to blink pin position in ledPins array is the same as ledsOn
      if (ledsOn < 5) {
        for (int i = 0; i < ledsOn; i++) { 
          digitalWrite(ledPins[i], HIGH);
        }
        digitalWrite(ledPins[ledsOn], ledState); // This pin is blinking
       } else { // Fires only when ledsOn is equal to 5
        for (int i = 0; i < ledsOn; i++) { // These pins are on
          digitalWrite(ledPins[i], HIGH);
        } // No blinking pins, so no final line for variable state LED
      }

      // Pulsate reminder LED if needSync is true CHANGE AFTER DEBUG
      if (needSync == true) {
        float in, out;
        for (in = 0; in < 6.283; in = in + 0.001)
          {
            out = sin(in) * 127.5 + 127.5;
            analogWrite(reminderLed,out);
          }
      } else {
        if ((hour() > wakeHour) && (hour() < sleepHour)) {
          ledShouldOn = true;
        } else if ((hour() == wakeHour) || (hour() == sleepHour)) {
          if (hour() == wakeHour) {
            if (minute() >= wakeMin) {
              ledShouldOn = true;
            } else if (minute() < wakeMin) {
              ledShouldOn = false;
            }
          }
          if (hour() == sleepHour) {
            if (minute() >= sleepMin) {
              ledShouldOn = false;
            } else if (minute() < sleepMin) {
              ledShouldOn = true;
            }
          }
        } else if ((hour() > sleepHour) || (hour() < wakeHour)) {
          ledShouldOn = false;
        }
      }
   }
}

// Function is called when time is 12 am
void waterReset() {
    if (needSync == true) {
    // Do nothing because time is inorrect
  } else {
    waterConsumption[0] = waterConsumption[1];
    waterConsumption[1] = waterConsumption[2];
    waterConsumption[2] = waterConsumption[3];

    // Reset water count for the day
    waterConsumption[3] = 0;
    total_water = 0;
  }
}
