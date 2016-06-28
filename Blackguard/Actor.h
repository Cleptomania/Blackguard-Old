#pragma once

class Actor : public Persistent {
public:
	int x, y;
	int ch;
	const char *name;
	TCODColor col;
	bool blocks;
	bool fovOnly;
	Attacker *attacker;
	Destructible *destructible;
	Ai *ai;
	Pickable *pickable;
	Container *container;

	Actor(int x, int y, int ch, const char *name, const TCODColor &col);
	~Actor();
	float getDistance(int cx, int cy) const;
	void update();
	void render() const;
	void save(TCODZip &zip);
	void load(TCODZip &zip);
};