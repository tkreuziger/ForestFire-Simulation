
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <random>
#include <chrono>
#include <cmath>
#include <omp.h>
#include "model.h"

#define PI 3.1415926535  // 4 * atan(1)


Model::Model(void) : randomGenerator(std::random_device{}()) {
	currentIteration = 0;
}

Model::~Model(void) {
    temperatureMap.clear();
    fireMap.clear();
    treeMap.clear();
}

void Model::loadFromFile(const char* filename) {
	std::ifstream file(filename);
	std::string line;
    while (std::getline(file, line))
    {
        std::istringstream is_line(line);
        std::string key;
        if (std::getline(is_line, key, '='))
        {
            std::string value;
            if (key[0] == '#')
                continue;

            if (std::getline(is_line, value))
				processOptionPair(key, value);
        }
    }

	init();
}

void Model::processOptionPair(const std::string& key, const std::string& value) {
	if (key == "mapSize")
		modelOptions.mapSize = std::stoi(value);
	else if (key == "mapGranularity")
		modelOptions.mapGranularity = std::stoi(value);
	else if (key == "tempMean")
		modelOptions.tempMean = std::stod(value);
	else if (key == "tempStd")
		modelOptions.tempStd = std::stod(value);
	else if (key == "numberStartFires")
		modelOptions.numberStartFires = std::stoi(value);
	else if (key == "winddx")
		modelOptions.winddx = std::stod(value);
	else if (key == "winddy")
		modelOptions.winddy = std::stod(value);
	else if (key == "treeRatio")
		modelOptions.treeRatio = std::stod(value);
	else if (key == "numberIterations")
		modelOptions.numberIterations = std::stoi(value);
	else if (key == "initialTreeMap")
		modelOptions.initialTreeMap = value;
	else if (key == "outputToFile")
		modelOptions.outputToFile = (value == "1") ? true : false; 
}

void Model::init(void) {
	std::cout << "initializing..." << std::endl;
	auto startTime = std::chrono::high_resolution_clock::now();
	
	internalSize = ((modelOptions.mapGranularity - 1) / CHUNK_SIZE + 1) * CHUNK_SIZE;
	int size = getInternalArraySize();
	
	temperatureMap.reserve(size);
	initRandomTemperatureMap();
	
	auto endTime = std::chrono::high_resolution_clock::now();
	std::cout << " [1] initialized temperature, took " << std::chrono::duration<double>(endTime - startTime).count() << " seconds" << std::endl;

    treeMap.assign(size, 0);
	if (modelOptions.initialTreeMap != "")
		readTreeMapFromFile(modelOptions.initialTreeMap);
	else {
		initRandomTreeMap();
		writeTreeMapToFile("start_trees.txt");
	}
	
	endTime = std::chrono::high_resolution_clock::now();
	std::cout << " [2] initialized trees, took " << std::chrono::duration<double>(endTime - startTime).count() << " seconds" << std::endl;

	fireMap.assign(size, 0);
	initRandomFireMap();
	
	endTime = std::chrono::high_resolution_clock::now();
	std::cout << " [3] initialized fire, took " << std::chrono::duration<double>(endTime - startTime).count() << " seconds" << std::endl;

    currentWindX = modelOptions.winddx;
    currentWindY = modelOptions.winddy;
	currentWindDir = getWindDirection(currentWindX, currentWindY);
	
	endTime = std::chrono::high_resolution_clock::now();
  	auto elapsed_seconds = std::chrono::duration<double>(endTime - startTime).count();
	std::cout << "fully initialized, took " << elapsed_seconds << " seconds" << std::endl;
}

void Model::init(ModelOptions options){
	modelOptions = options;
	init();
}

