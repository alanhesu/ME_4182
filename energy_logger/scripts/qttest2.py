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

#graphicsview
tt = pg.GraphicsLayoutWidget()
tt.addPlot()
vb = tt.addViewBox()
vb.setMaximumWidth(100)

bb = pg.GraphicsLayoutWidget()
q = bb.addPlot()
q.plot(x = np.arange(1000), y = np.arange(1000), clear = True)
cb = bb.addViewBox()
cb.setMaximumWidth(100)

## Create a grid layout to manage the widgets size and position
layout = QtGui.QGridLayout()
w.setLayout(layout)

## Add widgets to the layout in their proper positions
layout.addWidget(tt, 0, 1, 1, 1)
layout.addWidget(bb, 1, 2, 1, 1)

## Display the widget as a new window
w.show()

QtGui.QApplication.processEvents()

## Start the Qt event loop
app.exec_()

