#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>

using std::chrono::nanoseconds;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

static std::vector<std::vector<std::vector<double>>> matrices;

static std::vector<std::vector<double>> createMatrix(const std::size_t n, const std::size_t seed = 123456789)
{
    std::vector<std::vector<double>> matrix(n, std::vector<double>(n));
    for (std::size_t i = 0; i < n; ++i)
        for (std::size_t j = 0; j < n; ++j)
            matrix[i][j] = ((static_cast<double>(seed) + i) * j) / n;

    return matrix;
}

static void compute(const std::size_t i, const std::size_t k1, const std::size_t k2, std::ofstream& matrixTimeFile, const std::size_t numOfThreads)
{
    const std::vector<std::vector<double>>& matrix = matrices[i];
    std::size_t n = matrix.size();
    std::vector<std::vector<double>> result(n, std::vector<double>(n));

    auto payload_begin = high_resolution_clock::now();

    for (std::size_t row = 0; row < n; ++row)
        for (std::size_t col = 0; col < n; ++col)
            result[row][col] = k1 * matrix[row][col] + k2 * matrix[row][col];

    auto payload_end = high_resolution_clock::now();
    auto elapsed = duration_cast<nanoseconds>(payload_end - payload_begin);
	auto elapsed_count = elapsed.count() * 1e-9;

    matrixTimeFile << n << "," << elapsed_count << "\n";
    std::cout << std::format("Payload Time for {}x{} matrix: {} seconds.\n", n, n, elapsed_count);
}

static void threadFunction(std::size_t threadIndex, std::size_t numOfThreads, std::size_t k1, std::size_t k2, std::ofstream& matrixTimeFile)
{
    for (std::size_t j = threadIndex; j < matrices.size(); j += numOfThreads)
        compute(j, k1, k2, matrixTimeFile, numOfThreads);
}

int main()
{
    std::vector<std::size_t> numOfThreads = { 1, 2, 4, 8, 16, 32, 64, 128 };
    std::vector<std::size_t> ns = { 10000, 1000, 500, 100, 50 };
    std::ofstream threadsTimeFile("threads_time.csv");
    threadsTimeFile << "threads,time\n";

    for (std::size_t n : ns)
    {
        std::vector<std::vector<double>> matrix = createMatrix(n);
        matrices.push_back(matrix);
    }

    for (const auto& numOfThread : numOfThreads)
    {
        std::vector<std::thread> threads;
        std::size_t k1 = 2127731231738;
        std::size_t k2 = 8332412445367;

        std::ofstream matrixTimeFile("matrix_time.csv");
        matrixTimeFile << "matrix_size(n),time\n";

        for (std::size_t i = 0; i < numOfThread; ++i)
            threads.push_back(std::thread(threadFunction, i, numOfThread, k1, k2, std::ref(matrixTimeFile)));

        auto begin = high_resolution_clock::now();
        for (auto& thread : threads)
            thread.join();

        auto end = high_resolution_clock::now();
        auto elapsed = duration_cast<nanoseconds>(end - begin);
		auto elapsed_count = elapsed.count() * 1e-9;

        threadsTimeFile << std::format("{},{}\n", threads.size(), elapsed_count);
        std::cout << std::format("Total Time for {} threads: {} seconds.\n", threads.size(), elapsed_count);

        matrixTimeFile.close();
    }
	threadsTimeFile.close();

    return 0;
}