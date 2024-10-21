//
// Created by PP on 03.01.2017.
//

#ifndef FORESTFIRE_TREE_H
#define FORESTFIRE_TREE_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>

#define BURNED_DOWN_TREE_INDEX 126
#define BURNED_DOWN_VALUE 101
#define TREE_CATCH_FIRE_TEMP 400
#define TREE_BURNED_TEMPERATURE 500
#define TEMPERATURE_FIRE_RISE 50

struct tree {
    int id, prob, radius, burnSpeed;
    std::string type;
};

class TreeInfos {
    public:
		TreeInfos(const char* filename);

		void getTreeInfo(const char* filename);

		int getTreeNumber(void) const { return tInfo.size(); }
		tree getTree(const int index) { return tInfo[index]; }

	private:
		std::unordered_map<int, tree> tInfo;
};

#endif //FORESTFIRE_TREE_H

