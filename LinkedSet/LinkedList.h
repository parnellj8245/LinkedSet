#include <ostream>
#include <stdexcept>
#include "ListNode.h"
#include "LinkedSet.h"

template <typename T>
class ConstLinkedListIterator;

template <typename T>
class MutableLinkedListIterator;

// A container class for a singly-linked list
template <typename T>
class LinkedList
{
public:
    // Default constructor
    LinkedList() = default;

    // Destructor
    ~LinkedList();

    // Copy constructor
    LinkedList(const LinkedList<T>& original);

    // Copy assignment op
    LinkedList<T>& operator= (const LinkedList<T>& original);

    // Move constructor
    LinkedList(LinkedList<T>&& original);

    // Move assignment op
    LinkedList<T>& operator= (LinkedList<T>&& original);

    // Clear list without destroying container
    void clear();

    // Add node to the beginning of the list
    void addFirst(T value);

    // Add node to the end of the list
    void addLast(T value);

    // Remove node from the beginning of the list
    void removeFirst();

    // Get element at the beginning of the list
    const T& getFirst() const;

    // Get element at the beginning of the list
    T& getFirst();

    // Get element at the end of the list
    const T& getLast() const;

    // Get element at the end of the list
    T& getLast();

    // Get number of nodes in the list
    unsigned int getSize() const;

    // Start of forward iterator
    ConstLinkedListIterator<T> begin() const;

    // End of forward iterator
    ConstLinkedListIterator<T> end() const;

    template <typename T2>
    friend class MutableLinkedListIterator;

    // Start of forward mutable iterator
    MutableLinkedListIterator<T> begin();

    // End of forward mutable iterator
    MutableLinkedListIterator<T> end();


private:
    // Pointer to first node in the list
    ListNode<T>* first{ nullptr };

    // Pointer to last node in the list
    ListNode<T>* last{ nullptr };

    // Number of nodes in the list
    unsigned int size{ 0 };
};

template<typename T>
LinkedList<T>::~LinkedList()
{
    clear();
}

template<typename T>
LinkedList<T>::LinkedList(const LinkedList<T>& original)
{
    ListNode<T>* newNode{ original.first };





    while (newNode != nullptr) {

        this->addLast(newNode->value);

        newNode = newNode->next;

    }

    delete newNode;
}

template<typename T>
LinkedList<T>& LinkedList<T>::operator=(const LinkedList<T>& original)
{
    if (!(first == original.first && last == original.last && size == original.size)) {
        this->clear();

        ListNode<T>* newNode{ original.first };



        while (newNode != nullptr) {

            this->addLast(newNode->value);

            newNode = newNode->next;

        }

        delete newNode;




        return *this;
    }
}

template<typename T>
LinkedList<T>::LinkedList(LinkedList<T>&& original)
    : first { original.first }, last { original.last }, size { original.size }
{
    original.first = nullptr;
    original.last = nullptr;
    original.size = 0;
}

template<typename T>
LinkedList<T>& LinkedList<T>::operator=(LinkedList<T>&& original)
{
    if (!(first == original.first && last == original.last && size == original.size)) {
        first = original.first;
        last = original.last;
        size = original.size;
        original.first = nullptr;
        original.last = nullptr;
        original.size = 0;
        return *this;
    }
}

template<typename T>
void LinkedList<T>::clear()
{
    // Keep track of the next node to delete.
    ListNode<T>* toDelete{ first };

    // Loop until toDelete == nullptr
    while (toDelete)
    {
        // Use first as temp storage
        first = toDelete->next;

        delete toDelete;

        // Advance to the next node.
        toDelete = first;
    }
    // first should now be nullptr.

    last = nullptr;
    size = 0;
}

template <typename T>
void LinkedList<T>::addFirst(T value)
{
    // Create a new node
    ListNode<T>* newNode{ new ListNode<T>() };

    // Store the new element in the new node
    newNode->value = value;

    // Link the new node to the old first node
    newNode->next = first;

    // Set the new node as the first node
    first = newNode;

    if (!last)
    {
        // If there is just one element in the 
        // list, first and last are the same.
        last = first;
    }

    size++;
}

template<typename T>
void LinkedList<T>::addLast(T value)
{
    if (size == 0) {
        ListNode<T>* newNode{ new  ListNode<T>() };
        newNode->value = value;
        first = newNode;
        last = newNode;
        size++;

    }
    else {
        ListNode<T>* newNode{ new ListNode<T>() };
        newNode->value = value;

        last->next = newNode;
        last = newNode;
        size++;

    }

}

template<typename T>
void LinkedList<T>::removeFirst()
{
    if (size == 0) {
        throw std::out_of_range("Empty list");
    }
    if (size == 1) {
        delete first;
        first = nullptr;
        last = nullptr;
        size--;
    }
    else {
        ListNode<T>* tmpNode{ first };
        first = tmpNode->next;
        delete tmpNode;
        size--;
    }
}

template <typename T>
T const& LinkedList<T>::getFirst() const
{
    if (first)
    {
        return first->value;
    }
    else
    {
        throw std::out_of_range("Empty list");
    }
}

template <typename T>
T& LinkedList<T>::getFirst()
{
    if (first)
    {
        return first->value;
    }
    else
    {
        throw std::out_of_range("Empty list");
    }
}

template <typename T>
T const& LinkedList<T>::getLast() const
{
    if (last)
    {
        return last->value;
    }
    else
    {
        throw std::out_of_range("Empty list");
    }
}

template <typename T>
T& LinkedList<T>::getLast()
{
    if (last)
    {
        return last->value;
    }
    else
    {
        throw std::out_of_range("Empty list");
    }
}

template<typename T>
unsigned int LinkedList<T>::getSize() const
{
    return size;
}

template <typename T>
std::ostream& operator << (
    std::ostream& os, const LinkedList<T>& list)
{
    if (list.getSize() == 0)
    {
        // Special case: empty list
        os << "[]";
    }
    else
    {
        // Print opening bracket and first element
        os << "[" << list.getFirst();

        auto i{ ++list.begin() }; // Start at second element
        while (i != list.end())
        {
            // Print a comma, then the next element
            os << ", " << *i;

            // Advance iterator to the next node
            i++;
        }

        // Print closing bracket
        os << "]";
    }

    return os;
}



#include "ConstLinkedListIterator.h"
#include "MutableLinkedListIterator.h"

template <typename T>
ConstLinkedListIterator<T> LinkedList<T>::begin() const
{
    return ConstLinkedListIterator<T>{first};
}

template <typename T>
ConstLinkedListIterator<T> LinkedList<T>::end() const
{
    return ConstLinkedListIterator<T>{nullptr};
}

template <typename T>
MutableLinkedListIterator<T> LinkedList<T>::begin()
{
    return MutableLinkedListIterator<T>{first, * this};
}

template <typename T>
MutableLinkedListIterator<T> LinkedList<T>::end()
{
    return MutableLinkedListIterator<T>{nullptr, * this};
}
