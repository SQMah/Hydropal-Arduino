volatile int flow_frequency; // Measures flow sensor pulses
unsigned int ml_sec; // Calculated millilitres/second
unsigned int total_ml;

unsigned char flowsensor = 2; // Sensor Input [UPDATE WITH PIN]
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
   total_ml= 0;
}


void loop () //Add repeated functions here
{
   currentTime = millis();
   // Every second, calculate and print ml/sec
   if(currentTime >= (cloopTime + 1000))
   {
      cloopTime = currentTime; // Updates cloopTime
      // Pulse frequency (Hz) = 8.1Q, Q is flow rate in L/min.
      ml_sec = (flow_frequency / ( 60 * 8.1 * 1000 ) ); // (Pulse frequency) / 8.1Q * 60 seconds * 1000 ml = flowrate in ml/second
      total_ml = total_ml + ml_sec;
      
      flow_frequency = 0; // Reset Counter
      Serial.print(total_ml, DEC);
      Serial.println(" total ml");
   }
}
