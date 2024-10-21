
#include <cmath>
#include "wind.h"

double getWindAngle(const double wx, const double wy) {
	double angle = std::acos(wx / std::sqrt(wx * wx + wy * wy)) * 180.0 / PI;
	if (wy < 0)
		angle = 360 - angle;

	return angle;
}

WindDirection getWindDirection(const double wx, const double wy) {
	double angle = getWindAngle(wx, wy);

	if (angle <= 22.5 || angle > 337.5)
		return WD_EAST;
	else if (angle > 22.5 && angle <= 67.5)
		return WD_NORTH_EAST;
	else if (angle > 67.5 && angle <= 112.5)
		return WD_NORTH;
	else if (angle > 112.5 && angle <= 157.5)
		return WD_NORTH_WEST;
	else if (angle > 157.5 && angle <= 202.5)
		return WD_WEST;
	else if (angle > 202.5 && angle <= 247.5)
		return WD_SOUTH_WEST;
	else if (angle > 247.5 && angle <= 292.5)
		return WD_SOUTH;
	else //if (angle > 292.5 && angle <= 337.5)
		return WD_SOUTH_EAST;
}

int getWindStrength(const double wx, const double wy) {
    return std::ceil(std::sqrt(wx * wx + wy * wy));
}

std::string getWindDirectionName(const WindDirection dir) {
	switch (dir) {
		case WD_NORTH:
			return "north";
		case WD_NORTH_EAST:
			return "north east";
		case WD_EAST:
			return "East";
		case WD_SOUTH_EAST:
			return "south east";
		case WD_SOUTH:
			return "south";
		case WD_SOUTH_WEST:
			return "south west";
		case WD_WEST:
			return "west";
		case WD_NORTH_WEST:
			return "north west";
		default:
			return "";
	}
}

