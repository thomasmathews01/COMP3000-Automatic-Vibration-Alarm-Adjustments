#include "CudaAdder.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

__global__ void addKernel(int* c, const int* a, const int* b, int size) {
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < size) {
		c[i] = a[i] + b[i];
	}
}

/* CUDA - Compute Ability 5.3
 * This gives us ... https://en.wikipedia.org/wiki/CUDA#Version_features_and_specifications
 *
 */

std::vector<float> CudaAdder::add_matrices(const std::vector<float>& first, const std::vector<float>& second) {
	auto output = std::vector<float>(first.size());
	int* dev_a = nullptr;
	int* dev_b = nullptr;
	int* dev_c = nullptr;

	cudaMalloc((void**)&dev_a, first.size() * sizeof(float));
	cudaMalloc((void**)&dev_b, first.size() * sizeof(float));
	cudaMalloc((void**)&dev_c, first.size() * sizeof(float));

	cudaMemcpy(dev_a, first.data(), size * sizeof(float), cudaMemcpyHostToDevice);
	cudaMemcpy(dev_b, second.data(), size * sizeof(float), cudaMemcpyHostToDevice);

	addKernel<<<2, (first.size() + 1) / 2>>>(dev_c, dev_a, dev_b, first.size());

	cudaDeviceSynchronize();

	cudaMemcpy(output.data(), dev_c, first.size() * sizeof(float), cudaMemcpyDeviceToHost);

	cudaFree(dev_a);
	cudaFree(dev_b);
	cudaFree(dev_c);
	cudaDeviceReset();
}
