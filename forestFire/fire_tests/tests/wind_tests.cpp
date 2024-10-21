
#include "model.h"
#include "gtest/gtest.h"


TEST(WindTests, ClearWindDirectionTest) {
	// test all clear directions
	EXPECT_EQ(WD_NORTH, getWindDirection(0, 1));
	EXPECT_EQ(WD_NORTH_EAST, getWindDirection(1, 1));
	EXPECT_EQ(WD_EAST, getWindDirection(1, 0));
	EXPECT_EQ(WD_SOUTH_EAST, getWindDirection(1, -1));
	EXPECT_EQ(WD_SOUTH, getWindDirection(0, -1));
	EXPECT_EQ(WD_SOUTH_WEST, getWindDirection(-1, -1));
	EXPECT_EQ(WD_WEST, getWindDirection(-1, 0));
	EXPECT_EQ(WD_NORTH_WEST, getWindDirection(-1, 1));

	// test directions with different sizes
	EXPECT_EQ(WD_NORTH, getWindDirection(0, 5));
	EXPECT_EQ(WD_SOUTH, getWindDirection(0, -3));
}

TEST(WindTests, RoundedWindDirectionTest) {
	// test directions which are between two barriers
	EXPECT_EQ(WD_EAST, getWindDirection(10, 1));
	EXPECT_EQ(WD_WEST, getWindDirection(-10, 1));
	EXPECT_EQ(WD_NORTH, getWindDirection(1, 10));
	EXPECT_EQ(WD_SOUTH, getWindDirection(1, -10));
}

TEST(WindTests, WindStrengthTest) {
	// test the radius of the wind 
	EXPECT_EQ(5, getWindStrength(3, 4));
	EXPECT_EQ(2, getWindStrength(1, 1));

	EXPECT_EQ(5, getWindStrength(-3, 4));
	EXPECT_EQ(5, getWindStrength(3, -4));

	EXPECT_EQ(0, getWindStrength(0, 0));
	EXPECT_EQ(1, getWindStrength(0.1, 0));
	EXPECT_EQ(1, getWindStrength(0, 0.1));	
}

