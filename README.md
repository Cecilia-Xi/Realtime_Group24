# Realtime_Group24

## Fingerprint unlocking


<p align="center">
    <a href = "https://github.com/xiguo0806/Realtime_Group24/blob/main/figure/fingerlogo.jpeg">
        <img src="figure/fingerlogo.jpeg" alt="Logo" height="300">
    </a>
    <p align="center">Use fingerprint to unlock doors</p>
</p>

<p align="center">
    <a href="https://www.youtube.com/watch?v=Mex_W0Kkss0">Youtube</a>
    &nbsp;
    &nbsp;
    <a href="https://www.bilibili.com/video/BV1RY4y1e7MV?pop_share=1">Bilibili</a>
</p>
<p align="center">
   
</p>

<p align="center">
    <a href="https://github.com/xiguo0806/Realtime_Group24/issues" alt="Issues">
        <img src="https://img.shields.io/github/issues/xiguo0806/Realtime_Group24.svg" /></a>
    <a href="https://github.com/xiguo0806/Realtime_Group24/blob/main/LICENSE" alt="License">
        <img src="https://img.shields.io/github/license/xiguo0806/Realtime_Group24.svg" /></a>
    <a href="https://github.com/xiguo0806/Realtime_Group24/releases" alt="Tag">
        <img src="https://img.shields.io/github/v/release/xiguo0806/Realtime_Group24.svg?color=blue&include_prereleases" alt="build status"></a>
</p>

- [Realtime_Group24](#realtime_group24)
  - [Fingerprint unlocking](#fingerprint-unlocking)
- [About the project](#about-the-project)
  - [Build environment](#build-environment)
  - [Target](#target)
  - [Lab environment](#lab-environment)
    - [Electronic devices](#electronic-devices)
    - [Start using](#start-using)
      - [Install](#install)
  - [License](#license)
  - [Author](#author)
    - [update progress table](#update-progress-table)
    - [progress description](#progress-description)

# About the project
As smart homes become very popular, using chips to automate the operation of the home has become very useful, so we designed a solution to program a Raspberry Pi with C++, then control an electronically controlled door lock through fingerprint recognition, and finally achieve Smart unlocking function.

## Build environment
+ C++
+ github

## Target
On the basis of the original AS608 writing framework, a new AS608 framework that can be fingerprinted and recognized is rewritten in the form of package receipt and package delivery, and then the recognized function is returned on the Raspberry Pi to execute the function of the control switch door lock.

## Lab environment
### Electronic devices
+ Raspberry Pi
+ AS608 
+ electric lock
+ relay



### Start using
Installation Environment Requirements
+ WiringPi

#### Install
+ Decompress WiringPi. h

```unzip WiringPi.zip```

+ Installing WiringPi. h

```cd WiringPi```

```./build```
+ Go to the demo for running the test file

```cd demo```

+ Compile and run
```make```

```./fp```
+ Upon entry it will run to recognise the fingerprint to unlock the door, based on the existing fingerprint library.
+ The external button in the settings allows you to pause the fingerprint recognition and go to the thread where the new fingerprint was recorded, and after successful recording, the fingerprint recognition is reawakened and the fingerprint detection continues.
## License
see license information

## Author
+ Xinyu Ren (https://github.com/Qizui)

+ Shaobo Yang (https://github.com/vincent972123)

+ Chongzhi Gao (https://github.com/c712g285)

+ Shimeng Xi (https://github.com/xiguo0806)

---
### update progress table
From 1.25 to 4.11.

1: Starting from January 25th: Familiarize yourself with the pin-corresponding functions on the Raspberry Pi through examples.

2: From January 31st: use pins to implement functions of various components (fingerprint reader, electric lock); write front-end controls; (set tokens, etc.).

3: Start on March 10: (x) (AS608 is too difficult to write, delaying the progress) Start on March 21: Integration and debugging of the entire framework.



### progress description
3.21 The current version is just a relatively complete AS608 module, (split string and convert, calculate the checksum of the accepted package, GetImage implementation, GenChar implementation, compare, find, store, read, delete fingerprints, etc.) (Compared to It is much more complicated to imagine.😂) Then there is the start application that uses the pins to control the electric lock motor. Write a JS front-end control script again.
However, all the above functions are only implemented on the Raspberry Pi and cannot be interacted yet.

3.23 The current components of the updated motor part have been uploaded, so all components have been uploaded, but since all external functions need to be changed into classes to implement the callback mechanism of the entire code, the code needs to be rewritten.
Since the current AS608 is written in c, many codes are in the form of external functions to guide the control of the process, and the process actually needs to be called in the form of a class, so the task at hand is to rewrite the code
