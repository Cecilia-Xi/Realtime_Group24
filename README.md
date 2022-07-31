# <div align=center> Fingerprint Locker </div>


<div align=center><img width = '250' height ='250' src ="https://user-images.githubusercontent.com/31944208/181933886-f17e06ab-4812-4e36-973f-ed0a9678bc0f.png"/></div>

<p align="center">
    <a href="https://youtu.be/Vcys27fCmiU">Youtube</a>
    &nbsp;
    &nbsp;
    <a href="https://www.bilibili.com/video/BV1wF411u73p/">Bilibili</a>
</p>



<p align="center">
    <a href="https://github.com/xiguo0806/Realtime_Group24/issues" alt="Issues">
        <img src="https://img.shields.io/github/issues/xiguo0806/Realtime_Group24.svg" /></a>
    <a href="https://github.com/xiguo0806/Realtime_Group24/blob/main/LICENSE" alt="License">
        <img src="https://img.shields.io/github/license/xiguo0806/Realtime_Group24.svg" /></a>
    <a href="https://github.com/xiguo0806/Realtime_Group24/releases" alt="Tag">
        <img src="https://img.shields.io/github/v/release/xiguo0806/Realtime_Group24.svg?color=blue&include_prereleases" alt="build status"></a>
</p>



# About the project
As smart homes become very popular, using chips to automate the operation of the home has become very useful, so we designed a solution to program a Raspberry Pi with C++, then control an electronically controlled door lock through fingerprint recognition, and finally achieve Smart unlocking function.

## Installation
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
