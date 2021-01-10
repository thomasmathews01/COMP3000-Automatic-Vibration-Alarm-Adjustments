//#include "SOM.h"
//#include "../../Utils/STLExtensions.h"
//#include <algorithm>
//#include <numeric>
//
//constexpr std::array<float, ARRAY_DIMENSIONS * ARRAY_DIMENSIONS> get_base_distances() {
//	- std::array<float, ARRAY_DIMENSIONS * ARRAY_DIMENSIONS> array{};
//	- std::fill(array.begin(), array.end(), -1.f * std::numeric_limits<float>::max());
//	- return array;
//	- }
//- map_t::iterator SOM::get_best_matching_unit(const point_t& input_point) {
//	- constexpr auto calculate_euclidean_distance_from = [] (const point_t& p1)
//														 - {
//															 - return [p1] (const point_t& p2)
//		- {
//			- const auto sum_of_squares = std::transform_reduce(p1.cbegin(), p1.cend(), p2.cbegin(), 0.f, STLE::sqr_diff, STLE::add);
//			- return sqrtf(sum_of_squares); // This step is actually unnecessary, since the absolute ordering of the elements will NOT change as a result of applying this function. TODO: Remove as part of benchmarking process.
//			- };
//		- };
//	- const auto euclidean_distances = get_base_distances();
//	- std::transform(map->cbegin(), map->cend(), euclidean_distances.begin(), calculate_euclidean_distance_from(input_point));
//	- return map->begin() + distance(euclidean_distances.cbegin(), std::min_element(euclidean_distances.cbegin(), euclidean_distances.cend()));
//	- }
//- std::vector<SOM::neighbourhood_member> SOM::get_neighbourhood_around(const map_t::iterator& node, const float squared_radius) {
//	- std::vector<SOM::neighbourhood_member> neighbourhood = {{node, 1}};
//	- const auto idx = std::distance(map->begin(), node);
//	- const auto centroid_x = idx % ARRAY_DIMENSIONS; // Grid is going to have to become a separate class just for the readability.
//	- const auto centroid_y = idx / ARRAY_DIMENSIONS;
//	- for (int x = 0; x < ARRAY_DIMENSIONS; ++x) {
//		- for (int y = 0; y < ARRAY_DIMENSIONS; ++y) {
//			- const auto distance = pow((centroid_x - x), 2) + pow((centroid_y - y), 2);
//			- if (distance <= squared_radius)
//				- neighbourhood.emplace_back(map->begin() + (x, y + (x * ARRAY_DIMENSIONS)));
//			- }
//		- }
//	- // TODO: Lookup spacial indexes like R-Trees, which offer a much faster way to do this, that may be substitutable with compile time constants.
//		- // For the initial implementation we just need to use bounding boxes, anything more than radius away in any dimensions is obviously not a candidate,
//			- // And there is also a limit where it has to be in the range.
//				- // We just need to then test all the values that lie between the two, which should be a substantially reduced computation.
//					- return neighbourhood;
//	- }
//- point_t SOM::update_node_with_new_weight(const map_t::iterator& old_weights, const point_t& matched_weight, float learning_rate, float neighbourhood_reduction) {
//	- point_t updated_node;
//	- std::transform(old_weights->cbegin(), old_weights->cend(), matched_weight.cbegin(), [learning_rate, neighbourhood_reduction](const float& original_value, const float& new_value)
//																						  - {
//																							  - const auto change = (new_value - original_value) * learning_rate * neighbourhood_reduction;
//		- return original_value + change;
//		- });
//	- return updated_node;
//	- }
//- void SOM::train(const std::vector<point_t>& data_set) {
//	- const auto learning_rate = 0.1;
//	- const auto iterations = 1000;
//	- for (int i = 0; i < iterations; ++i) {
//		- for (const auto& pt : data_set) {
//			- const auto best_matching_unit = get_best_matching_unit(pt);
//			- const auto neighbourhood = get_neighbourhood_around(best_matching_unit, 0);
//			- std::for_each(neighbourhood.cbegin(), neighbourhood.cend(), [&] (const auto& member)
//																		  - {
//																			  - update_node_with_new_weight(member.node, pt, member.discount_rate, learning_rate);
//				- });
//			- }
//		- }
//	- }
