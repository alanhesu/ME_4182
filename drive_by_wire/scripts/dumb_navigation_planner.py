#!/usr/bin/env python
import rospy
import time
import math
from geometry_msgs.msg import Twist

pub = rospy.Publisher('cmd_vel', Twist, queue_size=10)
rospy.init_node('dumb_navigation_planner', anonymous=True)
rate = rospy.Rate(30)
val = Twist()
fin = True
ind = -1

if rospy.has_param('dumb_plan'):
    plan = rospy.get_param('dumb_plan')
else:
    rospy.signal_shutdown('No plan found!')

def dumb_navigation_planner():
    global prev_time, fin, val, duration, ind

    while not rospy.is_shutdown():
        if fin:
            prev_time = rospy.get_time()
            ind += 1
            if ind*3 + 3 > len(plan):
                val.linear.x = 0
                val.angular.z = 0
                duration = 1
                rospy.loginfo('plan ended')
                pub.publish(val)
                break
            else:
                val.linear.x = plan[ind*3]
                val.angular.z = plan[ind*3 + 1]
                duration = plan[ind*3 + 2]
            rospy.loginfo('Linear velocity of {} and angular velocity of {} for {} seconds'.format(val.linear.x, val.angular.z, duration))
            pub.publish(val)
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