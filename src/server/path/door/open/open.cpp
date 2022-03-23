#include "open.h"
#include "../../base.h"
#include "../../../module/motor.h"
#include "../../../../share/module/led.h"

#include <unistd.h>
#include <mutex>
#include <thread>

extern Led   g_greenLed;
extern Motor g_motor;
extern WxNotifier g_wxNotifier;


static std::mutex s_mutex;

/* Notify all members */
static bool notify_door_opened(const std::string& from, const std::string& user) {
    std::string msg = user + "通过";
    if (from == "built-in") {
        msg += "指纹";
    }
    else {
        msg += from;
    }
    msg += "开了门";
    LDebug("{}", msg);
    return g_wxNotifier.notifyAll(msg);
}

/* Background thread: executing the door opening process */
static void open_the_door_thread(const std::string& from, const std::string& user) {
    if (!s_mutex.try_lock()) {
        LWarn("Motor is busy");
        return;
    }

    // Open the door
    g_motor.rotateCW();

    // Determination of door opening channels: C software, mobile app, fingerprint
    if (from == "pc") {
        LInfo("From PC. {} 开了门", user);
        g_greenLed.lightT(2000, false);
        sleep(8);
    }
    else if (from == "app") {
        LInfo("From APP. {} 开了门", user);
        g_greenLed.lightT(2000, false);
        sleep(5);
    }
    // Request from the Raspberry Pi's built-in clien (i.e. opening the door via fingerprint)
    else if (from == "built-in") {
        LInfo("From built-in. {} 开了门", user);
        //g_greenLed.lightT(2000, false);
        sleep(5);
    }
    else if (from == "web") {
        LInfo("From Web. {} 开了门", user);
        g_greenLed.lightT(2000, false);
        sleep(5);
    }
    else {
        // won't get here
        LInfo("From UNKNOWN way:{}. {} 开了门", from, user);
        g_greenLed.lightT(2000, false);
        sleep(5);
    }

    // Close the door
    g_motor.rotateCCW();
    LInfo("Door opened");

    // attention!
    s_mutex.unlock();

    // Notify all dormitory members
    if (!notify_door_opened(from, user)) {
        LError("Notify dorm members failed");
    }
}


/* uri callback function */
void cb_door_open(Request& req, Response& res) {
    LTrace("Handle door_open");
    Json::Value root, data;

    // When expanded here, a `_user` variable is defined, which is the name of the person who opened the door
    CHECK_TOKEN_EX(LV_ROOT | LV_ADMIN | LV_GUEST);

    // @from Request source: Raspberry Pi built-in client, Windows software request, mobile phone
    // @who member id of the dormitory requesting the door
    CHECK_PARAM_STR(from);

    // External requests
    if (from != "pc" && from != "built-in" && from != "app" && from != "web") {
        RETURN_INVALID_PARAM(from);
    }

    // Backend execution of the door opening process
    std::thread t(open_the_door_thread, from, _user);
    t.detach();

    RETURN_OK();
}

