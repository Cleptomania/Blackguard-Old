#include "Main.h"
#define TORCH_RADIUS 10.0f
#define SQUARED_TORCH_RADIUS (TORCH_RADIUS * TORCH_RADIUS)

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;

static const int MAX_ROOM_MONSTERS = 3;

static const int MAX_ROOM_ITEMS = 2;

class BspListener : public ITCODBspCallback {
private:
	Map &map;
	int roomNum;
	int lastx, lasty;
public:
	BspListener(Map &map) : map(map), roomNum(0) {}
	bool visitNode(TCODBsp *node, void *userData) {
		bool withActors = (bool)userData;
		if (node->isLeaf()) {
			int x, y, w, h;
			w = map.rng->getInt(ROOM_MIN_SIZE, node->w - 2);
			h = map.rng->getInt(ROOM_MIN_SIZE, node->h - 2);
			x = map.rng->getInt(node->x + 1, node->x + node->w - w - 1);
			y = map.rng->getInt(node->y + 1, node->y + node->h - h - 1);
			map.createRoom(roomNum == 0, x, y, x + w - 1, y + h - 1, withActors);
			if (roomNum != 0) {
				map.dig(lastx, lasty, x + w / 2, lasty);
				map.dig(x + w / 2, lasty, x + w / 2, y + h / 2);
			}
			lastx = x + w / 2;
			lasty = y + h / 2;
			roomNum++;
		}
		return true;
	}
};

Map::Map(int width, int height) : width(width), height(height) {
	seed = TCODRandom::getInstance()->getInt(0, 0x7FFFFFFF);
}

Map::~Map() {
	delete[] tiles;
	delete map;
}

void Map::init(bool withActors) {
	rng = new TCODRandom(seed, TCOD_RNG_CMWC);
	tiles = new Tile[width*height];
	map = new TCODMap(width, height);
	noise = new TCODNoise(1);
	torchx = 0.0f;
	TCODBsp bsp(0, 0, width, height);
	bsp.splitRecursive(rng, 8, ROOM_MAX_SIZE, ROOM_MAX_SIZE, 1.5f, 1.5f);
	BspListener listener(*this);
	bsp.traverseInvertedLevelOrder(&listener, (void *)withActors);
}

void Map::dig(int x1, int y1, int x2, int y2) {
	if (x2 < x1) {
		int tmp = x2;
		x2 = x1;
		x1 = tmp;
	}
	if (y2 < y1) {
		int tmp = y2;
		y2 = y1;
		y1 = tmp;
	}
	for (int tilex = x1; tilex <= x2; tilex++) {
		for (int tiley = y1; tiley <= y2; tiley++) {
			map->setProperties(tilex, tiley, true, true);
		}
	}
}

void Map::createRoom(bool first, int x1, int y1, int x2, int y2, bool withActors) {
	dig(x1, y1, x2, y2);
	if (!withActors) {
		return;
	}
	if (first) {
		engine.player->x = (x1 + x2) / 2;
		engine.player->y = (y1 + y2) / 2;
	} else {
		TCODRandom *rng = TCODRandom::getInstance();
		int nbMonsters = rng->getInt(0, MAX_ROOM_MONSTERS);
		while (nbMonsters > 0) {
			int x = rng->getInt(x1, x2);
			int y = rng->getInt(y1, y2);
			if (canWalk(x, y)) {
				addMonster(x, y);
			}
			nbMonsters--;
		}

		int nbItems = rng->getInt(0, MAX_ROOM_ITEMS);
		while (nbItems > 0) {
			int x = rng->getInt(x1, x2);
			int y = rng->getInt(y1, y2);
			if (canWalk(x, y)) {
				addItem(x, y);
			}
			nbItems--;
		}
	}
	engine.stairs->x = (x1 + x2) / 2;
	engine.stairs->y = (y1 + y2) / 2;
}

void Map::addMonster(int x, int y) {
	TCODRandom *rng = TCODRandom::getInstance();
	if (rng->getInt(0, 100) < 80) {
		Actor *orc = new Actor(x, y, 'o', "Orc", TCODColor::desaturatedGreen);
		orc->destructible = new MonsterDestructible(10, 0, "Dead Orc", 50);
		orc->attacker = new Attacker(3);
		orc->ai = new MonsterAi();
		engine.actors.push(orc);
	} else {
		Actor *troll = new Actor(x, y, 't', "Troll", TCODColor::darkerGreen);
		troll->destructible = new MonsterDestructible(16, 1, "Dead Troll", 100);
		troll->attacker = new Attacker(4);
		troll->ai = new MonsterAi();
		engine.actors.push(troll);
	}
}

