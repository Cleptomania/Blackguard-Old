#pragma once

class Serializable {
public:
	virtual void load(json j);
	virtual void save(json j);
protected:
	json saveColor(TCODColor col);
	TCODColor *loadColor(json j);
};