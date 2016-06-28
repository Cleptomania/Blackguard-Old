#pragma once
class Actor;

class Destructible : public Persistent {
public:
	int xp;
	float maxHp;
	float hp;
	float defense;
	const char *corpseName;

	Destructible(float maxHp, float defense, const char *corpseName, int xp);
	~Destructible();
	inline bool isDead() { return hp <= 0; }
	float heal(float amount);
	float takeDamage(Actor *owner, float damage);
	virtual void die(Actor *owner);
	void save(TCODZip &zip);
	void load(TCODZip &zip);
	static Destructible *create(TCODZip &zip);
protected:
	enum DestructibleType {
		MONSTER, PLAYER
	};
};

class MonsterDestructible : public Destructible {
public:
	MonsterDestructible(float maxHp, float defense, const char *corpseName, int xp);
	void save(TCODZip &zip);
	void die(Actor *owner);
};

class PlayerDestructible : public Destructible {
public:
	PlayerDestructible(float maxHp, float defense, const char *corpseName);
	void save(TCODZip &zip);
	void die(Actor *owner);
};