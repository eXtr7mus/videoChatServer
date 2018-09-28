#include "stdafx.h"
#include "Server.h"


void DataTransfer(SOCKET first, SOCKET second)
{
	fd_set master;
	FD_ZERO(&master);
	FD_SET(first, &master);
	struct timeval timeout;
	timeout.tv_sec = SOCKET_READ_TIMEOUT_SEC;
	timeout.tv_usec = 0;
	while (true) {
		char buff[200000];
		fd_set copy = master;
		int fd_res = select(0, &copy, nullptr, nullptr, &timeout);
		if (fd_res > 0) {
			int result = recv(first, buff, MAX_BUF_SIZE, 0);
			if (result > 0) {
				std::cout << "Message here! " << result << std::endl;
				send(second, buff, result, 0);
			}
		}
	}
	
}


bool Server::Init() {
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsInit = WSAStartup(ver, &data);
	return wsInit == 0;
}

void Server::Run()
{
	std::cout << "Server started on " << m_ipAddress << " on port " << m_port << std::endl;
	SOCKET listening = CreateSocket();
	SOCKET firstClient = WaitForConnection(listening);
	SOCKET secondClient = WaitForConnection(listening);
	closesocket(listening);
	std::thread firstThread(DataTransfer, firstClient, secondClient);
	std::thread secondThread(DataTransfer, secondClient, firstClient);
	secondThread.detach();
	firstThread.detach();
	
	int i;
	std::cout << "Ended" << std::endl;
	std::cin >> i;
	closesocket(firstClient);
	closesocket(secondClient);
}





void Server::Cleanup()
{
	WSACleanup();
}

SOCKET Server::CreateSocket() {
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening != INVALID_SOCKET) {
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(m_port);
		inet_pton(AF_INET, m_ipAddress.c_str(), &hint.sin_addr);

		int bindOK = bind(listening, (sockaddr*)&hint, sizeof(hint));
		if (bindOK != SOCKET_ERROR) {
			int listenOK = listen(listening, SOMAXCONN);
			if (listenOK == SOCKET_ERROR) {
				return -1;
			}
		}
		else {
			return -1;
		}
	}
	return listening;
}

SOCKET Server::WaitForConnection(SOCKET listening)
{
	char buf[4096];
	ZeroMemory(buf, 4096);
	sockaddr_in clientInf;
	int clientInfSize = sizeof(clientInf);
	SOCKET client = accept(listening, (sockaddr*)&clientInf, &clientInfSize);
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		std::cout << host << " NAME : " << buf << " connected on port " << service << std::endl;
	}
	else {
		inet_ntop(AF_INET, &clientInf.sin_addr, host, NI_MAXHOST);
		std::cout << host << " NAME : " << buf << " connected on port " << ntohs(clientInf.sin_port) << std::endl;
	}
	if (client != SOCKET_ERROR) return client;
	return SOCKET_ERROR;
}


Server::Server(std::string ipAddress, int port) :
	m_ipAddress(ipAddress), m_port(port)
{}

Server::~Server()
{
	Cleanup();
}
