#include <stdio.h>
#include <string>
#include "DataBase.h"
#include "LobbyServer.h"

int main()
{
	g_LobbyServer = new LobbyServer("10.0.128.206", 4444);
	g_LobbyServer->run();
}