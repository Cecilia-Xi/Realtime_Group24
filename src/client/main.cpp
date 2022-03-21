#include <stdlib.h>
#include <signal.h>
#include "../share/log/logger.h"
#include "../share/util/path.h"
#include "door_client.h"

// Global variables
DoorClient g_doorClient;

// Project catalogue
std::string g_projDir = util::getProjDir();
// Data catalogue
std::string g_dataDir = g_projDir + "/data";
// Configuration file directory
std::string g_cfgDir  = g_projDir + "/config";
// Journal Catalogue
std::string g_logsDir = g_projDir + "/logs/client";


// Capture Ctrl+C
void handleCtrlC(int num) {
    LWarn("Catch SIGINT");
    g_doorClient.quit();
}


// Main functions
int main(int argc, char* argv[]) {
    LInfo("============================================");
    signal(SIGINT, handleCtrlC);

    // Initialization
    if (!g_doorClient.setup()) {
        LError("Door client setup failed");
        return 1;
    }

    // Main Cycle
    g_doorClient.run();

    return 0;
}

