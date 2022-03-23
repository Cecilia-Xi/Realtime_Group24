#ifndef __MOTOR_H__
#define __MOTOR_H__

#include <string>
#include <mutex>


class Motor {
public:
    ~Motor();
public:
    bool setup();

    bool rotateCW();
    bool rotateCCW();

private:
    bool readConfig_(const std::string& cfgFile); // Read the configuration file
    void stop_(int stop_pin);   // StopPin_ goes high to stop the motor
    void reset_();              // Reset pin level all low to stop the motor
private:
    int pin1_ = -1;
    int pin2_ = -1;
    int stopPinCW_  = -1;  // General switches
    int stopPinCCW_ = -1;  // Photoelectric switches
    std::mutex mutex_;
};


#endif // __MOTOR_H__
