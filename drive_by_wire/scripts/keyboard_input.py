#!/usr/bin/env python

import rospy
from std_msgs.msg import String
from geometry_msgs.msg import Twist
from drive_by_wire.msg import Cart_values
from std_msgs.msg import Bool

# import keyboard

pub = rospy.Publisher('manual', Bool, queue_size=10)
rospy.init_node('keyboard_input', anonymous=True)
rate = rospy.Rate(30) # 10hz
val = Bool()
val.data = False

def keyboard_input():
    global val
    while not rospy.is_shutdown():
        text = raw_input()
        # if 'w' in text:
        #     if vel_msg.linear.x < vel_max_linear:
        #         vel_msg.linear.x += vel_linear_inc
        # if 's' in text:
        #     if vel_msg.linear.x > -1*vel_max_linear:
        #         vel_msg.linear.x -= vel_linear_inc
        # if 'd' in text:
        #     if vel_msg.angular.z > -1*vel_max_angular:
        #         vel_msg.angular.z -= vel_angular_inc
        # if 'a' in text:
        #     if vel_msg.angular.z < vel_max_angular:
        #         vel_msg.angular.z += vel_angular_inc
        if 'b' in text:
            val.data = not val.data
        # if 'n' in text:
        #     vel_msg.linear.z = 0
        pub.publish(val)
        rate.sleep()

if __name__ == '__main__':
    try:
        keyboard_input()
    except rospy.ROSInterruptException:
        pass