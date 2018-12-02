from PyQt5 import QtGui
import pyqtgraph as pg
import numpy as np
import time
import plotutils

#Initialize
pg.setConfigOption('background', 'w')
pg.setConfigOption('foreground', 'k')
app = QtGui.QApplication([])
w = QtGui.QWidget()
layout = QtGui.QGridLayout()
w.setLayout(layout)

titles = ['Voltage', 'Current', 'Charge', 'Power']
plotpacks = plotutils.setPlots(layout, titles)

#display the plots
w.show()

#main loop
x = np.arange(1000)
while True:
	time.sleep(0.2)
	y = np.random.normal(size = 1000)
	plotutils.updatePlots(plotpacks, [x], [y,y,y,y], 20)
	QtGui.QApplication.processEvents()

#Always exec Qt
app.exec_()