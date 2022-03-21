#pragma once
#include <string>
#include "module/fp_module.h"
#include "user/user_manager.h"


class DoorClient {
public:
    bool setup();
    void run();
    bool quit() { shouldQuit_ = true; }

private:
    /* Send a request to the server to open the door */
    bool openTheDoor_(int pageID, int score);

private:
    FpModule fpModule_;
    UserManager userManager_;
    int errorCount_;
    bool shouldQuit_ = false;
};
