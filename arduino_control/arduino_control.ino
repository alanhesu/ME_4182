#include <ros.h>
#include <drive_by_wire/Cart_values.h>
#include <std_msgs/Bool.h>

ros::NodeHandle nh;
float pedalVoltage;
float steeringVoltage;
float brakeVoltage;
int pedal = 5;
int brake = 6;
int pedalSwitch = 7;

std_msgs::Bool tick_msg;
ros::Publisher pub_hall("tick", &tick_msg);
int reading=0;
int prereading=0;
int HallVolt=0;
int preHall=1;
int SpeedSensorPin=2; 
int COUNT=0;


void callback(const drive_by_wire::Cart_values& data) {
//  pedalVoltage = data.throttle;
//  steeringVoltage = data.steering_angle;
//  brakeVoltage = data.brake;
//  if (pedalVoltage != 0) {
//    digitalWrite(pedalSwitch, HIGH);
//  } else {
//    digitalWrite(pedalSwitch, LOW);
//  }
//  analogWrite(pedal, pedalVoltage*51);
//  analogWrite(brake, brakeVoltage*51);  
  if (data.throttle > 0) {
    digitalWrite(pedalSwitch, HIGH);
    analogWrite(pedal, data.throttle*51);
  } else if (data.throttle < 0) {
    digitalWrite(pedal, 0);
    digitalWrite(pedalSwitch, LOW);
    analogWrite(brake, brakeVoltage*51);
  } else {
    digitalWrite(pedalSwitch, LOW);
  }
}

ros::Subscriber<drive_by_wire::Cart_values> sub("Arduino_commands", &callback);

void setup() {
  // put your setup code here, to run once:
  pinMode(pedal, OUTPUT);
  pinMode(brake, OUTPUT);
  pinMode(pedalSwitch, OUTPUT);
  pinMode(SpeedSensorPin, INPUT); //////initialize interupt pins to INPUT (floatpin issue)
  nh.initNode();
  nh.subscribe(sub);
  nh.advertise(pub_hall);
//  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  HallVolt=digitalRead(SpeedSensorPin);

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
