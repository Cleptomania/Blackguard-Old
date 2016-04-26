#include "Main.h"

Engine::Engine(int screenWidth, int screenHeight) : screenWidth(screenWidth), screenHeight(screenHeight), gameStatus(STARTUP), fovRadius(10) {
	TCODConsole::setCustomFont("arial12x12.png", TCOD_FONT_LAYOUT_TCOD | TCOD_FONT_TYPE_GREYSCALE);
	TCODConsole::initRoot(screenWidth, screenHeight, "Blackguard", false);
	player = new Actor(40, 25, '@', "Player", TCODColor::white);
	player->destructible = new PlayerDestructible(30, 2, "your body");
	player->attacker = new Attacker(5);
	player->ai = new PlayerAi();
	actors.push(player);
	map = new Map(120, 68);
	gui = new Gui();
	gui->message(TCODColor::red, "Welcome to Blackguard!\n Great treasure awaits you in the dungeons ahead.");
}

Engine::~Engine() {
	actors.clearAndDelete();
	delete map;
	delete gui;
}

void Engine::sendToBack(Actor *actor) {
	actors.remove(actor);
	actors.insertBefore(actor, 0);
}

void Engine::update() {
	if (gameStatus == STARTUP) map->computeFov();
	gameStatus = IDLE;
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse);
	player->update();
	if (gameStatus == NEW_TURN) {
		for (Actor **iterator = actors.begin(); iterator != actors.end(); iterator++) {
			Actor *actor = *iterator;
			if (actor != player) {
				actor->update();
			}
		}
	}
}

void Engine::render() {
	TCODConsole::root->clear();
	map->render();

	for (Actor **iterator = actors.begin(); iterator != actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (map->isInFov(actor->x, actor->y)) {
			actor->render();
		}
	}
	gui->render();
}
