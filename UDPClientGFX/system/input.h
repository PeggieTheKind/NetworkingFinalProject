#pragma once

#include <common.h>

gdpNamespaceBegin

class Input
{
public:
	Input();
	~Input();

	void Update();

	void PressKey(unsigned char key);
	void ReleaseKey(unsigned char key);

	void PressSpecial(int key);
	void ReleaseSpecial(int key);

	void MoveMouse(int x, int y);
	void DragMouse(int x, int y);

	void MouseButton(int button, int state);

private:
	unsigned char m_Keys[255];
	unsigned char m_LastKeys[255];

	int m_LastMouseX;
	int m_LastMouseY;
	int m_CurrMouseX;
	int m_CurrMouseY;
};

gdpNamespaceEnd