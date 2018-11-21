#!/usr/bin/env python
import rospy
from pid import Pid
from geometry_msgs.msg import Twist
from nav_msgs.msg import Odometry
from drive_by_wire.msg import Cart_values

stamp = 0.0
prev = 0.0

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
    global vel_curr, stamp, prev
    vel_curr = data.twist.twist
    prev = stamp
    stamp = data.header.stamp.to_sec()
    elapsed = stamp - prev
    if elapsed != 0:
        val.throttle = pid_vel.pid(vel_curr.linear.x, vel_setp.linear.x, stamp)
        if vel_curr.linear.x != 0:
            val.steering_angle = round(pid_turn.pid(vel_curr.angular.z, vel_setp.angular.z, stamp)/vel_curr.linear.x)
    pub.publish(val)

pub = rospy.Publisher('Arduino_commands', Cart_values, queue_size=10)
rospy.init_node('hardware_controller', anonymous=True)
rospy.Subscriber("cmd_vel", Twist, callback_keyboard)
rospy.Subscriber("odom", Odometry, callback_odom)
rate = rospy.Rate(30)
val = Cart_values()
vel_setp = Twist()
vel_curr = Twist()

kp_vel = rospy.get_param('kp_vel')
ki_vel = rospy.get_param('ki_vel')
kd_vel = rospy.get_param('kd_vel')
kp_turn = rospy.get_param('kp_turn')
ki_turn = rospy.get_param('ki_turn')
kd_turn = rospy.get_param('kd_turn')
pid_vel = Pid(kp_vel, ki_vel, kd_vel)
pid_turn = Pid(kp_turn, ki_turn, kd_turn)

def hardware_controller():
    while not rospy.is_shutdown():
        rate.sleep()

if __name__ == '__main__':
    try:
        hardware_controller()
    except rospy.ROSInterruptException:
        pass