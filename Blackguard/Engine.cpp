#include "Main.h"

Engine::Engine(int screenWidth, int screenHeight) : screenWidth(screenWidth), screenHeight(screenHeight), level(1), gameStatus(STARTUP), fovRadius(10) {
	TCODConsole::setCustomFont("arial12x12.png", TCOD_FONT_LAYOUT_TCOD | TCOD_FONT_TYPE_GREYSCALE);
	TCODConsole::initRoot(screenWidth, screenHeight, "Blackguard", false);
	gui = new Gui();
}

Engine::~Engine() {
	term();
	delete gui;
}

void Engine::init() {
	player = new Actor(40, 25, '@', "player", TCODColor::white);
	player->destructible = new PlayerDestructible(30, 2, "your body");
	player->attacker = new Attacker(5);
	player->ai = new PlayerAi();
	player->container = new Container(26);
	actors.push(player);
	cam = new Camera(screenWidth, screenWidth - 7, player->x - (screenWidth / 2), player->y - (screenHeight / 2));
	stairs = new Actor(0, 0, '>', "stairs", TCODColor::white);
	stairs->blocks = false;
	stairs->fovOnly = false;
	actors.push(stairs);
	map = new Map(screenWidth * 3, screenHeight * 3);
	map->init(true);
	gui->message(TCODColor::red, "Welcome to Blackguard!\nGreat treasure awaits you in the dungeons ahead.");
	gameStatus = STARTUP;
}

void Engine::term() {
	actors.clearAndDelete();
	if (map) delete map;
	gui->clear();
}

bool Engine::pickATile(int *x, int *y, float maxRange) {
	while (!TCODConsole::isWindowClosed()) {
		render();
		for (int cx = 0; cx < map->width; cx++) {
			for (int cy = 0; cy < map->height; cy++) {
				if (map->isInFov(cx, cy) && (maxRange == 0 || player->getDistance(cx, cy) <= maxRange)) {
					TCODColor col = TCODConsole::root->getCharBackground(cx, cy);
					col = col * 1.2f;
					TCODConsole::root->setCharBackground(cx, cy, col);
				}
			}
		}
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse);
		if (map->isInFov(mouse.cx, mouse.cy) && (maxRange == 0 || player->getDistance(mouse.cx, mouse.cy) <= maxRange)) {
			TCODConsole::root->setCharBackground(mouse.cx, mouse.cy, TCODColor::white);
			if (mouse.lbutton_pressed) {
				*x = mouse.cx;
				*y = mouse.cy;
				return true;
			}
		}
		if (mouse.rbutton_pressed || lastKey.vk != TCODK_NONE) {
			return false;
		}
		TCODConsole::flush();
	}
	return false;
}

bool Engine::isInCamera(Actor *actor) {
	if (actor->x >= cam->x && actor->x <= cam->x2 && actor->y >= cam->y && actor->y <= cam->y2)
		return true;
	else
		return false;
}

Actor *Engine::getActor(int x, int y) const {
	for (Actor **iterator = actors.begin(); iterator != actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor->x == x && actor->y == y && actor->destructible && !actor->destructible->isDead()) {
			return actor;
		}
	}
	return NULL;
}

Actor *Engine::getClosestMonster(int x, int y, float range) const {
	Actor *closest = NULL;
	float bestDistance = 1E6f;
	for (Actor **iterator = actors.begin(); iterator != actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor != player && actor->destructible && !actor->destructible->isDead()) {
			float distance = actor->getDistance(x, y);
			if (distance < bestDistance && (distance <= range || range == 0.0f)) {
				bestDistance = distance;
				closest = actor;
			}
		}
	}
	return closest;
}

void Engine::nextLevel() {
	level++;
	gui->message(TCODColor::lightViolet, "You take a moment to rest, and recover your strength.");
	player->destructible->heal(player->destructible->maxHp / 2);
	gui->message(TCODColor::red, "After a rare moment of peace, you descend\ndeeper into the dungeon...");
	delete map;
	for (Actor **iterator = actors.begin(); iterator != actors.end(); iterator++) {
		if (*iterator != player && *iterator != stairs) {
			delete *iterator;
			iterator = actors.remove(iterator);
		}
	}
	map = new Map(160, 93);
	map->init(true);
	gameStatus = STARTUP;
}

void Engine::sendToBack(Actor *actor) {
	actors.remove(actor);
	actors.insertBefore(actor, 0);
}

void Engine::update() {
	if (gameStatus == STARTUP) map->computeFov();
	gameStatus = IDLE;
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse);
	if (lastKey.vk == TCODK_ESCAPE) {
		save();
		load(true);
	}
	player->update();
	int ncx = player->x - (screenWidth / 2);
	int ncy = player->y - (screenHeight / 2);
	if (ncx < 0) ncx = 0;
	if (ncy < 0) ncy = 0;
	if (ncx + cam->width > map->width) ncx = map->width;
	if (ncx + cam->height > map->height) ncx = map->height;
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
		if (isInCamera(actor))
			if (!actor->fovOnly && map->isExplored(actor->x, actor->y) || map->isInFov(actor->x, actor->y)) {
				actor->render();
			}
	}
	gui->render();
}
