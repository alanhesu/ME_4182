#include <Stepper.h>
#include <Encoder.h>

/////////////////////////////////////////////////////////////////////////////////
// STEPPER
// List Stepper Constants
const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins:
Stepper myStepper(stepsPerRevolution, 33, 35, 37, 39);

// Stepper Variables
const int counts_p_rev = 1600;

// ENCODER INPUTS
// Encoder Input Pins
int EncAPin = 3; //interrupt 4 (blue wire)
int EncBPin = 2; //interrupt 5 (green wire)
int EncIndPin = 18; //interrupt 2

// Initialize the encoder library on interrupt pins:
Encoder myEnc(EncAPin, EncBPin);

// CALCULATION VARIABLES
long oldPosition = -999;
long newPosition = 0;
int errPos = 0;

// Calculation Variables
int newPos = -5000; // Variable for scaled wheel desired position
int actPos = 0;
int oldpos = 0;

volatile int index = 0;

void indec() {
  index = digitalRead(EncIndPin);
}

int countSteps = 0;

void setup() {
  pinMode(29, OUTPUT);
  // put your setup code here, to run once:
  pinMode(EncAPin, INPUT); //////initialize interupt pins to INPUT (floatpin issue)
  pinMode(EncBPin, INPUT); //////initialize interupt pins to INPUT (floatpin issue)
  pinMode(EncIndPin, INPUT); //////initialize interupt pins to INPUT (floatpin issue)
  attachInterrupt(2, indec, CHANGE); // Encoder Index ------- interrupt 2, pin 21
  myStepper.setSpeed(175);
//  myStepper.step(-150);

  Serial.begin(9600);
}

void loop() {
   // Read the Encoder interrupts:
  long newPosition = myEnc.read();
  Serial.println(newPosition);
  actPos = newPosition;

  errPos = newPos - actPos;
  if (errPos < -40) {
    myStepper.step(7);
    //Firmata.sendAnalog(0, errPos); ////////////////////// check error value in cosole
  }
  else if (errPos > 40) {
    myStepper.step(-7);
    //Firmata.sendAnalog(1, errPos); ////////////////////// check error value in cosole
  }  
}
