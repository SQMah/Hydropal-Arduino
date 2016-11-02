volatile int flow_frequency; // Measures flow sensor pulses
float ml_sec; // Calculated ml/sec
float total_water; // Total water consumed, in ml
unsigned char flowsensor = 2; // Sensor Input [PIN NUMBER UPDATE AS REQUIRED]
unsigned long currentTime;
unsigned long cloopTime;
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
   total_water = 1500;
}
void loop ()
{
   currentTime = millis();
   // Every second, calculate and print litres/hour
   if(currentTime >= (cloopTime + 1000))
   {
      cloopTime = currentTime; // Updates cloopTime
      // Pulse frequency (Hz) = 8.1, Q is flow rate in L/min.
      ml_sec = (flow_frequency * 2.0576); // (Pulse frequency x 60) / 8.1 Q = flowrate in L/hour, therefore (Pulse frequency x 1000 / 8.1Q x 60) is flowrate in ml/sec
      total_water = total_water + ml_sec;
      flow_frequency = 0; // Reset Counter
      if (total_water >= 1000) {
        Serial.print( total_water / 1000 ); // Total water in L
        Serial.println(" L consumed");
      } else {
        Serial.print( total_water ); // Total water in ml
        Serial.println(" ml consumed");
      }
   }
}
