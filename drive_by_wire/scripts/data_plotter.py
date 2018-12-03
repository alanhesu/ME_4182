#!/usr/bin/env python
from PyQt5 import QtGui
import pyqtgraph as pg
import numpy as np
import time
import plotutils
from datamgmt import CartData
import rospy
from drive_by_wire.msg import coulomb_counter_vals
from nav_msgs.msg import Odometry
from tf.transformations import euler_from_quaternion


vals = dict()
titles = ['Voltage (V)','Current (A)','Power (W)', 'Charge (mAh)', 'Energy (J)', 'Velocity (m/s)', 'Angle (degrees)']
for cate in titles:
	vals[cate] = 0

CData = CartData(titles, 200)

#Initialize data
properties = list(titles)
idr = 0

#Initialize plots
pg.setConfigOption('background', 'w')
pg.setConfigOption('foreground', 'k')
app = QtGui.QApplication([])
w = QtGui.QWidget()
layout = QtGui.QGridLayout()
w.setLayout(layout)


buttonsWidget, optionsarr = plotutils.buttonArray(titles)
layout.addWidget(buttonsWidget, 0, 0, 1, 2)

samplelabel = QtGui.QLabel('Number of samples to show: ')
samplesnum = QtGui.QLineEdit('100')
layout.addWidget(samplelabel, 1, 0, 1, 1)
layout.addWidget(samplesnum, 1, 1, 1, 1)

plotpacks = plotutils.setPlots(layout, properties, 0, 2)

#display the plots



def update(updated_categories):
	global idr, samplesnum, CData, optionsarr, titles, properties, plotpacks
	if samplesnum.text().isdigit():
		CData.setSampleLimit(int(samplesnum.text()))

	for idx, opt in enumerate(optionsarr):
		if opt.isChecked() and titles[idx] not in properties:
			properties.append(titles[idx])
			plotutils.clearLayout(layout)
			plotpacks = plotutils.setPlots(layout, properties, 0, 2)
		elif not opt.isChecked() and titles[idx] in properties:
			properties.remove(titles[idx])
			plotutils.clearLayout(layout)
			plotpacks = plotutils.setPlots(layout, properties, 0, 2)


	for cate in updated_categories:
		dpoint = vals[cate]
		CData.addTime(cate, idr)
		CData.addData(cate, dpoint)

	for idx, cate in enumerate(properties):
		plotutils.updatePlot(plotpacks[idx], CData.getTime(cate), CData.getCategory(cate), (idx, 7))

	QtGui.QApplication.processEvents()
	idr += 1


def callback_odom(data):
	global vals, titles
	pos = data.pose.pose
	rpy = euler_from_quaternion([pos.orientation.x, pos.orientation.y, pos.orientation.z, pos.orientation.w])
	vals[titles[5]] = data.twist.twist.linear.x
	vals[titles[6]] = rpy[2]
	

def callback_energy(data):
	global vals, titles
	vals[titles[0]] = data.voltage
	vals[titles[1]] = data.current
	vals[titles[2]] = data.power
	vals[titles[3]] = data.charge
	vals[titles[4]] = data.energy
	


rospy.init_node('data_plotter', anonymous=True)
rospy.Subscriber('energy', coulomb_counter_vals, callback_energy)
rospy.Subscriber('odom', Odometry, callback_odom)
rate = rospy.Rate(30)


def data_plotter():
	global itr, w, app, titles, vals
	w.show()
	while not rospy.is_shutdown():
		rospy.loginfo(vals)
		update(titles)
		rate.sleep()

	#Always exec Qt
	app.exec_()

if __name__ == '__main__':
	try:
		data_plotter()
	except rospy.ROSInterruptException:
 		pass