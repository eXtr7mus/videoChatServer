// videochat_server.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "Server.h"

int main()
{
	Server server("192.168.0.1", 53500);
	if (server.Init()) {
		server.Run();
	}
    return 0;
}

