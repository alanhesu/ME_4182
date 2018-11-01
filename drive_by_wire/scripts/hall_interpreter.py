#!/usr/bin/env python
import rospy
import time
import math
from nav_msgs.msg import Odometry
from std_msgs.msg import Bool

def callback_tick(data):
    global count, prev_time_tick, elapsed_time_tick
    if data:
        count = count + 1
        elapsed_time_tick = rospy.get_time() - prev_time_tick
        prev_time_tick = rospy.get_time()


pub = rospy.Publisher('odom', Odometry, queue_size=10)
rospy.init_node('hall_interpreter', anonymous=True)
rospy.Subscriber("tick", Bool, callback_tick)
rate = rospy.Rate(10)

count = 0
prev_time = 0.0
prev_time_tick = 0.0
elapsed_time_tick = 0.0
wheel_radius = .42
odom = Odometry()

def hall_interpreter():
    global prev_time, count

    while not rospy.is_shutdown():
        curr_time = rospy.get_time()
        if curr_time - prev_time >= .5:
            # rps = count/4.0/(curr_time - prev_time)
            if elapsed_time_tick == 0:
                rps = 0
            else:
                rps = .25/elapsed_time_tick
            vel = rps*wheel_radius*2*math.pi
            odom.header.stamp = rospy.Time.from_sec(curr_time)            
            odom.twist.twist.linear.x = vel
            pub.publish(odom)
            # rospy.loginfo(vel)            
            count = 0
            prev_time = curr_time
        rate.sleep()

if __name__ == '__main__':
    try:
        hall_interpreter()
    except rospy.ROSInterruptException:
        pass