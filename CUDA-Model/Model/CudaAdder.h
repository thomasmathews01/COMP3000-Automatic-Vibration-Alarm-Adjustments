#pragma once
#include <vector>

class CudaAdder {
public:
	/// Add Matrices using GPU resources
	/// \param first first matrix to add
	/// \param second second matrix to add
	/// \return float matrix, where result[i] = first[i] + second[i]
	static std::vector<float> add_matrices(const std::vector<float>& first, const std::vector<float>& second);
};


