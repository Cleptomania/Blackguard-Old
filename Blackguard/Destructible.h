#pragma once
class Actor;

class Destructible : public Serializable {
public:
	float maxHp;
	float hp;
	float defense;
	const char *corpseName; //name once actor is dead or destroyed
	enum DestructibleType {
		MONSTER, PLAYER
	} type;

	Destructible(float maxHp, float defense, const char *corpseName, DestructibleType type);
	virtual ~Destructible() {};
	inline bool isDead() { return hp <= 0; }
	float heal(float amount);
	float takeDamage(Actor *owner, float damage);
	virtual void die(Actor *owner);
	void load(json j);
	void save(json j);
	static Destructible *create(json j);
};

class MonsterDestructible : public Destructible {
public:
	MonsterDestructible(float maxHp, float defense, const char *corpseName);
	void die(Actor *owner);
};

class PlayerDestructible : public Destructible {
public:
	PlayerDestructible(float maxHp, float defense, const char *corpseName);
	void die(Actor *owner);
};