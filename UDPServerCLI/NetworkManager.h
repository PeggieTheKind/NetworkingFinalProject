#pragma once

// WinSock2 Windows Sockets
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

// Need to link Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#include <chrono>
#include <vector>

namespace net
{
	struct PlayerPosition
	{
		float x;
		float z;
		float l;
		float r;
	};

	struct ClientInfo : public PlayerPosition
	{
		sockaddr_in addr;
		int addrLen;
	};

	class NetworkManager
	{
	public:
		NetworkManager();
		~NetworkManager();

		void Initialize();
		void Destroy();

		void Update();

	private:
		void HandleRECV();
		void BroadcastUpdatesToClients();

		bool m_Initialized = false;
		bool m_Connected = false;
		float m_Wait = 0;

		SOCKET m_ListenSocket;

		// 
		// Time
		std::chrono::high_resolution_clock::time_point m_NextBroadcastTime;

		//
		// ConnectedClients
		std::vector<ClientInfo> m_ConnectedClients;

	};

	const float RETRY_TIME_MS = 10000;
}