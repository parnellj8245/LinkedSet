#pragma once
#include "ListNode.h"

// Forward mutable iterator for a linked list
template <typename T>
class MutableLinkedListIterator
{
public:
    // Construct from a starting node and a reference to the list.
    MutableLinkedListIterator(ListNode<T>* start, LinkedList<T>& list);

    // Pre-increment operator (++i):
    // Advances iterator to the next node.
    MutableLinkedListIterator<T>& operator ++ ();

    // Post-increment operator (i++):
    // Advances iterator to the next node.
    void operator ++ (int);

    // Equality operator; checks if iterators are at the same node.
    bool operator == (const MutableLinkedListIterator<T>& other) const;

    // Inequality operator; checks if iterators are not at the same node.
    bool operator != (const MutableLinkedListIterator<T>& other) const;

    // Dereference to access value at the current node.
    T& operator * ();

    // Dereference to access value at the current node.
    T* operator -> ();

    // Is there another node after the current one?
    bool hasNext() const;

    // Get the value at the node after the current one.
    T& peekNext();

    // Add a node after the current one.
    void addNext(T value);

    // Remove the node after the current one.
    void removeNext();


private:
    // The node the iterator is currently visiting.
    ListNode<T>* current;

    // The list that is being iterated (and potentially modified).
    LinkedList<T>& list;
};

template <typename T>
MutableLinkedListIterator<T>::MutableLinkedListIterator(ListNode<T>* start, LinkedList<T>& list)
    : current{ start }, list{ list }
{
}

template <typename T>
MutableLinkedListIterator<T>& MutableLinkedListIterator<T>::operator ++ ()
{
    // Advance to the next node.
    current = current->next;

    // Pre-increment operator (++i) can be easily chained.
    return *this;
}

template <typename T>
void MutableLinkedListIterator<T>::operator ++ (int)
{
    // Advance to the next node.
    current = current->next;

    // Chain assignment disabled for pre-increment.
}

template <typename T>
bool MutableLinkedListIterator<T>::operator == (const MutableLinkedListIterator<T>& other) const
{
    return this->current == other.current;
}

template <typename T>
bool MutableLinkedListIterator<T>::operator != (const MutableLinkedListIterator<T>& other) const
{
    return !(*this == other);
}

template <typename T>
T& MutableLinkedListIterator<T>::operator * ()
{
    return current->value;
}

template <typename T>
T* MutableLinkedListIterator<T>::operator -> ()
{
    return &(current->value);
}

template<class T>
T& MutableLinkedListIterator<T>::peekNext()
{
    ListNode<T>* next = current->next;

    // Prevent null pointer access when trying to peek past the end of the list.
    if (hasNext())
    {
        return next->value;
    }
    else
    {
        throw std::logic_error(
            "No item to peek at");
    }
}

template<class T>
void MutableLinkedListIterator<T>::addNext(T value)
{
    // Create a new node
    ListNode<T>* newNode{ new ListNode<T>() };

    // Store the new element in the new node
    newNode->value = value;

    
    //empty list
    if (list.getSize() == 0) {
        list.first = newNode;
        list.last = newNode;
    }
    //List has a single element
    if (list.getSize() == 1) {
        current->next = newNode;
        list.last = newNode;
    }
    else{
        //function called on last element in list
        if (current->next == nullptr) {
            current->next = newNode;
            list.last = newNode;
        }
        //function called on element with element(s) after
        else {
            newNode->next = current->next;
            current->next = newNode;   
        }
    }
    //increment size
    this->list.size++;
}


template<class T>
bool MutableLinkedListIterator<T>::hasNext()
const
{
    // Check that neither this node nor the next are nullptr.
    return current != nullptr
        && current->next != nullptr;
}

template<class T>
void MutableLinkedListIterator<T>::removeNext()
{
    // The node to be deleted.
    ListNode<T>* next = current->next;

    // Prevent null pointer access when trying to remove past the end of the list.
    if (hasNext())
    {
        // Link current to the node that comes after the node being deleted.
        current->next = next->next;

        // If the node to be deleted is the last node, the current node becomes the last node.
        if (list.last == next)
        {
            list.last = current;
        }

        delete next;
        list.size--;
    }
    else
    {
        // Thrown if the list is empty.
        throw std::logic_error(
            "No item to remove");
    }
}
