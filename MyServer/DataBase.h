#pragma once
#include <iostream>
#include <string.h>
#include <mysql/mysql.h>
#include <vector>

class DataBase
{
private:
	MYSQL sqlHander;
	MYSQL_ROW row;
	MYSQL_RES* result;
	MYSQL_FIELD* field;
public:
	DataBase() {}
	~DataBase() 
	{
		mysql_close(&sqlHander);
	}
	int Connect(std::string host, std::string username, std::string password, std::string databaseName);

	int AddAccount(std::string strUserName, std::string strPassword);
	int QueryAccount(std::string strUserName, std::string strPassword);
};

