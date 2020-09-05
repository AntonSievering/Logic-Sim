// THE MAIN CPP FILEs
#define OLC_PGE_APPLICATION
#include "Engine.h"

// Entry point into the program
int main()
{
	// the Engine object
	Engine engine;
	// if the Engine can start
	if (engine.Construct(800, 800, 1, 1, false, false))
		// start the Engine
		engine.Start();
}