
#ifndef FORESTFIRE_MODEL_H
#define FORESTFIRE_MODEL_H

#include <vector>
#include <string>
#include <random>

#include "tree.h"
#include "wind.h"
#include <boost/align/aligned_allocator.hpp>

template <typename T>
using aligned_vector = std::vector<T, boost::alignment::aligned_allocator<T, 64> >;

#define CHUNK_SIZE (64 / sizeof(double))
#define WIND_UPPER_LIMIT 8

#define WIND_UPPER_LIMIT 8


struct ModelOptions {
	int mapSize;
	int mapGranularity;
	double tempMean, tempStd;
	int numberStartFires;
	int numberIterations;
	double winddx, winddy;
	double treeRatio;
	std::string initialTreeMap;
	bool outputToFile;
};

std::string getRoundedString(double number, int precision);

class Model {
	public:
		Model(void);
		~Model(void);

		void loadFromFile(const char* filename);

		void init(void);
		void init(ModelOptions options);
		void execute(void);

		void setTreeInfos(TreeInfos* infos) { treeInfos = infos; }
		ModelOptions getModelOptions(void) const { return modelOptions; }

		int getNumberMapPoints(void) const { return modelOptions.mapGranularity * modelOptions.mapGranularity; }
		int getRealSize(void) const { return modelOptions.mapGranularity; }
		int getInternalWidth(void) const { return internalSize; }
		int getInternalArraySize(void) const { return internalSize * modelOptions.mapGranularity; }

	private:
		std::mt19937 randomGenerator;
		ModelOptions modelOptions;
		TreeInfos* treeInfos;
		int internalSize;

		int currentIteration;
		double currentWindX, currentWindY;
		WindDirection currentWindDir;

		/*std::vector<double> temperatureMap;
		std::vector<char> fireMap;
		std::vector<char> treeMap;*/
		aligned_vector<double> temperatureMap;
		aligned_vector<char> fireMap;
		aligned_vector<char> treeMap;

		void processOptionPair(const std::string& key, const std::string& value);

		void initRandomTemperatureMap(void);
		void initRandomFireMap(void);
		void initRandomTreeMap(void);
		bool isThereATree(const int x, const int y, const int radius);
		void placeTree(const int x, const int y, const tree* t);

		void updateTemperatureMap(void);
		void updateFireMap(void);
		void updateWind(void);

		void writeTemperatureMapToFile(const char* filename);
		void writeFireMapToFile(const char* filename);
		void writeTreeMapToFile(const char* filename);
		void writeStatsToFile(const char* filename, const char* msg);

		void readTreeMapFromFile(const std::string& filename);

		double findFireProbability(const int x, const int y);
};

#endif

