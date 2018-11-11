import rospy
from geometry_msgs.msg import Twist
from nav_msgs.msg import Odometry

class Pid:
    def __init__(self, kp, ki, kd):
        self.kp = kp
        self.ki = ki
        self.kd = kd
        self.stamp = 0
        self.accum = 0
        self.prev = 0

    def pid(self, curr, setp, stamp):
        error = setp - curr
        elapsed = self.stamp - self.prev
        if elapsed == 0:
            return
        self.accum += error
        self.prev = self.stamp
        return kp*error + kd*error/elapsed + ki*accum