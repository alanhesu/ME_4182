#!/usr/bin/env python
import rospy
import rosbag
import time
import math
from energy_logger.msg import coulomb_counter_vals

def callback_counter(data):
    global voltage, current, power, charge, energy, temperature
    #the 6 values we get out of the Coulomb Counter topic

    global bag
    #So we can log the messages

    voltage = data.voltage
    current = data.current
    power = data.current
    charge = data.charge
    energy = data.energy
    temperature = data.temperature

    bag.write('EnergyData',mdata)

    rospy.loginfo((voltage, current, power, charge, energy, temperature))


bag = rosbag.Bag('test.bag','w')

rospy.Subscriber('Energy_Readings', coulomb_counter_vals, callback_counter)  # setup sub to get coulomb counter

