## Realtime_Group24

#About the project content
As smart homes become very popular, using chips to automate the operation of the home has become very useful, so we designed a solution to program a Raspberry Pi with C++, then control an electronically controlled door lock through fingerprint recognition, and finally achieve Smart unlocking function.

# build environment
· C++
· github

# Target
On the basis of the original AS608 writing framework, a new AS608 framework that can be fingerprinted and recognized is rewritten in the form of package receipt and package delivery, and then the recognized function is returned on the Raspberry Pi to execute the function of the control switch door lock.

# lab environment
Raspberry Pi, AS608 fingerprint recognition hardware, electric lock
Development tools: C++
Operating System: Ubuntu

start using
Installation Environment Requirements
· WiringPi

Install
cd

run

# License
see license information

# author
· Xinyu Ren (https://github.com/Qizui)
· Shaobo Yang ()
· Chongzhi Gao ()
· Shimeng Xi (https://github.com/xiguo0806)


# update progress table
From 1.25 to 4.11.

1: Starting from January 25th: Familiarize yourself with the pin-corresponding functions on the Raspberry Pi through examples.

2: From January 31st: use pins to implement functions of various components (fingerprint reader, electric lock); write front-end controls; (set tokens, etc.).

3: Start on March 10: (x) (AS608 is too difficult to write, delaying the progress) Start on March 21: Integration and debugging of the entire framework.

# Comment
Don't know how far it can go, the possibilities are not very good, but anyway the overall framework of AS608 code refactoring modules and callback and setter effect implementation must be done! Hope to do better in the future.

# progress description
3.21 The current version is just a relatively complete AS608 module, (split string and convert, calculate the checksum of the accepted package, GetImage implementation, GenChar implementation, compare, find, store, read, delete fingerprints, etc.) (Compared to It is much more complicated to imagine.😂) Then there is the start application that uses the pins to control the electric lock motor. Write a JS front-end control script again.
However, all the above functions are only implemented on the Raspberry Pi and cannot be interacted yet.

3.23 The current components of the updated motor part have been uploaded, so all components have been uploaded, but since all external functions need to be changed into classes to implement the callback mechanism of the entire code, the code needs to be rewritten.
Since the current AS608 is written in c, many codes are in the form of external functions to guide the control of the process, and the process actually needs to be called in the form of a class, so the task at hand is to rewrite the code
