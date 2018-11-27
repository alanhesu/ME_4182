#!/usr/bin/env python
import rospy
import time
import math
from geometry_msgs.msg import Twist
from geometry_msgs.msg import Pose
from std_msgs.msg import Int32

def callback_tick(data):
    global ind, start, time_a, count
    if ind == 2 and not start:
        start = True
        time_a = rospy.get_time()
    elif ind == 3 and start:
        rps = 2.23*(1.0/2312)*count/(rospy.get_time() - time_a) # 4 ticks per revolution                        
        vel = rps*.215*2*math.pi
        rospy.loginfo(vel)
        start = False

    if start:
        count += data.data

pub_twist = rospy.Publisher('cmd_vel', Twist, queue_size=10)
pub_pose = rospy.Publisher('cmd_pose', Pose, queue_size=10)
rospy.init_node('dumb_navigation_planner', anonymous=True)
rospy.Subscriber("tick", Int32, callback_tick)
rate = rospy.Rate(30)
val = Twist()
pos = Pose()
fin = True
ind = -1
start = False
count = 0

if rospy.has_param('dumb_plan'):
    plan = rospy.get_param('dumb_plan')
else:
    rospy.signal_shutdown('No plan found!')

def dumb_navigation_planner():
    global prev_time, fin, val, pos, duration, ind

    while not rospy.is_shutdown():
        if fin:
            prev_time = rospy.get_time()
            ind += 1
            if ind*3 + 3 > len(plan):
                val.linear.x = 0
                duration = 1
                rospy.loginfo('plan ended')
                pub_twist.publish(val)
                break
            else:
                val.linear.x = plan[ind*3]
                pos.orientation.z = plan[ind*3 + 1]
                duration = plan[ind*3 + 2]
            rospy.loginfo('Linear velocity of {} and orientation of {} for {} seconds'.format(val.linear.x, pos.orientation.z, duration))
            pub_twist.publish(val)
            pub_pose.publish(pos)
            fin = False
        else:
            if rospy.get_time() - prev_time >= duration:
                fin = True
        rate.sleep()

if __name__ == '__main__':
    try:
        dumb_navigation_planner()
    except rospy.ROSInterruptException:
        pass