#pragma once

//strucut for the especial information block
struct Information_Block_Structure
{
	//keepts the first block of free list address
	unsigned short int firstBlockOfFreeList = 0;
	//keeps the first block of the inode list address
	unsigned short int firstBlockOfInodeList = 0;

};