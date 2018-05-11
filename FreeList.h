#ifndef FreeList_h
#define FreeList_h
#include "List.h"
#include "Link.h"
#include "Block_Address_Structure.h"

// free list class to store inode list or free blocks
class FreeList: public List<Block_Address_Structure>
{
public:
	FreeList();

	//clear all items from the list
	void clear();

	
	~FreeList();

	
private:
	unsigned int numberOfAddressInBlock;


	
};


#endif