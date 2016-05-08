#include <fstream>
#include <string>
#include <iostream>
#include "Main.h"

void Gui::save(json j) {
	json j2;
	j2["messages"] = json::array();
	for (Message **iterator = log.begin(); iterator != log.end(); iterator++) {
		json j3 =   {"message", { {"text", (*iterator)->text}, saveColor((*iterator)->col)} };
		j2["messages"].operator+=(j3);
	}
	j["gui"] = j2;
}

/*
void Gui::load(json j) {
	json j2 = j.at("messages");
	int nbMessages = j2.count("message");
	if (nbMessages > 0) {
		json::iterator iterator = j2.begin();
		for (json::iterator iterator = j2.begin(); iterator != j2.end(); iterator++) {
			json j3 = *iterator;
			std::string text = j3.value("text", "");
			TCODColor col = loadColor(j3.find("color"));
		}
	}
}
*/

void Map::save(json j) {
	json j2;
	j2["seed"] = seed;
	j2["tiles_explored"] = json::array;
	for (int i = 0; i < width * height; i++) {
		j2["tiles_explored"].operator+=({ i, tiles[i].explored });
	}
	j["map"] = j2;
}

void Actor::save(json j) {
	json j2;
	j2["x"] = x;
	j2["y"] = y;
	j2["ch"] = ch;
	j2["col"] = saveColor(col);
	j2["name"] = name;
	j2["blocks"] = blocks;
	
	if (attacker) {
		attacker->save(j2);
	} else {
		j2["attacker"] = false;
	}

	if (destructible) {
		destructible->save(j2);
	} else {
		j2["destructible"] = false;
	}

	if (container) {
		container->save(j2);
	} else {
		j2["container"] = false;
	}

	if (pickable) {
		pickable->save(j2);
	} else {
		j2["pickable"] = false;
	}

	if (ai) {
		ai->save(j2);
	}
	else {
		j2["ai"] = false;
	}

	j["actor"] = j2;
}

void Attacker::save(json j) {
	json j2;
	j2["power"] = power;
	j["attacker"] = j2;
}

void Destructible::save(json j) {
	json j2;
	j2["maxHp"] = maxHp;
	j2["hp"] = hp;
	j2["defense"] = defense;
	j2["corpseName"] = corpseName;
	if (type == PLAYER) {
		j2["type"] = "player";
	} else if (type == MONSTER) {
		j2["type"] = "monster";
	}
	j["destructible"] = j2;
}

void Container::save(json j) {
	json j2;
	j2["size"] = size;
	for (Actor **iterator = inventory.begin(); iterator != inventory.end(); iterator++) {
		(*iterator)->save(j2["actors"]);
	}
	j["container"] = j2;
}

void Healer::save(json j) {
	json j2;
	j2["amount"] = amount;
	j["healer"] = j2;
}

void LightningBolt::save(json j) {
	json j2;
	j2["range"] = range;
	j2["damage"] = damage;
	j["lightning_bolt"] = j2;
}

void Fireball::save(json j) {
	json j2;
	j2["range"] = range;
	j2["damage"] = damage;
	j["fireball"] = j2;
}

void Confuser::save(json j) {
	json j2;
	j2["nbTurns"] = nbTurns;
	j2["range"] = range;
	j["confuser"] = j2;
}

void MonsterAi::save(json j) {
	json j2;
	j2["move_count"] = moveCount;
	j["monsterai"] = j2;
}

void ConfusedMonsterAi::save(json j) {
	json j2;
	j2["nbTurns"] = nbTurns;
	j["confusedmonsterai"] = j2;
}

void Engine::save() {
	if (player->destructible->isDead()) {
		TCODSystem::deleteFile("game.txt");
	} else {
		json j;
		j["width"] = map->width;
		j["height"] = map->height;
		map->save(j);
		player->save(j);
		j["numActors"] = (actors.size() - 1);
		for (Actor **iterator = actors.begin(); iterator = actors.end(); iterator++) {
			if (*iterator != player) {
				(*iterator)->save(j);
			}
		}
		gui->save(j);
		std::string output = j.dump(4);
		std::ofstream out("game.txt");
		out << output;
		out.close();
	}
}

TCODColor *Serializable::loadColor(json j) {
	int h = j.value("h", 0);
	int s = j.value("s", 0);
	int v = j.value("v", 0);

	TCODColor *col = new TCODColor(h, s, v);
	return col;
	
}

void Serializable::save(json j) {}

json Serializable::saveColor(TCODColor col) {
	int h = col.getHue();
	int s = col.getSaturation();
	int v = col.getValue();

	json j = { "color", {{"h", h}, {"s", s}, {"v", v}} };
	return j;
}

