#pragma once
#include "LinkedList.h"

template <typename T>
class LinkedSet
{
public:
	// Checks if the set contains a particular item.
	// Returns true if the item is found; false otherwise.
	bool contains(const T& item) const;

	// Add an item to the set if it doesn't already exist.
	// Return true if an item was added; false otherwise.
	bool add(const T& item);

	// Remove an item from the set.
	// Return true if an item was removed; false otherwise.
	bool remove(const T& item);

	// Remove all items from the set.
	void clear();

	// Get the number of elements in the set.
	unsigned int getSize() const;

	// Create an iterator that starts at the beginning of the set.
	ConstLinkedListIterator<T> begin() const;

	// Create an iterator that has reached the end of the set.
	ConstLinkedListIterator<T> end() const;

	// Create an iterator that starts at the beginning of the set.
	MutableLinkedListIterator<T> begin();

	// Create an iterator that has reached the end of the set.
	MutableLinkedListIterator<T> end();

	template <typename T2>
	friend std::ostream& operator << (std::ostream& out, const LinkedSet<T2>& orderedList);

private:
	// The underlying linked list.
	LinkedList<T> list;
};

template<typename T>
bool LinkedSet<T>::contains(const T& item) const
{
	//loop to traverse list
	for (ConstLinkedListIterator<T> i{ list.begin() }; i != list.end(); i++) {
		//if the item matches the element at certain point return true
		if (*i == item) {
			return true;
		}
	}
	return false;
}

template<typename T>
bool LinkedSet<T>::add(const T& item)
{
	//check to see if item passed in is in the list and add it if it isnt
	if (!(this->contains(item))) {

		//make sure the list isnt empty and add the item if it is.
		if (list.getSize() == 0) {
			list.addFirst(item);
			return true;
		}

		//case where item being added is the smallest in the list
		if (list.getFirst() > item) {
			list.addFirst(item);
			return true;
		}

		//loop to traverse the rest of the list
		for (MutableLinkedListIterator<T> i{ list.begin() }; i != list.end(); i++) {
			//check to see if iterator is at the end of the list
			if (i.hasNext()) {
				//next item is bigger so item is inserted after current
				if (i.peekNext() > item) {
					i.addNext(item);
					return true;
				}
			}
			//iterator is at end of list
			else {
				i.addNext(item);
				return true;
			}
		}
	}
	//item was found in list
	else {
		return false;
	}
}

	template<typename T>
	bool LinkedSet<T>::remove(const T & item)
	{
		//for loop to traverse list
		for (MutableLinkedListIterator<T> i{ list.begin() }; i != list.end(); i++) {
			//if item macthes first element remove first and return true
			if (*i == item && *i == this->list.getFirst()) {
				this->list.removeFirst();
				return true;
			}
			//check if there is an element after current and remove it if it equals item
			if (i.hasNext() && i.peekNext() == item) {
				i.removeNext();
				return true;
			}
		}
		return false;
	}

	template<typename T>
	void LinkedSet<T>::clear()
	{
		list.clear();
	}

	template<typename T>
	unsigned int LinkedSet<T>::getSize() const
	{
		return list.getSize();
	}

	template<typename T>
	ConstLinkedListIterator<T> LinkedSet<T>::begin() const
	{
		return list.begin();
	}

	template<typename T>
	ConstLinkedListIterator<T> LinkedSet<T>::end() const
	{
		return list.end();
	}

	template<typename T>
	MutableLinkedListIterator<T> LinkedSet<T>::begin()
	{
		return list.begin();
	}

	template<typename T>
	MutableLinkedListIterator<T> LinkedSet<T>::end()
	{
		return list.end();
	}


	template <typename T>
	std::ostream& operator << (std::ostream & out, const LinkedSet<T> & set)
	{
		out << set.list;
		return out;
	}
