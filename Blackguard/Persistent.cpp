#include "Main.h"

void Gui::save(TCODZip &zip) {
	zip.putInt(log.size());
	for (Message **iterator = log.begin(); iterator != log.end(); iterator++) {
		zip.putString((*iterator)->text);
		zip.putColor(&(*iterator)->col);
	}
}

void Gui::load(TCODZip &zip) {
	int nbMessages = zip.getInt();
	while (nbMessages > 0) {
		const char *text = zip.getString();
		TCODColor col = zip.getColor();
		message(col, text);
		nbMessages--;
	}
}

void Map::save(TCODZip &zip) {
	zip.putInt(seed);
	for (int i = 0; i < width * height; i++) {
		zip.putInt(tiles[i].explored);
	}
}

void Map::load(TCODZip &zip) {
	seed = zip.getInt();
	init(false);
	for (int i = 0; i < width * height; i++) {
		tiles[i].explored = zip.getInt();
	}
}

void Actor::save(TCODZip &zip) {
	zip.putInt(x);
	zip.putInt(y);
	zip.putInt(ch);
	zip.putColor(&col);
	zip.putString(name);
	zip.putInt(blocks);
	zip.putInt(fovOnly);
	zip.putInt(attacker != NULL);
	zip.putInt(destructible != NULL);
	zip.putInt(ai != NULL);
	zip.putInt(pickable != NULL);
	zip.putInt(container != NULL);
	if (attacker) attacker->save(zip);
	if (destructible) destructible->save(zip);
	if (ai) ai->save(zip);
	if (pickable) pickable->save(zip);
	if (container) container->save(zip);
}

void Actor::load(TCODZip &zip) {
	x = zip.getInt();
	y = zip.getInt();
	ch = zip.getInt();
	col = zip.getColor();
	name = _strdup(zip.getString());
	blocks = zip.getInt();
	fovOnly = zip.getInt();
	bool hasAttacker = zip.getInt();
	bool hasDestructible = zip.getInt();
	bool hasAi = zip.getInt();
	bool hasPickable = zip.getInt();
	bool hasContainer = zip.getInt();

	if (hasAttacker) {
		attacker = new Attacker(0.0f);
		attacker->load(zip);
	}

	if (hasDestructible) {
		destructible = Destructible::create(zip);
	}

	if (hasAi) {
		ai = Ai::create(zip);
	}

	if (hasPickable) {
		pickable = Pickable::create(zip);
	}

	if (hasContainer) {
		container = new Container(0);
		container->load(zip);
	}
}

void Attacker::save(TCODZip &zip) {
	zip.putFloat(power);
}

void Attacker::load(TCODZip &zip) {
	power = zip.getFloat();
}

void Destructible::save(TCODZip &zip) {
	zip.putInt(xp);
	zip.putFloat(maxHp);
	zip.putFloat(hp);
	zip.putFloat(defense);
	zip.putString(corpseName);
}

void Destructible::load(TCODZip &zip) {
	xp = zip.getInt();
	maxHp = zip.getFloat();
	hp = zip.getFloat();
	defense = zip.getFloat();
	corpseName = _strdup(zip.getString());
}

void PlayerDestructible::save(TCODZip &zip) {
	zip.putInt(PLAYER);
	Destructible::save(zip);
}

void MonsterDestructible::save(TCODZip &zip) {
	zip.putInt(MONSTER);
	Destructible::save(zip);
}

Destructible *Destructible::create(TCODZip &zip) {
	DestructibleType type = (DestructibleType) zip.getInt();
	Destructible *destructible = NULL;
	switch (type) {
		case MONSTER: destructible = new MonsterDestructible(0, 0, NULL, 0); break;
		case PLAYER: destructible = new PlayerDestructible(0, 0, NULL); break;
	}
	destructible->load(zip);
	return destructible;
}

void Container::save(TCODZip &zip) {
	zip.putInt(size);
	zip.putInt(inventory.size());
	for (Actor **iterator = inventory.begin(); iterator != inventory.end(); iterator++) {
		(*iterator)->save(zip);
	}
}

void Container::load(TCODZip &zip) {
	size = zip.getInt();
	int nbActors = zip.getInt();
	while (nbActors > 0) {
		Actor *actor = new Actor(0, 0, 0, NULL, TCODColor::white);
		actor->load(zip);
		inventory.push(actor);
		nbActors--;
	}
}

Pickable *Pickable::create(TCODZip &zip) {
	PickableType type = (PickableType)zip.getInt();
	Pickable *pickable = NULL;
	switch (type) {
		case HEALER: pickable = new Healer(0); break;
		case LIGHTNING_BOLT: pickable = new LightningBolt(0, 0); break;
		case CONFUSER: pickable = new Confuser(0, 0); break;
		case FIREBALL: pickable = new Fireball(0, 0); break;
	}
	pickable->load(zip);
	return pickable;
}

void Healer::save(TCODZip &zip) {
	zip.putInt(HEALER);
	zip.putFloat(amount);
}

void Healer::load(TCODZip &zip) {
	amount = zip.getFloat();
}

void LightningBolt::save(TCODZip &zip) {
	zip.putInt(LIGHTNING_BOLT);
	zip.putFloat(range);
	zip.putFloat(damage);
}

