#include "fp_module.h"
#include "as608.h"
#include "../util/util.h"
#include "../../share/log/logger.h"
#include "../../share/json/json.h"

#include <wiringSerial.h>
#include <wiringPi.h>
#include <fstream>
#include <string>
#include <chrono>

extern int g_fd;
extern int g_verbose;
extern AS608 g_as608;
extern std::string g_cfgDir;


FpModule::FpModule() : relayPin_(-1), redLed_(2, "red") {
    g_fd = -1;
    g_verbose = 0;
}

FpModule::~FpModule() {
    powerOFF_();
}


// Initial configuration
bool FpModule::setup() {
    if (!readConfig_(g_cfgDir + "/as608.json")) {
        LError("Read file:{}/as608.json failed", g_cfgDir);
        return false;
    }

    if (!redLed_.setup()) {
        LError("Red led setup failed");
        return false;
    }

    pinMode(g_as608.detect_pin, INPUT);
    pinMode(relayPin_, OUTPUT);
    digitalWrite(relayPin_, LOW);

    // Temporary power supply
    // Each time power is supplied, the AS608 module is initialised and checked for successful initialisation
    if (!powerON_()) {
        LError("FpModule power on failed");
        return false;
    }
    powerOFF_();

    return true;
}


// Capture fingerprints and match
// @maxFailCount: maximum number of attempts
// @maxBlockTimeMs: maximum wait time (milliseconds)
bool FpModule::match(int maxFailCount, int maxBlockTimeMs, int& pageID, int& score) {
    if (!powerON_()) {
        LError("Power on failed");
        return false;
    }

    int timeConsumedUs = 0;
    int maxBlockTimeUs = maxBlockTimeMs * 1000;
    while (maxFailCount--) {
        if (match_(pageID, score, timeConsumedUs)) {
            break;
        }
        LError("match failed, timeConsumed(us):{}", timeConsumedUs);
        redLed_.lightT(500, false);
        maxBlockTimeUs -= timeConsumedUs;
        if (maxBlockTimeUs <= 0) {
            LError("Timeout");
            break;
        }
    }

    powerOFF_();
    return maxFailCount > 0 && maxBlockTimeUs > 0;
}

bool FpModule::matchOnce(int maxBlockTimeMs, int& pageID, int& score) {
    return match(1, maxBlockTimeMs, pageID, score);
}

bool FpModule::match_(int& pageID, int& score, int& timeConsumedUs) {
    auto start = std::chrono::system_clock::now();
    int ret = false;
    if (PS_GetImage()) {
        if (PS_GenChar(1)) {
            if (PS_Search(1, 0, 299, &pageID, &score)) {
                ret = true;
            }
        }
    }
    //int ret = PS_Identify(&pageID, &score);
    auto end = std::chrono::system_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    timeConsumedUs = dur.count();
    return ret;
}


// Powering the chip
bool FpModule::powerON_() {
    // Open relay, power supply
    digitalWrite(relayPin_, HIGH); 
    // Delay to wait for the AS608 module to be ready
    delay(500);

    if (g_fd > 0) {
        // won't get here
        serialClose(g_fd);
        g_fd = -1;
    }

    // Open the serial port
    g_fd = serialOpen(serialFile_.c_str(), g_as608.baud_rate);
    if (g_fd <= 0) {
        LError("Open serial file failed");
        return false;
    }

    // Initialising the AS608 module
    delay(500);
    if (!PS_Setup(g_as608.chip_addr, g_as608.password)) {
        LError("AS608 setup failed");
        return false;
    }

    return true;
}

// Powering down the chip
bool FpModule::powerOFF_() {
    // Close the serial port
    if (g_fd > 0) {
        serialClose(g_fd);
        g_fd = -1;     // Very very important ! ! !
    }

    // Switch off the relay and the module is powered down
    delay(100);
    digitalWrite(relayPin_, LOW);

    return true;
}

// Reading json type configuration files
bool FpModule::readConfig_(const std::string& cfgFile) {
    std::ifstream ifs(cfgFile);
    if (!ifs) {
        LError("Open file:{} failed", cfgFile);
        return false;
    }

    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(ifs, root)) {
        LError("Parse file:{} failed", cfgFile);
        return false;
    }

    if (checkNull(root, "verbose", "address", "password", "baud_rate",
                  "relay_pin", "detect_pin", "serial_file"))
    {
        LError("Missing param:verbose/address/...");
        return false;
    }

    // Chip address
    std::string addr = getStrVal(root, "address");
    if (addr.empty()) {
        LError("Invalid address");
        return false;
    }
    g_as608.chip_addr = util::hexStrToUInt(addr.c_str());
    // Whether there is a password and what it is
    std::string pwd = getStrVal(root, "password");
    if (pwd.empty() || pwd == "none") {
        g_as608.has_password = 0;
    }
    else {
        g_as608.has_password = 1;
        g_as608.password = util::hexStrToUInt(pwd.c_str());
    }
    // Baud rate
    g_as608.baud_rate = getIntVal(root, "baud_rate");
    // GPIO port for finger detection
    g_as608.detect_pin = getIntVal(root, "detect_pin");
    // Level of detail of output information
    g_verbose = getIntVal(root, "verbose");

    // Relay signal GPIO port
    relayPin_ = getIntVal(root, "relay_pin");
    // Serial communication portal
    serialFile_ = getStrVal(root, "serial_file");

    return true;
}

