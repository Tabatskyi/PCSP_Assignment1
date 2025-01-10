#include <iostream>
#include <thread>
#include <chrono>

using std::chrono::nanoseconds;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

static std::vector<std::vector<std::vector<std::size_t>>> matrices;

static std::vector<std::vector<std::size_t>> createMatrix(const std::size_t n)
{
    std::vector<std::vector<std::size_t>> matrix(n, std::vector<std::size_t>(n));
    for (std::size_t i = 0; i < n; ++i)
        for (std::size_t j = 0; j < n; ++j)
            matrix[i][j] = i * n + j;

    return matrix;
}

static void compute(const std::size_t i, const std::size_t k1, const std::size_t k2)
{
    std::vector<std::vector<std::size_t>> matrix = matrices[i];
    std::size_t n = matrix.size();
    std::vector<std::vector<std::size_t>> result(n, std::vector<std::size_t>(n));

    auto payload_begin = high_resolution_clock::now();

    for (std::size_t i = 0; i < n; ++i)
        for (std::size_t j = 0; j < n; ++j)
            result[i][j] = k1 * matrix[i][j] + k2 * matrix[i][j];

    auto payload_end = high_resolution_clock::now();
    auto elapsed = duration_cast<nanoseconds>(payload_end - payload_begin);
    std::cout << std::format("Payload Time for {}x{} matrix: {} seconds.\n", n, n, elapsed.count() * 1e-9);
}

static void threadFunction(std::size_t threadIndex, std::size_t numOfThread, std::size_t k1, std::size_t k2)
{
    for (std::size_t j = threadIndex; j < matrices.size(); j += numOfThread)
    {
        compute(j, k1, k2);
		std::cout << std::format("Thread {} finished computing matrix {}.\n", threadIndex, j);
    }
}

int main()
{
    std::vector<std::size_t> numOfThreads = { 1, 2, 4, 8, 16, 32, 64, 128 };
    std::vector<std::size_t> ns = { 10000, 1000, 500, 100, 50 };

    for (std::size_t n : ns)
    {
        std::vector<std::vector<std::size_t>> matrix = createMatrix(n);
        matrices.push_back(matrix);
    }

    for (const auto& numOfThread : numOfThreads)
    {
        std::vector<std::thread> threads;
        std::size_t k1 = 212773123;
        std::size_t k2 = 313241244;

        for (std::size_t i = 0; i < numOfThread; ++i)
            threads.push_back(std::thread(threadFunction, i, numOfThread, k1, k2));

        auto begin = high_resolution_clock::now();

        for (auto& thread : threads)
            thread.join();

        auto end = high_resolution_clock::now();
        auto elapsed = duration_cast<nanoseconds>(end - begin);

        std::cout << std::format("Total Time for {} threads: {} seconds.\n", threads.size(), elapsed.count() * 1e-9);
    }

    return 0;
}