void Model::execute(void) {
	std::cout << "start simuation" << std::endl;
	auto startTime = std::chrono::high_resolution_clock::now();

	while (currentIteration < modelOptions.numberIterations) {
		if (modelOptions.outputToFile) {
			writeTemperatureMapToFile(std::string("temp_" + std::to_string(currentIteration) + ".txt").c_str());
			writeFireMapToFile(std::string("fire_" + std::to_string(currentIteration) + ".txt").c_str());
			writeTreeMapToFile(std::string("tree_" + std::to_string(currentIteration) + ".txt").c_str());

			writeStatsToFile("stats.txt", std::string("Wind to " + getWindDirectionName(currentWindDir) + " (" + getRoundedString(currentWindX, 2) + ", " + getRoundedString(currentWindY, 2) + "), strength: " + std::to_string(getWindStrength(currentWindX, currentWindY))).c_str());
		}

		updateTemperatureMap();
		//updateFireMap();
		//updateWind();
		
        currentIteration++;
	}

	auto endTime = std::chrono::high_resolution_clock::now();
  	auto elapsed_seconds = std::chrono::duration<double>(endTime - startTime).count();
	std::cout << "end simulation" << std::endl;
	std::cout << "took " << elapsed_seconds << " seconds" << std::endl;
}

void Model::updateWind() {
	double angle = std::acos(currentWindX / std::sqrt(currentWindX * currentWindX + currentWindY * currentWindY)) * 180.0 / PI;
	if (currentWindY < 0)
		angle = 360 - angle;

	std::uniform_real_distribution<> dirDis(-22.5, 22.5);
	std::uniform_real_distribution<> strDis(0.65, 1.1);

	double strength = getWindStrength(currentWindX, currentWindY);
	strength *= strDis(randomGenerator);

	if (strength > WIND_UPPER_LIMIT)
		strength = WIND_UPPER_LIMIT;
	
	angle += dirDis(randomGenerator);
	angle = angle / 180.0 * PI;
	currentWindX = strength * std::cos(angle);
	currentWindY = strength * std::sin(angle);	
}

void Model::updateTemperatureMap(void) {
	int size = modelOptions.mapGranularity;
	int isize = internalSize;

	// inner points
	#pragma omp parallel for firstprivate(size, isize)
	for (int i = 1; i < size - 1; ++i) {
		double* row = temperatureMap.data() + i * isize;
		double* last_row = temperatureMap.data() + (i - 1) * isize;
		double* next_row = temperatureMap.data() + (i + 1) * isize;

        #pragma omp parallel for firstprivate(size, row, last_row, next_row)
		//#pragma omp simd aligned(row, next_row, last_row:64)
		for (int j = 1; j < size - 1; ++j)
			row[j] = (row[j] + row[j + 1] + row[j - 1] + next_row[j] + last_row[j]) * 0.2;
	}

	// edges
	int a = (size - 1) * internalSize;
    #pragma omp parallel for firstprivate(size, a)
	for (int i = 1; i < size - 1; ++i) {
		int b = i * internalSize + size - 1;
		temperatureMap[i] = (temperatureMap[i] + temperatureMap[i - 1] + temperatureMap[i + 1] + temperatureMap[internalSize + i]) * 0.25;
		temperatureMap[a + i] = (temperatureMap[a + i] + temperatureMap[a + i - 1] + temperatureMap[a + i + 1] + temperatureMap[a - internalSize + i]) * 0.25;
		temperatureMap[i * internalSize] = (temperatureMap[i * internalSize] + temperatureMap[i * internalSize + 1] + temperatureMap[(i - 1) * internalSize] + temperatureMap[(i + 1) * internalSize]) * 0.25;
		temperatureMap[b] = (temperatureMap[b] + temperatureMap[b - 1] + temperatureMap[b + internalSize] + temperatureMap[b - internalSize]) * 0.25;
	}

	// corners
	int b = (size - 1) * internalSize + size - 1;
	temperatureMap[0] = (temperatureMap[0] + temperatureMap[1] + temperatureMap[internalSize]) * 0.33333;
	temperatureMap[size - 1] = (temperatureMap[size - 1] + temperatureMap[size - 2] + temperatureMap[2 * size - 1]) * 0.33333;
	temperatureMap[a] = (temperatureMap[a] + temperatureMap[a + 1] + temperatureMap[(size - 2) * internalSize]) * 0.33333;
	temperatureMap[b] = (temperatureMap[b] + temperatureMap[b - 1] + temperatureMap[(size - 1) * internalSize - 1]) * 0.33333;
}

