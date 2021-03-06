import rospy
import tf

if __name__ == '__main__':
    rospy.init_node('imu_tf')
    #initializes a node called 'imu_tf'

    br = tf.TransformBroadcaster()
    #initializes the transform broadcaster as br.
    while not rospy.is_shutdown():
        br.sendTransform((0.0, 2.0, 0.0),
                         (0.0, 0.0, 0.0, 1.0),
                         rospy.Time.now(),
                         "imu",
                         "base_cart")
        #sends a static transform between the from the cart frame to
        #the imu frame
        rate = rospy.Rate(10.0)
        rate.sleep()