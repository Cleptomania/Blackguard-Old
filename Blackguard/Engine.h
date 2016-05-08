#pragma once
class Actor;
class Map;
class Gui;

class Engine {
public:
	enum GameStatus {
		STARTUP,
		IDLE,
		NEW_TURN,
		VICTORY,
		DEFEAT
	} gameStatus;
	int screenWidth;
	int screenHeight;
	Gui *gui;
	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;
	TCODList<Actor *> actors;
	Actor *player;
	Map *map;
	int fovRadius;

	Engine(int screenWidth, int screenHeight);
	~Engine();
	bool pickATile(int *x, int *y, float maxRange = 0.0f);
	Actor *getActor(int x, int y) const;
	Actor *getClosestMonster(int x, int y, float range) const;
	void sendToBack(Actor *actor);
	void update();
	void render();
	void init();
	void save();
};

extern Engine engine;