void Model::updateFireMap(void) {
	std::uniform_real_distribution<> dis(0, 1);

	currentWindDir = getWindDirection(currentWindX, currentWindY);

	int size = modelOptions.mapGranularity;
	int isize = internalSize;
	#pragma omp parallel for firstprivate(size, isize)
	for (int i = 0; i < size; ++i) {
        char* tree_row = treeMap.data() + i * isize;
        char* fire_row = fireMap.data() + i * isize;
        double* temp_row = temperatureMap.data() + i * isize;

        #pragma omp parallel for firstprivate(size, tree_row, fire_row, temp_row)
		for (int j = 0; j < size; ++j) {
			if (tree_row[j] == 0)
				continue;

			if (tree_row[j] == BURNED_DOWN_TREE_INDEX) {
				if (temp_row[j] > TREE_BURNED_TEMPERATURE)
					temp_row[j] -= 10;
				
				continue;
			}

			if (fire_row[j] >= BURNED_DOWN_VALUE) {
				tree_row[j] = BURNED_DOWN_TREE_INDEX;
				fire_row[j] = 0;
			}
			else if (fire_row[j] > 0) {
				fire_row[j] += treeInfos->getTree(tree_row[j]).burnSpeed;
				temp_row[j] += TEMPERATURE_FIRE_RISE;
			}
			else {//if (fireMap[index] == 0)        
                if (findFireProbability(j, i) >= dis(randomGenerator))
					fire_row[j] = 1;
			}
		}
	}
}

