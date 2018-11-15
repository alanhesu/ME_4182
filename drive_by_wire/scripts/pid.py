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
        self.t_prev = 0
        self.e_prev = 0
        self.pid_prev = 0

    def pid(self, curr, setp, stamp):
        elapsed = stamp - self.t_prev
        if elapsed == 0:
            return self.pid_prev
        error = setp - curr
        de = (error - self.e_prev)/elapsed
        ie = (error + self.e_prev) * elapsed / 2

        self.accum += ie
        self.t_prev = stamp
        self.e_prev = error
        self.pid_prev = kp*error + kd*de + ki*self.accum
        return self.pid_prev