//
// Created by liuguang on 2021/12/19.
//
#include "command.h"
#include "../services/database_connection.h"
#include "../services/server.h"
#include <iostream>
#include <climits>
#include <libgen.h>
#include <cstring>

namespace cmd {
    int runCommand(int argc, char *argv[]) {
        using std::cout;
        using std::endl;
        using services::DatabaseConnection;
        for (int i = 0; i < argc; i++) {
            cout << "arg[" << i << "] = " << argv[i] << endl;
        }
        {
            char pathBuffer[PATH_MAX];
            char* distPath=realpath (argv[0],pathBuffer);
            distPath=dirname(distPath);
            distPath=strcat(distPath,"/config.yaml");
            cout<<"config file path: "<<distPath<<endl;
            services::Server server;
            server.initConfig(distPath);
            try{
                server.initDatabase();
            }catch (const char* err){
                cout << "connection error: " << err << endl;
                return 1;
            }
        }
        return 0;
    }
}