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
            return runStopCommand(argv);
        } else if (commandName == "show_users") {
            return runShowUsers(argv);
        } else if (commandName == "version") {
            return runVersionCommand();
        }
        return runUpCommand(argc, argv, true);
    }
}
