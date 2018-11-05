#!/usr/bin/env python
import rospy
from geometry_msgs.msg import Twist
from nav_msgs.msg import Odometry
from drive_by_wire.msg import Cart_values

def callback_keyboard(data):
    # constant = 1
    global vel_setp
    vel_setp = data
    # linear_vel = data.linear.x
    # angular_vel = data.angular.z
    # val.throttle = linear_vel
    # val.brake = data.linear.z
    # if linear_vel != 0:
    #     val.steering_angle = constant * (angular_vel / linear_vel)
    # else:
    #     val.steering_angle = 0
    # rospy.loginfo(val.steering_angle)

def callback_odom(data):
    global vel_curr, stamp
    vel_curr = data.twist.twist
    stamp = data.header.stamp.to_sec()

def vel_pid():
    global stamp, prev, accum, kp, kd, ki, val
    error = vel_setp.linear.x - vel_curr.linear.x
    elapsed = stamp - prev
    if elapsed == 0:
        # rospy.loginfo("elapsed time is zero!")
        return
    accum += error

    val.throttle = kp*error + kd*error/elapsed + ki*accum
    prev = stamp
    # val.throttle = vel_setp.linear.x
    rospy.loginfo(val.throttle)

pub = rospy.Publisher('Arduino_commands', Cart_values, queue_size=10)
rospy.init_node('hardware_controller', anonymous=True)
rospy.Subscriber("keyboard", Twist, callback_keyboard)
rospy.Subscriber("odom", Odometry, callback_odom)
rate = rospy.Rate(10)
val = Cart_values()
vel_setp = Twist()
vel_curr = Twist()

stamp = .001
prev = 0
accum = 0
kp = 1
kd = 2
ki = .1

def hardware_controller():
    while not rospy.is_shutdown():
        vel_pid()
        pub.publish(val)
        rate.sleep()

if __name__ == '__main__':
    try:
        hardware_controller()
    except rospy.ROSInterruptException:
        pass