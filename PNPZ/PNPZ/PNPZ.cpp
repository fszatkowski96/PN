// PNPZ.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define MYPORT 3490
#define BACKLOG 5
#define DEFAULT_BUFLEN 512

// Temat 7: Program w asemblerze – klient  oraz serwer bazujący na protokole TCP/IP 
// odbierający i interpretujący przesyłany przez aplikacje klienckie tekst. Wielowątkowość.

void __cdecl Echo(void * arg)
{
	SOCKET client_socket = *((SOCKET*) arg);
	
	struct sockaddr sa;
	int sa_length = INET_ADDRSTRLEN;
	int sa_result = getpeername(client_socket, &sa, &sa_length);

	struct sockaddr_in client_address = *(struct sockaddr_in*)&sa;

	char str[INET_ADDRSTRLEN];
	inet_ntop(client_address.sin_family, &client_address.sin_addr, str, INET_ADDRSTRLEN);
	std::cout << "Connection accepted from " << str << ", " << ntohs(client_address.sin_port) << std::endl;

	while (true)
	{		
		char recvbuf[DEFAULT_BUFLEN];
		int bytes_recv;

		bytes_recv = recv(client_socket, recvbuf, DEFAULT_BUFLEN, 0);

		if (bytes_recv == -1)
		{
			std::cout << "Error receiving data from : " << str << std::endl << WSAGetLastError() << std::endl;
			_getch();
		}

		if (bytes_recv == 0)
		{
			std::cout << "Connection with client " << str << "closed!" << std::endl;
			_getch();
			closesocket(client_socket);
			_endthread();
		}
		std::cout << "Data received from : " << str << std::endl << recvbuf << std::endl;

		const char *ack = "0";
		int send_result = send(client_socket, ack, strlen(ack), 0);
		if (send_result == -1)
		{
			std::cout << "Error sending acknowledgement." << std::endl;
		}
	}
}

int main(int argc, char * argv[])
{
	//initialize WinSock
	WSADATA wsa_data;

	//check if its working
	int return_value = 0;
	return_value = WSAStartup(MAKEWORD(2, 2), &wsa_data);

	if (return_value)
	{
		return_value = WSAGetLastError();
		if (return_value == WSANOTINITIALISED)
			std::cout << "WSA not initialialised" << std::endl;
		else
			std::cout << "WSAStartup failed." << WSAGetLastError() << std::endl;
	}
	std::cout << "WinSock working" << std::endl;

	// SERVER
	// create tcp socket - internet protocol adress family, tcp socket, TCP protocol type
	SOCKET listen_socket, client_socket;
	listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in my_address, client_address;

	//check if its working
	if (listen_socket == INVALID_SOCKET)
	{
		std::cout << "Socket cannot be created " << WSAGetLastError() << std::endl;
		_getch();
		return 0;
	}

	ZeroMemory((char *)&my_address, sizeof(my_address));

	// set sockets adress parameters
	// internet protocol adress family
	my_address.sin_family = AF_INET;
	// machine ip adress
	inet_pton(AF_INET, "127.0.0.1", &my_address.sin_addr);
	// any unused port
	my_address.sin_port = htons(MYPORT);
	// set last parameter to zero
	memset(&(my_address.sin_zero), '\0', 8);

	// bind socket to port of choice
	if (bind(listen_socket, (struct sockaddr *) &my_address, sizeof(struct sockaddr)) == SOCKET_ERROR)
	{
		std::cout << "Socket cannot be binded: " << WSAGetLastError() << std::endl;
		_getch();
		return 0;
	}

	// listen on this port - backlog - max. number of connections
	if (listen(listen_socket, BACKLOG) == SOCKET_ERROR)
	{
		std::cout << "Listening unsucesfull, error: " << WSAGetLastError() << std::endl;
		_getch();
		return 0;
	}

	// create client socket thread when there is client trying to connect

	while (true)
	{
		int add_size = sizeof(struct sockaddr_in);
		client_socket = accept(listen_socket, (struct sockaddr *) &client_address, &add_size);

		if (client_socket == INVALID_SOCKET)
		{
			std::cout << "Error connecting to client: " << WSAGetLastError() << std::endl;
			_getch();
			return 0;
		}

		_beginthread(Echo, 0, (void*) &client_socket);
	}

	return 0;
}
