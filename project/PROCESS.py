""" Python Processing
	Austin H. Dial
	4/30/2016
"""

"""  REF: http://www.tutorialspoint.com/python/string_split.htm
     REF: https://docs.python.org/2/library/binascii.html#binascii.unhexlify
     REF: https://docs.python.org/2/library/functions.html#int
"""

import pylab
import sys
import LL
import binascii

DATin = open("PrettyData.txt", "r")

Data = LL.List()

""" Two_Dec function
   Takes a binary string as it's argument and convert the 2's complement into a decimal
   and returns said decimal.
"""
def Two_Dec(value, size):
    """compute the 2's compliment of int value val"""
    if (value & (1 << (size - 1))) != 0:      # if sign bit is set e.g., 8bit: 128-255
        value = value - (1 << size)           # compute negative value
    return value                              # return positive value as is


# For each line of the file, take in data and export it to a graph file
with open("PrettyData.txt") as DATin:
    for line in DATin: # iterate over each line
        otime, oX_L, oX_H, oY_L, oY_H, oZ_L, oZ_H = line.split() # split it by whitespace
        
        # Convert the time to integer
        time = int(otime)
        
        # Convert X-axis data to integer
        X_L = Two_Dec(int(oX_L,16), 8)
        X_H = Two_Dec(int(oX_H,16), 8)
        
        # Convert Y-axis data
        Y_L = Two_Dec(int(oY_L,16), 8)
        Y_H = Two_Dec(int(oY_H,16), 8)
        
        # Convert Z-axis data
        Z_L = Two_Dec(int(oZ_L,16), 8)
        Z_H = Two_Dec(int(oZ_H,16), 8)
        
        # Now let's instantiate a new node for it
        Cargo = LL.Cargo(time, X_L, X_H, Y_L, Y_H, Z_L, Z_H)
        Node = LL.Node(Cargo)
        
        Data.INSERT(Cargo)

""" Linked List population test
    After the above code is executed we should have the PrettyData.txt file loaded
    into local memory and that means we should be able to present it loaclly.
"""
current = Data.head

#while (current):
print(Node.Cargo.PDATA())
#    Node = Node.next









