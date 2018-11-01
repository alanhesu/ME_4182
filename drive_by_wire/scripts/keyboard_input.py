#!/usr/bin/env python

import rospy
from std_msgs.msg import String
from geometry_msgs.msg import Twist
# import keyboard

pub = rospy.Publisher('keyboard', Twist, queue_size=10)
rospy.init_node('keyboard_input', anonymous=True)
rate = rospy.Rate(10) # 10hz
vel_msg = Twist()
vel_max_linear = 5
min_turn_radius = 3
vel_max_angular = vel_max_linear * min_turn_radius
vel_linear_inc = 3
vel_angular_inc = .1

def keyboard_input():
    global vel_msg
    while not rospy.is_shutdown():
        text = raw_input()
        if 'w' in text:
            if vel_msg.linear.x < vel_max_linear:
                vel_msg.linear.x += vel_linear_inc
        if 's' in text:
            if vel_msg.linear.x > -1*vel_max_linear:
                vel_msg.linear.x -= vel_linear_inc
        if 'd' in text:
            if vel_msg.angular.z > -1*vel_max_angular:
                vel_msg.angular.z -= vel_angular_inc
        if 'a' in text:
            if vel_msg.angular.z < vel_max_angular:
                vel_msg.angular.z += vel_angular_inc
        if 'b' in text:
            vel_msg.linear.z = 3
            vel_msg.linear.x = 0
        if 'n' in text:
            vel_msg.linear.z = 0
        pub.publish(vel_msg)
        rospy.loginfo(vel_msg)
        rate.sleep()

    vel_msg.linear.x = 0
    vel_msg.linear.z = 0
    vel_msg.angular.z = 0
    pub.publish(vel_msg)


if __name__ == '__main__':
    try:
        keyboard_input()
    except rospy.ROSInterruptException:
        pass