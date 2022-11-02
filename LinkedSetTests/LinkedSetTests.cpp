#include "pch.h"
#include "CppUnitTest.h"
#include <ctime>
#include <chrono>
#include <array>
#include <algorithm>
#include "../LinkedSet/LinkedSet.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

template <>
std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<ConstLinkedListIterator<signed char>>(const ConstLinkedListIterator<signed char>& iterator)
{
    // Hack to check if the iterator is an end() iterator.
    const LinkedList<signed char> dummy {};
    if (iterator == dummy.end())
    {
        return L"end()";
    }
    else
    {
        // If not the end(), format the contents of the iterator.
        return Microsoft::VisualStudio::CppUnitTestFramework::ToString(*iterator);
    }
}

template <typename T, unsigned int N>
void checkList(const std::array<T, N>& expected, const LinkedList<T>& list)
{
    // Make sure that the size of the linked list is as expected
    Assert::AreEqual(N, list.getSize(), L"getSize()");

    if (N > 0)
    {
        // check basic list attributes
        Assert::AreEqual(expected[0], list.getFirst(), L"getFirst()");
        Assert::AreEqual(expected[N - 1], list.getLast(), L"getLast()");
    }
    else
    {
        // Make sure an exception is thrown when trying to access an element from an empty list.
        Assert::ExpectException<std::logic_error>([&] { list.getFirst(); }, L"getFirst()");
        Assert::ExpectException<std::logic_error>([&] { list.getLast(); }, L"getLast()");
    }

    unsigned int i { 0 };

    // Check using an iterator.
    for (T item : list)
    {
        Assert::IsTrue(i < N, L"Too many elements visited by iterator.");
        Assert::AreEqual(expected[i], item, L"Element visited by iterator");
        i++;
    }

    Assert::AreEqual(N, i, L"Number of elements visited by iterator");
}

void checkSetEmpty(const LinkedSet<signed char>& set)
{
    Assert::AreEqual(0u, set.getSize(), L"size");

    // Make sure the list doesn't contain anything initially.
    for (int n { -128 }; n < 128; n++)
    {
        Assert::IsFalse(set.contains(n), L"Set should not contain any items.");
    }

    // Check using the iterators.
    Assert::AreEqual(set.end(), set.begin(), L"begin()");
}

template <unsigned int N>
void checkSetBase(const std::array<signed char, N>& expected, const LinkedSet<signed char>& set, unsigned int expectedSize = N)
{
    // Check the size of the set
    Assert::AreEqual(expectedSize, set.getSize(), L"size");

    // Make sure the contents of the set are correct at the end.
    for (unsigned int i { 0 }; i < expectedSize; i++)
    {
        Assert::IsTrue(set.contains(expected[i]), L"Set does not contain an expected item.");
    }

    // Make sure no extra numbers were added.
    for (int n { -128 }; n < 128; n++)
    {
        bool added { false };
        for (unsigned int j { 0 }; j < expectedSize; j++)
        {
            if (n == expected[j])
            {
                added = true;
            }
        }

        if (!added)
        {
            Assert::IsFalse(set.contains(n), L"Set contains an unexpected item.");
        }
    }
}

template <unsigned int N>
void checkSetUnordered(const std::array<signed char, N>& expected, const LinkedSet<signed char>& set, unsigned int expectedSize = N)
{
    checkSetBase(expected, set, expectedSize);

    unsigned int i { 0 };
    std::array<signed char, N> numbersInSet {};

    // Check using the iterators.
    for (signed char n : set)
    {
        Assert::IsTrue(i < expectedSize, L"Too many elements visited by iterator.");
        numbersInSet[i] = n;
        i++;
    }

    Assert::AreEqual(expectedSize, i, L"Number of elements visited by iterator");

    // Sort the numbers added and the numbers visited by the iterator.
    std::array<signed char, N> expectedSorted { expected };
    std::sort(expectedSorted.begin(), expectedSorted.begin() + expectedSize);
    std::sort(numbersInSet.begin(), numbersInSet.begin() + expectedSize);

    for (unsigned int i { 0 }; i < expectedSize; i++)
    {
        Assert::AreEqual(expectedSorted[i], numbersInSet[i], L"Element visited by iterator");
    }
}

template <unsigned int N>
void checkSetOrdered(const std::array<signed char, N>& expected, const LinkedSet<signed char>& set, unsigned int expectedSize = N)
{
    checkSetBase(expected, set, expectedSize);

    unsigned int i { 0 };

    // Sort the numbers added.
    std::array<signed char, N> expectedSorted { expected };
    std::sort(expectedSorted.begin(), expectedSorted.begin() + expectedSize);

    // Check using the iterators.
    for (signed char n : set)
    {
        Assert::IsTrue(i < expectedSize, L"Too many elements visited by iterator.");
        Assert::AreEqual(expectedSorted[i], n, L"Element visited by iterator");
        i++;
    }

    Assert::AreEqual(expectedSize, i, L"Number of elements visited by iterator");
}

