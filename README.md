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
Considered the electronic technology expeditious raise in the 20th century, people choosed smart home which becomes a big fashionable trend. Embedding chips and system into home gadget automates the operation freely, easily and remotable. Therefore, we proposed a smart lock and produced it. The product implemented with Raspberry Pi 4B, and a fingerprint controlled solenoid lock. This program completely coding in C++.

## Installation
### Hardware
                
* Raspberry Pi 4
* AS608 Fingerprint sensor
* Relay
* Breadboard
* Solenoid lock


### Software dependent environment
WiringPi(Version 2.61-1)
* `git clone https://github.com/WiringPi/WiringPi.git`
* Then go to the wiringpi folder`./build`
* If no permission please type `sudo chmod 777 build` in your terminal and than `./build`

CppThread

* The CppThread.h file is a CppThread Interface, which is a reference cite from the origin Github repository <https://github.com/berndporr/cppThread> authorized by professor Bernd Porr. 
* You only need to git clone the CppThread.h file to get the Interface. 
* Copy the link below to download the whole CppThread folder with an example.(The specific implementation can be modified according to your needs, please refer to the CppThread.h file in this project)
```
$ git clone https://github.com/berndporr/cppThread
```
QT5
* `sudo apt-get install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools`

QT Creator(optional)
* `sudo apt-get install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools qtcreator`
### Run the software
* If you have qtcreator just open the .pro file to build and run.
* Compile and run in the terminal.
```
$ qmake ~/poject_GUI/finger.pro -o ~/tmp_path/
$ make --directory=~/tmp_path/
$ cd tmp_path
$ ./finger
```






## Authors(Contact information is on the [Wiki](https://github.com/xiguo0806/Realtime_Group24/wiki/Authors))

+ Shaobo Yang 

+ Chongzhi Gao 

+ Xinyu Ren 

+ Shimeng Xi


If you have any questions about the project or ideas for improvement, please contact the author.

---
