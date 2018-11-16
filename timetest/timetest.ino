#include <ros.h>
#include <std_msgs/Float64.h>
#include <ros/time.h>
#include <Time.h>

ros::NodeHandle nh;
std_msgs::Float64 ti;
ros::Publisher pub("time", &ti);
time_t t;

void setup() {
  // put your setup code here, to run once:
  nh.initNode();  
  nh.advertise(pub);
}

void loop() {
  // put your main code here, to run repeatedly:
  t = now();
  ti.data = t;
  pub.publish(&ti);
  nh.spinOnce();  
  delay(1);
}
