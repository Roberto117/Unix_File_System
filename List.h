#ifndef List_h
#define List_h
#pragma pack(4)
#include "DLList.hpp"
// sub class for keeping list of address
template<class Elem>
class List : public DLList<Elem>
{
public:
	//initialize the lsit

	List();
	//get the size of list in bytes
	int sizeOfList();
	//insert an item into the fence->next
	bool insertItem(Elem &);
	

	~List();

};



template <class Elem>
List<Elem>::List()
{
	//get the size of a single item
	int bytesPerItem = sizeof(Elem);
	//figure out the max ammount of item that can be on the list until it reaches the size of a block of data
	int itemsPerBlock = 1024 / bytesPerItem;
	Elem temp;
	//store an empty item in to the end of the list till we reach the size fo a block of data
	for (int i = 0; i < itemsPerBlock; i++)
	{
		DLList<Elem>::append(temp);
	}
}
template <class Elem>
List<Elem>::~List() {

}

//get the total size of the list in bytes
template <class Elem>
int List<Elem>::sizeOfList()
{
	//set the list to the start
	DLList<Elem>::setStart();
	
	Elem temp;
	int size = 0;
	//get the size of each item stored in the list and add up their size in bytes
	for (int i = 0; i < DLList<Elem>::totalLenght() ; i++)
	{
		DLList<Elem>::getValue(temp);

		size += sizeof(temp);
	}

	return size;
}

template <class Elem>
bool List<Elem>::insertItem(Elem & item)
{
	//insert an item into the fence->next
	Elem temp;

	//remove item
	if (!DLList<Elem>::remove(temp)) 
		return false;
	//insert item
	DLList<Elem>::insert(item);
	return true;
}

#endif
