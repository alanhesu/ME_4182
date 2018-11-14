#include <LTC2944.h>
#include <ros.h>
#include <drive_by_wire/Cart_values.h>
#include <std_msgs/Bool.h>
#include <Cart.h>
#include <ros/time.h>

enum State {off, start1, start2, start3, rest, pedal1, pedal2, pedal3, brake1};
State state;
ros::Time prevTime;

ros::NodeHandle nh;

// Pedal and brake signals
float pedalVoltage = 0;
float steeringVoltage = 0;
float brakeVoltage = 0;
int pedal = 5;
int brake = 6;
int pedalSwitch = 7;

// Wheel encoder
std_msgs::Bool tick_msg;
ros::Publisher pub_hall("tick", &tick_msg);
int reading=0;
int prereading=0;
volatile int HallVolt=0;
int preHall=1;
int SpeedSensorPin=2;
int COUNT=0;


void callback(const drive_by_wire::Cart_values& data) {
  pedalVoltage = data.throttle;
  steeringVoltage = data.steering_angle;
  brakeVoltage = data.brake;
}

ros::Subscriber<drive_by_wire::Cart_values> sub("Arduino_commands", &callback);

void setup() {
  // put your setup code here, to run once:
  pinMode(pedal, OUTPUT);
  pinMode(brake, OUTPUT);
  pinMode(pedalSwitch, OUTPUT);
  Serial.begin(57600);
  // pinMode(SpeedSensorPin, INPUT); //////initialize interupt pins to INPUT (floatpin issue)
  attachInterrupt(digitalPinToInterrupt(SpeedSensorPin), tick, CHANGE);
  nh.initNode();
  nh.subscribe(sub);
  nh.advertise(pub_hall);

  state = off;  
  prevTime = nh.now();
}

void loop() {
  // State machine
  switch(state) {
    case off:
      state = start1;
      prevTime = nh.now();
      break;
    case start1:
      CLOSE_RELAY(EMERGENCY_RELAY);
      CLOSE_RELAY(ACCEL_ENCODER_ENABLE);
      CLOSE_RELAY(BRAKE_ENCODER_ENABLE);
      if (nh.now().toSec() - prevTime.toSec() >= SWITCHING_TIME/1000) {
        prevTime = nh.now();
        state = start2;       
      }
      break;
    case start2:
      CLOSE_RELAY(IGNITION_RELAY);
      if (nh.now().toSec() - prevTime.toSec() >= 2) {
        prevTime = nh.now();
        state = start3;
      }
      break;
    case start3:
      CLOSE_RELAY(FORWARD_RELAY);
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
  }

  if ((!HallVolt) && (preHall==1)) {
    COUNT+=1;
    tick_msg.data = true;
    pub_hall.publish(&tick_msg);
    preHall=0;
//    Serial.write("tick");
//    nh.loginfo(COUNT);

  //} else {
  } else if (HallVolt){
    preHall=1;
  }
  nh.spinOnce();
  delay(1);
}

void tick() {
  Serial.println("tick");
  HallVolt = !HallVolt;
}
