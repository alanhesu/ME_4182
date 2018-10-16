#include <ros.h>
#include <drive_by_wire/Cart_values.h>

ros::NodeHandle nh;
float pedalVoltage;
float steeringVoltage;
float brakeVoltage;
int pedal = 5;
int brake = 6;
int pedalSwitch = 7;

void callback(const drive_by_wire::Cart_values& data) {
  pedalVoltage = data.throttle;
  steeringVoltage = data.steering_angle;
  brakeVoltage = data.brake;
  if (pedalVoltage != 0) {
    digitalWrite(pedalSwitch, HIGH);
  } else {
    digitalWrite(pedalSwitch, LOW);
  }
  analogWrite(pedal, pedalVoltage*51);
  analogWrite(brake, brakeVoltage*51);  
}

ros::Subscriber<drive_by_wire::Cart_values> sub("Arduino_commands", &callback);

void setup() {
  // put your setup code here, to run once:
  pinMode(pedal, OUTPUT);
  pinMode(brake, OUTPUT);
  pinMode(pedalSwitch, OUTPUT);
  nh.initNode();
  nh.subscribe(sub);
}

void loop() {
  // put your main code here, to run repeatedly:
  nh.spinOnce();
  delay(1);
}
