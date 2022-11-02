#pragma once
#include "ListNode.h"

// Forward iterator for a linked list
template <typename T>
class ConstLinkedListIterator
{
public:
    // Construct from a starting node
    ConstLinkedListIterator(const ListNode<T>* start);

    // Pre-increment operator (++i):
    // Advances iterator to the next node.
    ConstLinkedListIterator<T>& operator ++ ();

    // Post-increment operator (i++):
    // Advances iterator to the next node.
    void operator ++ (int);

    // Equality operator; checks if iterators are at the same node.
    bool operator == (const ConstLinkedListIterator<T>& other) const;

    // Inequality operator; checks if iterators are not at the same node.
    bool operator != (const ConstLinkedListIterator<T>& other) const;

    // Dereference to access value at the current node.
    const T& operator * () const;

    // Dereference to access value at the current node.
    const T* operator -> () const;

private:
    // The node the iterator is currently visiting.
    const ListNode<T>* current;
};

template <typename T>
ConstLinkedListIterator<T>::ConstLinkedListIterator(const ListNode<T>* start)
    : current{ start }
{
}

template <typename T>
ConstLinkedListIterator<T>& ConstLinkedListIterator<T>::operator ++ ()
{
    // Advance to the next node.
    current = current->next;

    // Pre-increment operator (++i) can be easily chained.
    return *this;
}

template <typename T>
void ConstLinkedListIterator<T>::operator ++ (int)
{
    // Advance to the next node.
    current = current->next;

    // Chain assignment disabled for pre-increment.
}

template <typename T>
bool ConstLinkedListIterator<T>::operator == (const ConstLinkedListIterator<T>& other) const
{
    return this->current == other.current;
}

template <typename T>
bool ConstLinkedListIterator<T>::operator != (const ConstLinkedListIterator<T>& other) const
{
    return !(*this == other);
}

template <typename T>
const T& ConstLinkedListIterator<T>::operator * () const
{
    return current->value;
}

template <typename T>
const T* ConstLinkedListIterator<T>::operator -> () const
{
    return &(current->value);
}