namespace LinkedSetTests
{
    TEST_CLASS(LinkedSetTests)
    {
    public:
        TEST_CLASS_INITIALIZE(Initialize)
        {
            // Seed the random number generator.
            time_t seed;
            time(&seed);
            srand(static_cast<unsigned int>(seed));
        }

        TEST_METHOD(Part1_AddFirst)
        {
            LinkedList<int> list {};

            // Add items to the front of the list
            for (int i = 0; i < 10; i++)
            {
                list.addFirst(i);
                Assert::AreEqual(i, list.getFirst(), L"getFirst()");
                Assert::AreEqual(0, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Array to store values expected in the linked list
            std::array<int, 10> expectedArr { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

            // Run a subroutine to check the contents of the list
            checkList(expectedArr, list);
        }

        TEST_METHOD(Part1_AddFirstDestructorMemoryLeakCheck)
        {
            // Magic to tell us if there's a memory leak.
            _CrtMemState state1, state2, state3;

            _CrtMemCheckpoint(&state1);

            // Check for memory leaks
            for (unsigned int i { 0 }; i < 1000; i++)
            {
                {
                    LinkedList<int> list {};
                    for (int i = 0; i < 10; i++)
                    {
                        list.addFirst(i);
                        Assert::AreEqual(i, list.getFirst(), L"getFirst()");
                        Assert::AreEqual(0, list.getLast(), L"getLast()");
                        Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
                    }
                }
                // Destructor for original should have been called.

                _CrtMemCheckpoint(&state2);

                // If this assertion fails, you have a memory leak.
                Assert::AreEqual(0, _CrtMemDifference(&state3, &state1, &state2), L"Memory leak");

                state1 = state2;
            }
        }
        TEST_METHOD(Part1_AddLast)
        {
            LinkedList<int> list {};

            // Add items to the back of the list
            for (int i = 0; i < 10; i++)
            {
                list.addLast(i);
                Assert::AreEqual(0, list.getFirst(), L"getFirst()");
                Assert::AreEqual(i, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Array to store values expected in the linked list
            std::array<int, 10> expectedArr { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

            // Run a subroutine to check the contents of the list
            checkList(expectedArr, list);
        }

        TEST_METHOD(Part1_AddLastDestructorMemoryLeakCheck)
        {
            // Magic to tell us if there's a memory leak.
            _CrtMemState state1, state2, state3;

            _CrtMemCheckpoint(&state1);

            // Check for memory leaks
            for (unsigned int i { 0 }; i < 1000; i++)
            {
                {
                    LinkedList<int> list {};
                    for (int i = 0; i < 10; i++)
                    {
                        list.addLast(i);
                        Assert::AreEqual(0, list.getFirst(), L"getFirst()");
                        Assert::AreEqual(i, list.getLast(), L"getLast()");
                        Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
                    }
                }
                // Destructor for original should have been called.

                _CrtMemCheckpoint(&state2);

                // If this assertion fails, you have a memory leak.
                Assert::AreEqual(0, _CrtMemDifference(&state3, &state1, &state2), L"Memory leak");

                state1 = state2;
            }
        }

        TEST_METHOD(Part1_RemoveExceptionAddFirst)
        {
            LinkedList<int> list {};

            // Try to remove from an empty list.
            Assert::ExpectException<std::logic_error>([&] { list.removeFirst(); }, L"removeFirst() - expected exception");
            checkList(std::array<int, 0>{}, list);

            // Add items to the front of the list
            for (int i = 0; i < 10; i++)
            {
                list.addFirst(i);
                Assert::AreEqual(i, list.getFirst(), L"getFirst()");
                Assert::AreEqual(0, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Array to store values expected in the linked list
            std::array<int, 10> expectedArr { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

            // Run a subroutine to check the contents of the list
            checkList(expectedArr, list);
        }

        TEST_METHOD(Part1_RemoveExceptionAddLast)
        {
            LinkedList<int> list {};

            // Try to remove from an empty list.
            Assert::ExpectException<std::logic_error>([&] { list.removeFirst(); }, L"removeFirst() - expected exception");
            checkList(std::array<int, 0>{}, list);

            // Add items to the back of the list
            for (int i = 0; i < 10; i++)
            {
                list.addLast(i);
                Assert::AreEqual(0, list.getFirst(), L"getFirst()");
                Assert::AreEqual(i, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Array to store values expected in the linked list
            std::array<int, 10> expectedArr { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

            // Run a subroutine to check the contents of the list
            checkList(expectedArr, list);
        }

        TEST_METHOD(Part1_AddFirstAddLast)
        {
            LinkedList<int> list {};

            // Add items to the front of the list
            for (int i = 0; i < 10; i++)
            {
                list.addFirst(i);
                Assert::AreEqual(i, list.getFirst(), L"getFirst()");
                Assert::AreEqual(0, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Add items to the end of the list
            for (int i = 10; i < 20; i++)
            {
                list.addLast(i);
                Assert::AreEqual(9, list.getFirst(), L"getFirst()");
                Assert::AreEqual(i, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Array to store values expected in the linked list
            std::array<int, 20> expectedArr { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

            // Run a subroutine to check the contents of the list
            checkList(expectedArr, list);
        }

        TEST_METHOD(Part1_AddLastAddFirst)
        {
            LinkedList<int> list {};

            // Add items to the back of the list
            for (int i = 0; i < 10; i++)
            {
                list.addLast(i);
                Assert::AreEqual(0, list.getFirst(), L"getFirst()");
                Assert::AreEqual(i, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Add items to the front of the list
            for (int i = 10; i < 20; i++)
            {
                list.addFirst(i);
                Assert::AreEqual(i, list.getFirst(), L"getFirst()");
                Assert::AreEqual(9, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Array to store values expected in the linked list
            std::array<int, 20> expectedArr { 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

            // Run a subroutine to check the contents of the list
            checkList(expectedArr, list);
        }

        TEST_METHOD(Part1_AddFirstAddLastRemoveFirst)
        {
            LinkedList<int> list {};

            // Add items to the front of the list
            for (int i = 0; i < 10; i++)
            {
                list.addFirst(i);
                Assert::AreEqual(i, list.getFirst(), L"getFirst()");
                Assert::AreEqual(0, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Add items to the back of the list
            for (int i = 10; i < 20; i++)
            {
                list.addLast(i);
                Assert::AreEqual(9, list.getFirst(), L"getFirst()");
                Assert::AreEqual(i, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Remove items from the front of the list
            for (int i = 0; i < 15; i++)
            {
                list.removeFirst();
                Assert::AreEqual(19, list.getLast(), L"getLast()");
                Assert::AreEqual(19u - i, list.getSize(), L"getSize()");
            }

            // Array to store values expected in the linked list
            std::array<int, 5> expectedArr { 15, 16, 17, 18, 19 };

            // Run a subroutine to check the contents of the list
            checkList(expectedArr, list);
        }

        TEST_METHOD(Part1_AddLastAddFirstRemoveFirst)
        {
            LinkedList<int> list {};

            // Add items to the back of the list
            for (int i = 0; i < 10; i++)
            {
                list.addLast(i);
                Assert::AreEqual(0, list.getFirst(), L"getFirst()");
                Assert::AreEqual(i, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Add items to the front of the list
            for (int i = 10; i < 20; i++)
            {
                list.addFirst(i);
                Assert::AreEqual(i, list.getFirst(), L"getFirst()");
                Assert::AreEqual(9, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Remove items from the front of the list
            for (int i = 0; i < 15; i++)
            {
                list.removeFirst();
                Assert::AreEqual(9, list.getLast(), L"getLast()");
                Assert::AreEqual(19u - i, list.getSize(), L"getSize()");
            }

            // Array to store values expected in the linked list
            std::array<int, 5> expectedArr { 5, 6, 7, 8, 9 };

            // Run a subroutine to check the contents of the list
            checkList(expectedArr, list);
        }

        TEST_METHOD(Part1_AddFirstRemoveFirstAddFirst)
        {
            LinkedList<int> list {};

            // Add items to the front of the list
            for (int i = 0; i < 10; i++)
            {
                list.addFirst(i);
                Assert::AreEqual(i, list.getFirst(), L"getFirst()");
                Assert::AreEqual(0, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Remove items from the front of the list
            for (int i = 0; i < 5; i++)
            {
                list.removeFirst();
                Assert::AreEqual(0, list.getLast(), L"getLast()");
                Assert::AreEqual(9u - i, list.getSize(), L"getSize()");
            }

            // Add items to the front of the list
            for (int i = 10; i < 20; i++)
            {
                list.addFirst(i);
                Assert::AreEqual(i, list.getFirst(), L"getFirst()");
                Assert::AreEqual(0, list.getLast(), L"getLast()");
                Assert::AreEqual(i - 4u, list.getSize(), L"getSize()");
            }

            // Array to store values expected in the linked list
            std::array<int, 15> expectedArr { 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 4, 3, 2, 1, 0 };

            // Run a subroutine to check the contents of the list
            checkList(expectedArr, list);
        }

        TEST_METHOD(Part1_AddLastRemoveFirstAddFirst)
        {
            LinkedList<int> list {};

            // Add items to the back of the list
            for (int i = 0; i < 10; i++)
            {
                list.addLast(i);
                Assert::AreEqual(0, list.getFirst(), L"getFirst()");
                Assert::AreEqual(i, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Remove items from the front of the list
            for (int i = 0; i < 5; i++)
            {
                list.removeFirst();
                Assert::AreEqual(9, list.getLast(), L"getLast()");
                Assert::AreEqual(9u - i, list.getSize(), L"getSize()");
            }

            // Add items to the front of the list
            for (int i = 10; i < 20; i++)
            {
                list.addFirst(i);
                Assert::AreEqual(i, list.getFirst(), L"getFirst()");
                Assert::AreEqual(9, list.getLast(), L"getLast()");
                Assert::AreEqual(i - 4u, list.getSize(), L"getSize()");
            }

            // Array to store values expected in the linked list
            std::array<int, 15> expectedArr { 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 5, 6, 7, 8, 9 };

            // Run a subroutine to check the contents of the list
            checkList(expectedArr, list);
        }

        TEST_METHOD(Part1_AddFirstRemoveFirstAddLast)
        {
            LinkedList<int> list {};

            // Add items to the front of the list
            for (int i = 0; i < 10; i++)
            {
                list.addFirst(i);
                Assert::AreEqual(i, list.getFirst(), L"getFirst()");
                Assert::AreEqual(0, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Remove items from the front of the list
            for (int i = 0; i < 5; i++)
            {
                list.removeFirst();
                Assert::AreEqual(0, list.getLast(), L"getLast()");
                Assert::AreEqual(9u - i, list.getSize(), L"getSize()");
            }

            // Add items to the back of the list
            for (int i = 10; i < 20; i++)
            {
                list.addLast(i);
                Assert::AreEqual(4, list.getFirst(), L"getFirst()");
                Assert::AreEqual(i, list.getLast(), L"getLast()");
                Assert::AreEqual(i - 4u, list.getSize(), L"getSize()");
            }

            // Array to store values expected in the linked list
            std::array<int, 15> expectedArr { 4, 3, 2, 1, 0, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

            // Run a subroutine to check the contents of the list
            checkList(expectedArr, list);
        }

        TEST_METHOD(Part1_AddLastRemoveFirstAddLast)
        {
            LinkedList<int> list {};

            // Add items to the back of the list
            for (int i = 0; i < 10; i++)
            {
                list.addLast(i);
                Assert::AreEqual(0, list.getFirst(), L"getFirst()");
                Assert::AreEqual(i, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Remove items from the front of the list
            for (int i = 0; i < 5; i++)
            {
                list.removeFirst();
                Assert::AreEqual(9, list.getLast(), L"getLast()");
                Assert::AreEqual(9u - i, list.getSize(), L"getSize()");
            }

            // Add items to the back of the list
            for (int i = 10; i < 20; i++)
            {
                list.addLast(i);
                Assert::AreEqual(5, list.getFirst(), L"getFirst()");
                Assert::AreEqual(i, list.getLast(), L"getLast()");
                Assert::AreEqual(i - 4u, list.getSize(), L"getSize()");
            }

            // Array to store values expected in the linked list
            std::array<int, 15> expectedArr { 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

            // Run a subroutine to check the contents of the list
            checkList(expectedArr, list);
        }

        TEST_METHOD(Part1_AddFirstRemoveAllAddFirst)
        {
            LinkedList<int> list {};

            // Add items to the front of the list
            for (int i = 0; i < 10; i++)
            {
                list.addFirst(i);
                Assert::AreEqual(i, list.getFirst(), L"getFirst()");
                Assert::AreEqual(0, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Remove items from the front of the list
            for (int i = 0; i < 10; i++)
            {
                list.removeFirst();

                if (i < 9)
                {
                    Assert::AreEqual(0, list.getLast(), L"getLast()");
                }
                Assert::AreEqual(9u - i, list.getSize(), L"getSize()");
            }

            // Try to remove another item.
            Assert::ExpectException<std::logic_error>([&] { list.removeFirst(); }, L"removeFirst() - expected exception");
            checkList(std::array<int, 0>{}, list);

            // Add items to the front of the list
            for (int i = 10; i < 20; i++)
            {
                list.addFirst(i);
                Assert::AreEqual(i, list.getFirst(), L"getFirst()");
                Assert::AreEqual(10, list.getLast(), L"getLast()");
                Assert::AreEqual(i - 9u, list.getSize(), L"getSize()");
            }

            // Array to store values expected in the linked list
            std::array<int, 10> expectedArr { 19, 18, 17, 16, 15, 14, 13, 12, 11, 10 };

            // Run a subroutine to check the contents of the list
            checkList(expectedArr, list);
        }

        TEST_METHOD(Part1_AddFirstRemoveAllAddLast)
        {
            LinkedList<int> list {};

            // Add items to the front of the list
            for (int i = 0; i < 10; i++)
            {
                list.addFirst(i);
                Assert::AreEqual(i, list.getFirst(), L"getFirst()");
                Assert::AreEqual(0, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Remove items from the front of the list
            for (int i = 0; i < 10; i++)
            {
                list.removeFirst();

                if (i < 9)
                {
                    Assert::AreEqual(0, list.getLast(), L"getLast()");
                }
                Assert::AreEqual(9u - i, list.getSize(), L"getSize()");
            }

            // Try to remove another item.
            Assert::ExpectException<std::logic_error>([&] { list.removeFirst(); }, L"removeFirst() - expected exception");
            checkList(std::array<int, 0>{}, list);

            // Add items to the back of the list
            for (int i = 10; i < 20; i++)
            {
                list.addLast(i);
                Assert::AreEqual(10, list.getFirst(), L"getFirst()");
                Assert::AreEqual(i, list.getLast(), L"getLast()");
                Assert::AreEqual(i - 9u, list.getSize(), L"getSize()");
            }

            // Array to store values expected in the linked list
            std::array<int, 10> expectedArr { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

            // Run a subroutine to check the contents of the list
            checkList(expectedArr, list);
        }

        TEST_METHOD(Part1_AddLastRemoveAllAddFirst)
        {
            LinkedList<int> list {};

            // Add items to the front of the list
            for (int i = 0; i < 10; i++)
            {
                list.addLast(i);
                Assert::AreEqual(0, list.getFirst(), L"getFirst()");
                Assert::AreEqual(i, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Remove items from the front of the list
            for (int i = 0; i < 10; i++)
            {
                list.removeFirst();

                if (i < 9)
                {
                    Assert::AreEqual(9, list.getLast(), L"getLast()");
                }
                Assert::AreEqual(9u - i, list.getSize(), L"getSize()");
            }

            // Try to remove another item.
            Assert::ExpectException<std::logic_error>([&] { list.removeFirst(); }, L"removeFirst() - expected exception");
            checkList(std::array<int, 0>{}, list);

            // Add items to the front of the list
            for (int i = 10; i < 20; i++)
            {
                list.addFirst(i);
                Assert::AreEqual(i, list.getFirst(), L"getFirst()");
                Assert::AreEqual(10, list.getLast(), L"getLast()");
                Assert::AreEqual(i - 9u, list.getSize(), L"getSize()");
            }

            // Array to store values expected in the linked list
            std::array<int, 10> expectedArr { 19, 18, 17, 16, 15, 14, 13, 12, 11, 10 };

            // Run a subroutine to check the contents of the list
            checkList(expectedArr, list);
        }

        TEST_METHOD(Part1_AddLastRemoveAllAddLast)
        {
            LinkedList<int> list {};

            // Add items to the front of the list
            for (int i = 0; i < 10; i++)
            {
                list.addLast(i);
                Assert::AreEqual(0, list.getFirst(), L"getFirst()");
                Assert::AreEqual(i, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Remove items from the front of the list
            for (int i = 0; i < 10; i++)
            {
                list.removeFirst();

                if (i < 9)
                {
                    Assert::AreEqual(9, list.getLast(), L"getLast()");
                }
                Assert::AreEqual(9u - i, list.getSize(), L"getSize()");
            }

            // Try to remove another item.
            Assert::ExpectException<std::logic_error>([&] { list.removeFirst(); }, L"removeFirst() - expected exception");
            checkList(std::array<int, 0>{}, list);

            // Add items to the back of the list
            for (int i = 10; i < 20; i++)
            {
                list.addLast(i);
                Assert::AreEqual(10, list.getFirst(), L"getFirst()");
                Assert::AreEqual(i, list.getLast(), L"getLast()");
                Assert::AreEqual(i - 9u, list.getSize(), L"getSize()");
            }

            // Array to store values expected in the linked list
            std::array<int, 10> expectedArr { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

            // Run a subroutine to check the contents of the list
            checkList(expectedArr, list);
        }

        TEST_METHOD(Part1_RemoveMemoryLeakCheck)
        {
            // Initialize the original
            LinkedList<int> list {};
            for (int i = 0; i < 10; i++)
            {
                list.addFirst(i);
            }

            // Magic to tell us if there's a memory leak.
            _CrtMemState state1, state2, state3;

            _CrtMemCheckpoint(&state1);

            // Check for memory leaks
            for (unsigned int i { 0 }; i < 1000; i++)
            {
                list.addFirst(-1);
                list.removeFirst();

                _CrtMemCheckpoint(&state2);

                // If this assertion fails, you have a memory leak.
                Assert::AreEqual(0, _CrtMemDifference(&state3, &state1, &state2), L"Memory leak");

                state1 = state2;
            }

            std::array<int, 10> expected { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
            checkList(expected, list);
        }
        TEST_METHOD(Part1_AddFirstClearAddFirst)
        {
            LinkedList<int> list {};

            // Add items to the front of the list
            for (int i = 0; i < 10; i++)
            {
                list.addFirst(i);
                Assert::AreEqual(i, list.getFirst(), L"getFirst()");
                Assert::AreEqual(0, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Clear the list.
            list.clear();
            checkList(std::array<int, 0>{}, list);

            // Try to remove another item.
            Assert::ExpectException<std::logic_error>([&] { list.removeFirst(); }, L"removeFirst() - expected exception");
            checkList(std::array<int, 0>{}, list);

            // Add items to the front of the list
            for (int i = 10; i < 20; i++)
            {
                list.addFirst(i);
                Assert::AreEqual(i, list.getFirst(), L"getFirst()");
                Assert::AreEqual(10, list.getLast(), L"getLast()");
                Assert::AreEqual(i - 9u, list.getSize(), L"getSize()");
            }

            // Array to store values expected in the linked list
            std::array<int, 10> expectedArr { 19, 18, 17, 16, 15, 14, 13, 12, 11, 10 };

            // Run a subroutine to check the contents of the list
            checkList(expectedArr, list);
        }

        TEST_METHOD(Part1_AddFirstClearAddLast)
        {
            LinkedList<int> list {};

            // Add items to the front of the list
            for (int i = 0; i < 10; i++)
            {
                list.addFirst(i);
                Assert::AreEqual(i, list.getFirst(), L"getFirst()");
                Assert::AreEqual(0, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Clear the list.
            list.clear();
            checkList(std::array<int, 0>{}, list);

            // Try to remove another item.
            Assert::ExpectException<std::logic_error>([&] { list.removeFirst(); }, L"removeFirst() - expected exception");
            checkList(std::array<int, 0>{}, list);

            // Add items to the back of the list
            for (int i = 10; i < 20; i++)
            {
                list.addLast(i);
                Assert::AreEqual(10, list.getFirst(), L"getFirst()");
                Assert::AreEqual(i, list.getLast(), L"getLast()");
                Assert::AreEqual(i - 9u, list.getSize(), L"getSize()");
            }

            // Array to store values expected in the linked list
            std::array<int, 10> expectedArr { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

            // Run a subroutine to check the contents of the list
            checkList(expectedArr, list);
        }

        TEST_METHOD(Part1_AddLastClearAddFirst)
        {
            LinkedList<int> list {};

            // Add items to the front of the list
            for (int i = 0; i < 10; i++)
            {
                list.addLast(i);
                Assert::AreEqual(0, list.getFirst(), L"getFirst()");
                Assert::AreEqual(i, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Clear the list.
            list.clear();
            checkList(std::array<int, 0>{}, list);

            // Try to remove another item.
            Assert::ExpectException<std::logic_error>([&] { list.removeFirst(); }, L"removeFirst() - expected exception");
            checkList(std::array<int, 0>{}, list);

            // Add items to the front of the list
            for (int i = 10; i < 20; i++)
            {
                list.addFirst(i);
                Assert::AreEqual(i, list.getFirst(), L"getFirst()");
                Assert::AreEqual(10, list.getLast(), L"getLast()");
                Assert::AreEqual(i - 9u, list.getSize(), L"getSize()");
            }

            // Array to store values expected in the linked list
            std::array<int, 10> expectedArr { 19, 18, 17, 16, 15, 14, 13, 12, 11, 10 };

            // Run a subroutine to check the contents of the list
            checkList(expectedArr, list);
        }

        TEST_METHOD(Part1_AddLastClearAddLast)
        {
            LinkedList<int> list {};

            // Add items to the front of the list
            for (int i = 0; i < 10; i++)
            {
                list.addLast(i);
                Assert::AreEqual(0, list.getFirst(), L"getFirst()");
                Assert::AreEqual(i, list.getLast(), L"getLast()");
                Assert::AreEqual(i + 1u, list.getSize(), L"getSize()");
            }

            // Clear the list.
            list.clear();
            checkList(std::array<int, 0>{}, list);

            // Try to remove another item.
            Assert::ExpectException<std::logic_error>([&] { list.removeFirst(); }, L"removeFirst() - expected exception");
            checkList(std::array<int, 0>{}, list);

            // Add items to the back of the list
            for (int i = 10; i < 20; i++)
            {
                list.addLast(i);
                Assert::AreEqual(10, list.getFirst(), L"getFirst()");
                Assert::AreEqual(i, list.getLast(), L"getLast()");
                Assert::AreEqual(i - 9u, list.getSize(), L"getSize()");
            }

            // Array to store values expected in the linked list
            std::array<int, 10> expectedArr { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

            // Run a subroutine to check the contents of the list
            checkList(expectedArr, list);
        }

        TEST_METHOD(Part1_CopyConstructor)
        {
            // Initialize the original
            LinkedList<int> original {};
            for (int i = 0; i < 10; i++)
            {
                original.addLast(i);
            }

            // Make a copy
            LinkedList<int> copy { original };

            // Check the original and the copy.
            std::array<int, 10> expectedOriginal { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
            checkList(expectedOriginal, original);
            checkList(expectedOriginal, copy);
        }

        TEST_METHOD(Part1_CopyConstructorModifyCopy)
        {
            // Initialize the original
            LinkedList<int> original {};
            for (int i = 0; i < 10; i++)
            {
                original.addLast(i);
            }

            // Make a copy
            LinkedList<int> copy { original };

            // Check the original and the copy.
            std::array<int, 10> expectedOriginal { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
            checkList(expectedOriginal, original);
            checkList(expectedOriginal, copy);

            // Modify the copy:
            copy.getFirst() = 11;
            copy.getLast() = 12;

            // Original should be unchanged.
            Assert::AreEqual(0, original.getFirst());
            Assert::AreEqual(9, original.getLast());

            // Copy should be modified.
            Assert::AreEqual(11, copy.getFirst());
            Assert::AreEqual(12, copy.getLast());

            // Add to the copy:
            copy.addFirst(13);
            copy.addLast(14);

            // Check the original and the modified copy:
            std::array<int, 12> expectedModified { 13, 11, 1, 2, 3, 4, 5, 6, 7, 8, 12, 14 };
            checkList(expectedOriginal, original);
            checkList(expectedModified, copy);

            // Remove from the copy:
            copy.removeFirst();
            copy.removeFirst();
            copy.removeFirst();

            // Check the original and the modified copy:
            std::array<int, 9> expectedModified2 { 2, 3, 4, 5, 6, 7, 8, 12, 14 };
            checkList(expectedOriginal, original);
            checkList(expectedModified2, copy);
        }

        TEST_METHOD(Part1_CopyConstructorModifyOriginal)
        {
            // Initialize the original
            LinkedList<int> original {};
            for (int i = 0; i < 10; i++)
            {
                original.addLast(i);
            }

            // Make a copy
            LinkedList<int> copy { original };

            // Check the original and the copy.
            std::array<int, 10> expectedOriginal { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
            checkList(expectedOriginal, original);
            checkList(expectedOriginal, copy);

            // Modify the original:
            original.getFirst() = 11;
            original.getLast() = 12;

            // Copy should be unchanged.
            Assert::AreEqual(0, copy.getFirst());
            Assert::AreEqual(9, copy.getLast());

            // Original should be modified.
            Assert::AreEqual(11, original.getFirst());
            Assert::AreEqual(12, original.getLast());

            // Add to the copy:
            original.addFirst(13);
            original.addLast(14);

            // Check the original and the modified copy:
            std::array<int, 12> expectedModified { 13, 11, 1, 2, 3, 4, 5, 6, 7, 8, 12, 14 };
            checkList(expectedModified, original);
            checkList(expectedOriginal, copy);

            // Remove from the copy:
            original.removeFirst();
            original.removeFirst();
            original.removeFirst();

            // Check the original and the modified copy:
            std::array<int, 9> expectedModified2 { 2, 3, 4, 5, 6, 7, 8, 12, 14 };
            checkList(expectedModified2, original);
            checkList(expectedOriginal, copy);
        }

        TEST_METHOD(Part1_CopyAssignment)
        {
            // Initialize the original
            LinkedList<int> original {};
            for (int i = 0; i < 10; i++)
            {
                original.addLast(i);
            }

            // A linked list that will have a copy.
            LinkedList<int> copy {};

            // Add some data that should be wiped when we replace it with a copy.
            copy.addFirst(99);
            copy.addLast(100);

            // Make a copy
            copy = original;

            // Check the original and the copy.
            std::array<int, 10> expectedOriginal { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
            checkList(expectedOriginal, original);
            checkList(expectedOriginal, copy);
        }

        TEST_METHOD(Part1_CopyAssignmentModifyCopy)
        {
            // Initialize the original
            LinkedList<int> original {};
            for (int i = 0; i < 10; i++)
            {
                original.addLast(i);
            }

            // A linked list that will have a copy.
            LinkedList<int> copy {};

            // Add some data that should be wiped when we replace it with a copy.
            copy.addFirst(99);
            copy.addLast(100);

            // Make a copy
            copy = original;

            // Check the original and the copy.
            std::array<int, 10> expectedOriginal { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
            checkList(expectedOriginal, original);
            checkList(expectedOriginal, copy);

            // Modify the copy:
            copy.getFirst() = 11;
            copy.getLast() = 12;

            // Original should be unchanged.
            Assert::AreEqual(0, original.getFirst());
            Assert::AreEqual(9, original.getLast());

            // Copy should be modified.
            Assert::AreEqual(11, copy.getFirst());
            Assert::AreEqual(12, copy.getLast());

            // Add to the copy:
            copy.addFirst(13);
            copy.addLast(14);

            // Check the original and the modified copy:
            std::array<int, 12> expectedModified { 13, 11, 1, 2, 3, 4, 5, 6, 7, 8, 12, 14 };
            checkList(expectedOriginal, original);
            checkList(expectedModified, copy);

            // Remove from the copy:
            copy.removeFirst();
            copy.removeFirst();
            copy.removeFirst();

            // Check the original and the modified copy:
            std::array<int, 9> expectedModified2 { 2, 3, 4, 5, 6, 7, 8, 12, 14 };
            checkList(expectedOriginal, original);
            checkList(expectedModified2, copy);
        }

        TEST_METHOD(Part1_CopyAssignmentModifyOriginal)
        {
            // Initialize the original
            LinkedList<int> original {};
            for (int i = 0; i < 10; i++)
            {
                original.addLast(i);
            }

            // A linked list that will have a copy.
            LinkedList<int> copy {};

            // Add some data that should be wiped when we replace it with a copy.
            copy.addFirst(99);
            copy.addLast(100);

            // Make a copy
            copy = original;

            // Check the original and the copy.
            std::array<int, 10> expectedOriginal { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
            checkList(expectedOriginal, original);
            checkList(expectedOriginal, copy);

            // Modify the original:
            original.getFirst() = 11;
            original.getLast() = 12;

            // Copy should be unchanged.
            Assert::AreEqual(0, copy.getFirst());
            Assert::AreEqual(9, copy.getLast());

            // Original should be modified.
            Assert::AreEqual(11, original.getFirst());
            Assert::AreEqual(12, original.getLast());

            // Add to the copy:
            original.addFirst(13);
            original.addLast(14);

            // Check the original and the modified copy:
            std::array<int, 12> expectedModified { 13, 11, 1, 2, 3, 4, 5, 6, 7, 8, 12, 14 };
            checkList(expectedModified, original);
            checkList(expectedOriginal, copy);

            // Remove from the copy:
            original.removeFirst();
            original.removeFirst();
            original.removeFirst();

            // Check the original and the modified copy:
            std::array<int, 9> expectedModified2 { 2, 3, 4, 5, 6, 7, 8, 12, 14 };
            checkList(expectedModified2, original);
            checkList(expectedOriginal, copy);
        }

        TEST_METHOD(Part1_CopyAssignmentMemoryLeakCheck)
        {
            // Initialize the original
            LinkedList<int> original {};
            for (int i = 0; i < 10; i++)
            {
                original.addLast(i);
            }

            // A linked list that will copy from the original.
            LinkedList<int> copy { original };

            // Magic to tell us if there's a memory leak.
            _CrtMemState state1, state2, state3;

            _CrtMemCheckpoint(&state1);

            // Check for memory leaks
            for (unsigned int i { 0 }; i < 1000; i++)
            {
                copy.addFirst(-1);
                copy = original;

                _CrtMemCheckpoint(&state2);

                // If this assertion fails, you have a memory leak.
                Assert::AreEqual(0, _CrtMemDifference(&state3, &state1, &state2), L"Memory leak");

                state1 = state2;
            }

            std::array<int, 10> expectedOriginal { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
            checkList(expectedOriginal, original);
            checkList(expectedOriginal, copy);
        }

        TEST_METHOD(Part1_CopySelfAssignment)
        {
            // Initialize the original
            LinkedList<int> list {};
            for (int i = 0; i < 10; i++)
            {
                list.addLast(i);
            }

            // Check the list
            std::array<int, 10> expected { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
            checkList(expected, list);

            // attempt self-assignment
            list = list;

            // Check the list again
            checkList(expected, list);
        }

        TEST_METHOD(Part1_Move)
        {
            // Initialize the original
            LinkedList<int> list1 {};
            for (int i = 0; i < 10; i++)
            {
                list1.addLast(i);
            }

            // Make a copy
            LinkedList<int> list2 { };
            for (int i = 10; i < 15; i++)
            {
                list2.addLast(i);
            }

            // Swap the lists using move constructor / assignment.
            std::swap(list1, list2);

            // Check the lists after swapping.
            std::array<int, 5> expected1 { 10, 11, 12, 13, 14 };
            std::array<int, 10> expected2 { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
            checkList(expected1, list1);
            checkList(expected2, list2);
        }

        TEST_METHOD(Part1_MoveMemoryLeakCheck)
        {
            // Initialize the original
            LinkedList<int> list1 {};
            for (int i = 0; i < 10; i++)
            {
                list1.addLast(i);
            }

            // Make a copy
            LinkedList<int> list2 { };
            for (int i = 10; i < 15; i++)
            {
                list2.addLast(i);
            }

            // Magic to tell us if there's a memory leak.
            _CrtMemState state1, state2, state3;

            _CrtMemCheckpoint(&state1);

            // Check for memory leaks
            for (unsigned int i { 0 }; i < 1000; i++)
            {
                // Swap the lists using move constructor / assignment.
                std::swap(list1, list2);

                _CrtMemCheckpoint(&state2);

                // If this assertion fails, you have a memory leak.
                Assert::AreEqual(0, _CrtMemDifference(&state3, &state1, &state2), L"Memory leak");

                state1 = state2;
            }

            // Check the lists after swapping 1000 times.
            std::array<int, 10> expected1 { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
            std::array<int, 5> expected2 { 10, 11, 12, 13, 14 };
            checkList(expected1, list1);
            checkList(expected2, list2);
        }

        TEST_METHOD(Part1_MoveStressTest)
        {
            // Initialize the original
            LinkedList<int> list1 {};
            for (int i = 0; i < 1000000; i++)
            {
                list1.addLast(i);
            }

            // Make a copy
            LinkedList<int> list2 { };
            for (int i = 1000000; i < 2000000; i++)
            {
                list2.addLast(i);
            }

            auto start { std::chrono::system_clock::now() };

            for (int i = 0; i < 1000000; i++)
            {
                // Swap the lists using move constructor / assignment.
                std::swap(list1, list2);

                std::chrono::duration<double> computationTime { std::chrono::system_clock::now() - start };

                Assert::IsTrue(computationTime < std::chrono::seconds { 10 }, L"Timed out");
            }

            Assert::AreEqual(1000000u, list1.getSize());
            Assert::AreEqual(0, list1.getFirst());
            Assert::AreEqual(999999, list1.getLast());

            Assert::AreEqual(1000000u, list2.getSize());
            Assert::AreEqual(1000000, list2.getFirst());
            Assert::AreEqual(1999999, list2.getLast());

            auto iterator1 { list1.begin() };
            for (int i = 0; i < 1000000; i++)
            {
                Assert::AreEqual(i, *iterator1, L"Element visited by iterator");
                iterator1++;
            }

            auto iterator2 { list2.begin() };
            for (int i = 1000000; i < 2000000; i++)
            {
                Assert::AreEqual(i, *iterator2, L"Element visited by iterator");
                iterator2++;
            }
        }

        TEST_METHOD(Part1_MoveSelfAssignment)
        {
            // Initialize the original
            LinkedList<int> list {};
            for (int i = 0; i < 10; i++)
            {
                list.addLast(i);
            }

            // Check the list
            std::array<int, 10> expected { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
            checkList(expected, list);

            // attempt self-assignment
            list = std::move(list);

            // Check the list again
            checkList(expected, list);
        }

        TEST_METHOD(Part2_Add)
        {
            LinkedSet<signed char> set {};

            checkSetEmpty(set);

            std::array<signed char, 12> numbersAdded {};

            // Add ten random numbers.
            for (int i { 0 }; i < 10; i++)
            {
                signed char numberToAdd;

                do
                {
                    // Generate a random number betwen -99 and 99.
                    numberToAdd = rand() % 199 - 99;
                }
                while (set.contains(numberToAdd)); // Make sure the set doesn't already contain the number.

                Assert::IsTrue(set.add(numberToAdd), L"add() was expected to return true.");
                numbersAdded[i] = numberToAdd;

                Assert::AreEqual(i + 1u, set.getSize(), L"size");
                Assert::IsTrue(set.contains(numberToAdd), L"Set does not contain an expected item.");
            }

            // Add a number smaller than all the other numbers.
            Assert::IsTrue(set.add(-100), L"add() was expected to return true.");
            numbersAdded[10] = -100;
            Assert::AreEqual(11u, set.getSize(), L"size");
            Assert::IsTrue(set.contains(-100), L"Set does not contain an expected item.");

            // Add a number larger than all the other numbers.
            Assert::IsTrue(set.add(100), L"add() was expected to return true.");
            numbersAdded[11] = 100;
            Assert::AreEqual(12u, set.getSize(), L"size");
            Assert::IsTrue(set.contains(100), L"Set does not contain an expected item.");

            checkSetUnordered(numbersAdded, set);
        }

        TEST_METHOD(Part2_AddDuplicate)
        {
            LinkedSet<signed char> set {};

            checkSetEmpty(set);

            std::array<signed char, 12> numbersAdded {};

            // Add ten random numbers.
            for (int i { 0 }; i < 10; i++)
            {
                signed char numberToAdd;

                do
                {
                    // Generate a random number betwen -99 and 99.
                    numberToAdd = rand() % 199 - 99;
                }
                while (set.contains(numberToAdd)); // Make sure the set doesn't already contain the number.

                Assert::IsTrue(set.add(numberToAdd), L"add() was expected to return true.");
                numbersAdded[i] = numberToAdd;

                Assert::AreEqual(i + 1u, set.getSize(), L"size");
                Assert::IsTrue(set.contains(numberToAdd), L"Set does not contain an expected item.");
            }

            // Add a number smaller than all the other numbers.
            Assert::IsTrue(set.add(-100), L"add() was expected to return true.");
            numbersAdded[10] = -100;
            Assert::AreEqual(11u, set.getSize(), L"size");
            Assert::IsTrue(set.contains(-100), L"Set does not contain an expected item.");

            // Add a number larger than all the other numbers.
            Assert::IsTrue(set.add(100), L"add() was expected to return true.");
            numbersAdded[11] = 100;
            Assert::AreEqual(12u, set.getSize(), L"size");
            Assert::IsTrue(set.contains(100), L"Set does not contain an expected item.");

            checkSetUnordered(numbersAdded, set);

            for (signed char n : numbersAdded)
            {
                // Try adding a duplicate of each element.
                Assert::IsFalse(set.add(n), L"add() was expected to return false.");
                Assert::AreEqual(12u, set.getSize(), L"size"); // Size shouldn't change.
            }

            checkSetUnordered(numbersAdded, set);
        }

        TEST_METHOD(Part2_AddDuplicateStressTest)
        {
            LinkedSet<signed char> set {};

            checkSetEmpty(set);

            Assert::IsTrue(set.add(0), L"add() was expected to return true.");

            Assert::AreEqual(1u, set.getSize(), L"size");
            Assert::IsTrue(set.contains(0), L"Set does not contain an expected item.");

            // Try to add a million duplicate copies of a number.
            for (int i { 0 }; i < 1000000; i++)
            {
                Assert::IsFalse(set.add(0), L"add() was expected to return true.");

                Assert::AreEqual(1u, set.getSize(), L"size");
                Assert::IsTrue(set.contains(0), L"Set does not contain an expected item.");
            }

            auto start { std::chrono::system_clock::now() };

            // Check a million times that another number isn't there.
            for (int i { 0 }; i < 1000000; i++)
            {
                Assert::IsFalse(set.contains(1), L"Set does not contain an expected item.");

                std::chrono::duration<double> computationTime { std::chrono::system_clock::now() - start };

                Assert::IsTrue(computationTime < std::chrono::seconds { 30 }, L"Timed out");
            }
        }

        TEST_METHOD(Part2_AddRemove)
        {
            LinkedSet<signed char> set {};

            checkSetEmpty(set);

            // Try removing from an empty set.
            for (int n { -128 }; n < 128; n++)
            {
                Assert::IsFalse(set.remove(n), L"remove() was expected to return false.");
            }

            checkSetEmpty(set);

            std::array<signed char, 12> numbersAdded {};

            // Add ten random numbers.
            for (int i { 0 }; i < 10; i++)
            {
                signed char numberToAdd;

                do
                {
                    // Generate a random number betwen -99 and 99.
                    numberToAdd = rand() % 199 - 99;
                }
                while (set.contains(numberToAdd)); // Make sure the set doesn't already contain the number.

                set.add(numberToAdd);
                numbersAdded[i] = numberToAdd;

                Assert::AreEqual(i + 1u, set.getSize(), L"size");
                Assert::IsTrue(set.contains(numberToAdd), L"Set does not contain an expected item.");
            }

            // Add a number smaller than all the other numbers.
            set.add(-100);
            numbersAdded[10] = -100;
            Assert::AreEqual(11u, set.getSize(), L"size");
            Assert::IsTrue(set.contains(-100), L"Set does not contain an expected item.");

            // Add a number larger than all the other numbers.
            set.add(100);
            numbersAdded[11] = 100;
            Assert::AreEqual(12u, set.getSize(), L"size");
            Assert::IsTrue(set.contains(100), L"Set does not contain an expected item.");

            checkSetUnordered(numbersAdded, set);

            for (signed char n : numbersAdded)
            {
                // Try adding a duplicate of each element.
                Assert::IsFalse(set.add(n), L"add() was expected to return false.");
                Assert::AreEqual(12u, set.getSize(), L"size"); // Size shouldn't change.
            }

            checkSetUnordered(numbersAdded, set);

            // Try to remove items that aren't in the set.
            for (int n { -128 }; n < 128; n++)
            {
                bool added { false };
                for (int j { 0 }; j < 12; j++)
                {
                    if (n == numbersAdded[j])
                    {
                        added = true;
                    }
                }

                if (!added)
                {
                    Assert::IsFalse(set.remove(n), L"remove() was expected to return false.");
                    Assert::AreEqual(12u, set.getSize(), L"size");
                }
            }

            checkSetUnordered(numbersAdded, set);

            // Remove the largest number.
            Assert::IsTrue(set.remove(100), L"remove() was expected to return true.");
            Assert::IsFalse(set.contains(100), L"Set contains an unexpected item.");
            checkSetUnordered(numbersAdded, set, 11);

            // Remove the smallest number.
            Assert::IsTrue(set.remove(-100), L"remove() was expected to return true.");
            Assert::IsFalse(set.contains(-100), L"Set contains an unexpected item.");
            checkSetUnordered(numbersAdded, set, 10);

            for (int i { 9 }; i >= 0; i--)
            {
                // Check the set after the previous remove.
                checkSetUnordered(numbersAdded, set, i + 1);

                // Remove each element from the set.
                Assert::IsTrue(set.remove(numbersAdded[i]), L"remove() was expected to return true.");
                Assert::IsFalse(set.contains(numbersAdded[i]), L"Set contains an unexpected item.");
            }

            // Make sure the set still behaves like it's empty.
            checkSetEmpty(set);

            // Try removing from an empty set.
            for (int n { -128 }; n < 128; n++)
            {
                Assert::IsFalse(set.remove(n), L"remove() was expected to return false.");
            }

            checkSetEmpty(set);
        }

        TEST_METHOD(Part3_AddNext)
        {
            LinkedList<int> list {};

            std::array<int, 20> expected {};

            // Add ten random numbers.
            for (int i { 0 }; i < 10; i++)
            {
                list.addLast(rand() % 199 - 99);
                expected[2 * i] = list.getLast();
                expected[2 * i + 1] = -list.getLast();
            }

            Assert::AreEqual(10u, list.getSize(), L"size");

            // Add negative of each number immediately afterwards.
            for (auto i { list.begin() }; i != list.end(); i++)
            {
                i.addNext(-*i);
                i++; // Advance to the negative copy.
            }

            Assert::AreEqual(expected[19], list.getLast());

            checkList(expected, list);
        }

        TEST_METHOD(Part3_AddNextRemoveNextMemoryLeakCheck)
        {
            LinkedList<int> list {};

            std::array<int, 1000> expected {};

            // Add 1000 random numbers.
            for (int i { 0 }; i < 1000; i++)
            {
                list.addLast(rand() % 199 - 99);
                expected[i] = list.getLast();
            }

            checkList(expected, list);

            // Magic to tell us if there's a memory leak.
            _CrtMemState state1, state2, state3;

            _CrtMemCheckpoint(&state1);

            // Add negative of each number immediately afterwards.
            for (auto i { list.begin() }; i != list.end(); i++)
            {
                // Add a node and immediately remove it.
                i.addNext(-*i);
                i.removeNext();

                _CrtMemCheckpoint(&state2);

                // If this assertion fails, you have a memory leak.
                Assert::AreEqual(0, _CrtMemDifference(&state3, &state1, &state2), L"Memory leak");

                state1 = state2;
            }

            checkList(expected, list);
        }

        TEST_METHOD(Part3_AddSorted)
        {
            LinkedSet<signed char> set {};

            checkSetEmpty(set);

            std::array<signed char, 12> numbersAdded {};

            // Add ten random numbers.
            for (int i { 0 }; i < 10; i++)
            {
                signed char numberToAdd;

                do
                {
                    // Generate a random number betwen -99 and 99.
                    numberToAdd = rand() % 199 - 99;
                }
                while (set.contains(numberToAdd)); // Make sure the set doesn't already contain the number.

                Assert::IsTrue(set.add(numberToAdd), L"add() was expected to return true.");
                numbersAdded[i] = numberToAdd;

                Assert::AreEqual(i + 1u, set.getSize(), L"size");
                Assert::IsTrue(set.contains(numberToAdd), L"Set does not contain an expected item.");
            }

            // Add a number smaller than all the other numbers.
            Assert::IsTrue(set.add(-100), L"add() was expected to return true.");
            numbersAdded[10] = -100;
            Assert::AreEqual(11u, set.getSize(), L"size");
            Assert::IsTrue(set.contains(-100), L"Set does not contain an expected item.");

            // Add a number larger than all the other numbers.
            Assert::IsTrue(set.add(100), L"add() was expected to return true.");
            numbersAdded[11] = 100;
            Assert::AreEqual(12u, set.getSize(), L"size");
            Assert::IsTrue(set.contains(100), L"Set does not contain an expected item.");

            checkSetOrdered(numbersAdded, set);
        }

        TEST_METHOD(Part3_AddSortedDuplicate)
        {
            LinkedSet<signed char> set {};

            checkSetEmpty(set);

            std::array<signed char, 12> numbersAdded {};

            // Add ten random numbers.
            for (int i { 0 }; i < 10; i++)
            {
                signed char numberToAdd;

                do
                {
                    // Generate a random number betwen -99 and 99.
                    numberToAdd = rand() % 199 - 99;
                }
                while (set.contains(numberToAdd)); // Make sure the set doesn't already contain the number.

                Assert::IsTrue(set.add(numberToAdd), L"add() was expected to return true.");
                numbersAdded[i] = numberToAdd;

                Assert::AreEqual(i + 1u, set.getSize(), L"size");
                Assert::IsTrue(set.contains(numberToAdd), L"Set does not contain an expected item.");
            }

            // Add a number smaller than all the other numbers.
            Assert::IsTrue(set.add(-100), L"add() was expected to return true.");
            numbersAdded[10] = -100;
            Assert::AreEqual(11u, set.getSize(), L"size");
            Assert::IsTrue(set.contains(-100), L"Set does not contain an expected item.");

            // Add a number larger than all the other numbers.
            Assert::IsTrue(set.add(100), L"add() was expected to return true.");
            numbersAdded[11] = 100;
            Assert::AreEqual(12u, set.getSize(), L"size");
            Assert::IsTrue(set.contains(100), L"Set does not contain an expected item.");

            checkSetOrdered(numbersAdded, set);

            for (signed char n : numbersAdded)
            {
                // Try adding a duplicate of each element.
                Assert::IsFalse(set.add(n), L"add() was expected to return false.");
                Assert::AreEqual(12u, set.getSize(), L"size"); // Size shouldn't change.
            }

            checkSetOrdered(numbersAdded, set);
        }

        TEST_METHOD(Part3_AddSortedRemove)
        {
            LinkedSet<signed char> set {};

            checkSetEmpty(set);

            // Try removing from an empty set.
            for (int n { -128 }; n < 128; n++)
            {
                Assert::IsFalse(set.remove(n), L"remove() was expected to return false.");
            }

            checkSetEmpty(set);

            std::array<signed char, 12> numbersAdded {};

            // Add ten random numbers.
            for (int i { 0 }; i < 10; i++)
            {
                signed char numberToAdd;

                do
                {
                    // Generate a random number betwen -99 and 99.
                    numberToAdd = rand() % 199 - 99;
                }
                while (set.contains(numberToAdd)); // Make sure the set doesn't already contain the number.

                set.add(numberToAdd);
                numbersAdded[i] = numberToAdd;

                Assert::AreEqual(i + 1u, set.getSize(), L"size");
                Assert::IsTrue(set.contains(numberToAdd), L"Set does not contain an expected item.");
            }

            // Add a number smaller than all the other numbers.
            set.add(-100);
            numbersAdded[10] = -100;
            Assert::AreEqual(11u, set.getSize(), L"size");
            Assert::IsTrue(set.contains(-100), L"Set does not contain an expected item.");

            // Add a number larger than all the other numbers.
            set.add(100);
            numbersAdded[11] = 100;
            Assert::AreEqual(12u, set.getSize(), L"size");
            Assert::IsTrue(set.contains(100), L"Set does not contain an expected item.");

            checkSetOrdered(numbersAdded, set);

            for (signed char n : numbersAdded)
            {
                // Try adding a duplicate of each element.
                Assert::IsFalse(set.add(n), L"add() was expected to return false.");
                Assert::AreEqual(12u, set.getSize(), L"size"); // Size shouldn't change.
            }

            checkSetOrdered(numbersAdded, set);

            // Try to remove items that aren't in the set.
            for (int n { -128 }; n < 128; n++)
            {
                bool added { false };
                for (int j { 0 }; j < 12; j++)
                {
                    if (n == numbersAdded[j])
                    {
                        added = true;
                    }
                }

                if (!added)
                {
                    Assert::IsFalse(set.remove(n), L"remove() was expected to return false.");
                    Assert::AreEqual(12u, set.getSize(), L"size");
                }
            }

            checkSetOrdered(numbersAdded, set);

            // Remove the largest number.
            Assert::IsTrue(set.remove(100), L"remove() was expected to return true.");
            Assert::IsFalse(set.contains(100), L"Set contains an unexpected item.");
            checkSetOrdered(numbersAdded, set, 11);

            // Remove the smallest number.
            Assert::IsTrue(set.remove(-100), L"remove() was expected to return true.");
            Assert::IsFalse(set.contains(-100), L"Set contains an unexpected item.");
            checkSetOrdered(numbersAdded, set, 10);

            for (int i { 9 }; i >= 0; i--)
            {
                // Check the set after the previous remove.
                checkSetOrdered(numbersAdded, set, i + 1);

                // Remove each element from the set.
                Assert::IsTrue(set.remove(numbersAdded[i]), L"remove() was expected to return true.");
                Assert::IsFalse(set.contains(numbersAdded[i]), L"Set contains an unexpected item.");
            }

            // Make sure the set still behaves like it's empty.
            checkSetEmpty(set);

            // Try removing from an empty set.
            for (int n { -128 }; n < 128; n++)
            {
                Assert::IsFalse(set.remove(n), L"remove() was expected to return false.");
            }

            checkSetEmpty(set);
        }
    };
}
