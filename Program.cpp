#include <iostream>
#include <thread>
#include <chrono>

using std::chrono::nanoseconds;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

static std::vector<std::vector<std::vector<int>>> matrices;

static std::vector<std::vector<int>> createMatrix(const std::size_t n)
{
	std::vector<std::vector<int>> matrix(n, std::vector<int>(n));
	for (std::size_t i = 0; i < n; ++i)
		for (std::size_t j = 0; j < n; ++j)
			matrix[i][j] = i * n + j;
	
	return matrix;
}

static void compute(const std::size_t i, const std::size_t k1, const std::size_t k2)
{
	std::vector<std::vector<int>> matrix = matrices[i];
	std::size_t n = matrix.size();
	std::vector<std::vector<int>> result(n, std::vector<int>(n));

	auto payload_begin = high_resolution_clock::now();

	for (std::size_t i = 0; i < n; ++i)
		for (std::size_t j = 0; j < n; ++j)
			result[i][j] = k1 * matrix[i][j] + k2 * matrix[i][j];

	auto payload_end = high_resolution_clock::now();
	auto elapsed = duration_cast<nanoseconds>(payload_end - payload_begin);
	std::cout << std::format("Payload Time for {}x{} matrix: {} seconds.\n", n, n, elapsed.count() * 1e-9);
}

int main() 
{
	std::vector<std::size_t> numOfThreads = { 1, 2, 4, 8, 16, 32, 64, 128 };
	std::vector<std::size_t> ns = { 50, 100, 500, 1000, 10000 };
	std::vector<std::thread> threads;

	for (const std::size_t n : ns)
	{
		std::vector<std::vector<int>> matrix = createMatrix(n);
		matrices.push_back(matrix);
	}

	for (const auto& numOfThread : numOfThreads)
	{
		auto begin = high_resolution_clock::now();
		std::size_t i = 0;
		for (std::size_t i = 0; i < matrices.size(); i++)
		{
			std::size_t k1 = 2;
			std::size_t k2 = 3;
			for (std::size_t j = 0; j < numOfThread || j < ns.size(); j++)
				threads.push_back(std::thread(compute, i, k1, k2));
		}

		auto end = high_resolution_clock::now();
		auto elapsed = duration_cast<nanoseconds>(end - begin);

		std::cout << std::format("Total Time for {} threads: {} seconds.\n", numOfThread, elapsed.count() * 1e-9);
	}


	for (auto& thread : threads)
		thread.join();

	return 0;
}