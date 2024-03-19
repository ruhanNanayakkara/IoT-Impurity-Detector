#include <SharpIR.h>
#include <stdio.h>

#define SOUND 340 // Speed Of Sound
#define MODEL 20150 // IR Sensor Model (FP2Y0A02YK0F)
#define IR A0 // IR Port

const int trigPin = 12; // Supersonic Trigger
const int echoPin = 10; // Supersonic Echo
const int motorPin1 = 5; // Motor A
const int motorPin2 = 6; // Motor A
const int motorPin3 = 9; // Motor B
const int motorPin4 = 8; // Motor B
const int LEDPin = 4; // LED

// Defining Global Variables
float _duration = 0.0f, _distVertical, _baseVal;
int _distHorizontal;
bool _firstReading = false, _motorRunning = false;

// Initialising IR Module Object
SharpIR SharpIR(IR, MODEL);

// Initialising I/O Pins & Vertical Distance Base Value
void setup()
{
    //Start Serial Communication
    Serial.begin(9600);
  
    // Supersonic Sensor Pins
    pinMode(trigPin, OUTPUT); // Trigger Output
    pinMode(echoPin, INPUT); // Echo Input

    // Setting All Motor Pins To Output
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    pinMode(motorPin3, OUTPUT);
    pinMode(motorPin4, OUTPUT);

    // Setting LED Pin To Output
    pinMode(LEDPin, OUTPUT);

    digitalWrite(motorPin2, LOW); // Always Remains LOW
    digitalWrite(motorPin4, LOW); // Always Remains LOW

    // Get Base Value
    operateSupersonic(); // Update Vertical Distance
    _baseVal = _distVertical; // Save Initial Distance As Base Value
}

// Decides If Motors Should Run
void operateMotor(bool shouldRun)
{
    // Check If Motor State Updated
    if (_motorRunning == shouldRun) return; // No Action If State Unchanged
    _motorRunning = shouldRun; // Update Variable If State Changed

    if (_motorRunning) // Run Motors
    {
        digitalWrite(motorPin1, HIGH);
        digitalWrite(motorPin3, HIGH);
    }
    else // Stop Motors
    {
        digitalWrite(motorPin1, LOW);
        digitalWrite(motorPin3, LOW);
    }
}

// Converts Srting To Boolean Equivalent
bool toBool(String s)
{
    if (s == "True") return true;  
    else return false;
}

// Gets Vertical Distance Using Supersonic (Digital) Sensor
void operateSupersonic()
{
    // Clear Trigger
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    // Switch On TriggerFor 10 Microseconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, HIGH);

    // Get Echo  Duration Time
    float newDuration = pulseIn(echoPin,  HIGH);
    newDuration *= 0.5f; // Divide By Half To Cancel Return Time

    // Update Only If New Duration Beyond Error Margin (10%)
    if (abs(newDuration - _duration) > 0.2f * _duration)
      _duration = newDuration;

    // Dividing Speed Of Sound By 10000 To Convert 1 Second To 100 Microseconds
    _distVertical = _duration * static_cast<float>(SOUND/10000.0f);
}

// Gets Horizontal Distance Using Infrared (Analog) Sensor
void operateInfrared()
{
    _distHorizontal = SharpIR.distance();
}

// Turns LED On Or Off Depending On Motor State
void operateLED()
{
    if (!_motorRunning) digitalWrite(LEDPin, HIGH);
    else digitalWrite(LEDPin, LOW);
}

void serialOutput()
{
    Serial.print("h");
    Serial.print(_distHorizontal); // Prints Horizontal Dsitance For Edge Server
    Serial.print(","); // Delimiter
    Serial.print("b");
    Serial.print(_baseVal); // Prints Base Vertical Distance Value For Edge Server
    Serial.print(",");
    Serial.print("v");
    Serial.println(_distVertical); // Prints Vertical Distance For Edge Server
}

void loop()
{
    operateInfrared();
    operateSupersonic();
    serialOutput();
    operateMotor(toBool(Serial.readStringUntil('\n')));
    operateLED();
}
