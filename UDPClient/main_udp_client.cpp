#pragma once

// WinSock2 Windows Sockets
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <conio.h>

#include <string>

// Need to link Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

// First, make it work (messy), then organize

#define DEFAULT_PORT 8412

int main(int arg, char** argv)
{
	// Initialize WinSock
	WSADATA wsaData;
	int result;

	// Set version 2.2 with MAKEWORD(2,2)
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		printf("WSAStartup failed with error %d\n", result);
		return 1;
	}
	printf("WSAStartup successfully!\n");





	// Socket
	SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (serverSocket == INVALID_SOCKET) {
		printf("socket failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	printf("socket created successfully!\n");

	const int bufLen = 32;
	char buffer[bufLen];
	std::string buf = "Hello";
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8412);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int addrLen = sizeof(addr);
	while (true)
	{
		if (_kbhit())
		{
			int ch = _getch();
			if (ch == 27) break;
		}

		result = sendto(serverSocket, buf.c_str(), buf.length(), 0, (SOCKADDR*)&addr, addrLen);
		if (result == SOCKET_ERROR) {
			printf("send failed with error %d\n", WSAGetLastError());
			closesocket(serverSocket);
			WSACleanup();
			return 1;
		}

		result = recvfrom(serverSocket, buffer, bufLen, 0, (SOCKADDR*)&addr, &addrLen);
		if (result == SOCKET_ERROR) {
			printf("recvfrom failed with error %d\n", WSAGetLastError());
			closesocket(serverSocket);
			WSACleanup();
			return 1;
		}

		printf("From: %s:%d: %s\n", inet_ntoa(addr.sin_addr), addr.sin_port, buffer);

		Sleep(5000);
	}

	closesocket(serverSocket);
	WSACleanup();

	return 0;
}