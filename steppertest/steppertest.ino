#include <Stepper.h>

/////////////////////////////////////////////////////////////////////////////////
// STEPPER
// List Stepper Constants
const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins:
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

// Stepper Variables
const int counts_p_rev = 1600;

void setup() {
  // put your setup code here, to run once:
  myStepper.setSpeed(175);
  myStepper.step(30);
}

void loop() {
  /*
  // put your main code here, to run repeatedly:
  //////////////////////////////////////////////////////////////////////////////////////
  // STEERING LOOP

  // Read the interrupts:
  wheelVal = pwm_value3;
  //analogPin=0;
  //Firmata.sendAnalog(analogPin, pwm_value3); //send value of steering position in micros (should be between 1000 and 2000 micros
  wheelDir = digitalRead(WheelDirPin);

  // Read the Encoder interrupts:
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    //Firmata.sendAnalog(0, newPosition);
    //Firmata.sendAnalog(0, 0000);
    //Firmata.sendAnalog(0, millis());
  }
  actPos = map(oldPosition, 0, 7500, 0, counts_p_rev);
  newPos = map(wheelVal, 0, 255, 0, counts_p_rev);
    
  //if (actPos !=oldpos){  ////////////// chech value of actPos and newPos in console
    //Firmata.sendAnalog(0, actPos);
    //Firmata.sendAnalog(1, newPos);
    //oldpos=actPos;
  //}
  
  if (wheelDir == HIGH) {
    newPos = -newPos;
  }

  errPos = newPos - actPos;
  if (errPos < -20) {
    myStepper.step(30);
    //Firmata.sendAnalog(0, errPos); ////////////////////// check error value in cosole
  }
  else if (errPos > 20) {
    myStepper.step(-30);
    //Firmata.sendAnalog(1, errPos); ////////////////////// check error value in cosole
  }
  
  //Steering Wheel feedback for useability meetering ///////////////////////////////////////////////////////
  //nowMillis = millis();
  //sampInt=50;
  if (abs(actPos - oldpos)> 10) {
    oldpos=actPos;  
    if (actPos>=0){
      Firmata.sendAnalog(0, actPos);
      Firmata.sendAnalog(1, 1); // determine if actPos HIGH is right or left
    }else{
      Firmata.sendAnalog(0, -1*actPos);
      Firmata.sendAnalog(1, 0); // determine if actPos LOW is right or left
    }
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  */
}
