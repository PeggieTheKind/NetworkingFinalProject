#include <engine/engine.h>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <gameplay/playermovementsystem.h>

using namespace gdp;

Engine g_Engine;

void PressKey_Callback(unsigned char key, int x, int y)
{
	g_Engine.PressKey(key);
}

void ReleaseKey_Callback(unsigned char key, int x, int y)
{
	g_Engine.ReleaseKey(key);
}

void PressSpecialKey_Callback(int key, int x, int y)
{
	g_Engine.PressSpecialKey(key);
}

void ReleaseSpecialKey_Callback(int key, int x, int y)
{
	g_Engine.ReleaseSpecialKey(key);
}

void Idle_Callback()
{
	g_Engine.Update();
}

void Reshape_Callback(int w, int h)
{
	g_Engine.Resize(w, h);
}

void Render_Callback()
{
	g_Engine.Update();
	g_Engine.Render();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1200, 800);
	glutCreateWindow("UDP Multiplayer game!");

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		fprintf(stderr, "GLEW initialization failed: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	glutIgnoreKeyRepeat(1);

	glutKeyboardFunc(PressKey_Callback);
	glutKeyboardUpFunc(ReleaseKey_Callback);
	glutSpecialFunc(PressSpecialKey_Callback);
	glutSpecialUpFunc(ReleaseSpecialKey_Callback);

	glutDisplayFunc(Render_Callback);
	glutIdleFunc(Render_Callback);
	glutReshapeFunc(Reshape_Callback);
	glDisable(GL_CULL_FACE);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	g_Engine.Initialize();
	g_Engine.AddSystem(new PlayerMovementSystem());

	glutMainLoop();

	g_Engine.Destroy();

	return 0;
}