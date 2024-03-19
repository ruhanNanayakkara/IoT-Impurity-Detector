#include <SharpIR.h>

#define IR A0 // define signal pin
#define model 20150 // used 20150 because model FP2Y0A02YK0F is used

SharpIR SharpIR(IR, model);
void setup() {
 Serial.begin(9600);
}

void loop() {
    // Sharp IR code for Robojax.com
    delay(500);   

  unsigned long startTime = millis();  // takes the time before the loop on the library begins

  int dis = SharpIR.distance();  // this returns the distance to the object you're measuring

  Serial.print("Distance: ");  // returns it to the serial monitor
  Serial.println(dis);
  Serial.println(analogRead(A0));
  unsigned long endTime = millis() - startTime;  // the following gives you the time taken to get the measurement
 Serial.print("Time taken (ms): ");
 Serial.println(endTime);
}
