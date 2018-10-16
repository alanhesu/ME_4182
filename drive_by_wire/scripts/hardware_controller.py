#!/usr/bin/env python
import rospy
from geometry_msgs.msg import Twist
from drive_by_wire.msg import Cart_values

def callback(data):
    constant = 1

    global val
    linear_vel = data.linear.x
    angular_vel = data.angular.z
    val.throttle = linear_vel
    val.brake = data.linear.z
    if linear_vel != 0:
        val.steering_angle = constant * (angular_vel / linear_vel)
    else:
        val.steering_angle = 0
    rospy.loginfo(val.steering_angle)

pub = rospy.Publisher('Arduino_commands', Cart_values, queue_size=10)
rospy.init_node('hardware_controller', anonymous=True)
rospy.Subscriber("keyboard", Twist, callback)
rate = rospy.Rate(10)
val = Cart_values()

def hardware_controller():
    while not rospy.is_shutdown():
        rospy.loginfo(val)
        pub.publish(val)
        rate.sleep()

if __name__ == '__main__':
    try:
        hardware_controller()
    except rospy.ROSInterruptException:
        pass