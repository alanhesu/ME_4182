#!/usr/bin/env python
import rospy
import time
import math
from unwrapper import Unwrapper
from geometry_msgs.msg import Twist
from geometry_msgs.msg import Pose
from std_msgs.msg import Int32
from nav_msgs.msg import Odometry
from tf.transformations import euler_from_quaternion
from sensor_msgs.msg import PointCloud2
import sensor_msgs.point_cloud2 as pc2

def callback_tick(data):
    global ind, start, time_a, count
    if ind == 2 and not start:
        start = True
        time_a = rospy.get_time()
    elif ind == 3 and start:
        rps = 2.23*(1.0/2312)*count/(rospy.get_time() - time_a) # 4 ticks per revolution
        vel = rps*.215*2*math.pi
        # rospy.loginfo(vel)
        start = False

    if start:
        count += data.data

# def callback_scan(data):
#     global scan
    # rospy.loginfo(len(data.ranges))

def callback_cloud(data):
    global ind, duration, chassis_width, obj_thresh, obj_accum, stop_dist
    ll = [0, -1*chassis_width/2]
    ur = [stop_dist, chassis_width/2]
    rospy.loginfo(obj_accum)
    if obj_accum > 0:
        obj_accum -= 3       
    for p in pc2.read_points(data, field_names = ('x', 'y', 'z', 'intensity', 'index'), skip_nans=True):
        # rospy.loginfo(p)
        p = (-1*p[0], p[1], p[2], p[3], p[4])
        if p[0] > ll[0] and p[0] < ur[0] and p[1] > ll[1] and p[1] < ur[1]:          
            rospy.loginfo(p[0])
            obj_accum += 1
            if obj_accum >= obj_thresh:
                rospy.loginfo('Object threshold reached')
                #ind = len(plan) + 1
                #duration = 0
            # rospy.loginfo('Close point detected!')
            # rospy.loginfo(p)

def callback_odom(data):
    global pose_curr, vel_curr
    pose_curr = data.pose.pose
    vel_curr = data.twist.twist

pub_twist = rospy.Publisher('cmd_vel', Twist, queue_size=10)
pub_pose = rospy.Publisher('cmd_pose', Pose, queue_size=10)
rospy.init_node('dumb_navigation_planner', anonymous=True)
rospy.Subscriber("tick", Int32, callback_tick)
rospy.Subscriber('odom', Odometry, callback_odom)
# rospy.Subscriber('scan_filtered', LaserScan, callback_scan)
rospy.Subscriber('cloud_filtered', PointCloud2, callback_cloud)
rate = rospy.Rate(30)

val = Twist()
pos = Pose()
pose_curr = Pose()
vel_curr = Twist()
fin = True
ind = -1
start = False
count = 0
chassis_width = 1.143
chassis_width = rospy.get_param('chassis_width')
stop_dist = .5
stop_dist = rospy.get_param('stopping_distance')
obj_thresh = 3
obj_accum = 0

unwrapper = Unwrapper(math.pi)

if rospy.has_param('dumb_plan'):
    plan = rospy.get_param('dumb_plan')
else:
    rospy.signal_shutdown('No plan found!')

def dumb_navigation_planner():
    global prev_time, fin, val, pos, duration, ind, pose_curr

    while not rospy.is_shutdown():
        if fin:
            prev_time = rospy.get_time()
            ind += 1
            if ind*3 + 3 > len(plan):
                val.linear.x = -5
                duration = 1
                rospy.loginfo('plan ended')
                pub_twist.publish(val)
                break
            else:
                rpy = euler_from_quaternion([pose_curr.orientation.x,
                    pose_curr.orientation.y,
                    pose_curr.orientation.z,
                    pose_curr.orientation.w])
                rpy = (rpy[0], rpy[1], -1*unwrapper.unwrap(rpy[2]))
                val.linear.x = plan[ind*3]
                #<heading sent to be actually interpreted> = <commanded heading> + <current heading>
                pos.orientation.z = plan[ind*3 + 1] + rpy[2]
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