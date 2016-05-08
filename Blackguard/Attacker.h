#pragma once
class Actor;

class Attacker : public Serializable {
public:
	float power;

	Attacker(float power);
	void attack(Actor *owner, Actor *target);
	void save(json j);
};