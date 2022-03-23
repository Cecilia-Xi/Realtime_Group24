## Realtime_Group24

We have designed a solution for programming a Raspberry Pi in C++ and then controlling an electronically controlled door lock via fingerprint recognition.

# Introduction
This is an introduction to the devices used in this project

# AS608.
# Electronically controlled lock.

# Aims
To rewrite a new AS608 framework capable of fingerprint entry and recognition by sending packets in the form of receiving packets based on the original AS608 writing framework, and then return the recognized function on the Raspberry Pi to perform the function of controlling the on and off on the door lock.

# Experimental environment
Raspberry Pi, AS608 fingerprint recognition hardware, electronically controlled lock
Development tool: C++
Operating system: Ubuntu

# Progress schedule
From 1.25 to 4.11.

1: Start on 25 January: familiarisation with the corresponding functions of the pins on the Raspberry Pi through examples.

2: From 31 January: implementation of the functions of the various components (fingerprint reader, electric lock) using the pins; writing of the front-end controls; (setting tokens, etc.).

3: Started 10 March: (x) (AS608 was too difficult to write, which delayed progress) Started 21 March: integration of the whole framework and debugging.

# Remarks
I don't know how far I can go, the possibilities are not too good, but whatever the case the AS608 code refactoring module and the overall framework of the implementation of the callback and setter effect must be completed! Hopefully it will be done better subsequently.

# Progress notes
3.21 The current version is just a more complete AS608 module, (splitting the string and converting it, calculating the checksum of the accepted packet, GetImage implementation, GenChar implementation, compare, find, store, read, delete fingerprints and other implementations,) (much more complex than expected. 😂) Then there is the use of pins for the start-up application control of the electric lock motor. Again a JS front end control script was written.
However, all the above functions were only implemented on the Raspberry Pi and still not interactive.

3.23 The current components for the updated motor section have been uploaded, so all components have been uploaded, but the code needs to be rewritten due to the requirement to change all external functions to classes to implement the callback mechanism for the entire code.
As the current AS608 is written in c, much of the code is in the form of external functions to guide the control of the process, and the process actually needs to be called in the form of a class, so the task at hand is to rewrite the code
