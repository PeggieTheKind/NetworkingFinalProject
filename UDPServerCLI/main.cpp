#include "NetworkManager.h"

#include <conio.h>

int main(int argc, char** argv)
{
	net::NetworkManager networkManager;
	networkManager.Initialize();

	bool running = true;
	while (running)
	{
		networkManager.Update();
		if (_kbhit())
		{
			if (_getch() == 27)
			{
				running = false;
			}
		}
	}

	networkManager.Destroy();

	return 0;
}