#pragma once
class Actor;

class Ai : public Persistent {
public:
	Ai();
	virtual ~Ai() {};
	virtual void update(Actor *owner) = 0;
	static Ai *create(TCODZip &zip);
protected:
	enum AiType {
		MONSTER, CONFUSED_MONSTER, PLAYER
	};
};

class MonsterAi : public Ai {
public:
	MonsterAi();
	void update(Actor *owner);
	void save(TCODZip &zip);
	void load(TCODZip &zip);
protected:
	int moveCount;
	void moveOrAttack(Actor *owner, int targetx, int targety);
};

class ConfusedMonsterAi : public Ai {
public:
	ConfusedMonsterAi(int nbTurns, Ai *oldAi);
	void update(Actor *owner);
	void save(TCODZip &zip);
	void load(TCODZip &zip);
protected:
	int nbTurns;
	Ai *oldAi;
};

class PlayerAi : public Ai {
public:
	int xpLevel;
	PlayerAi();
	int getNextLevelXp();
	void save(TCODZip &zip);
	void load(TCODZip &zip);
	void update(Actor *owner);
protected:
	Actor *choseFromInventory(Actor *owner);
	void handleActionKey(Actor *owner, int ascii);
	bool moveOrAttack(Actor *owner, int targetx, int targety);
};