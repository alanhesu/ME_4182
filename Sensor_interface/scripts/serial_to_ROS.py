import rospy
import serial
from nmea_msgs.msg import Sentence
# Sentence isn't a ros message-- may have to create one???

ser = serial.Serial() #change
print(ser.name)
ser.baudrate = 19200
ser.port = 'COM1'
def ():
    pub = rospy.Publisher('GPS_msg', Sentence, queue_size=10)
    rospy.init_node('serial_to_ROS', anonymous=True)
    rospy.Rate(10)

    ##do stuff
    serial_line = ser.readline() #unsure of how to implement timeout
    msg = Sentence()
    msg.sentence = serial_line
    pub.publish(msg)

if __name__ == '__main__':
    try:
        serial_to_ROS()
    except rospy.ROSInterruptException:
        pass