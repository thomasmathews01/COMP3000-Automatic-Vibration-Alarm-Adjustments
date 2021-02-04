#include <gtest/gtest.h>
#include <Statistics/SOM.h>
#include <chrono>

using namespace std::chrono_literals;

TEST (SOMTests, canCompileTests) {
    ASSERT_TRUE(true);
}

TEST (SOMTests, NeighbourHoodFuntionBasicCase) {
    const SOM<100, 100, 1000>::point_t test_point(50, 50);

    SOM<100, 100, 1000> som;

    const auto found_indices = som.points_within_distance(test_point, 1) | ranges::to<std::vector>();
    ASSERT_EQ(5, found_indices.size()); // The elements above, below and beside, as well as the element itself.

    EXPECT_TRUE(ranges::contains(found_indices, SOM<100, 100, 1000>::point_t(50, 50)));
    EXPECT_TRUE(ranges::contains(found_indices, SOM<100, 100, 1000>::point_t(51, 50)));
    EXPECT_TRUE(ranges::contains(found_indices, SOM<100, 100, 1000>::point_t(49, 50)));
    EXPECT_TRUE(ranges::contains(found_indices, SOM<100, 100, 1000>::point_t(50, 49)));
    EXPECT_TRUE(ranges::contains(found_indices, SOM<100, 100, 1000>::point_t(50, 51)));
}

TEST (SOMTests, NeighbourHoodFuntionZeroDistanceReturnsBMUItself) {
    const SOM<100, 100, 1000>::point_t test_point(50, 50);

    SOM<100, 100, 1000> som;

    const auto found_indices = som.points_within_distance(test_point, 0) | ranges::to<std::vector>();
    ASSERT_EQ(1, found_indices.size()); // The elements above, below and beside, as well as the element itself.

    EXPECT_TRUE(ranges::contains(found_indices, SOM<100, 100, 1000>::point_t(50, 50)));
}

TEST (SOMTests, DoesntIncludePointsOutsideMapBoundary) {
    const SOM<100, 100, 1000>::point_t test_point(0, 0);
    SOM<100, 100, 1000> som;

	const auto found_indices = som.points_within_distance(test_point, 1) | ranges::to<std::vector>();
    ASSERT_EQ(3, found_indices.size()); // The elements above and beside, as well as the element itself.

    EXPECT_TRUE(ranges::contains(found_indices, SOM<100, 100, 1000>::point_t(0, 0)));
    EXPECT_TRUE(ranges::contains(found_indices, SOM<100, 100, 1000>::point_t(0, 1)));
    EXPECT_TRUE(ranges::contains(found_indices, SOM<100, 100, 1000>::point_t(1, 0)));
}
TEST (SOMTests, FindsNeighbourhoodInReasonableTime) {
    const SOM<100, 100, 1000>::point_t test_point(50, 50);
    SOM<100, 100, 1000> som;

    const auto start = std::chrono::high_resolution_clock::now();
	const auto found_indices = som.points_within_distance(test_point, 1) | ranges::to<std::vector>();
    const auto end = std::chrono::high_resolution_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us. " << std::endl;
    ASSERT_LE(end - start, 10ms);
}