void LightningBolt::load(TCODZip &zip) {
	range = zip.getFloat();
	damage = zip.getFloat();
}

void Fireball::save(TCODZip &zip) {
	zip.putInt(FIREBALL);
	zip.putFloat(range);
	zip.putFloat(damage);
}

void Fireball::load(TCODZip &zip) {
	range = zip.getFloat();
	damage = zip.getFloat();
}

void Confuser::save(TCODZip &zip) {
	zip.putInt(CONFUSER);
	zip.putInt(nbTurns);
	zip.putFloat(range);
}

void Confuser::load(TCODZip &zip) {
	nbTurns = zip.getInt();
	range = zip.getFloat();
}

Ai *Ai::create(TCODZip &zip) {
	AiType type = (AiType) zip.getInt();
	Ai *ai = NULL;
	switch (type) {
		case PLAYER: ai = new PlayerAi(); break;
		case MONSTER: ai = new MonsterAi(); break;
		case CONFUSED_MONSTER: ai = new ConfusedMonsterAi(0, NULL); break;
	}
	ai->load(zip);
	return ai;
}

void MonsterAi::save(TCODZip &zip) {
	zip.putInt(MONSTER);
	zip.putInt(moveCount);
}

void MonsterAi::load(TCODZip &zip) {
	moveCount = zip.getInt();
}

void ConfusedMonsterAi::save(TCODZip &zip) {
	zip.putInt(CONFUSED_MONSTER);
	zip.putInt(nbTurns);
	oldAi->save(zip);
}

void ConfusedMonsterAi::load(TCODZip &zip) {
	nbTurns = zip.getInt();
	oldAi = Ai::create(zip);
}

void PlayerAi::save(TCODZip &zip) {
	zip.putInt(PLAYER);
}

void PlayerAi::load(TCODZip &zip) {}

const int SAVEGAME_VERSION = 0x1100;

void Engine::save() {
	if (player->destructible->isDead()) {
		TCODSystem::deleteFile("game.sav");
	} else {
		TCODZip zip;
		zip.putInt(SAVEGAME_VERSION);
		zip.putInt(map->width);
		zip.putInt(map->height);
		zip.putInt(level);
		map->save(zip);
		player->save(zip);
		stairs->save(zip);
		zip.putInt(actors.size() - 2);
		for (Actor **iterator = actors.begin(); iterator != actors.end(); iterator++) {
			if (*iterator != player && *iterator != stairs) {
				(*iterator)->save(zip);
			}
		}
		gui->save(zip);
		zip.saveToFile("game.sav");
	}
}

void Engine::load(bool pause) {
	TCODZip zip;
	int version = 0;
	if (TCODSystem::fileExists("game.sav")) {
		zip.loadFromFile("game.sav");
		version = zip.getInt();
	}
	engine.gui->menu.clear();
	if (!pause) {
		if (TCODSystem::fileExists("game.sav") && version == SAVEGAME_VERSION) {
			engine.gui->menu.addItem(Menu::CONTINUE, "Continue");
			engine.gui->menu.addItem(Menu::NEW_GAME, "New Game");
		} else {
			engine.gui->menu.addItem(Menu::NEW_GAME, "Start Game");
		}
	} else {
		if (!player->destructible->isDead()) {
			engine.gui->menu.addItem(Menu::CONTINUE, "Resume");
		}
		engine.gui->menu.addItem(Menu::NEW_GAME, "New Game");
	}
	engine.gui->menu.addItem(Menu::EXIT, "Exit");
	
	Menu::MenuItemCode menuItem = engine.gui->menu.pick(pause ? Menu::PAUSE : Menu::MAIN);
	if (menuItem == Menu::EXIT || menuItem == Menu::NONE) {
		exit(0);
	} else if (menuItem == Menu::NEW_GAME) {
		engine.term();
		engine.init();
	} else {
		engine.term();
		int width = zip.getInt();
		int height = zip.getInt();
		level = zip.getInt();
		map = new Map(width, height);
		map->load(zip);
		player = new Actor(0, 0, 0, NULL, TCODColor::white);
		player->load(zip);
		actors.push(player);
		stairs = new Actor(0, 0, 0, NULL, TCODColor::white);
		stairs->load(zip);
		actors.push(stairs);
		int nbActors = zip.getInt();
		while (nbActors > 0) {
			Actor *actor = new Actor(0, 0, 0, NULL, TCODColor::white);
			actor->load(zip);
			actors.push(actor);
			nbActors--;
		}
		gui->load(zip);
		gameStatus = STARTUP;
	}
	/*if (TCODSystem::fileExists("game.sav")) {
		TCODZip zip;
		zip.loadFromFile("game.sav");
		int width = zip.getInt();
		int height = zip.getInt();
		level = zip.getInt();
		map = new Map(width, height);
		map->load(zip);
		player = new Actor(0, 0, 0, NULL, TCODColor::white);
		player->load(zip);
		actors.push(player);
		stairs = new Actor(0, 0, 0, NULL, TCODColor::white);
		stairs->load(zip);
		actors.push(stairs);
		int nbActors = zip.getInt();
		while (nbActors > 0) {
			Actor *actor = new Actor(0, 0, 0, NULL, TCODColor::white);
			actor->load(zip);
			actors.push(actor);
			nbActors--;
		}
		gui->load(zip);
	} else {
		engine.init();
	}*/
}
