#include "Main.h"

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;

static const int MAX_ROOM_MONSTERS = 3;

class BspListener : public ITCODBspCallback {
private:
	Map &map;
	int roomNum;
	int lastx, lasty;
public:
	BspListener(Map &map) : map(map), roomNum(0) {}
	bool visitNode(TCODBsp *node, void *userData) {
		if (node->isLeaf()) {
			int x, y, w, h;
			TCODRandom *rng = TCODRandom::getInstance();
			w = rng->getInt(ROOM_MIN_SIZE, node->w - 2);
			h = rng->getInt(ROOM_MIN_SIZE, node->h - 2);
			x = rng->getInt(node->x + 1, node->x + node->w - w - 1);
			y = rng->getInt(node->y + 1, node->y + node->h - h - 1);
			map.createRoom(roomNum == 0, x, y, x + w - 1, y + h - 1);
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
	tiles = new Tile[width*height];
	map = new TCODMap(width, height);
	TCODBsp bsp(0, 0, width, height);
	bsp.splitRecursive(NULL, 8, ROOM_MAX_SIZE, ROOM_MAX_SIZE, 1.5f, 1.5f);
	BspListener listener(*this);
	bsp.traverseInvertedLevelOrder(&listener, NULL);
}

Map::~Map() {
	delete[] tiles;
	delete map;
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

void Map::createRoom(bool first, int x1, int y1, int x2, int y2) {
	dig(x1, y1, x2, y2);
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
	}
}

void Map::addMonster(int x, int y) {
	TCODRandom *rng = TCODRandom::getInstance();
	if (rng->getInt(0, 100) < 80) {
		Actor *orc = new Actor(x, y, 'o', "Orc", TCODColor::desaturatedGreen);
		orc->destructible = new MonsterDestructible(10, 0, "Dead Orc");
		orc->attacker = new Attacker(3);
		orc->ai = new MonsterAi();
		engine.actors.push(orc);
	} else {
		Actor *troll = new Actor(x, y, 't', "Troll", TCODColor::darkerGreen);
		troll->destructible = new MonsterDestructible(16, 1, "Dead Troll");
		troll->attacker = new Attacker(4);
		troll->ai = new MonsterAi();
		engine.actors.push(troll);
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
	map->computeFov(engine.player->x, engine.player->y, engine.fovRadius);
}

void Map::render() const {
	static const TCODColor lightWallBack(255, 221, 229);
	static const TCODColor darkWallBack(43, 38, 76);
	static const TCODColor wallFore(0, 0, 0);

	static const TCODColor lightGroundBack(10, 7, 33);
	static const TCODColor darkGroundBack(15, 15, 38);
	static const TCODColor groundFore(255, 255, 255);

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			if (isInFov(x, y)) {
				TCODConsole::root->setCharBackground(x, y, isWall(x, y) ? lightWallBack : lightGroundBack);
				TCODConsole::root->setChar(x, y, isWall(x, y) ? '#' : '.');
				TCODConsole::root->setCharForeground(x, y, groundFore);
			} else if (isExplored(x, y)) {
				TCODConsole::root->setCharBackground(x, y, isWall(x, y) ? darkWallBack : darkGroundBack);
				TCODConsole::root->setChar(x, y, isWall(x, y) ? '#' : '.');
				TCODConsole::root->setCharForeground(x, y, groundFore);
			} else {
				TCODConsole::root->setCharBackground(x, y, TCODColor::black);
			}
		}
	}
}