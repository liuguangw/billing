//
// Created by liuguang on 2021/12/19.
//

#include <string>
#include "run_up.h"
#include "run_stop.h"
#include "run_show_users.h"
#include "run_version.h"

namespace cmd {

    int runCommand(int argc, char *argv[]) {
        std::string commandName;
        if (argc >= 2) {
            commandName = argv[1];
        }
        if (commandName == "up") {
            return runUpCommand(argc, argv, false);
        } else if (commandName == "stop") {
            return runStopCommand(argc, argv);
        } else if (commandName == "show_users") {
            return runShowUsers(argc, argv);
        } else if (commandName == "version") {
            return runVersionCommand(argc, argv);
        }
        return runUpCommand(argc, argv, true);
    }
}
