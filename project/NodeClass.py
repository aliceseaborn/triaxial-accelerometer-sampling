""" Node Class Testing
    Austin H. Dial
    5/2/2016
"""  

class Node:
    def __init__(self, cargo=None, next=None):  # Set to none by default
        self.cargo = cargo                      # Call data package cargo
        self.next  = next                       # Next sector for next Node

    def __str__(self):
        return str(self.cargo)
        
def Print_Dat(Node):        # Print elements of node list
    while Node:             # While there node 
        print (Node),
        Node = Node.next
    print
        
def cls():              # Convenient clear command for testing 
    print ("\n" * 100)  # Print that sh*t

