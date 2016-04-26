#pragma once

class Actor {
public:
	int x, y;
	int ch;
	const char *name;
	TCODColor col;
	bool blocks;
	Attacker *attacker;
	Destructible *destructible;
	Ai *ai;

	Actor(int x, int y, int ch, const char *name, const TCODColor &col);
	void update();
	void render() const;
};