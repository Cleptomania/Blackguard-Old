#include <iostream>
#include <fstream>
#include "json.hpp"
using json = nlohmann::json;
#include "Main.h"

Actor MonsterFactory::loadMonster(char name) {
	std::regex expression("<tag>([^<]*)</tag>");
	std::ifstream in("monsters" + name + '.txt');
	
	std::string line;
	while (std::getline(in, line)) {
		std::smatch results;
		if (std::regex_search(line, results, expression))
			std::cout << results[1] << '\n';
	}
}