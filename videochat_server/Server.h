#pragma once

#include <WS2tcpip.h>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <memory>
#include <thread>
#include <mutex>
#pragma comment(lib, "ws2_32.lib")


const int MAX_BUF_SIZE = 20736000;
const int SOCKET_READ_TIMEOUT_SEC = 2;

class Server
{
private:
	std::string m_ipAddress;
	int m_port;
	std::map <int, std::string> clients;
	
	
public:
	Server(std::string ipAddress, int port);
	~Server();
	void Send(SOCKET m_sock, const char *frame, int size);

	bool Init();

	friend void DataTransfer(SOCKET first, SOCKET second);

	void Run();



	void Cleanup();

	SOCKET CreateSocket();

	SOCKET WaitForConnection(SOCKET listening);


};







