
#ifndef FORESTFIRE_WIND_H
#define FORESTFIRE_WIND_H

#include <string>

#define PI 3.1415926535

/**
 * The direction the wind blows into.
 */
enum WindDirection {
	WD_NORTH = 0,
	WD_NORTH_EAST = 1,
	WD_EAST = 2,
	WD_SOUTH_EAST = 3,
	WD_SOUTH = 4,
	WD_SOUTH_WEST = 5,
	WD_WEST = 6,
	WD_NORTH_WEST = 7
};

double getWindAngle(const double wx, const double wy);
WindDirection getWindDirection(const double wx, const double wy);
int getWindStrength(const double wx, const double wy);
std::string getWindDirectionName(const WindDirection dir);


#endif

