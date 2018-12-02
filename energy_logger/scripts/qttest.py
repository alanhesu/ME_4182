from PyQt5 import QtGui  # (the example applies equally well to PySide)
import pyqtgraph as pg
import numpy as np
import time


pg.setConfigOption('background', 'w')
pg.setConfigOption('foreground', 'k')


## Always start by initializing Qt (only once per application)
app = QtGui.QApplication([])

## Define a top-level widget to hold everything
w = QtGui.QWidget()

## Create some widgets to be placed inside
vtext = QtGui.QLabel('Voltage: NOT READY')
ctext = QtGui.QLabel('Current: NOT READY')
chtext = QtGui.QLabel('Charge: NOT READY')


voltage = pg.PlotWidget(title='Voltage')
voltage.addLegend()

voltage.showGrid(x = True, y = True, alpha = 0.5)
current = pg.PlotWidget(title='Current')
charge = pg.PlotWidget(title='Charge')

## Create a grid layout to manage the widgets size and position
layout = QtGui.QGridLayout()
w.setLayout(layout)

## Add widgets to the layout in their proper positions
layout.addWidget(voltage, 0, 1, 1, 1)  # plot goes on right side, spanning 3 rows
layout.addWidget(vtext, 0, 0)
layout.addWidget(current, 1, 1, 1, 1)  # plot goes on right side, spanning 3 rows
layout.addWidget(ctext, 1, 0)
layout.addWidget(charge, 2, 1, 1, 1)  # plot goes on right side, spanning 3 rows
layout.addWidget(chtext, 2, 0)

## Display the widget as a new window
w.show()

for i in range(0,3):
	voltage.plot(pen = (i,3), name = 'voltage')

x = np.arange(1000)
while True:
	time.sleep(0.2)
	y = np.random.normal(size = (3, 1000))
	voltage.plot(clear = True)
	for i in range(0,3):
		voltage.plot(x = x, y = y[i], pen = (i,3)) 

	#text.setText('<b>' + str(y[0][0]) + '</b>')

	QtGui.QApplication.processEvents()

## Start the Qt event loop
app.exec_()