void Map::addItem(int x, int y) {
	TCODRandom *rng = TCODRandom::getInstance();
	int dice = rng->getInt(0, 100);
	if (dice < 70) {
		Actor *healthPotion = new Actor(x, y, '!', "health potion", TCODColor::yellow);
		healthPotion->blocks = false;
		healthPotion->pickable = new Healer(4);
		engine.actors.push(healthPotion);
		engine.sendToBack(healthPotion);
	} else if (dice < 70+10) {
		Actor *scrollOfLightningBolt = new Actor(x, y, '#', "scroll of lightning bolt", TCODColor::lightYellow);
		scrollOfLightningBolt->blocks = false;
		scrollOfLightningBolt->pickable = new LightningBolt(5, 20);
		engine.actors.push(scrollOfLightningBolt);
		engine.sendToBack(scrollOfLightningBolt);
	} else if (dice < 70+10+10) {
		Actor *scrollOfFireball = new Actor(x, y, '#', "scroll of fireball", TCODColor::lightYellow);
		scrollOfFireball->blocks = false;
		scrollOfFireball->pickable = new Fireball(3, 12);
		engine.actors.push(scrollOfFireball);
		engine.sendToBack(scrollOfFireball);
	} else if (dice < 70+10+10+10) {
		Actor *scrollOfConfusion = new Actor(x, y, '#', "scroll of confusion", TCODColor::lightYellow);
		scrollOfConfusion->blocks = false;
		scrollOfConfusion->pickable = new Confuser(10, 8);
		engine.actors.push(scrollOfConfusion);
		engine.sendToBack(scrollOfConfusion);
	}
	
}

bool Map::isWall(int x, int y) const {
	return !map->isWalkable(x, y);
}

bool Map::isExplored(int x, int y) const {
	return tiles[x + y * width].explored;
}

bool Map::isInFov(int x, int y) const {
	if (x < 0 || x >= width || y < 0 || y >= height) {
		return false;
	}
	if (map->isInFov(x, y)) {
		tiles[x + y * width].explored = true;
		return true;
	}
	return false;
}

bool Map::canWalk(int x, int y) const {
	if (isWall(x, y)) {
		return false;
	}
	for (Actor **iterator = engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor->blocks && actor->x == x && actor->y == y) {
			return false;
		}
	}
	return true;
}

void Map::computeFov() {
	map->computeFov(engine.player->x, engine.player->y, (int) (TORCH_RADIUS), true, (TCOD_fov_algorithm_t) 0);
}

void Map::render() {
	static const TCODColor darkWall(0, 0, 60);
	static const TCODColor lightWall(130, 110, 50);

	static const TCODColor darkGround(25, 25, 40);
	static const TCODColor lightGround(200, 180, 50);

	float dx = 0.0f, dy = 0.0f, di = 0.0f;
	torchx += 0.2f;
	float tdx = torchx + 20.0f;
	dx = noise->get(&tdx) * 1.5f;
	tdx += 30.0f;
	dy = noise->get(&tdx) * 1.5f;
	di = 0.2f * noise->get(&torchx);

	for (int x = engine.cam->x; x < engine.cam->x2; x++) {
		for (int y = engine.cam->y; y < engine.cam->y2; y++) {
			bool visible = isInFov(x, y);
			bool explored = isExplored(x, y);
			bool wall = isWall(x, y);
			if (!visible) {
				if (explored) {
					TCODConsole::root->setCharBackground(x, y, wall ? darkWall : darkGround, TCOD_BKGND_SET);
				} else {
					TCODConsole::root->setCharBackground(x, y, TCODColor::black);
				}
			} else {
				TCODColor base = (wall ? darkWall : darkGround);
				TCODColor light = (wall ? lightWall : lightGround);
				float r = (float)((x - engine.player->x + dx) * (x - engine.player->x + dx) + (y - engine.player->y + dy) * (y - engine.player->y + dy));
				if (r < SQUARED_TORCH_RADIUS) {
					float l = (SQUARED_TORCH_RADIUS - r) / SQUARED_TORCH_RADIUS + di;
					l = CLAMP(0.0f, 1.0f, l);
					base = TCODColor::lerp(base, light, l);
				}
				light = base;
				TCODConsole::root->setCharBackground(x, y, light, TCOD_BKGND_SET);
			}
		}
	}
}