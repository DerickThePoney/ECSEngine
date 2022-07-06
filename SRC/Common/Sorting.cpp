#include "stdafx.h"

#include "Sorting.h"

#include <random>

namespace ECSEngine
{

std::size_t ChoosePivot(std::size_t begin, std::size_t end)
{
    return end;
}

void PrintArray(const std::vector<float>& a)
{
    forrange(i, 0, a.size()) std::cout << a[i] << " ";
    std::cout << "\n";
}

template<typename Container, typename T, typename Functor = std::less<>>
float SortArray(Container& parArray, Functor f = Functor{})
{
    auto start = std::chrono::high_resolution_clock::now();
    InPlaceSorting<Container, T>(parArray, 0, parArray.size() - 1, f);
    return (float)(std::chrono::high_resolution_clock::now() - start).count() / 1000000.f;
}

template<typename Container, typename T, typename Functor = std::less<>>
float SortArrayNoRecursion(Container& parArray, Functor f = Functor{})
{
    auto start = std::chrono::high_resolution_clock::now();
    InPlaceSortingNoRecursion<Container, T>(parArray, 0, parArray.size() - 1, f);
    return (float)(std::chrono::high_resolution_clock::now() - start).count() / 1000000.f;
}

template<typename Container, typename T, typename Functor = std::less<>>
float STDSortArray(Container& parArray, Functor f = Functor{})
{
    auto start = std::chrono::high_resolution_clock::now();
    std::sort(parArray.begin(), parArray.end(), f);
    return (float)(std::chrono::high_resolution_clock::now() - start).count() / 1000000.f;
}

struct LexSort
{
    constexpr auto operator()(const glm::vec2& a, const glm::vec2& b) const { return a.x < b.x || ((a.x == b.x) && (a.y < b.y)); }
};

void TestSorting()
{
    std::random_device rd;
    std::mt19937_64 gen64(rd());

    std::uniform_real_distribution<float> distrib(-10.f, 10.f);

    static constexpr std::size_t size[6] = { 10, 100, 1000, 10000, 100000, 1000000 };
    std::cout << " **************************************************************************************** \n";
    std::cout << "                                     FLOATS \n";
    std::cout << " **************************************************************************************** \n";
    forrange(i, 0, 6)
    {

        std::vector<float> a;
        a.resize(size[i]);
        forrange(j, 0, size[i]) { a[j] = distrib(gen64); }
        std::vector<float> b;
        float accu = 0.f;
        float accunorecurse = 0.f;
        float accustd = 0.f;

        constexpr size_t retries = 20;
        forrange(j, 0, retries)
        {
            b = a;
            accu += SortArray<std::vector<float>, float>(b);

            if (j == 0)
            {
#ifdef ENABLE_SECURITY_CHECKS
                for (size_t i = 0, j = 1; j < b.size(); i = j++)
                    if (b[i] > b[j])
                        std::cout << fmt::format("error on {} ({}) and {} ({})", i, b[i], j, b[j]);
#endif // ENABLE_SECURITY_CHECKS }
            }

            b = a;
            accunorecurse += SortArrayNoRecursion<std::vector<float>, float>(b);

            if (j == 0)
            {
#ifdef ENABLE_SECURITY_CHECKS
                for (size_t i = 0, j = 1; j < b.size(); i = j++)
                    if (b[i] > b[j])
                        std::cout << fmt::format("error on {} ({}) and {} ({})", i, b[i], j, b[j]);
#endif // ENABLE_SECURITY_CHECKS }
            }

            b = a;
            accustd += STDSortArray<std::vector<float>, float>(b);
        }

        std::cout << "RECURSE: Size " << size[i] << "\tRetries=" << retries << "\ttime total = " << accu << " ms\ttime average=" << accu / retries << " ms\n";
        std::cout << "NO RECURSE: Size " << size[i] << "\tRetries=" << retries << "\ttime total = " << accunorecurse << " ms\ttime average=" << accunorecurse / retries << " ms\n";
        std::cout << "STD: Size " << size[i] << "\tRetries=" << retries << "\ttime total = " << accustd << " ms\ttime average=" << accustd / retries << " ms\n";
    }

    std::cout << " **************************************************************************************** \n";
    std::cout << "                                     VECTORS \n";
    std::cout << " **************************************************************************************** \n";

    forrange(i, 0, 6)
    {

        std::vector<glm::vec2> a;
        a.resize(size[i]);
        forrange(j, 0, size[i])
        {
            a[j].x = distrib(gen64);
            a[j].y = distrib(gen64);
        }
        std::vector<glm::vec2> b;
        float accu = 0.f;
        float accunorecurse = 0.f;
        float accustd = 0.f;

        constexpr size_t retries = 20;
        forrange(j, 0, retries)
        {
            b = a;
            accu += SortArray<std::vector<glm::vec2>, glm::vec2, LexSort>(b);

            if (j == 0)
            {
#ifdef ENABLE_SECURITY_CHECKS
                LexSort sorter;
                for (size_t i = 0, j = 1; j < b.size(); i = j++)
                    if (!sorter(b[i], b[j]))
                        std::cout << fmt::format("error on {} ({}, {}) and {} ({}, {})", i, b[i].x, b[i].y, j, b[j].x, b[j].y);
#endif // ENABLE_SECURITY_CHECKS }
            }

            b = a;
            accunorecurse += SortArrayNoRecursion<std::vector<glm::vec2>, glm::vec2, LexSort>(b);

            if (j == 0)
            {
#ifdef ENABLE_SECURITY_CHECKS
                LexSort sorter;
                for (size_t i = 0, j = 1; j < b.size(); i = j++)
                    if (!sorter(b[i], b[j]))
                        std::cout << fmt::format("error on {} ({}, {}) and {} ({}, {})", i, b[i].x, b[i].y, j, b[j].x, b[j].y);
#endif // ENABLE_SECURITY_CHECKS }
            }

            b = a;
            accustd += STDSortArray<std::vector<glm::vec2>, glm::vec2, LexSort>(b);
        }

        std::cout << "RECURSE: Size " << size[i] << "\tRetries=" << retries << "\ttime total = " << accu << " ms\ttime average=" << accu / retries << " ms\n";
        std::cout << "NO RECURSE: Size " << size[i] << "\tRetries=" << retries << "\ttime total = " << accunorecurse << " ms\ttime average=" << accunorecurse / retries << " ms\n";
        std::cout << "STD: Size " << size[i] << "\tRetries=" << retries << "\ttime total = " << accustd << " ms\ttime average=" << accustd / retries << " ms\n";
    }
}

} // namespace ECSEngine