double Model::findFireProbability(const int x, const int y) {
	int size = modelOptions.mapGranularity;
	double tempProb = temperatureMap[x * internalSize + y] / TREE_CATCH_FIRE_TEMP;

	if (tempProb > 1)
		tempProb = 1;

	int radius = getWindStrength(currentWindX, currentWindY);
	int burningSum = 0, treeCount = 0;

	switch (currentWindDir) {
		case WD_EAST:
			for (int i = std::max(0, y - radius); i <= std::min(size - 1, y + radius); ++i) {
                char* row = fireMap.data() + i * internalSize;
				for (int j = std::max(0, x - radius); j < x; ++j) {
					if (row[j] > 0)
						burningSum++;
                    if (treeMap[i * internalSize + j] > 0 && treeMap[i * internalSize + j] < BURNED_DOWN_TREE_INDEX)
                        treeCount++;
                }
			}
			break;
		case WD_NORTH_EAST:
			for (int i = y; i <= std::min(size - 1, y + radius); ++i) {
                char* row = fireMap.data() + i * internalSize;
				for (int j = std::max(0, x - radius); j <= x; ++j) {
					if (row[j] > 0)
						burningSum++;
                    if (treeMap[i * internalSize + j] > 0 && treeMap[i * internalSize + j] < BURNED_DOWN_TREE_INDEX)
                        treeCount++;
                }
			}
			break;
		case WD_NORTH:
			for (int i = y + 1; i <= std::min(size - 1, y + radius); ++i) {
                char* row = fireMap.data() + i * internalSize;
				for (int j = std::max(0, x - radius); j <= std::min(size - 1, x + radius); ++j) {
					if (row[j] > 0)
						burningSum++;
                    if (treeMap[i * internalSize + j] > 0 && treeMap[i * internalSize + j] < BURNED_DOWN_TREE_INDEX)
                        treeCount++;
                }
			}
			break;
		case WD_NORTH_WEST:
			for (int i = y; i <= std::min(size - 1, y + radius); ++i) {
                char* row = fireMap.data() + i * internalSize;
				for (int j = x; j <= std::min(size - 1, x + radius); ++j) {
					if (row[j] > 0)
						burningSum++;
                    if (treeMap[i * internalSize + j] > 0 && treeMap[i * internalSize + j] < BURNED_DOWN_TREE_INDEX)
                        treeCount++;
                }
			}
			break;
		case WD_WEST:
			for (int i = std::max(0, y - radius); i <= std::min(size - 1, y + radius); ++i) {
                char* row = fireMap.data() + i * internalSize;
				for (int j = x + 1; j <= std::min(size - 1, x + radius); ++j) {
					if (row[j] > 0)
						burningSum++;
                    if (treeMap[i * internalSize + j] > 0 && treeMap[i * internalSize + j] < BURNED_DOWN_TREE_INDEX)
                        treeCount++;
                }
			}
			break;
		case WD_SOUTH_WEST:
			for (int i = std::max(0, y - radius); i <= y; ++i) {
                char* row = fireMap.data() + i * internalSize;
				for (int j = x; j <= std::min(size - 1, x + radius); ++j) {
					if (row[j] > 0)
						burningSum++;  
                    if (treeMap[i * internalSize + j] > 0 && treeMap[i * internalSize + j] < BURNED_DOWN_TREE_INDEX)
                        treeCount++;
                }
			}
			break;
		case WD_SOUTH:
			for (int i = std::max(0, y - radius); i < y; ++i) {
                char* row = fireMap.data() + i * internalSize;
				for (int j = std::max(0, x - radius); j <= std::min(size - 1, x + radius); ++j) {
					if (row[j] > 0)
						burningSum++;   
                    if (treeMap[i * internalSize + j] > 0 && treeMap[i * internalSize + j] < BURNED_DOWN_TREE_INDEX)
                        treeCount++;
                }
			}
			break;
		case WD_SOUTH_EAST:
			for (int i = std::max(0, y - radius); i <= y; ++i) {
                char* row = fireMap.data() + i * internalSize;
				for (int j = std::max(0, x - radius); j <= x; ++j) {
					if (row[j] > 0)
						burningSum++;   
                    if (treeMap[i * internalSize + j] > 0 && treeMap[i * internalSize + j] < BURNED_DOWN_TREE_INDEX)
                        treeCount++;
                }
			}
			break;
	}

    int all_burn_neighbors = 0;
    for (int i = std::max(0, y - radius); i <= std::min(size - 1, y + radius); ++i) {
        char* row = fireMap.data() + i * internalSize;
		for (int j = std::max(0, x - radius); j <= std::min(size - 1, x + radius); ++j)
            if (row[j] > 0)
		        all_burn_neighbors++;
    }
 
	double neighborProb = burningSum / (double)treeCount;
    if (treeCount == 0)
        neighborProb = 0;

    if (all_burn_neighbors > 0)
        return 0.3 * tempProb + 0.7 * neighborProb;
    else
        return 0;
}

void Model::initRandomTemperatureMap(void) {
	std::normal_distribution<double> dis(modelOptions.tempMean, modelOptions.tempStd);

	int size = getRealSize();
	#pragma omp parallel for firstprivate(size)
	for (int i = 0; i < size; ++i)
		#pragma omp parallel for firstprivate(i, size)
		for (int j = 0; j < size; ++j)
			temperatureMap[i * internalSize + j] = dis(randomGenerator);
}

void Model::initRandomFireMap(void) {
	std::uniform_int_distribution<> xDis(0, getRealSize() - 1);
	std::uniform_int_distribution<> yDis(0, getRealSize() - 1);
	
	int num = modelOptions.numberStartFires;    
	for (int i = 0; i < num; ++i) {
		int x = xDis(randomGenerator), y = yDis(randomGenerator);
		if (treeMap[x * internalSize + y] != 0 && fireMap[x * internalSize + y] == 0) {
			fireMap[x * internalSize + y] = 1;
			writeStatsToFile("stats.txt", std::string("Start fire: " + std::to_string(x) + ", " + std::to_string(y)).c_str());
		}
		else
			--i;
	}
}

