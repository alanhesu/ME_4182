#!/usr/bin/env python
import rospy
from laser_geometry import LaserProjection
from sensor_msgs.msg import PointCloud2
from sensor_msgs.msg import LaserScan

def callback_scan(data):
    global projector
    pub.publish(projector.projectLaser(data))

rospy.Subscriber('scan_filtered', LaserScan, callback_scan)
pub = rospy.Publisher('cloud_filtered', PointCloud2, queue_size=10)
rospy.init_node('scan_to_cloud', anonymous=True)
rate = rospy.Rate(30)
projector = LaserProjection()

def scan_to_cloud():
    while not rospy.is_shutdown():
        rate.sleep()

if __name__ == '__main__':
    try:
        scan_to_cloud()
    except rospy.ROSInterruptException:
        pass