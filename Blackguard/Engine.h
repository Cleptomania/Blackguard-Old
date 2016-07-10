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
	int level;
	Gui *gui;
	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;
	TCODList<Actor *> actors;
	Actor *player;
	Actor *stairs;
	Map *map;
	int fovRadius;
	Camera *cam;

	Engine(int screenWidth, int screenHeight);
	~Engine();
	bool pickATile(int *x, int *y, float maxRange = 0.0f);
	bool isInCamera(Actor *actor);
	Actor *getActor(int x, int y) const;
	Actor *getClosestMonster(int x, int y, float range) const;
	void nextLevel();
	void sendToBack(Actor *actor);
	void update();
	void render();
	void init();
	void term();
	void save();
	void load(bool pause);
};

extern Engine engine;