#!/usr/bin/env python
from PyQt5 import QtGui
import pyqtgraph as pg
import numpy as np
import time
import plotutils
from datamgmt import CartData
import rospy


def update(updated_categories):
	global itr, samplesnum, CData, optionsarr, titles, properties, plotpacks
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
		dpoint = np.random.normal(size = 1)[0]

		CData.addTime(cate, itr)
		CData.addData(cate, dpoint)

	for idx, cate in enumerate(properties):
		plotutils.updatePlot(plotpacks[idx], CData.getTime(cate), CData.getCategory(cate), (idx, 7))
	itr += 1

	QtGui.QApplication.processEvents()


titles = ['Voltage (V)','Current (A)','Power (W)', 'Charge (mAh)', 'Energy (J)', 'Velocity (m/s)', 'Angle (degrees)']
CData = CartData(titles, 200)

#Initialize data
properties = titles.copy()


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
samplesnum = QtGui.QLineEdit('1000')
layout.addWidget(samplelabel, 1, 0, 1, 1)
layout.addWidget(samplesnum, 1, 1, 1, 1)

plotpacks = plotutils.setPlots(layout, properties, 0, 2)

#display the plots

itr = 0

def data_plotter():
	global itr, w, app, titles

	w.show()
	while not rospy.is_shutdown():
		update(titles)
	
	#Always exec Qt
	app.exec_()

if __name__ == '__main__':
	try:
		data_plotter()
	except rospy.ROSInterruptException:
 		pass