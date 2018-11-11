#!/usr/bin/env python
import rospy
import time
import math
from geometry_msgs.msg import TwistWithCovarianceStamped
from std_msgs.msg import Bool

def callback_tick(data):
    global count, prev_time_tick, elapsed_time_tick
    if data:
        count = count + 1
        elapsed_time_tick = rospy.get_time() - prev_time_tick
        prev_time_tick = rospy.get_time()


pub = rospy.Publisher('twist0', TwistWithCovarianceStamped, queue_size=10)
rospy.init_node('hall_interpreter', anonymous=True)
rospy.Subscriber("tick", Bool, callback_tick)
rate = rospy.Rate(30)

count = 0
prev_time = 0.0
prev_time_tick = 0.0
elapsed_time_tick = 0.0
wheel_radius = .42
wheel_encoder = TwistWithCovarianceStamped()
if rospy.has_param('twist0_covariance'):
    wheel_encoder.twist.covariance = rospy.get_param('twist0_covariance')
vel_dumb = 0

def hall_interpreter():
    global prev_time, count, vel_dumb

    while not rospy.is_shutdown():
        if elapsed_time_tick == 0:
            rps = 0
        else:
            rps = .25/elapsed_time_tick # 4 ticks per revolution
        vel = rps*wheel_radius*2*math.pi
        wheel_encoder.header.stamp = rospy.Time.now()
        wheel_encoder.twist.twist.linear.x = vel
        vel_dumb += .05
        wheel_encoder.twist.twist.linear.x = vel_dumb
        count = 0
        pub.publish(wheel_encoder)
        rate.sleep()

if __name__ == '__main__':
    try:
        hall_interpreter()
    except rospy.ROSInterruptException:
        pass