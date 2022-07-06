#pragma once
namespace ECSEngine
{
std::size_t ChoosePivot(std::size_t begin, std::size_t end);

template<typename Container, typename T>
void Swap(Container& parArray, std::size_t a, std::size_t b)
{
    if (a == b)
        return;
    T pivot = parArray[a];
    parArray[a] = parArray[b];
    parArray[b] = pivot;
}

template<typename Container, typename T, typename Functor = std::less<>>
std::size_t PartitionArray(Container& parArray, Functor f, std::size_t pivot, std::size_t begin, std::size_t end)
{
    // swap pivot and end
    Swap<Container, T>(parArray, pivot, end);

    const T& pivotValueRef = parArray[end];
    std::size_t j = begin;
    forrange(i, begin, end)
    {
        if (f(parArray[i], pivotValueRef))
        {
            Swap<Container, T>(parArray, i, j);
            ++j;
        }
    }

    Swap<Container, T>(parArray, j, end);
    return j;
}

template<typename Container, typename T, typename Functor = std::less<>>
void InPlaceSorting(Container& parArray, std::size_t begin, std::size_t end, Functor f = Functor{})
{
    // quick sort implementation

    if (begin >= end)
        return;

    // 1- choose pivot
    std::size_t pivot = ChoosePivot(begin, end);

    // 2- partition
    pivot = PartitionArray<Container, T>(parArray, f, pivot, begin, end);

    // 3- InPlaceSorting on the partitionned array
    if (pivot > 0)
        InPlaceSorting<Container, T>(parArray, begin, pivot - 1, f);

    InPlaceSorting<Container, T>(parArray, pivot + 1, end, f);
}

template<typename Container, typename T, typename Functor = std::less<>>
FORCEINLINE void InPlaceSortingNoRecursion(Container& parArray, std::size_t begin, std::size_t end, Functor f = Functor{})
{
    // quick sort implementation
    std::vector<std::size_t> stack;
    stack.reserve(std::log2(parArray.size()) * 2);
    stack.push_back(begin);
    stack.push_back(end);

    std::size_t stackIdx = 0;

    while (stackIdx < stack.size())
    {
        std::size_t b = stack[stackIdx++];
        std::size_t e = stack[stackIdx++];
        if (b >= e)
            continue;

        // 1- choose pivot
        std::size_t pivot = ChoosePivot(b, e);

        // 2- partition
        pivot = PartitionArray<Container, T>(parArray, f, pivot, b, e);

        if (pivot > 0)
        {
            stack.push_back(b);
            stack.push_back(pivot - 1);
        }
        stack.push_back(pivot + 1);
        stack.push_back(e);
    }
}

void TestSorting();
} // namespace ECSEngine