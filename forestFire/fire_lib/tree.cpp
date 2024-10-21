//
// Created by PP on 03.01.2017.
//

#include "tree.h"

TreeInfos::TreeInfos(const char* filename) {
	getTreeInfo(filename);
}

void TreeInfos::getTreeInfo(const char* filename) {
    std::string line;
    std::ifstream readfile(filename);

    int index, prob,radius,burnSpeed;
    std::string type, color;

    if (readfile.is_open()) {
        while(readfile >> index >> type >> radius >> burnSpeed >> color) {
			tree t;
			t.id = index;
			t.type = type;
			t.radius = radius;
			t.burnSpeed = burnSpeed;

            tInfo[index] = t;
            //std::cout << index << ", " << type << ", " << prob << ", " << radius << ", " << burnSpeed << std::endl;
        }
        readfile.close();
    }
    else 
		std::cout << "Unable to open file" << std::endl;
}


