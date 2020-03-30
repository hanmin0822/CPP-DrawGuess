#pragma once
#include <mysql.h>
//#include <WinSock2.h>
#pragma comment(lib,"libmysql.lib")

#include <list>
using namespace std;

class CMySql
{
public:
    CMySql(void);
    ~CMySql(void);
public:
	//连接数据库
    bool  ConnectMySql(char *host,char *user,char *pass,char *db);

	//断开连接
    void  DisConnect();

	//获取结果类的SQL语句
    bool  SelectMySql(char* szSql,int nColumn,list<string>& lstStr);
   
    //更新类的SQL语句：删除、插入、修改
    bool  UpdateMySql(char* szSql);
 
private:
    MYSQL *sock;   
	MYSQL_RES *results;   
	MYSQL_ROW record; 
};

