// Flowrate and water variables
volatile int flow_frequency; // Measures flow sensor pulses
float ml_sec; // Calculated ml/sec
float total_water; // Total water consumed, in ml
unsigned char flowsensor = 2; // Sensor input [PIN NUMBER UPDATE AS REQUIRED]

// Loop timers variables
unsigned long currentTime;
unsigned long cloopTime;

// LED variables
int ledPins[] = {3,4,5,6,7}; // LED Pins [PIN NUMBERS UPDATE AS REQUIRED, WRITE IN ORDER]
int ledState = LOW;
int ledsOn = 0;

void flow () // Interrupt function
{
   flow_frequency++;
}
void setup()
{
   pinMode(flowsensor, INPUT);
   digitalWrite(flowsensor, HIGH); // Optional Internal Pull-Up
   Serial.begin(9600);
   attachInterrupt(0, flow, RISING); // Setup Interrupt
   sei(); // Enable interrupts
   currentTime = millis();
   cloopTime = currentTime;
   for (int i = 0; i < 5; i++) {
    pinMode(ledPins[i], OUTPUT);
   }
}
void loop ()
{
   currentTime = millis();
   // Every second, calculate and print litres/hour
   if(currentTime >= (cloopTime + 1000)) {
      cloopTime = currentTime; // Updates cloopTime
      // Pulse frequency (Hz) = 8.1, Q is flow rate in L/min.
      ml_sec = (flow_frequency * 2.0576); // (Pulse frequency x 60) / 8.1 Q = flowrate in L/hour, therefore (Pulse frequency x 1000 / 8.1Q x 60) is flowrate in ml/sec
      total_water = total_water + ml_sec;
      flow_frequency = 0; // Reset Counter

      // Converts serial output to L when over or equal 1L
      if (total_water >= 1000) {
        Serial.print( total_water / 1000 ); // Total water in L
        Serial.println(" L consumed");
      } else {
        Serial.print( total_water ); // Total water in ml
        Serial.println(" ml consumed");
      }

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
          i++; // Increments counter
        }
        digitalWrite(ledPins[ledsOn], ledState); // This pin is blinking
       } else { // Fires only when ledsOn is equal to 5
        for (int i = 0; i < ledsOn; i++) { // These pins are on
          digitalWrite(ledPins[i], HIGH);
        } // No blinking pins, so no final line for variable state LED
      }
   }
}
