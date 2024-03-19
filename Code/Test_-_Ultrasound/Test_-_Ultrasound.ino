// defines pins numbers
const int trigPin = 12;
const int echoPin = 10;
// defines variables
float duration;
float distance;
float base_value;
bool flag = false;
void setup() {
  Serial.begin(9600); // Starts the serial communication
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
}
void loop() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = (duration / 2) * 0.034;
  if (!flag)
  {
    base_value = distance;
    flag = true;
  }
  else
  {
    if ((distance / base_value) * 10 > 20)
    {
      Serial.print("Distance: ");
      Serial.println(distance);
      Serial.println("Base value: ");
      Serial.println(base_value);
      Serial.println("OBSTRUCTION!");    
    }
  }
  // Prints the distance on the Serial Monitor
  //Serial.print("Distance: ");
  //Serial.println(distance);
  delay(500);
}
