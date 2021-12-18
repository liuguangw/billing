#include <iostream>
#include <mysql.h>

using std::cout;
using std::endl;

int connectMysql();

int main() {
	cout<<"The MySQL client version is: " << mysql_get_client_info() << endl;
    std::cout << "Hello World!"<<std::endl;
    return connectMysql();
}

int connectMysql(){
	MYSQL *con = mysql_init(NULL);
	if(mysql_real_connect(con, "127.0.0.1", "root", "123456", "web", 3306, NULL, 0) == NULL)
	{
		cout<<"The authentication failed with the following message:"<<endl;
		cout<<mysql_error(con)<<endl;
		return 1;
	}
	cout<<"The connection is open"<<endl;
    const char * serverVersion = mysql_get_server_info(con);
	cout<<"server version: "<<serverVersion<<endl;
	mysql_close(con);
	return 0;
}
