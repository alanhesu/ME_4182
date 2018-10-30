#!/usr/bin/env python
import rospy
import time
import math
from nav_msgs.msg import Odometry
from std_msgs.msg import Bool

def callback_tick(data):
    global count
    if data:
        count = count + 1

pub = rospy.Publisher('odom', Odometry, queue_size=10)
rospy.init_node('hall_interpreter', anonymous=True)
rospy.Subscriber("tick", Bool, callback_tick)
rate = rospy.Rate(10)

count = 0
prev_time = 0.0
wheel_radius = .42
odom = Odometry()

def hall_interpreter():

    while not rospy.is_shutdown():
        curr_time = time.time()
        if curr_time - prev_time > .5:
            rps = count/4.0/(curr_time - prev_time)
            vel = rps*wheel_radius*2*math.pi
            odom.header.stamp.secs = curr_time
            odom.header.stamp.nsecs = (curr_time - math.floor(curr_time))*1000000000
            odom.twist.twist.linear.x = vel
            pub.publish(odom)
            rospy.loginfo(count)
        rate.sleep()

if __name__ == '__main__':
    try:
        hall_interpreter()
    except rospy.ROSInterruptException:
        pass