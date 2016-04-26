#pragma once
class Actor;

class Container {
public:
	int size;
	TCODList<Actor *> inventory;

	Container(int size);
	~Container();
	bool add(Actor *actor);
	void remove(Actor *actor);
};