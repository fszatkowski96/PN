// PNPZ_client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <conio.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEST_PORT 3490
#define DEFAULT_BUFLEN 512

// Temat 7: Program w asemblerze – klient  oraz serwer bazujący na protokole TCP/IP 
// odbierający i interpretujący przesyłany przez aplikacje klienckie tekst. Wielowątkowość.

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

	//CLIENT
	SOCKET connection_socket;
	connection_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//struct hostent *he;
	struct sockaddr_in dest_address;

	char sendbuf[DEFAULT_BUFLEN];

	if (connection_socket == INVALID_SOCKET)
	{
		std::cout << "Socket cannot be created " << WSAGetLastError() << std::endl;
		_getch();
		return 0;
	}

	ZeroMemory((char *)&dest_address, sizeof(dest_address));

	dest_address.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &dest_address.sin_addr);
	dest_address.sin_port = htons(DEST_PORT);
	memset(&(dest_address.sin_zero), '\0', 8);
	
	while (true)
	{
		return_value = connect(connection_socket, (sockaddr *)&dest_address, sizeof(struct sockaddr));
		if (return_value == INVALID_SOCKET)
		{
			std::cout << "Error connecting client socket: " << WSAGetLastError() << std::endl;
		}
		else
		{
			std::cout << "Connection to server succesful" << std::endl;
			break;
		}
	}

	while (true)
	{
		std::cout << "Enter data to send" << std::endl;
		
		std::cin.width(DEFAULT_BUFLEN);
		std::cin >> sendbuf;
		std::cin.ignore();

		int bytes_sent;

		bytes_sent = send(connection_socket, sendbuf, DEFAULT_BUFLEN, 0);
		
		if (bytes_sent == -1)
		{
			std::cout << "Error sending data: " << WSAGetLastError() << std::endl;
			_getch();
		}

		if (bytes_sent != DEFAULT_BUFLEN)
		{
			std::cout << "Some data couldn't be send" << std::endl;
			_getch();
		}

		char recvbuf[DEFAULT_BUFLEN];
		int bytes_recv;

		bytes_recv = recv(connection_socket, recvbuf, DEFAULT_BUFLEN, 0);
		if (bytes_recv == -1)
		{
			std::cout << "No acknowledgement from server" << std::endl;
		}
	}
	return 0;
}