void Model::initRandomTreeMap(void) {
	std::uniform_real_distribution<> dis(0, 1);
	double barrier = modelOptions.treeRatio;

	std::uniform_int_distribution<> treeDis(1, treeInfos->getTreeNumber() - 2);

	int size = modelOptions.mapGranularity;
	#pragma omp parallel for firstprivate(size)
	for (int i = 0; i < size; ++i) {
		#pragma omp parallel for firstprivate(size, i, barrier)
		for (int j = 0; j < size; ++j) {
			const tree t = treeInfos->getTree(treeDis(randomGenerator));
			
			#pragma omp critical
			if (!isThereATree(i, j, t.radius) && dis(randomGenerator) <= barrier)
				placeTree(i, j, &t);
		}
	}
}

bool Model::isThereATree(const int x, const int y, const int radius) {
	int size = modelOptions.mapGranularity;
	if (x - radius < 0 || x + radius >= size || y - radius < 0 || y + radius >= size)
		return true;

	for (int i = y - radius; i <= y + radius; ++i) {
		for (int j = x - radius; j <= x + radius; ++j) {
			if (treeMap[i * internalSize + j] != 0)
				return true;
		}
	}

	return false;
}

void Model::placeTree(const int x, const int y, const tree* t) {
	int radius = t->radius;
	int size = modelOptions.mapGranularity;

	if (x - radius < 0 || x + radius >= size || y - radius < 0 || y + radius >= size)
		return;

	#pragma omp parallel for firstprivate(radius)
	for (int i = y - radius; i <= y + radius; ++i) {
		#pragma omp parallel for firstprivate(radius, i, t)
		for (int j = x - radius; j <= x + radius; ++j) {
			treeMap[i * internalSize + j] = t->id;
		}
	}
}

void Model::writeTemperatureMapToFile(const char* filename) {
	std::ofstream file;
    file.open(filename);

	if (!file.is_open()) {
		std::cout << "could not open stats file!" << std::endl;
		return;
	}

	int size = modelOptions.mapGranularity;
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j)
            file << temperatureMap[i * internalSize + j] << " ";

        if (i != size - 1)
            file << "\n";
    }

    file.close();
}

void Model::writeFireMapToFile(const char* filename) {
	std::ofstream file;
    file.open(filename);

	if (!file.is_open()) {
		std::cout << "could not open stats file!" << std::endl;
		return;
	}

	int size = modelOptions.mapGranularity;
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j)
            file << (int)fireMap[i * internalSize + j] << " ";

        if (i != size - 1)
            file << "\n";
    }

    file.close();
}

void Model::writeTreeMapToFile(const char* filename) {
	std::ofstream file;
    file.open(filename);

	if (!file.is_open()) {
		std::cout << "could not open stats file!" << std::endl;
		return;
	}

	int size = modelOptions.mapGranularity;
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j)
            file << (int)treeMap[i * internalSize + j] << " ";

        if (i != size - 1)
            file << "\n";
    }

    file.close();
}

void Model::readTreeMapFromFile(const std::string& filename) {
	std::ifstream file;
    file.open(filename);

	if (!file.is_open()) {
		std::cout << "could not open file with start tree information!" << std::endl;
		initRandomTreeMap();
		return;
	}
	
	int size = modelOptions.mapGranularity;
	
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			int v;
			file >> v;
			treeMap[i * internalSize + j] = v;
		}
	}

    file.close();
}

void Model::writeStatsToFile(const char* filename, const char* msg) {
	std::ofstream file;
    file.open(filename, std::ios_base::app);

	if (!file.is_open()) {
		std::cout << "could not open stats file!" << std::endl;
		return;
	}

	file << msg << "\n";
	file.close();
}

std::string getRoundedString(double number, int precision) {
	std::stringstream stream;
	stream << std::fixed << std::setprecision(precision) << number;
	return stream.str();
}


