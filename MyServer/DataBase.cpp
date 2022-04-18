#include "DataBase.h"

int DataBase::Connect(std::string host, std::string username, std::string password, std::string databaseName)
{
	uint32_t client_flag = 0;
	int32_t port = 3306;
	mysql_init(&sqlHander);
	mysql_options(&sqlHander, MYSQL_OPT_RECONNECT, "1");
	mysql_options(&sqlHander, MYSQL_SET_CHARSET_NAME, "utf8");
	MYSQL* connect_result = mysql_real_connect(&sqlHander, host.c_str(), username.c_str(), password.c_str(), NULL, port, NULL, client_flag);
	if (NULL == connect_result)
	{
		printf("mysql_real_connect failed with code [%d] msg[%s]\n", mysql_errno(&sqlHander), mysql_error(&sqlHander));
		return 0;
	}

	if (mysql_ping(&sqlHander)) {
		printf("mysql ping failed\n");
		return 0;
	}

	if (mysql_select_db(&sqlHander, databaseName.c_str())) {
		printf("select db failed\n");
		return 0;
	}
	if (mysql_ping(&sqlHander))
	{
		printf("mysql_ping failed\n");
		return 0;
	}
	return 1;
}

int DataBase::AddAccount(std::string strUserName, std::string strPassword)
{
	//先查询账号是否存在
	char strSql[1024];
	sprintf(strSql, "select * from account where username='%s' and password='%s'", strUserName.c_str(), strPassword.c_str());
	if (mysql_real_query(&sqlHander, strSql, strlen(strSql)))
	{
		printf("mysql_real_query failed with code [%d] msg[%s]\n", mysql_errno(&sqlHander), mysql_error(&sqlHander));
		return 0;
	}
	MYSQL_RES* mysql_result = mysql_use_result(&sqlHander);
	if (mysql_result == nullptr && (mysql_field_count(&sqlHander) > 0))
	{
		printf("mysql_use_result failed with code [%d] msg[%s]\n", mysql_errno(&sqlHander), mysql_error(&sqlHander));
		return 0;
	}
	uint32_t row_count = static_cast<uint32_t>(mysql_num_rows(mysql_result));
	uint32_t field_count = static_cast<uint32_t>(mysql_num_fields(mysql_result));
	printf("mysql query, row_count = %d field_count = %d\n", row_count, field_count);

	MYSQL_ROW m_current_row = mysql_fetch_row(mysql_result);
	if (NULL != m_current_row)
	{
		int pID = atoi(m_current_row[0]);
		return 0;
	}

	sprintf(strSql, "INSERT INTO account VALUES('%s','%s');", strUserName.c_str(), strPassword.c_str());

	// 执行sql语句，成功返回0
	//int ret = mysql_query(&mysql, sql);		
	int ret = mysql_real_query(&sqlHander, strSql, (unsigned long)strlen(strSql));

	if (ret) {
		printf("Add account error:%s\n", mysql_error(&sqlHander));
		return 0;
	}
	printf("Add account success\n");
	return 1;
}

int DataBase::QueryAccount(std::string strUserName, std::string strPassword)
{
	char strSql[1024];
	sprintf(strSql, "select * from account where username='%s' and password='%s'", strUserName.c_str(), strPassword.c_str());
	if (mysql_real_query(&sqlHander, strSql, strlen(strSql)))
	{
		printf("mysql_real_query failed with code [%d] msg[%s]\n", mysql_errno(&sqlHander), mysql_error(&sqlHander));
		return -1;
	}
	MYSQL_RES* mysql_result = mysql_use_result(&sqlHander);
	if (mysql_result == nullptr && (mysql_field_count(&sqlHander) > 0))
	{
		printf("mysql_use_result failed with code [%d] msg[%s]\n", mysql_errno(&sqlHander), mysql_error(&sqlHander));
		return -1;
	}
	uint32_t row_count = static_cast<uint32_t>(mysql_num_rows(mysql_result));
	uint32_t field_count = static_cast<uint32_t>(mysql_num_fields(mysql_result));
	printf("mysql query, row_count = %d field_count = %d\n", row_count, field_count);

	MYSQL_ROW m_current_row = mysql_fetch_row(mysql_result);
	if (NULL != m_current_row)
	{
		int pID = atoi(m_current_row[0]);
		return pID;
	}
	return -1;
}
