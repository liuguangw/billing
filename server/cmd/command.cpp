//
// Created by liuguang on 2021/12/19.
//
#include "command.h"
#include "../services/database_connection.h"
#include <iostream>

namespace cmd {
    int runCommand(int argc, char *argv[]) {
        using std::cout;
        using std::endl;
        using services::DatabaseConnection;
        for (int i = 0; i < argc; i++) {
            cout << "arg[" << i << "] = " << argv[i] << endl;
        }
        DatabaseConnection connection("127.0.0.1","root","123456","web",3306);
        if(connection.connect()){
            cout<<"mysql version: "<<connection.serverVersion()<<endl;
        }else{
            cout<<"connection error: "<<connection.lastError()<<endl;
        }
        return 0;
    }
}