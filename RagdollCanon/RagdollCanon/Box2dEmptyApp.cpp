#include "Game.h"

using namespace sf;

int main(int argc, char* argv[])
{
	Game* Juego;
	Juego = new Game(800, 600, "RevoluteJoint - MAVII");
	Juego->Loop();

	return 0;
}

