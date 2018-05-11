#include "FreeList.h"



FreeList::FreeList()
{
	
}




FreeList::~FreeList()
{
}


void FreeList::clear()
{
	Block_Address_Structure temp{ 0 };
	DLList::setStart();
	do
	{
		insertItem(temp);
	
	} while (DLList::next());

	int n;

}