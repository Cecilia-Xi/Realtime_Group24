#include <stdlib.h>
#include <signal.h>
#include <wiringPi.h>

#include "../share/notify/wx.h"
#include "../share/util/path.h"
#include "../share/log/logger.h"
#include "../share/json/json.h"
#include "../share/module/led.h"
#include "module/motor.h"
#include "server/http_server.h"
#include "token/token_manager.h"

#include "path/door/open/open.h"
#include "path/server/stop/stop.h"
#include "path/token/new/new.h"

// Project catalogue
std::string g_projDir = util::getProjDir();
// Data directory
std::string g_dataDir = g_projDir + "/data/server";
// Configuration file directory
// A common server-client directory
std::string g_cfgDir  = g_projDir + "/config";
// Journal directory
std::string g_logsDir = g_projDir + "/logs/server";

Led          g_greenLed(1, "green");
Motor        g_motor;
WxNotifier   g_wxNotifier;
TokenManager g_tokenMgr;
HttpServer   g_svr;

bool setup();
void handleCtrlC(int num);


/********************************* main() begin **********************/

int main(int argc, char* argv[])
{
    LInfo("============================================");

    // Capture ctrl+c
    signal(SIGINT, handleCtrlC);

    // Random seeds
    srand(time(NULL));

    // Initial configuration
    if (!setup()) {
        LError("Setup failed");
        return 1;
    }

    ThreadPool tp;
    tp.set_pool_size(2);

    g_svr.set_thread_pool(&tp);
    g_svr.add_bind_ip("127.0.0.1");
    g_svr.set_port(5001);

    /* Register URI callback function */
    g_svr.add_mapping("/server/stop", cb_server_stop, GET_METHOD);
    g_svr.add_mapping("/door/open",   cb_door_open,   GET_METHOD);
    g_svr.add_mapping("/token/new",   cb_token_new,   GET_METHOD);

    // Inform the administrator that the program started normally
    if (!g_wxNotifier.notifyRoot("DoorServerStarted")) {
        LError("Notify master failed");
    }

    // Turn on the listening client
    g_svr.start_sync();

    // Inform the administrator that the program exited normally
    if (!g_wxNotifier.notifyRoot("DoorServerStopped")) {
        LError("Notify master failed");
    }

    LInfo("Server stopped");
    spdlog::shutdown();
    return 0;
}

/********************************* main() end *************************/

// Capture Ctrl+C
void handleCtrlC(int num) {
    LWarn("Catch Ctrl+C");
    g_svr.stop();
}

// Initial configuration
bool setup() {
    // Initialize the wiringPi library
    if (-1 == wiringPiSetup()) {
        LError("WiringPi setup failed");
        return false;
    }

    // Initialising the motor
    if (!g_motor.setup()) {
        LError("Motor setup failed");
        return false;
    }

    // Initializing Led
    if (!g_greenLed.setup()) {
        LError("Led setup failed");
        return false;
    }

    // Initialising the notifier
    if (!g_wxNotifier.setup()) {
        LError("WxNotifier setup failed");
        return false;
    }

    // Initialize the token manager
    if (!g_tokenMgr.setup()) {
        LError("TokenManager setup failed");
        return false;
    }

    return true;
}

