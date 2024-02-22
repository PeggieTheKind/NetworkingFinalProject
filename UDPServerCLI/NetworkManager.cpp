#include "NetworkManager.h"

#include <memory>

#define SERVER_PORT 8412
#define SERVER_IP "127.0.0.1"

namespace net
{
	// Move this to a structure for each client
	sockaddr_in addr;
	int addrLen;

	class Buffer
	{
	public:
		Buffer() { }
		~Buffer() { }

		std::vector<uint8_t> data;
	};

	NetworkManager::NetworkManager()
	{
	}

	NetworkManager::~NetworkManager()
	{

	}

	void NetworkManager::Initialize()
	{
		// Initialize WinSock
		WSADATA wsaData;
		int result;

		// Set version 2.2 with MAKEWORD(2,2)
		result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != 0) {
			printf("WSAStartup failed with error %d\n", result);
			return;
		}
		printf("WSAStartup successfully!\n");


		// Socket
		m_ListenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (m_ListenSocket == INVALID_SOCKET) {
			printf("socket failed with error %d\n", WSAGetLastError());
			WSACleanup();
			return;
		}
		printf("socket created successfully!\n");

		// using sockaddr_in
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(SERVER_PORT);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

		// Bind 
		result = bind(m_ListenSocket, (SOCKADDR*)&addr, sizeof(addr));
		if (result == SOCKET_ERROR) {
			printf("bind failed with error %d\n", WSAGetLastError());
			closesocket(m_ListenSocket);
			WSACleanup();
			return;
		}
		printf("bind was successful!\n");

		unsigned long nonblock = 1;
		result = ioctlsocket(m_ListenSocket, FIONBIO, &nonblock);
		if (result == SOCKET_ERROR) {
			printf("set socket to nonblocking failed with error %d\n", WSAGetLastError());
			closesocket(m_ListenSocket);
			WSACleanup();
			return;
		}
		printf("set socket to nonblocking was successful!\n");

		printf("NetworkManager running...\n");

		m_NextBroadcastTime = std::chrono::high_resolution_clock::now();

		m_Initialized = true;
	}

	void NetworkManager::Destroy()
	{
		if (!m_Initialized)
		{
			return;
		}

		closesocket(m_ListenSocket);
		WSACleanup();

		m_Initialized = false;
	}

	void NetworkManager::Update()
	{
		if (!m_Initialized)
		{
			return;
		}

		// Handle all recv data
		HandleRECV();

		// Process everything

		// Send information/data back to clients
		BroadcastUpdatesToClients();
	}

	void NetworkManager::HandleRECV()
	{
		// Read
		sockaddr_in addr;
		int addrLen = sizeof(addr);

		const int bufLen = 16;	// recving 4 floats only
		char buffer[bufLen];
		int result = recvfrom(m_ListenSocket, buffer, bufLen, 0, (SOCKADDR*)&addr, &addrLen);
		if (result == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				// Not a real error, we expect this.
				// -1 is an error, 0 is disconnected, >0 is a message
				// WSA uses this as a flag to check if it is a real error
				return;
			}
			else
			{
				// TODO: We want to handle this differently.
				printf("recvfrom failed with error %d\n", WSAGetLastError());
				Destroy();
				//closesocket(m_ListenSocket);
				//WSACleanup();
				return;
			}
		}

		// Compare to see if the addr is already registered
		// If it is not registered, we add it
		// If it is registered, we can set the data
		int clientId = -1;
		for (int i = 0; i < m_ConnectedClients.size(); i++)
		{
			ClientInfo& client = m_ConnectedClients[i];
			if (client.addr.sin_addr.s_addr == addr.sin_addr.s_addr
				&& client.addr.sin_port == addr.sin_port)
			{
				clientId = i;
				break;
			}
		}

		if (clientId == -1)
		{
			// Add the client
			ClientInfo newClient;
			newClient.addr = addr;
			newClient.addrLen = sizeof(addr);
			m_ConnectedClients.push_back(newClient);
			clientId = m_ConnectedClients.size() - 1;
		}

		ClientInfo& client = m_ConnectedClients[clientId];
		
		memcpy(&client.x, (const void*)&(buffer[0]), sizeof(float));
		memcpy(&client.z, (const void*)&(buffer[4]), sizeof(float));
		memcpy(&client.l, (const void*)&(buffer[8]), sizeof(float));
		memcpy(&client.r, (const void*)&(buffer[12]), sizeof(float));

		printf("From: %s:%d: {%.2f, %.2f}\n", inet_ntoa(client.addr.sin_addr), client.addr.sin_port, client.x, client.z);
	}

	void NetworkManager::BroadcastUpdatesToClients()
	{
		std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
		if (m_NextBroadcastTime > currentTime)
		{
			return;
		}

		printf("broadcast!\n");

		m_NextBroadcastTime = currentTime + std::chrono::milliseconds(200);

		// Add 20 ms to the next broadcast time from now()
		//m_NextBroadcastTime 

		const int length = sizeof(PlayerPosition) * 4;
		char data[length];


		PlayerPosition positions[4];

		for (int i = 0; i < m_ConnectedClients.size(); i++)
		{
			memcpy(&data[i * sizeof(PlayerPosition)], &m_ConnectedClients[i].x, sizeof(float));
			memcpy(&data[i * sizeof(PlayerPosition) + sizeof(float)], &m_ConnectedClients[i].z, sizeof(float));
		}

		// Write
		for (int i = 0; i < m_ConnectedClients.size(); i++)
		{
			ClientInfo& client = m_ConnectedClients[i];
			int result = sendto(m_ListenSocket, &data[0], length, 0, (SOCKADDR*)&client.addr, client.addrLen);
			if (result == SOCKET_ERROR) {
				// TODO: We want to handle this differently.
				printf("send failed with error %d\n", WSAGetLastError());
				closesocket(m_ListenSocket);
				WSACleanup();
				return;
			}
		}
	}


} // namespace net