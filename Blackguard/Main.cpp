#include "libtcod.hpp"
#include "Engine.h"

Engine engine(120, 75);

int main() {
	while (!TCODConsole::isWindowClosed()) {
		engine.update();
		engine.render();
		TCODConsole::flush();
	}
	return 0;
}