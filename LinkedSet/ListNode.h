#pragma once

// A struct for representing a single node of a singly-linked list.
template <typename T>
struct ListNode
{
public:
    // The data stored in the node.
    T value;

    // A pointer to the next node in the list.
    ListNode<T>* next{ nullptr };
};
