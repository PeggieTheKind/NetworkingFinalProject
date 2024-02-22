#pragma once

// WinSock2 Windows Sockets
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <vector>

// Need to link Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

// First, make it work (messy), then organize

#define DEFAULT_PORT "8412"

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
	SOCKET listenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (listenSocket == INVALID_SOCKET) {
		printf("socket failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	printf("socket created successfully!\n");



	// using sockaddr_in
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8412);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Bind 
	result = bind(listenSocket, (SOCKADDR*)&addr, sizeof(addr));
	if (result == SOCKET_ERROR) {
		printf("bind failed with error %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	printf("bind was successful!\n");





	struct sockaddr_in clientInfo;
	int clientInfoLength = sizeof(sockaddr_in);
	while (true)
	{
		// Read
		const int bufLen = 32;
		char buffer[bufLen];
		result = recvfrom(listenSocket, buffer, bufLen, 0, (SOCKADDR*)&clientInfo, &clientInfoLength);
		if (result == SOCKET_ERROR) {
			printf("recvfrom failed with error %d\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}
		
		printf("From: %s:%d: %s\n", inet_ntoa(clientInfo.sin_addr), clientInfo.sin_port, buffer);

		// Write
		result = sendto(listenSocket, buffer, result, 0, (SOCKADDR*)&clientInfo, clientInfoLength);
		if (result == SOCKET_ERROR) {
			printf("send failed with error %d\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}
	}
	// Close
	closesocket(listenSocket);

	// TODO Close connection for each client socket
	//closesocket(clientSocket);
	WSACleanup();

	return 0;
}