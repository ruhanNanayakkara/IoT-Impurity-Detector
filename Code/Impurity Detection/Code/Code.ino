#include <SharpIR.h>

#define SOUND 340 // Speed Of Sound
#define MODEL 20150 // IR Sensor Model (FP2Y0A02YK0F)
#define IR A0 // IR Port

const int trigPin = 12; // Supersonic Trigger
const int echoPin = 10; // Supersonic Echo
const int motorPin1 = 5; // Motor A
const int motorPin2 = 6; // Motor A
const int motorPin3 = 9; // Motor B
const int motorPin4 = 8; // Motor B

// Defining Global Variables
float _duration = 0.0f, _distVertical, _baseVal;
int _distHorizontal;
bool _firstReading = false, _motorRunning = false;

// Initialising IR Module Object
SharpIR SharpIR(IR, MODEL);

// Initialising I/O Pins & Vertical Distance Base Value
void setup()
{
    // Supersonic Sensor Pins
    pinMode(trigPin, OUTPUT); // Trigger Output
    pinMode(echoPin, INPUT); // Echo Input

    // Setting All Motor Pins To Output
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    pinMode(motorPin3, OUTPUT);
    pinMode(motorPin4, OUTPUT);

    digitalWrite(motorPin1, LOW); // Always Remains LOW
    digitalWrite(motorPin3, LOW); // Always Remains LOW

    // Get Base Value
    operateSupersonic(); // Update Vertical Distance
    _baseVal = _distVertical; // Save Initial Distance As Base Value
}

// Determines If Device Should Move Or Stop
bool shouldRun()
{
    if (abs(_distVertical - _baseVal) > 0.1 * _baseVal) return false; // Impurity Detected
    if (_distHorizontal <= 25) return false; // Wall Ahead
    return true;
}

// Decides If Motors Should Run
void operateMotor()
{
    // Check If Motor State Updated
    if (_motorRunning == shouldRun()) return; // No Action If State Unchanged
    _motorRunning = shouldRun(); // Update Variable If State Changed

    if (_motorRunning) // Run Motors
    {
        digitalWrite(motorPin2, HIGH);
        digitalWrite(motorPin4, HIGH);
    }
    else // Stop Motors
    {
        digitalWrite(motorPin2, LOW);
        digitalWrite(motorPin4, LOW);
    }
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

void loop()
{
    operateSupersonic(); // Update Vertical Dsitance
    operateInfrared(); // Update Horizontal Distance
    operateMotor(); // Update Motor State
}
