#include <stdio.h>
#include "Main.h"

Destructible::Destructible(float maxHp, float defense, const char *corpseName, DestructibleType type) : maxHp(maxHp), hp(maxHp), defense(defense), corpseName(corpseName), type(type) {}

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

MonsterDestructible::MonsterDestructible(float maxHp, float defense, const char *corpseName) : Destructible(maxHp, defense, corpseName, MONSTER) {}

void MonsterDestructible::die(Actor *owner) {
	engine.gui->message(TCODColor::lightGrey, "%s is dead", owner->name);
	Destructible::die(owner);
}

PlayerDestructible::PlayerDestructible(float maxHp, float defense, const char *corpseName) : Destructible(maxHp, defense, corpseName, PLAYER) {}

void PlayerDestructible::die(Actor *owner) {
	engine.gui->message(TCODColor::red, "You Died!");
	Destructible::die(owner);
	engine.gameStatus = Engine::DEFEAT;
}
