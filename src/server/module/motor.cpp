#include "motor.h"
#include "../../share/json/json.h"
#include "../../share/log/logger.h"

#include <wiringPi.h>
#include <fstream>

extern std::string g_cfgDir;


Motor::~Motor() {
    reset_();
}

// Initialization
bool Motor::setup() {
    if (!readConfig_(g_cfgDir + "/motor.json")) {
        LError("Read config {}/motor.json failed", g_cfgDir);
        return false;
    }

    pinMode(pin1_, OUTPUT);
    pinMode(pin2_, OUTPUT);
    pinMode(stopPinCW_, INPUT);
    pinMode(stopPinCCW_, INPUT);
    pullUpDnControl(stopPinCW_,  PUD_DOWN);
    pullUpDnControl(stopPinCCW_, PUD_DOWN);

    reset_();

    return true;
}


// Clockwise rotation
bool Motor::rotateCW() {
    if (mutex_.try_lock()) {
        // Start turning
        digitalWrite(pin1_, HIGH);
        digitalWrite(pin2_, LOW);
        // Stop rotation
        stop_(stopPinCW_);
        mutex_.unlock();
        return true;
    }
    return false;
}


// Counterclockwise rotation
bool Motor::rotateCCW() {
    if (mutex_.try_lock()) {
        // Start turning
        digitalWrite(pin1_, LOW);
        digitalWrite(pin2_, HIGH);
        // Stop rotation
        stop_(stopPinCCW_);
        mutex_.unlock();
        return true;
    }
    return false;
}


// Stop motor rotation
// Blocking
void Motor::stop_(int stop_pin) {
    int count = 20;
    while (--count > 0) {
        if (digitalRead(stop_pin) == HIGH) {
            break;
        }
        delay(100); // Maximum blockage of 3s
    }
    reset_();
}


// Resetting the GPIO pins
void Motor::reset_() {
    digitalWrite(pin1_, LOW);
    digitalWrite(pin2_, LOW);
}


// Read the configuration file
bool Motor::readConfig_(const std::string& cfgFile) {
    std::ifstream ifs(cfgFile);
    if (!ifs) {
        LError("Open file {} failed", cfgFile);
        return false;
    }

    Json::Reader reader;
    Json::Value  root;
    if (!reader.parse(ifs, root)) {
        LError("Parse file {} failed", cfgFile);
        return false;
    }

    if (checkNull(root, "motor_pin1", "motor_pin2", "stop_pin_cw", "stop_pin_ccw")) {
        LError("Missing key:motor_pin1/... in file {}", cfgFile);
        return false;
    }

    pin1_ = getIntVal(root, "motor_pin1");
    pin2_ = getIntVal(root, "motor_pin2");
    stopPinCW_  = getIntVal(root, "stop_pin_cw");
    stopPinCCW_ = getIntVal(root, "stop_pin_ccw");

    return true;
}

