#pragma once
#include <array>
#include <memory>
#include <random>
#include <range/v3/all.hpp>

using namespace ranges;

template<int NodeDims, int FeatureDims>
class SOM {
public:
	void initialise();
private:
	std::unique_ptr<std::array<std::array<float, FeatureDims>, NodeDims * NodeDims>> nodes;
};

template<int NodeDims, int FeatureDims>
void SOM<NodeDims, FeatureDims>::initialise() {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::normal_distribution<float> dist(-1.f * std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

	generate(*nodes | views::join, [&dist, &rng]() mutable { return dist(rng); });
}


