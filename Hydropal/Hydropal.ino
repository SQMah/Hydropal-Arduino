// Connect Bluetooth RXD to pin D9
// Connect Bluetooth TXD to pin D8
// PWM D10 as a result will not be usable

#include <Time.h>
#include <TimeAlarms.h>
#include <AltSoftSerial.h>
#include <LowPower.h>

AltSoftSerial BTSerial;

#define SOP '<'
#define EOP '>'

bool started = false;
bool ended = false;
 
char inData[100]; // creates an 80 character array called "inData"
byte index; //creates a variable type=byte called "index"

// Index 0 is 3 days ago etc. Index 3 is today
int waterConsumption[ ] = {0,0,0,0};
long lastDrink;

// Flowrate and water variables
volatile int flow_frequency; // Measures flow sensor pulses
float ml_sec; // Calculated ml/sec
unsigned char flowsensor = 2; // Sensor input [PIN NUMBER UPDATE AS REQUIRED]

// Loop timers variables
unsigned long currentTime;
unsigned long cloopTime;

// LED variables
int reminderLed = 11; // Pin number of reminderLed
String reminderState = "ON"; // Is LED reminder preference transmitted by phone, defaults to ON
int reminderMinutes = 60; // Defaults to 60 minutes before reminder on LED
bool ledState = LOW;
bool ledShouldOn = false;

// Alarm
int wakeHour = 7;
int wakeMin = 0;
int sleepHour = 22;
int sleepMin = 0;

// Checks if bottle is synced
bool needSync = true;

// Checks if bottle should log data
bool onOffIntake = true;

void flow () // Interrupt function for flow rate sensor
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
      if(index < 99)
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
    int twelfthCommaIndex = dataString.indexOf(',', eleventhCommaIndex+1);
    int thirteenthCommaIndex = dataString.indexOf(',', twelfthCommaIndex+1);
    int fourteenthCommaIndex = dataString.indexOf(',', thirteenthCommaIndex+1);
    int fifteenthCommaIndex = dataString.indexOf(',', fourteenthCommaIndex+1);
    int sixteenthCommaIndex = dataString.indexOf(',', fifteenthCommaIndex+1);
    

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
    sleepMin = dataString.substring(eleventhCommaIndex+1, twelfthCommaIndex).toInt();
    int todayVolume = dataString.substring(twelfthCommaIndex+1, thirteenthCommaIndex).toInt();
    int yesterdayVolume = dataString.substring(thirteenthCommaIndex+1, fourteenthCommaIndex).toInt();
    int twoDayVolume = dataString.substring(fourteenthCommaIndex+1, fifteenthCommaIndex).toInt();
    int threeDayVolume = dataString.substring(fifteenthCommaIndex+1, sixteenthCommaIndex).toInt();
    onOffIntake = dataString.substring(sixteenthCommaIndex+1);
    

    // Checks if volume data received from device is larger than the one on the board, if it is so, then the volume on the board is updated
    if (todayVolume >= waterConsumption[3]) {
      waterConsumption[3] = todayVolume;
    }

    if (yesterdayVolume >= waterConsumption[2]) {
      waterConsumption[2] = yesterdayVolume;
    }

    if (twoDayVolume >= waterConsumption[1]) {
      waterConsumption[1] = twoDayVolume;
    }

    if (threeDayVolume >= waterConsumption[0]) {
      waterConsumption[0] = threeDayVolume;
    }
    
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
  if (needSync == true) {
    //Pulsate LED
        float in, out;
        for (in = 0; in < 6.283; in = in + 0.001)
          {
            out = sin(in) * 127.5 + 127.5;
            analogWrite(reminderLed,out);
          }
  } else {
     currentTime = millis();
      
     // Every second, calculate and print litres/hour
     if(currentTime >= (cloopTime + 500)) {
        cloopTime = currentTime; // Updates cloopTime
  
        // Checks for any alarms, i.e. resets total_water at 12 am
        Alarm.delay(0);
        
        // Pulse frequency (Hz) = 8.1, Q is flow rate in L/min.
        ml_sec = (flow_frequency * 3.3); // (Pulse frequency x 60) / 5 Q = flowrate in L/hour, therefore (Pulse frequency x 1000 / 5Q x 60) is flowrate in ml/sec
        // Add ml_sec to consumption of the day
        if (onOffIntake == "ON") {
          // Track intake
          waterConsumption[3] = waterConsumption[3] + ml_sec;
        } else {
          // Don't track
        }
        
        flow_frequency = 0; // Reset Counter
  
        //Check if any water was consumed
        if (ml_sec >= 1.0) {
          lastDrink = time_t();
        }

        // Check wake and sleep times
        if ((hour() > wakeHour) && (hour() < sleepHour)) {
          ledShouldOn = true;
        } else if ((hour() == wakeHour) || (hour() == sleepHour)) {
          if (hour() == wakeHour) {
            if (minute() >= wakeMin) {
              ledShouldOn = true;
            } else if (minute() < wakeMin) {
              ledShouldOn = false;
            }
          } else if (hour() == sleepHour) {
            if (minute() >= sleepMin) {
              ledShouldOn = false;
            } else if (minute() < sleepMin) {
              ledShouldOn = true;
            }
          }
        } else if ((hour() > sleepHour) || (hour() < wakeHour)) {
          ledShouldOn = false;
        }
        
        // If the LED is off turn it on and vice-versa:
        if (ledState == LOW) {
          ledState = HIGH;
        } else {
          ledState = LOW;
        }

        // Check if user has not consumped water for more seconds than the reminderMinutes
        if (((now()-lastDrink) >= (reminderMinutes * 60)) && ledShouldOn == true) {
          digitalWrite(reminderLed, ledState);
        } else {
          //Sleep time
          digitalWrite(reminderLed, LOW);
        }
     }
  }
}

// Function is called when time is 12 am
void waterReset() {
  // Shift array down by one
  waterConsumption[0] = waterConsumption[1];
  waterConsumption[1] = waterConsumption[2];
  waterConsumption[2] = waterConsumption[3];

  // Reset water count for the day
  waterConsumption[3] = 0;
}
