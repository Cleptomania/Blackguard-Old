#include <stdio.h>
#include "Main.h"

Destructible::Destructible(float maxHp, float defense, const char *corpseName, int xp) : maxHp(maxHp), hp(maxHp), defense(defense), xp(xp) {
	this->corpseName = _strdup(corpseName);
}

Destructible::~Destructible() {
	free((char *) corpseName);
}

float Destructible::heal(float amount) {
	hp += amount;
	if (hp > maxHp) {
		amount -= hp - maxHp;
		hp = maxHp;
	}
	return amount;
}

float Destructible::takeDamage(Actor *owner, float damage) {
	damage -= defense;
	if (damage > 0) {
		hp -= damage;
		if (hp <= 0) {
			die(owner);
		}
	}
	else {
		damage = 0;
	}
	return damage;
}

void Destructible::die(Actor *owner) {
	owner->ch = '%';
	owner->col = TCODColor::darkRed;
	owner->name = corpseName;
	owner->blocks = false;
	engine.sendToBack(owner);
}

MonsterDestructible::MonsterDestructible(float maxHp, float defense, const char *corpseName, int xp) : Destructible(maxHp, defense, corpseName, xp) {}

void MonsterDestructible::die(Actor *owner) {
	engine.gui->message(TCODColor::lightGrey, "%s is dead. You gain %d xp", owner->name, xp);
	engine.player->destructible->xp += xp;
	Destructible::die(owner);
}

PlayerDestructible::PlayerDestructible(float maxHp, float defense, const char *corpseName) : Destructible(maxHp, defense, corpseName, 0) {}

void PlayerDestructible::die(Actor *owner) {
	engine.gui->message(TCODColor::red, "You Died!");
	Destructible::die(owner);
	engine.gameStatus = Engine::DEFEAT;
}
