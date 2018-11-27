#include <LTC2944.h>
#include <ros.h>
#include <drive_by_wire/Cart_values.h>
#include <std_msgs/Int32.h>
#include "Cart/Cart.h"
#include <ros/time.h>
#include <Stepper.h>
#include <Encoder.h>

// State machine
enum State {off, start1, start2, start3, rest, pedal1, pedal2, pedal3, brake1};
State state;
ros::Time prevTime;

ros::NodeHandle nh;

// Pedal and brake signals
float pedalVoltage = 0;
float steeringVoltage = 0;
float brakeVoltage = 0;
float fitted;

// Wheel encoder
std_msgs::Int32 tick_msg;
ros::Publisher pub_hall("tick", &tick_msg);
int reading=0;
int prereading=0;
volatile int HallVolt=0;
int preHall=1;
int COUNT=0;
unsigned long prev_time = 0;
unsigned long elapsed_time = 0;

/////////////////////////////////////////////////////////////////////////////////
// STEPPER
// List Stepper Constants
// for your motor
const int stepsPerRevolution = 200;
const int encoderLim = 8000;

// initialize the stepper library on pins:
Stepper myStepper(stepsPerRevolution, ST_4, ST_3, ST_2, ST_1);

// Stepper Variables
const int counts_p_rev = 1600;

// Initialize the encoder library on interrupt pins:
Encoder myEnc(STE_2, STE_1);

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
  pedalVoltage = data.throttle + .7;
  steeringVoltage = data.steering_angle;
  newPos = steeringVoltage; // Set these equal to each other for now
  brakeVoltage = data.brake + .7;
}

ros::Subscriber<drive_by_wire::Cart_values> sub("Arduino_commands", &callback);

void setup() {
  // Pedal and brake
  pinMode(ACCEL, OUTPUT);
  pinMode(BRAKE, OUTPUT);    

  // Wheel encoder
  attachInterrupt(digitalPinToInterrupt(HALL_SENSOR), tick, RISING);

  // Stepper motor
  pinMode(STE_1, INPUT); //////initialize interupt pins to INPUT (floatpin issue)
  pinMode(STE_2, INPUT); //////initialize interupt pins to INPUT (floatpin issue)
  pinMode(STE_3, INPUT); //////initialize interupt pins to INPUT (floatpin issue)
  attachInterrupt(STE_3, indec, CHANGE); // Wait hold on.. they attached it to EncpinB (pin2) originally...
  myStepper.setSpeed(175);
  
  nh.initNode();
  nh.subscribe(sub);
  nh.advertise(pub_hall);

  state = off;  
  prevTime = nh.now();

  pinMode(IGNITION_RELAY,OUTPUT);  
  pinMode(FORWARD_RELAY,OUTPUT);  
  pinMode(ACCEL_ENCODER_ENABLE,OUTPUT);
  analogWrite(6,0); // set accel voltage as 0
  pinMode(BRAKE_ENCODER_ENABLE,OUTPUT);
  analogWrite(5,0); // set brake voltage as 0
  delay(5000);

  prev_time = millis();
}

void loop() {
  // State machine
  switch(state) {
    case off:
      state = rest;
      prevTime = nh.now();
      break;
    case start1:
//      CLOSE_RELAY(EMERGENCY_RELAY);
//      CLOSE_RELAY(ACCEL_ENCODER_ENABLE);
//      CLOSE_RELAY(BRAKE_ENCODER_ENABLE);
      if (nh.now().toSec() - prevTime.toSec() >= SWITCHING_TIME/1000) {
        prevTime = nh.now();
        state = start2;       
      }
      break;
    case start2:
//      CLOSE_RELAY(IGNITION_RELAY);
      if (nh.now().toSec() - prevTime.toSec() >= 2) {
        prevTime = nh.now();
        state = start3;
      }
      break;
    case start3:
//      CLOSE_RELAY(FORWARD_RELAY);
      state = rest;
      break;
     case rest:
      if (pedalVoltage > 0) {
        prevTime = nh.now();
        state = pedal1;
      } else if (pedalVoltage < -1) {
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
        analogWrite(ACCEL, 0);
        state = pedal3;
      } else {
        fitted = .0487*pow(pedalVoltage,3) - .5527*pow(pedalVoltage,2) + 2.3975*pedalVoltage - 1.4343;
        analogWrite(ACCEL, constrain(fitted, 0, 5)*51);
        state = pedal2;
      }
      break;
    case pedal3:
      OPEN_RELAY(ACCEL_FAILSAFE);
      state = rest;
      break;
    case brake1:
      if (pedalVoltage > -1) {
        analogWrite(BRAKE, 0);
        state = rest;
      } else {
        fitted = .0487*pow(pedalVoltage+1,3) - .5527*pow(pedalVoltage+1,2) + 2.3975*(pedalVoltage+1) - 1.4343;
        analogWrite(BRAKE, -1*constrain(fitted, 0, 5)*51);
        state = brake1;
      }
      break;
  }

  // Wheel encoder
  elapsed_time = millis() - prev_time;
  if (elapsed_time >= 33) {
    tick_msg.data = COUNT;
    pub_hall.publish(&tick_msg);
    COUNT = 0;
    prev_time = millis();
  }

  // Stepper
  // Read the Encoder interrupts:
  long newPosition = myEnc.read();  
  actPos = newPosition;

  errPos = newPos - actPos;
  if (errPos < -40 && actPos > -1*encoderLim) {
    myStepper.step(-7);    
  }
  else if (errPos > 40 && actPos < encoderLim) {
    myStepper.step(7);    
  }  
  
  nh.spinOnce();
  delay(1);
}

void tick() {
  COUNT++;
//  HallVolt = !HallVolt;
//  if ((!HallVolt) && (preHall==1)) {
//    COUNT+=1;
////    tick_msg.data = true;
////    pub_hall.publish(&tick_msg);
//    preHall=0;  
//  } else if (HallVolt){
//    preHall=1;
//  }
}
