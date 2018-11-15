#include <LTC2944.h>
#include <ros.h>
#include <drive_by_wire/Cart_values.h>
#include <std_msgs/Bool.h>
#include "Cart/Cart.h"
#include <ros/time.h>
#include <Stepper.h>
//#include <Encoder.h>

// State machine
enum State {off, start1, start2, start3, start4, rest, pedal1, pedal2, pedal3, brake1};
State state;
ros::Time prevTime;

ros::NodeHandle nh;

// Pedal and brake signals
float pedalVoltage = 0;
float steeringVoltage = 0;
float brakeVoltage = 0;

// Wheel encoder
std_msgs::Bool tick_msg;
ros::Publisher pub_hall("tick", &tick_msg);
int reading=0;
int prereading=0;
volatile int HallVolt=0;
int preHall=1;
int COUNT=0;

/////////////////////////////////////////////////////////////////////////////////
// STEPPER
// List Stepper Constants
const int stepsPerRevolution = 200;
// for your motor

// initialize the stepper library on pins:
Stepper myStepper(stepsPerRevolution, ST_4, ST_3, ST_2, ST_1);

// Stepper Variables
const int counts_p_rev = 1600;

// Initialize the encoder library on interrupt pins:
//Encoder myEnc(STE_2, STE_1);

// CALCULATION VARIABLES
long oldPosition = -999;
long newPosition = 0;
int errPos = 0;

// Calculation Variables
int newPos = 0; // Variable for scaled wheel desired position
int actPos = 0;
int oldpos = 0;

volatile int index = 0;

void indec() {
  index = digitalRead(STE_3);
}

void callback(const drive_by_wire::Cart_values& data) {
  //pedalVoltage = data.throttle;
  //pedalVoltage = 3;
  steeringVoltage = data.steering_angle;
  newPos = steeringVoltage; // Set these equal to each other for now
  brakeVoltage = data.brake;
}

ros::Subscriber<drive_by_wire::Cart_values> sub("Arduino_commands", &callback);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(29,OUTPUT); // emergency relay
  delay(2000);
  pinMode(27,OUTPUT); // ignition relay
  delay(500);
  pinMode(23,OUTPUT); // forward relay
  delay(5000);
  pinMode(24,OUTPUT); // accelerator encoder enable
  analogWrite(6,0); // set accelerator to 0
  pinMode(22,OUTPUT); // brake encoder enable
  analogWrite(5,0); // set brake to 0
  delay(5000);
  pinMode(26,OUTPUT); // trigger accelerator failsafe
  // Pedal and brake
  //Kevin: disabled until after other pins get enabled
  //pinMode(ACCEL, OUTPUT);
  //pinMode(BRAKE, OUTPUT);    

  // Wheel encoder
  attachInterrupt(digitalPinToInterrupt(HALL_SENSOR), tick, CHANGE);

  // Stepper motor
  pinMode(STE_1, INPUT); //////initialize interupt pins to INPUT (floatpin issue)
  pinMode(STE_2, INPUT); //////initialize interupt pins to INPUT (floatpin issue)
  pinMode(STE_3, INPUT); //////initialize interupt pins to INPUT (floatpin issue)
  attachInterrupt(STE_3, indec, CHANGE); // Wait hold on.. they attached it to EncpinB (pin2) originally...
  myStepper.setSpeed(175);
  
  nh.initNode();
  nh.subscribe(sub);
//  nh.advertise(pub_hall);

  state = off;  
  prevTime = nh.now();
}

void loop() {
  /*if (nh.now().toSec() - prevTime.toSec() >= 3) {
    digitalWrite(LED_BUILTIN, LOW);
    analogWrite(ACCEL, 3*34);
    
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
    CLOSE_RELAY(ACCEL_FAILSAFE);
  }*/
  pedalVoltage = 3;
  // State machine
  switch(state) {
    case off:
      state = rest;
      prevTime = nh.now();
      break;
    case start1:
      CLOSE_RELAY(EMERGENCY_RELAY);
      //CLOSE_RELAY(ACCEL_ENCODER_ENABLE);
      //CLOSE_RELAY(BRAKE_ENCODER_ENABLE);
      //if (nh.now().toSec() - prevTime.toSec() >= SWITCHING_TIME/1000) { // 2000ms on corey's code, 5ms here
      if (nh.now().toSec() - prevTime.toSec() >= 2) {
        prevTime = nh.now();
        state = start2;       
      }
      break;
    case start2:
      CLOSE_RELAY(IGNITION_RELAY);
      if (nh.now().toSec() - prevTime.toSec() >= 2) { // 500ms on corey's code
        prevTime = nh.now();
        state = start3;
      }
      break;
    case start3:
      CLOSE_RELAY(FORWARD_RELAY);

      //Kevin's changes:
      if (nh.now().toSec() - prevTime.toSec() >= 5) { // delay for 5000ms
        prevTime = nh.now();
        state = start4;
      }
      //end Kevin's changes:
      //state = rest;
      break;

    //Added another start state sequence for accelerator, brake pin, and failsafe initialization
    case start4:
      CLOSE_RELAY(ACCEL_ENCODER_ENABLE);
      analogWrite(ACCEL, 0);
      CLOSE_RELAY(BRAKE_ENCODER_ENABLE);
      analogWrite(BRAKE, 0);

      if (nh.now().toSec() - prevTime.toSec() >= 5) { // delay for 5000ms
        prevTime = nh.now();
        state = rest;
      }
      CLOSE_RELAY(ACCEL_FAILSAFE);
      break;
     case rest:
      if (pedalVoltage >= 0) {
        prevTime = nh.now();
        state = pedal1;
      } else if (pedalVoltage < 0) {
        prevTime = nh.now();
        state = brake1;
      }
      break;
    case pedal1:
      CLOSE_RELAY(ACCEL_FAILSAFE);
      if (nh.now().toSec() - prevTime.toSec() >= PEDAL_DELAY) {
        state = pedal2;
      }
      break;
    case pedal2:
      if (pedalVoltage < 0) {
        state = pedal3;
      } else {
        analogWrite(ACCEL, pedalVoltage*34);
        state = pedal2;
      }
      break;
    case pedal3:
      OPEN_RELAY(ACCEL_FAILSAFE);
      state = rest;
      break;
    case brake1:
      if (pedalVoltage > 0) {
        state = rest;
      } else {
        analogWrite(BRAKE, -1*pedalVoltage*34);
        state = brake1;
      }
      break; //added this in here-- was this break missing intentionally?
  }
  /* oddly enough this part of the control code causes the wheel to stop momentarily every 5-6 seconds*/
  // Wheel encoder
  if ((!HallVolt) && (preHall==1)) {
    COUNT+=1;
    tick_msg.data = true;
    //pub_hall.publish(&tick_msg);
    preHall=0;  
  } else if (HallVolt){
    preHall=1;
  }

  // Stepper
  // Read the Encoder interrupts:
  /*
  long newPosition = myEnc.read();  
  actPos = newPosition;

  errPos = newPos - actPos;
  if (errPos < -40) {
    myStepper.step(-7);    
  }
  else if (errPos > 40) {
    myStepper.step(7);    
  }  */
  
  nh.spinOnce();
  delay(1);
}

void tick() {  
  HallVolt = !HallVolt;
}
