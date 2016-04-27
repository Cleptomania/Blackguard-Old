#pragma once
class Actor;

class Ai {
public:
	virtual ~Ai() {};
	virtual void update(Actor *owner) = 0;
};

class MonsterAi : public Ai {
public:
	MonsterAi();
	void update(Actor *owner);
protected:
	int moveCount;
	void moveOrAttack(Actor *owner, int targetx, int targety);
};

class ConfusedMonsterAi : public Ai {
public:
	ConfusedMonsterAi(int nbTurns, Ai *oldAi);
	void update(Actor *owner);
protected:
	int nbTurns;
	Ai *oldAi;
};

class PlayerAi : public Ai {
public:
	void update(Actor *owner);
protected:
	Actor *choseFromInventory(Actor *owner);
	void handleActionKey(Actor *owner, int ascii);
	bool moveOrAttack(Actor *owner, int targetx, int targety);
};