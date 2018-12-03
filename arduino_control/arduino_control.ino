#include <ros.h>
#include <drive_by_wire/Cart_values.h>
#include <drive_by_wire/coulomb_counter_vals.h>
#include <std_msgs/Int32.h>
#include "src/Cart.h"
#include "src/LTC2944_Arduino.h"
#include "src/CoulombCounter.h"
#include <ros/time.h>
#include <Stepper.h>
#include <Encoder.h>

//Coulomb Counter
CoulombCounter::CCValues cc_data;
CoulombCounter cc = CoulombCounter(64, 0.0001, 4.0);
drive_by_wire::coulomb_counter_vals energyData;
ros::Publisher pub_energy("energy", &energyData);


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
bool isManual;

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
const int encoderLim = 7500;

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
  pedalVoltage = data.throttle;  
  steeringVoltage = data.steering_angle;
  newPos = steeringVoltage; // Set these equal to each other for now
  brakeVoltage = data.brake + .7;  
  isManual = data.is_manual;
}

ros::Subscriber<drive_by_wire::Cart_values> sub("Arduino_commands", &callback);

void setup() {
  //Setup I2C communications for the Coulomb Counter
  LTC2944_initialize();
  
  // Pedal and brake
  pinMode(ACCEL, OUTPUT);
  pinMode(BRAKE, OUTPUT);    

  // Wheel encoder
  attachInterrupt(digitalPinToInterrupt(HALL_SENSOR), tick, RISING);

  // Stepper motor
  CLOSE_RELAY(STEPPER_RELAY);
  pinMode(STE_1, INPUT); //////initialize interupt pins to INPUT (floatpin issue)
  pinMode(STE_2, INPUT); //////initialize interupt pins to INPUT (floatpin issue)
  pinMode(STE_3, INPUT); //////initialize interupt pins to INPUT (floatpin issue)
  attachInterrupt(STE_3, indec, CHANGE); // Wait hold on.. they attached it to EncpinB (pin2) originally...
  myStepper.setSpeed(175);

  //nh.setHardware->setBaud(57600);
  nh.initNode();
  nh.subscribe(sub);
  nh.advertise(pub_hall);
  nh.advertise(pub_energy);

  state = off;  
  prevTime = nh.now();

  prev_time = millis();
  isManual = false;
}

void loop() {
  // State machine
  switch(state) {
    case off:
      if (!isManual) {
        state = start1;
        prevTime = nh.now();
      } else {
        state = off;
      }
      break;
    case start1:
      pinMode(IGNITION_RELAY,OUTPUT);  
      pinMode(FORWARD_RELAY,OUTPUT);  
      pinMode(ACCEL_ENCODER_ENABLE,OUTPUT);
      analogWrite(6,0); // set accel voltage as 0
      pinMode(BRAKE_ENCODER_ENABLE,OUTPUT);
      analogWrite(5,0); // set brake voltage as 0
      delay(5000);
      state = rest;       
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
      } else if (pedalVoltage < 0) {
        prevTime = nh.now();
        state = brake1;
      }

//      if (isManual) {
//        state = off;
//      }
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
        fitted = -0.0164*pow(pedalVoltage ,3) - 0.0608*pow(pedalVoltage,2) + 1.7674*pedalVoltage;
        analogWrite(ACCEL, constrain(fitted, 0, 5)*51);
        state = pedal2;
      }

//      if (isManual) {
//        state = off;
//      }
      break;
    case pedal3:
      OPEN_RELAY(ACCEL_FAILSAFE);
      state = rest;
      break;
    case brake1:
      if (pedalVoltage > 0) {
        analogWrite(BRAKE, 0);
        state = rest;
      } else {
        //float offset = -1*pedalVoltage +.7;
        //fitted = .0487*pow(offset,3) - .5527*pow(offset,2) + 2.3975*(offset) - 1.4343;
        fitted = -1*pedalVoltage;
        analogWrite(BRAKE, constrain(fitted, 0, 5)*51);        
        state = brake1;
      }

//      if (isManual) {
//        state = off;
//      }
      break;
  }

  // Coulomb Counter
  cc_data = cc.update();
  energyData.voltage = cc_data.voltage;
  energyData.current = cc_data.current;
  energyData.power = cc_data.power;
  energyData.charge = cc_data.charge;
  energyData.temperature = cc_data.temperature;
  energyData.energy = cc_data.energy;
  pub_energy.publish(&energyData);
  

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
      myStepper.step(7);    
    }
    else if (errPos > 40 && actPos < encoderLim) {
      myStepper.step(-7);    
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
