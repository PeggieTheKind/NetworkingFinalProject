#include <network/networkmanager.h>

#include <memory>

#define SERVER_PORT 8412
#define SERVER_IP "127.0.0.1"

gdpNamespaceBegin

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
		m_ServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (m_ServerSocket == INVALID_SOCKET) {
			printf("socket failed with error %d\n", WSAGetLastError());
			WSACleanup();
			return;
		}
		printf("socket created successfully!\n");

		unsigned long nonblock = 1;
		result = ioctlsocket(m_ServerSocket, FIONBIO, &nonblock);
		if (result == SOCKET_ERROR) {
			printf("set nonblocking failed with error %d\n", result);
			return;
		}
		printf("set nonblocking successfully!\n");

		m_ServerAddr.sin_family = AF_INET;
		m_ServerAddr.sin_port = htons(SERVER_PORT);
		m_ServerAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
		m_ServerAddrLen = sizeof(m_ServerAddr);

		printf("NetworkManager running...\n");

		m_NextSendTime = std::chrono::high_resolution_clock::now();

		m_NetworkedPositions.resize(NUM_PLAYERS);

		m_Initialized = true;
	}

	void NetworkManager::Destroy()
	{
		if (!m_Initialized)
		{
			return;
		}

		closesocket(m_ServerSocket);
		WSACleanup();

		m_Initialized = false;
	}

	void NetworkManager::Update()
	{
		if (!m_Initialized)
		{
			return;
		}

		// Send information/data back to clients
		SendDataToServer();

		// Handle all recv data
		HandleRECV();

		// Process everything
	}

	void NetworkManager::SendPlayerPositionToServer(float x, float z, float l, float r, bool shot)
	{
		m_PlayerPosition.x = x;
		m_PlayerPosition.z = z;
		m_PlayerPosition.l = l;
		m_PlayerPosition.r = r;
		m_PlayerPosition.shot = shot;
	}

	void NetworkManager::HandleRECV()
	{
		// Read
		const int bufLen = sizeof(float) * 2 * NUM_PLAYERS;
		char buffer[bufLen];
		int result = recvfrom(m_ServerSocket, buffer, bufLen, 0, (SOCKADDR*)&m_ServerAddr, &m_ServerAddrLen);
		if (result == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				// Not a real error, we expect this.
				// -1 is an error, 0 is disconnected, >0 is a message
				// WSA uses this as a flag to check if it is a real error
				return;
			}
			// TODO: We want to handle this differently.
			printf("recvfrom failed with error %d\n", WSAGetLastError());
			Destroy();
			return;
		}


		memcpy(&m_NetworkedPositions[0].x, (const void*)&(buffer[0]), sizeof(float));
		memcpy(&m_NetworkedPositions[0].z, (const void*)&(buffer[4]), sizeof(float));

		memcpy(&m_NetworkedPositions[1].x, (const void*)&(buffer[8]), sizeof(float));
		memcpy(&m_NetworkedPositions[1].z, (const void*)&(buffer[12]), sizeof(float));

		memcpy(&m_NetworkedPositions[2].x, (const void*)&(buffer[16]), sizeof(float));
		memcpy(&m_NetworkedPositions[2].z, (const void*)&(buffer[20]), sizeof(float));

		memcpy(&m_NetworkedPositions[3].x, (const void*)&(buffer[24]), sizeof(float));
		memcpy(&m_NetworkedPositions[3].z, (const void*)&(buffer[28]), sizeof(float));
	}

	void NetworkManager::SendDataToServer()
	{
		std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
		if (m_NextSendTime > currentTime)
		{
			return;
		}

		m_NextSendTime = currentTime + std::chrono::milliseconds(50);
		
		// Add 20 ms to the next broadcast time from now()
		//m_NextBroadcastTime 


		// MessageQueue, loop through and send all messages
		// You may multiple servers, you are sending data to
		int result = sendto(m_ServerSocket, (const char*)&m_PlayerPosition, 
			sizeof(m_PlayerPosition), 0, (SOCKADDR*)&m_ServerAddr, m_ServerAddrLen);

		if (result == SOCKET_ERROR) {
			// TODO: We want to handle this differently.
			printf("send failed with error %d\n", WSAGetLastError());
			Destroy();
			return;
		}
	}


} // namespace net

gdpNamespaceEnd