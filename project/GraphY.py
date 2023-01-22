""" Basic Graphing - Because screw linked list class inheritance
    Austin H. Dial
    5/2/2016
"""

import matplotlib
import numpy as np
import pylab as pl

           
DFILE = np.loadtxt('Test.txt')

# plot the first column as x, and second column as y
pl.plot(DFILE[:,0], DFILE[:,3], 'b')   # Plot Y_L data
#pl.plot(DFILE[:,0], DFILE[:,4], 'b')   # Plot Y_H data

# Label Axes
pl.xlabel('Data Samples at 10/sec')
pl.ylabel('Y-Axis Data')
pl.xlim(0, 20)

#Execute
pl.show()