#!/usr/bin/env python
import rospy
import math
from pid import Pid
from unwrapper import Unwrapper
from geometry_msgs.msg import Twist
from geometry_msgs.msg import Pose
from nav_msgs.msg import Odometry
from std_msgs.msg import Bool
from drive_by_wire.msg import Cart_values
from tf.transformations import euler_from_quaternion
from sensor_msgs.msg import Imu


stamp = 0.0
prev = 0.0
imu_started = False

def callback_vel(data):
    # constant = 1
    global vel_setp, pid_vel
    vel_setp = data
    pid_vel.reset_accum()
    # linear_vel = data.linear.x
    # angular_vel = data.angular.z
    # val.throttle = linear_vel
    # val.brake = data.linear.z
    # if linear_vel != 0:
    #     val.steering_angle = constant * (angular_vel / linear_vel)
    # else:
    #     val.steering_angle = 0
    # rospy.loginfo(val.steering_angle)

def callback_pos(data):
    global pos_setp, pid_turn
    pos_setp = data
    pid_turn.reset_accum()

def callback_odom(data):
    global vel_curr, pos_curr, stamp, prev, imu_started
    if imu_started:
        vel_curr = data.twist.twist
        pos_curr = data.pose.pose
        rpy = euler_from_quaternion([pos_curr.orientation.x,
            pos_curr.orientation.y,
            pos_curr.orientation.z,
            pos_curr.orientation.w])
        rpy = (rpy[0], rpy[1], -1*unwrapper.unwrap(rpy[2]))
        # rospy.loginfo(rpy)
        prev = stamp
        stamp = data.header.stamp.to_sec()
        elapsed = stamp - prev
        if elapsed != 0:
            # val.throttle = pid_vel.pid(0, vel_setp.linear.x, stamp)
            # val.throttle = 0.0113*vel_setp.linear.x**5 + -0.1550*vel_setp.linear.x**4 + 0.8438*vel_setp.linear.x**3 + -2.3792*vel_setp.linear.x**2 + 4.0388*vel_setp.linear.x + 0.7273
            val.throttle = vel_setp.linear.x
            # if vel_curr.linear.x != 0:
            # val.steering_angle = round(pid_turn.pid(pos_curr.orientation.z, pos_setp.orientation.z, stamp)/vel_curr.linear.x)
            # val.steering_angle = round(pid_turn.pid(pos_curr.orientation.z*math.pi, pos_setp.orientation.z, stamp))

            # rpy[2] = <current yaw> |||| pos_setp.orientation.z = <commanded yaw> (pos_setp is setpoint)
            val.steering_angle = -1*round(pid_turn.pid(rpy[2], pos_setp.orientation.z, stamp))
            if not (vel_setp.linear.x == 0):
                val.steering_angle = val.steering_angle / vel_setp.linear.x
        rospy.loginfo(rpy[2])

        pub.publish(val)

def callback_imu(data):
    global imu_started
    imu_started = True
    rpy = euler_from_quaternion([data.orientation.x,
        data.orientation.y,
        data.orientation.z,
        data.orientation.w])

def callback_manual(data):
    global val
    val.is_manual = data.data

rospy.Subscriber('imu', Imu, callback_imu)
pub = rospy.Publisher('Arduino_commands', Cart_values, queue_size=10)
rospy.init_node('hardware_controller', anonymous=True)
rospy.Subscriber('cmd_vel', Twist, callback_vel)
rospy.Subscriber('cmd_pose',Pose, callback_pos)
rospy.Subscriber("odom", Odometry, callback_odom)
rospy.Subscriber('manual', Bool, callback_manual)
rate = rospy.Rate(30)
val = Cart_values()
vel_setp = Twist()
vel_curr = Twist()
pos_setp = Pose()
pos_curr = Pose()

kp_vel = rospy.get_param('kp_vel')
ki_vel = rospy.get_param('ki_vel')
kd_vel = rospy.get_param('kd_vel')
kp_turn = rospy.get_param('kp_turn')
ki_turn = rospy.get_param('ki_turn')
kd_turn = rospy.get_param('kd_turn')
pid_vel = Pid(kp_vel, ki_vel, kd_vel)
pid_turn = Pid(kp_turn, ki_turn, kd_turn)
unwrapper = Unwrapper(math.pi)

def hardware_controller():
    while not rospy.is_shutdown():
        rate.sleep()

if __name__ == '__main__':
    try:
        hardware_controller()
    except rospy.ROSInterruptException:
        pass