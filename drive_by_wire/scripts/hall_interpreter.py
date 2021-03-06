#!/usr/bin/env python
import rospy
import time
import math
from geometry_msgs.msg import TwistWithCovarianceStamped
from std_msgs.msg import Int32

def callback_tick(data):
    global count, prev_time_tick, elapsed_time_tick
    if data:
        count = data.data
        # elapsed_time_tick += rospy.get_time() - prev_time_tick
        # prev_time_tick = rospy.get_time()
        # rospy.loginfo(count)


pub = rospy.Publisher('twist0', TwistWithCovarianceStamped, queue_size=10)
rospy.init_node('hall_interpreter', anonymous=True)
rospy.Subscriber("tick", Int32, callback_tick)
rate = rospy.Rate(30)

count = 0
prev_time = 0.0
prev_time_tick = 0.0
elapsed_time_tick = 0.0
wheel_radius = .215
wheel_radius = rospy.get_param('wheel_radius')
wheel_encoder = TwistWithCovarianceStamped()
wheel_encoder.header.frame_id = 'base_link'
if rospy.has_param('twist0_covariance'):
    wheel_encoder.twist.covariance = rospy.get_param('twist0_covariance')
vel_dumb = 0

def hall_interpreter():
    global prev_time_tick, count, vel_dumb, elapsed_time_tick

    while not rospy.is_shutdown():
        elapsed_time_tick = rospy.get_time() - prev_time_tick
        if elapsed_time_tick == 0:
            rps = 0
        # elif count == 0 and rospy.get_time() - prev_time_tick > elapsed_time_tick:
        #     rps = .25/(rospy.get_time() - prev_time_tick)
        else:
            rps = 2.23*(1.0/2312)*count/elapsed_time_tick # 4 ticks per revolution                        
        vel = rps*wheel_radius*2*math.pi
        wheel_encoder.header.stamp = rospy.Time.now()
        wheel_encoder.twist.twist.linear.x = vel        
        prev_time_tick = rospy.get_time()    
        pub.publish(wheel_encoder)
        rate.sleep()

if __name__ == '__main__':
    try:
        hall_interpreter()
    except rospy.ROSInterruptException:
        pass