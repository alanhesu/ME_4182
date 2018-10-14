#!/usr/bin/env python
import rospy
from geometry_msgs import Twist
from drive_by_wire import cart_values


def callback(data):
    constant = 1

    global val
    linear_vel = data.linear.x
    angular_vel = data.angular.z
    val.steering_angle = constant * (angular_vel / linear_vel)
    rospy.loginfo(val.steering_angle)

def hardware_controller():
    pub = rospy.Publisher('Arduino_commands', cart_values, queue_size=10)
    rospy.init_node('hardware_controller', anonymous=True)
    rospy.Subscriber("keyboard", Twist, callback)
    rate = rospy.Rate(10)
    while not rospy.is_shutdown():
        val = cart_values()
        rospy.loginfo(val)
        pub.publish(val)
        rate.sleep()
        
if __name__ == '__main__':
    try:
        hardware_controller()
    except rospy.ROSInterruptException:
        pass