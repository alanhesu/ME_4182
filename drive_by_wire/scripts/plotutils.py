from PyQt5 import QtGui
import pyqtgraph as pg
import numpy as np
import time

def buttonArray(categories):
	buttons = QtGui.QWidget()
	buttonslayout = QtGui.QHBoxLayout(buttons)
	buttonarray = []
	for category in categories:
		button = QtGui.QCheckBox(category)
		button.setChecked(True)
		buttonslayout.addWidget(button)
		buttonarray.append(button)

	return buttons, buttonarray

def setPlot(layout, title, idx, col):
	text = QtGui.QLabel(title + ': NOT READY')
	plot = pg.PlotWidget(title = title)
	plot.showGrid(x = True, y = True, alpha = 0.3)

	layout.addWidget(plot, idx, col+1, 1, 1)
	layout.addWidget(text, idx, col)

	plotpack = (plot, text, title)

	return plotpack

def updatePlot(plot_pack, x, y, color, texlen = 30):
	plot = plot_pack[0]
	text = plot_pack[1]
	title = plot_pack[2]
	# print(y)
	text.setText((title + ": {0:03.4f}".format(y[-1])).ljust(texlen))
	plot.plot(x = x, y = y, clear = True, pen = color)


def setPlots(layout, titles, col, offset):
	plotpacks = []
	for idx in range(0,len(titles)):
		plotpacks.append(setPlot(layout, titles[idx], offset+idx, col))

	plotpacks[-1][0].setLabel('bottom', "Time(s)")	

	return plotpacks

def updatePlots(plotpacks, xs, ys, texlen = 30):
	numplots = len(plotpacks)
	if len(xs) == 1 and numplots > 1:
		for idx, plotpack in enumerate(plotpacks):
			updatePlot(plotpack, xs[0], ys[idx], (idx,numplots), texlen)
	else:
		for idx, plotpack in enumerate(plotpacks):
			updatePlot(plotpack, xs[idx], ys[idx], (idx,numplots), texlen)


def clearLayout(layout):
	for i in reversed(range(3,layout.count())): 
		layout.itemAt(i).widget().setParent(None)