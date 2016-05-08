#pragma once

class Gui : public Serializable {
public:
	Gui();
	~Gui();
	void render();
	void message(const TCODColor &col, const char *text, ...);
	void save(json j);

protected:
	struct Message {
		char *text;
		TCODColor col;
		Message(const char *text, const TCODColor &col);
		~Message();
	};
	
	TCODList<Message *> log;
	TCODConsole *con;

	void renderMouseLook();
	void renderBar(int x, int y, int width, const char *name, float value, float maxValue, const TCODColor &barColor, const TCODColor &backColor);
};