#include <iostream>
#include <thread>
#include <chrono>

using std::chrono::nanoseconds;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

static std::vector<std::vector<int>> createMatrix(const std::size_t n)
{
	std::vector<std::vector<int>> matrix(n, std::vector<int>(n));
	for (std::size_t i = 0; i < n; ++i)
		for (std::size_t j = 0; j < n; ++j)
			matrix[i][j] = i * n + j;
	
	return matrix;
}

static void compute(const std::size_t n, const std::size_t k1, std::size_t k2)
{
	std::vector<std::vector<int>> matrix1 = createMatrix(n);
	std::vector<std::vector<int>> matrix2 = createMatrix(n);
	std::vector<std::vector<int>> result(n, std::vector<int>(n));

	auto payload_begin = high_resolution_clock::now();

	for (std::size_t i = 0; i < n; ++i)
		for (std::size_t j = 0; j < n; ++j)
			result[i][j] = k1 * matrix1[i][j] + k2 * matrix2[i][j];

	auto payload_end = high_resolution_clock::now();
	auto elapsed = duration_cast<nanoseconds>(payload_end - payload_begin);
	std::cout << std::format("Payload Time for {}x{} matrix: {} seconds.", n, n, elapsed.count() * 1e-9) << std::endl;
}

int main() 
{
	std::vector<std::size_t> numOfThreads = { 2, 4, 8, 16, 32, 64, 128 };
	std::vector<std::size_t> ns = { 50, 100, 500, 1000, 10000 };
	std::vector<std::thread> threads;

	for (const auto& numOfThread : numOfThreads)
	{
		auto begin = high_resolution_clock::now();

		for (const auto& n : ns)
		{
			std::size_t k1 = 2;
			std::size_t k2 = 3;
			for (std::size_t i = 0; i < numOfThread; ++i)
				threads.push_back(std::thread(compute, n, k1, k2));
		}

		auto end = high_resolution_clock::now();
		auto elapsed = duration_cast<nanoseconds>(end - begin);

		std::cout << std::format("Total Time for {} threads: {} seconds.", numOfThread, elapsed.count() * 1e-9) << std::endl;
	}


	for (auto& thread : threads)
		thread.join();

	return 0;
}