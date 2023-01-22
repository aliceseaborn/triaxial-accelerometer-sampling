""" Linked List Class Testing
    Austin H. Dial
    5/2/2016
    
    REF: https://www.codefellows.org/blog/implementing-a-singly-linked-list-in-python
    REF: http://www.openbookproject.net/thinkcs/python/english2e/ch18.html    
"""

""" Node Class
    Defines the basic structure of a single node entry. This will hold our data
    as read from the PrettyData.txt file for graphing.
"""
class Node:
    def __init__(self, Cargo, next=None):  # Set to none by default
        self.Cargo = Cargo                      # Call data package Cargo
        self.next  = next                       # Next sector for next Node
        
    def DAT(self):
        return self.Cargo

    def NEXT(self):
        return self.next

    def Append(self, new):
        self.next = new
    
    def

""" Cargo Class
    Defines the data structure for the Cargo data segment of the Nodes within
    the linked list data structure to hold our axis data for graphing.
"""
class Cargo:
    def __init__(self, time, X_L, X_H, Y_L, Y_H, Z_L, Z_H): # Instantiation function
        self.time = int(time) # Unpack time segment and convert to integer locally
        self.X_L = int(X_L) # Unpack X_L and convert to integer locally
        self.X_H = int(X_H) # ...
        self.Y_L = int(Y_L) # ...
        self.Y_H = int(Y_H) # ...
        self.Z_L = int(Z_L) # ...
        self.Z_H = int(Z_H) # ...

        # Now compile the data into one array of all axis data segments
        self.DATA = [self.X_L, self.X_H, self.Y_L, self.Y_H, self.Z_L, self.Z_H]

    def TIME(self):         # Function to return time
        return self.time    # Return time segment
        
    def PDATA(self):         # Function to return data
        return self.DATA    # Return DATA segment

""" LL Class
    Defines the linked list class and its member functions to navigate and edit
    the linked list itself.
"""
class List:
    def __init__(self, head=None):
        self.head = head
        self.size = 0    
        
    def FIRST(self, Node, Cargo):       # Defines the header
        Node = Node.Cargo               # Node points to Cargo
        Node.next = self.head           # 
        self.head = Node
        self.size = self.size + 1
        
    def INSERT(self, Cargo):    # Add a new member to the beginning of the list
        new = Node(Cargo)       # Creates new node with Cargo = arg Cargo
        new.Append(self.head)   # Call append on Node to add new element with argument head
        self.head = new         # Reset the head of the list
        
    def SIZE(self):
        current = self.head     # Create variable to hold head location
        count = 0               # Create interation counter
        while current:          # While the current node is not the last
            count += 1          # Incremement the counter variable
            current = current.NEXT()    # Look at next node
        return count            # Return the LL size
    
        
def cls():              # Convenient clear command for testing 
    print ("\n" * 100)  # Print that sh*t
