
#include <iostream>
#include "model.h"
#include "tree.h"


int main(int argc, char** argv) {
	Model model;
	TreeInfos infos("../treeInfos.txt");
	model.setTreeInfos(&infos);

	// get the model parameters file from the command line
	if (argc == 2) {
		std::cout << "loading model parameters from '" << argv[1] << "'..." << std::endl;
		model.loadFromFile(argv[1]);
	}
	else { // or choose some default parameters
		std::cout << "setting default parameter values..." << std::endl;
		ModelOptions options;
		options.mapSize = 10;
		options.mapGranularity = 1000;
		options.tempMean = 20.0;
		options.tempStd = 3.0;
		options.numberStartFires = 2;
		options.numberIterations = 100;
		options.winddx = 2.0;
		options.winddy = 0.0;
		options.treeRatio = 0.3;
		options.initialTreeMap = "";
		options.outputToFile = 0;

		model.init(options);
	}

	model.execute();

	return 0;
}
