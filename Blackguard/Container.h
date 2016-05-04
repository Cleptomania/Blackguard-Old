#pragma once
class Actor;

class Container : public Serializable {
public:
	int size;
	TCODList<Actor *> inventory;

	Container(int size);
	~Container();
	bool add(Actor *actor);
	void remove(Actor *actor);
	void load(json j);
	void save(json j);
};