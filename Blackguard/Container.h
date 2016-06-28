#pragma once
class Actor;

class Container : public Persistent {
public:
	int size;
	TCODList<Actor *> inventory;

	Container(int size);
	~Container();
	bool add(Actor *actor);
	void remove(Actor *actor);
	void save(TCODZip &zip);
	void load(TCODZip &zip);
};