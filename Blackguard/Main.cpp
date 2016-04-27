#include "libtcod.hpp"
#include "Engine.h"

Engine engine(120, 75);

int main() {
	engine.load();
	while (!TCODConsole::isWindowClosed()) {
		engine.update();
		engine.render();
		TCODConsole::flush();
	}
	engine.save();
	return 0;
}