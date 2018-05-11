#include "Disk.h"
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;
//initiate the disk
Disk::Disk() {

	//creat the disl.bin file if it does not already exist
	disk = new fstream("disk.bin", ios::out | ios::in | ios::binary | ios::app);

	if (!disk)
	{
		cerr << "File could not be opened ";
		exit(EXIT_FAILURE);
	}
	//initate the list in memory
	freeList = new FreeList;
	inodeFreeList = new FreeList;


	disk->close();
	//make the disk 16mb if it isn't already
	createDiskFile();
	//read the information block from disk
	readFromDisk(infomationBlockAddress);
	readFromBlock(reinterpret_cast<char*>(&infoBlock), sizeof(Information_Block_Structure), 0);

	//get the first free block from disk
	getFirstFreeInodeAddress();
	currentDir = 0;


}


//delete the disk
Disk::~Disk()
{

	delete disk;
	delete freeList;
}

void Disk::reset()
{

	cout << "total Memory size \t\t" << getTotalSize() << endl;
	cout << "Inode size \t\t\t" << getInodeSize() << endl;
	cout << "Inode addres size \t\t" << sizeof(Inode_Address_Structure) << endl;
	cout << "Inode addres size \t\t" << sizeof(Block_Address_Structure) << endl;
	cout << "size of one block of the freeList  " << getSizeOfFreeList() << endl;


	cout << "Free List blocks" << endl;
	createFreeList();

	cout << "Inode List blocks" << endl;
	createInodeList();

	cout << "free List first block address" << endl;

	getFreeListFirstBlockAddress();

	cout << "size of one block of the freeList  " << getSizeOfFreeList() << endl;

	cout << endl << endl;
	cout << "free List Blocks after Inode took blocks" << endl;
	printCurrentFreeList();

	cout << "contes of first inode List " << endl;
	printCurrentInodeList();
	cout << "size of one block of the freeList  " << getSizeOfFreeList() << endl << endl;

	cout << "root directory" << endl;

	getfirstFreeBlock();

	getFirstFreeInodeAddress();
	char root[14] = { 'r','o','o','t' };
	createRootDir(root);
	readDir(currentDir);
}
//make the dik 16mb size
void Disk::createDiskFile() {

	disk = new fstream("disk.bin", ios::out | ios::in | ios::binary | ios::app);

	if (!disk)
	{
		cerr << "File could not be opened ";
		exit(EXIT_FAILURE);
	}
	//seek the end of file
	disk->seekg(0, ios::end);
	int lenght = disk->tellg();
	//if lenght is 0 then the file is empty so make it 16mb
	if (lenght == 0)
	{
		for (int i = 0; i < (TOTAL_SIZE / BLOCK_SIZE); i++) {

			disk->write(BLOCK_SIZE_BUFFER, BLOCK_SIZE);

		}

		reset();

	}

	disk->close();

}

//get the size of a single block of data
int Disk::getBlockSize()
{
	return BLOCK_SIZE;
}
//get the size of the disk in bytes
int Disk::getTotalSize()
{
	disk = new fstream("disk.bin", ios::in | ios::out | ios::binary);

	if (!disk)
	{
		cerr << "File could not be opened ";
		exit(EXIT_FAILURE);
	}
	//go to the end of the file
	disk->seekg(0, ios::end);
	//get the lenght of the disk
	int lenght = disk->tellg();

	disk->seekg(0, ios::beg);

	disk->close();
	//return the lenght
	return lenght;



}
//get the size of an inode
int Disk::getInodeSize()
{
	disk = new fstream("disk.bin", ios::in | ios::binary);

	if (!disk)
	{
		cerr << "File could not be opened ";
		exit(EXIT_FAILURE);
	}
	disk->seekg(0);
	//go to where the end of the first inode should be
	disk->seekg(sizeof(Inode_Structure)); ;



	int lenght = disk->tellg();

	disk->seekg(0);
	disk->close();
	//return the size
	return lenght;
}


//write an inode to disk
bool Disk::writeInode(int address, Inode_Structure inode)
{
	//check if ther addes is outside of the inode area
	if (address >= NUMBER_OF_INODES || address < 0) return false;
	//check if the inode is the correct size
	int lenght = sizeof(Inode_Structure);

	if (lenght != 64) return false;
	//get the block where inode should be at 
	int blockAddress = (sizeof(Inode_Structure) * address) / BLOCK_SIZE;
	//read the block from disk
	readFromDisk(blockAddress);
	//write the data to the block in memory

	unsigned short int offset = (address * sizeof(Inode_Structure) % BLOCK_SIZE);
	writeToBlock(reinterpret_cast<char*>(&inode), sizeof(Inode_Structure), offset);
	//write the block to disk

	writeToDisk(blockAddress);


	return true;

}
//read an inode from disk
bool Disk::readInode(int address, Inode_Structure &inode)
{
	//check that the address is not out of scope
	if (address >= NUMBER_OF_INODES || address < 0) return false;
	//get where the block from inode should be
	int blockAddress = (sizeof(Inode_Structure) * address) / BLOCK_SIZE;

	//read the block from disk
	readFromDisk(blockAddress);
	//read the block to memory

	unsigned short int offset = (address * sizeof(Inode_Structure) % BLOCK_SIZE);
	readFromBlock(reinterpret_cast<char*>(&inode), sizeof(Inode_Structure), offset);


	return true;
}


void Disk::createRootDir(char name[14])
{

	Inode_Address_Structure temp;
	Block_Address_Structure address;
	Block_Address_Structure blockOfData;
	time_t now;
	//copy the name to the structure

	for (int i = 0; i < 14; i++) temp.name[i] = name[i];

	//set the address
	temp.address = 0;
	//get the first block of data

	getFreeBlockAddress(blockOfData);

	Inode_Structure inode;
	//set the default size of the inode structure
	inode.file_size = (sizeof(Inode_Address_Structure) * 2),
		inode.access_time = inode.modificacion_time = inode.creation_time = time(&now);
	//set the name of the owner
	for (int i = 0; i < 14; i++) inode.owner[i] = name[i];
	//set the protection and count data
	inode.protection = 0;
	inode.count = 1;
	//set the bock address structure
	inode.direct_address0 = blockOfData.address;
	inode.direct_address1 = 0;
	inode.direct_address2 = 0;
	inode.direct_address3 = 0;
	inode.direct_address4 = 0;
	inode.direct_address5 = 0;
	inode.direct_address6 = 0;
	inode.direct_address7 = 0;
	inode.direct_address8 = 0;
	inode.direct_address9 = 0;
	inode.indirect_address = 0;
	//try to  the inode to disk
	if (!writeInode(temp.address, inode)) cout << "could not write to inode" << endl;


	//try to write the first inode dirs to disk
	Inode_Address_Structure firstDir;
	firstDir.name[0] = '.';
	firstDir.address = temp.address;



	if (!writeInodeData(blockOfData.address, firstDir)) cout << "could not write to Inode Data" << endl;

	Inode_Address_Structure secondDir;
	secondDir.name[0] = '.';
	secondDir.name[1] = '.';
	secondDir.address = temp.address;

	if (!(writeInodeData(blockOfData.address, secondDir, sizeof(Inode_Address_Structure)))) cout << "could not write to Inode Daata" << endl;

	Inode_Address_Structure nullDir;
	nullDir.name[0] = '@';
	nullDir.address = 0;

	for (int i = 2; i < BLOCK_SIZE / sizeof(Inode_Address_Structure); i++)
	{
		if (!(writeInodeData(blockOfData.address, nullDir, sizeof(Inode_Address_Structure) * i))) cout << "could not write to Inode Daata" << endl;
	}

}

//creat a dir
bool Disk::createDir(char name[14])
{
	Inode_Address_Structure temp;

	Block_Address_Structure address;
	Block_Address_Structure blockOfData;
	time_t now;
	//copy the name to the structure

	for (int i = 0; i < 14; i++) temp.name[i] = name[i];

	getInodeAddress(temp);
	//get the first block of data

	getFreeBlockAddress(blockOfData);

	Inode_Structure inode;
	//set the default size of the inode structure
	inode.file_size = (sizeof(Inode_Address_Structure) * 2),
		inode.access_time = inode.modificacion_time = inode.creation_time = time(&now);
	//set the name of the owner
	for (int i = 0; i < 14; i++) inode.owner[i] = name[i];
	//set the protection and count data
	inode.protection = 0;
	inode.count = 1;
	//set the bock address structure
	inode.direct_address0 = blockOfData.address;
	inode.direct_address1 = 0;
	inode.direct_address2 = 0;
	inode.direct_address3 = 0;
	inode.direct_address4 = 0;
	inode.direct_address5 = 0;
	inode.direct_address6 = 0;
	inode.direct_address7 = 0;
	inode.direct_address8 = 0;
	inode.direct_address9 = 0;
	inode.indirect_address = 0;
	//try to write the inode to disk
	//append the directory addres to cuurent inode
	appendToCurrentDir(temp);

	if (!writeInode(temp.address, inode)) cout << "could not write to inode" << endl;


	//try to write the first inode dirs to disk
	Inode_Address_Structure firstDir;
	firstDir.name[0] = '.';
	firstDir.address = temp.address;



	if (!writeInodeData(blockOfData.address, firstDir)) cout << "could not write to Inode Data" << endl;

	Inode_Address_Structure secondDir;
	secondDir.name[0] = '.';
	secondDir.name[1] = '.';
	secondDir.address = currentDir;

	if (!(writeInodeData(blockOfData.address, secondDir, sizeof(Inode_Address_Structure)))) cout << "could not write to Inode Daata" << endl;

	Inode_Address_Structure nullDir;
	nullDir.name[0] = '@';
	nullDir.address = 0;

	for (int i = 2; i < BLOCK_SIZE / sizeof(Inode_Address_Structure); i++)
	{
		if (!(writeInodeData(blockOfData.address, nullDir, sizeof(Inode_Address_Structure) * i))) cout << "could not write to Inode Daata" << endl;
	}


	return true;
}


void Disk::appendToCurrentDir(Inode_Address_Structure &dir)
{
	bool newBlock = false;
	bool emptyDir = false;
	bool indirectAccess = false;
	Inode_Structure inode;
	Inode_Address_Structure temp;
	Block_Address_Structure freeBlock{ 0 };
	time_t now;
	unsigned short blockNotFull;
	unsigned short int address;
	readInode(currentDir, inode);

	if (inode.file_size >= 522 * BLOCK_SIZE) return;
	blockNotFull = inode.file_size / BLOCK_SIZE;

	if (emptyDir = checkIfThereIsAnEmptyDir(blockNotFull, inode));



	if (inode.file_size % BLOCK_SIZE == 0 && !emptyDir)
	{
		newBlock = true;

		getFreeBlockAddress(freeBlock);

		if (blockNotFull == 10) fillBlockWithNullBlocks(freeBlock.address);

		else fillBlockWithEmptyDirs(freeBlock.address);
	}

	inode.file_size = inode.file_size + sizeof(Inode_Address_Structure);

	inode.access_time = inode.modificacion_time = time(&now);

	switch (blockNotFull)
	{
	case 0:

		address = inode.direct_address0;
		break;
	case 1:
		if (newBlock) inode.direct_address1 = freeBlock.address;

		address = inode.direct_address1;
		break;
	case 2:
		if (newBlock) inode.direct_address2 = freeBlock.address;
		address = inode.direct_address2;
		break;
	case 3:
		if (newBlock) inode.direct_address3 = freeBlock.address;
		address = inode.direct_address3;
		break;
	case 4:
		if (newBlock) inode.direct_address4 = freeBlock.address;
		address = inode.direct_address4;
		break;
	case 5:
		if (newBlock) inode.direct_address5 = freeBlock.address;
		address = inode.direct_address5;
		break;
	case 6:
		if (newBlock) inode.direct_address6 = freeBlock.address;
		address = inode.direct_address6;
		break;
	case 7:
		if (newBlock) inode.direct_address7 = freeBlock.address;
		address = inode.direct_address7;
		break;
	case 8:
		if (newBlock) inode.direct_address8 = freeBlock.address;
		address = inode.direct_address8;
		break;
	case 9:
		if (newBlock) inode.direct_address9 = freeBlock.address;
		address = inode.direct_address9;
		break;
	default:
		if (newBlock  && inode.indirect_address == 0) inode.indirect_address = freeBlock.address;
		address = inode.indirect_address;
		indirectAccess = true;
		break;


	}


	if (indirectAccess)
	{
		Block_Address_Structure temp;
		readFromDisk(address);

		readFromBlock(reinterpret_cast<char*>(&temp), sizeof(Block_Address_Structure), sizeof(Block_Address_Structure)*(blockNotFull - 10));
		if (newBlock)
		{
			if (blockNotFull == 10)
			{
				getFreeBlockAddress(temp);
				fillBlockWithEmptyDirs(temp.address);
			}

			else temp.address = freeBlock.address;

			readFromDisk(address);
			writeToBlock(reinterpret_cast<char*>(&temp), sizeof(Block_Address_Structure), sizeof(Block_Address_Structure) *(blockNotFull - 10));
			writeToDisk(address);

		}

		address = temp.address;
	}

	readFromDisk(address);
	int i = 0;
	for (i; i < BLOCK_SIZE / sizeof(Inode_Address_Structure); i++)
	{
		readFromBlock(reinterpret_cast<char*>(&temp), sizeof(Inode_Address_Structure), sizeof(Inode_Address_Structure)*i);
		if (temp.name[0] == '@') break;


	}



	writeInode(currentDir, inode);
	writeInodeData(address, dir, sizeof(Inode_Address_Structure)* i);

}
//write inode dirs
bool Disk::writeInodeData(int address, Inode_Address_Structure inodeAddress, int offset)
{
	//check if out of scope
	if (address < INODE_SPACE || address > TOTAL_BLOCK_SIZE) return false;
	//read the address given from disk
	readFromDisk(address);
	//write the data to memory and then to disk
	writeToBlock(reinterpret_cast<char*>(&inodeAddress), sizeof(Inode_Address_Structure), offset);
	writeToDisk(address);

	return true;
}

//read a dir from the inode space
bool Disk::readDir(int address)
{
	if (address >= INODE_SPACE || address < 0) return false;

	Inode_Structure temp;
	//get the inode from  disk
	readInode(address, temp);



	cout << endl;
	cout << "file size \t" << temp.file_size << endl;
	cout << "creation time \t" << temp.creation_time << endl;
	cout << "last acessed \t" << temp.access_time << endl;
	cout << "last modified\t" << temp.modificacion_time << endl;
	cout << "owner \t\t" << temp.owner << endl;
	cout << "protection \t" << (int)(temp.protection) << endl;
	cout << "count \t\t" << (int)(temp.count) << endl;
	cout << "address 0 \t" << temp.direct_address0 << endl;
	cout << "address 1 \t" << temp.direct_address1 << endl;
	cout << "address 2 \t" << temp.direct_address2 << endl;
	cout << "address 3 \t" << temp.direct_address3 << endl;
	cout << "address 4 \t" << temp.direct_address4 << endl;
	cout << "address 5 \t" << temp.direct_address5 << endl;
	cout << "address 6 \t" << temp.direct_address6 << endl;
	cout << "address 7 \t" << temp.direct_address7 << endl;
	cout << "address 8 \t" << temp.direct_address8 << endl;
	cout << "address 9 \t" << temp.direct_address9 << endl;
	cout << "indirect  \t" << temp.indirect_address << endl;
	//get the inode data

	if (temp.direct_address0 != 0) readInodeAddresses(temp.direct_address0);
	if (temp.direct_address1 != 0) readInodeAddresses(temp.direct_address1);
	if (temp.direct_address2 != 0) readInodeAddresses(temp.direct_address2);
	if (temp.direct_address3 != 0) readInodeAddresses(temp.direct_address3);
	if (temp.direct_address4 != 0) readInodeAddresses(temp.direct_address4);
	if (temp.direct_address5 != 0) readInodeAddresses(temp.direct_address5);
	if (temp.direct_address6 != 0) readInodeAddresses(temp.direct_address6);
	if (temp.direct_address7 != 0) readInodeAddresses(temp.direct_address7);
	if (temp.direct_address8 != 0) readInodeAddresses(temp.direct_address8);
	if (temp.direct_address8 != 0) readInodeAddresses(temp.direct_address9);
	if (temp.indirect_address != 0)
	{
		for (int i = 0; i < BLOCK_SIZE / sizeof(Block_Address_Structure); i++)
		{
			Block_Address_Structure blockOfDirs;
			readFromDisk(temp.indirect_address);

			readFromBlock(reinterpret_cast<char *>(&blockOfDirs), sizeof(Block_Address_Structure), sizeof(Block_Address_Structure) * i);

			if (blockOfDirs.address != 0)readInodeAddresses(blockOfDirs.address);
		}


	}
	return true;


}
//read addresses store in an inode dir
void Disk::readInodeAddresses(int address)
{

	Inode_Address_Structure dir;
	//get the block from disk
	readFromDisk(address);
	//read the block in memeory

	for (int i = 0; i < BLOCK_SIZE / sizeof(Inode_Address_Structure); i++)
	{
		readFromBlock(reinterpret_cast<char*>(&dir), sizeof(Inode_Address_Structure), sizeof(Inode_Address_Structure)*i);
		if (dir.name[0] == '@') continue;



		cout << dir.address << "\t" << dir.name << endl;
	}

	//output it 




}


//read a block from disk to memory
bool Disk::readFromDisk(int address)
{

	if (address < 0 || address >= TOTAL_BLOCK_SIZE) return false;

	disk = new fstream("disk.bin", ios::in | ios::out | ios::binary);

	if (!disk)
	{
		cerr << "File could not be opened ";
		exit(EXIT_FAILURE);
	}
	//get the address in disk
	disk->seekg(address * BLOCK_SIZE);
	int location = disk->tellg();
	//read the block from disk 
	disk->read(block, BLOCK_SIZE);

	disk->close();

	return true;

}


//write a block from meory to disk
bool Disk::writeToDisk(int address)
{
	if (address < 0 || address >= TOTAL_BLOCK_SIZE) return false;

	disk = new fstream("disk.bin", ios::in | ios::out | ios::binary);

	if (!disk)
	{
		cerr << "File could not be opened ";
		exit(EXIT_FAILURE);
	}
	//get the address on disk
	disk->seekp(address * BLOCK_SIZE);

	int location = disk->tellp();
	//write to disk 
	disk->write(block, BLOCK_SIZE);

	disk->close();

	return true;

}
//write to memory block
bool Disk::writeToBlock(char * data, int size, int offset)
{
	//make sure data given is not bigger than the block
	if (size + offset > BLOCK_SIZE) return false;
	int j = 0;
	//copy the data to the block
	for (int i = offset; i < size + offset; i++)
	{
		block[i] = data[j];
		j++;
	}
	return true;
}

//read from the block in memory 
bool Disk::readFromBlock(char *data, int size, int offset)
{

	//make sure the data given is not bigger than block
	if (size + offset > BLOCK_SIZE) return false;
	int j = 0;
	//write to data from block start from the offset
	for (int i = offset; i < size + offset; i++)
	{
		data[j] = block[i];
		j++;
	}

	return true;
}
//write a free list block to disk
bool Disk::writeFreeListToDisk(int address)
{
	//check is not out of scope

	if (address < INODE_SPACE || address >= TOTAL_BLOCK_SIZE) return false;
	freeList->setStart();
	Block_Address_Structure temp;
	//get the block from disk

	int i = 0;
	do
	{
		//if freelist fence->next still has a value outout it to memory
		if (freeList->getValue(temp))
		{
			writeToBlock(reinterpret_cast<char*>(&temp), sizeof(Block_Address_Structure), sizeof(Block_Address_Structure)*i);
		}
		i++;
	} while (freeList->next()); //do until we reach the end of the list
								//write to disk
	writeToDisk(address);

	getfirstFreeBlock();

	return true;
}
//get the address of the first free block
void Disk::getFreeListFirstBlockAddress()
{
	cout << infoBlock.firstBlockOfFreeList << endl;

}

//get the size of the free list
int Disk::getSizeOfFreeList()
{
	//return the size
	return freeList->sizeOfList();
}

//read a freee list block from disk
bool Disk::readFreeListFromDisk(int address)
{
	//check is not out of scope
	if (address < INODE_SPACE || address >= TOTAL_BLOCK_SIZE) return false;

	//read the disk to memory

	readFromDisk(address);

	Block_Address_Structure temp;
	//set the freelist at the beggining 
	freeList->setStart();
	for (int i = 0; i < BLOCK_SIZE / sizeof(Block_Address_Structure); i++)
	{
		//read till the block reacches the end 
		readFromBlock(reinterpret_cast<char*>(&temp), sizeof(Block_Address_Structure), sizeof(Block_Address_Structure) * i);
		freeList->insertItem(temp); //insert the item
		if (!freeList->next()) break;
	}


	return true;

}
//print the free list of blocks in a given address
void Disk::printFreeListBlock(int address)
{
	//read the block disk to memory
	readFreeListFromDisk(address);
	freeList->setStart();
	Block_Address_Structure temp;
	int i = 0;
	//read the list to console
	while (freeList->getValue(temp))
	{


		cout << temp.address << endl;
		i++;

		freeList->next();
	}

}


//creat the list of inodes at reset
void Disk::createInodeList()
{
	Block_Address_Structure block;
	//get a free block from the list
	getFreeBlockAddress(block);

	unsigned short int currentInodeListBlockAddress = block.address;
	//get the the addres for the first block of inodes
	infoBlock.firstBlockOfInodeList = currentInodeListBlockAddress;
	//inode free list set to start
	inodeFreeList->setStart();

	volatile int i;



	for (i = 1; i < NUMBER_OF_INODES; ++i)
	{


		//append the inode to list
		block.address = i;
		inodeFreeList->insertItem(block);


		//if list is full write it to block
		if (!inodeFreeList->next())
		{
			//move back one Link to save the pointer to the next block of addresses
			inodeFreeList->prev();
			Block_Address_Structure nextBlock;
			//get the next free block of pointers
			getFreeBlockAddress(nextBlock);
			cout << currentInodeListBlockAddress << endl;

			//insert it to the end of the list
			inodeFreeList->insertItem(nextBlock);
			//write it to disk
			writeInodeListToDisk(currentInodeListBlockAddress);
			//move to the current block of address being used
			currentInodeListBlockAddress = nextBlock.address;
			i = i - 2; //move back a bit to store the address that was deleted

					   //clear the list and set it to the beggning 
			inodeFreeList->clear();
			inodeFreeList->setStart();
		}

	}

	//write the last block of inodes to disk
	readFromDisk(currentInodeListBlockAddress);
	writeInodeListToDisk(currentInodeListBlockAddress);
	//update the information block on disk

	updateInfoBlock();

	getFirstFreeInodeAddress();

}
//get next free block in free list
void Disk::getFreeBlockAddress(Block_Address_Structure & address)
{

	Block_Address_Structure temp{ 0 };
	//get the value from thje free list
	//get the pointer to the first free avaible block in memory
	getfirstFreeBlock();
	//check if we have not reached the end of the list if not grab the value from the list
	if (freeList->checkIfNextAvaiable())
	{
		freeList->getValue(address);
	}

	else
	{
		//if we are in the last Link in the list then it is a pointer to the next free block in the list
		//we get the pointer from the list and store it temporarly in the address node
		freeList->getValue(address);
		//update the info block in memory to the new start of the free list and store the used up block
		temp.address = infoBlock.firstBlockOfFreeList;
		infoBlock.firstBlockOfFreeList = address.address;


		//read the free list from disk
		readFreeListFromDisk(infoBlock.firstBlockOfFreeList);
		//get the first pointer to an address
		getfirstFreeBlock();
		//update the info block in disk with the one in memory
		updateInfoBlock();
		//get the first avaible free block address
		freeList->getValue(address);




	}
	//if the list is not empty we add a 0 value to the free list to remove the address
	//if the list was empty and we move to the next list we add the freed free list block to the top of the list
	freeList->insertItem(temp);
	freeList->next();
	//write the free list back to disk
	writeFreeListToDisk(infoBlock.firstBlockOfFreeList);
}
//this function prints all the free blocks however if you use it you have to reset the disk 
//because I used it to simulate the removal of all free list blocks 
void Disk::printAllFreeBlocks()
{
	// if the flag is true then there is still items in the list
	bool flag = true;
	//do this for every block in free list
	for (int i = infoBlock.firstBlockOfFreeList; i > 16359; i--)
	{
		flag = true;
		while (true)
		{

			Block_Address_Structure temp{ 0 };
			Block_Address_Structure address;
			//get the value from thje free list
			//check if we have not reached the end of list
			if (freeList->checkIfNextAvaiable())
			{
				freeList->getValue(address);

				//output address
				cout << address.address << endl;
			}

			else
			{
				//if we have reached the end then add the freed block to the disk and move to the next
				freeList->getValue(address);
				temp.address = infoBlock.firstBlockOfFreeList;
				infoBlock.firstBlockOfFreeList = address.address;



				readFreeListFromDisk(infoBlock.firstBlockOfFreeList);
				getfirstFreeBlock();

				updateInfoBlock();

				freeList->getValue(address);
				//outout address
				cout << address.address << endl;
				//flag that we have reached the end of the list
				flag = false;
			}

			freeList->insertItem(temp);
			freeList->next();
			if (!flag) break;
			//if we have reached the end of list there is no need to write it to disk
			writeFreeListToDisk(infoBlock.firstBlockOfFreeList);

		}

		//set the value in free list to 0 

		//write the free list to disk



	}
}

void Disk::getfirstFreeBlock()
{
	//get the first address avaiable in the block
	readFreeListFromDisk(infoBlock.firstBlockOfFreeList);

	freeList->setStart();
	Block_Address_Structure temp;

	//loop until we find a link that is not 0
	do
	{
		freeList->getValue(temp);
		if (temp.address != 0) break;

		freeList->next();
	} while (true);

	freeList->getValue(temp);
}
//write the inode list to disk
bool Disk::writeInodeListToDisk(int address)
{
	if (address < INODE_SPACE || address >= TOTAL_BLOCK_SIZE) return false;
	inodeFreeList->setStart();
	Block_Address_Structure temp;



	int i = 0;
	//write to block until we reache the end of the list
	do
	{

		if (inodeFreeList->getValue(temp))
		{
			writeToBlock(reinterpret_cast<char*>(&temp), sizeof(Block_Address_Structure), sizeof(Block_Address_Structure)*i);
		}
		i++;
	} while (inodeFreeList->next());

	writeToDisk(address);

	getFirstFreeInodeAddress();

	return true;
}

//create the free list 
void Disk::createFreeList()
{
	Inode_Structure temp;
	Block_Address_Structure block;
	//get the first block of address
	unsigned short int currentFreeListBlockAddress = infomationBlockAddress - 1;

	//save the first free block
	infoBlock.firstBlockOfFreeList = currentFreeListBlockAddress;

	freeList->setStart();

	volatile unsigned short int i;

	for (i = INODE_SPACE; i < infomationBlockAddress; ++i)
	{
		if (currentFreeListBlockAddress == i) break;




		//insert the address to list in memory
		block.address = i;
		freeList->insertItem(block);


		//if list is full store it to disk
		if (!freeList->next())
		{

			//move one link back to store the pointer to the next block of the free list
			freeList->prev();
			//get the next block of free list
			Block_Address_Structure nextBlock{ 0 };

			nextBlock.address = currentFreeListBlockAddress - 1;

			cout << currentFreeListBlockAddress << endl;
			//insert to the end of the list
			freeList->insertItem(nextBlock);
			//write the list to disk
			writeFreeListToDisk(currentFreeListBlockAddress);
			i = i - 2;
			//move to the next block address
			--currentFreeListBlockAddress;
			//clear and set the free list to the start
			freeList->clear();
			freeList->setStart();
		}

	}



	//write the last block to disk
	writeFreeListToDisk(currentFreeListBlockAddress);
	//write the information block to disk

	updateInfoBlock();





}
//print the current list in use 
void Disk::printCurrentFreeList()
{
	printFreeListBlock(infoBlock.firstBlockOfFreeList);
}
//prints the current inode list in memory
void Disk::printCurrentInodeList()
{
	printInodeListBlock(infoBlock.firstBlockOfInodeList);
}
//prints a given inode block
void Disk::printInodeListBlock(int address)
{
	//read the block disk to memory
	readInodeListFromDisk(address);
	inodeFreeList->setStart();
	Block_Address_Structure temp;
	int i = 0;
	//read the list to console
	while (inodeFreeList->getValue(temp))
	{


		cout << temp.address << endl;
		i++;

		inodeFreeList->next();
	}

}


bool Disk::readInodeListFromDisk(int address)
{
	//check is not out of scope
	if (address < INODE_SPACE || address >= TOTAL_BLOCK_SIZE) return false;

	//read the disk to memory

	readFromDisk(address);

	Block_Address_Structure temp;
	//set the freelist at the beggining 
	inodeFreeList->setStart();
	for (int i = 0; i < BLOCK_SIZE / sizeof(Block_Address_Structure); i++)
	{
		//read till the block reacches the end 
		readFromBlock(reinterpret_cast<char*>(&temp), sizeof(Block_Address_Structure), sizeof(Block_Address_Structure) * i);
		inodeFreeList->insertItem(temp); //insert the item
		if (!inodeFreeList->next()) break;
	}


	return true;

}


void Disk::getFirstFreeInodeAddress()
{
	//get the first address avaiable in the block
	readInodeListFromDisk(infoBlock.firstBlockOfInodeList);
	inodeFreeList->setStart();
	Block_Address_Structure temp;

	//loop until we find a link that is not 0
	do
	{
		inodeFreeList->getValue(temp);
		if (temp.address == 0) inodeFreeList->next();
	} while (temp.address == 0);

}

void Disk::getInodeAddress(Inode_Address_Structure &freeAddress)
{

	Block_Address_Structure temp{ 0 };
	Block_Address_Structure address;
	//get the value from thje free list
	//get the pointer to the first free avaible block in memory
	getFirstFreeInodeAddress();
	//check if we have not reached the end of the list if not grab the value from the list
	if (inodeFreeList->checkIfNextAvaiable())
	{
		inodeFreeList->getValue(address);
	}

	else
	{
		//if we are in the last Link in the list then it is a pointer to the next free block in the list
		//we get the pointer from the list and store it temporarly in the address node
		inodeFreeList->getValue(address);
		//update the info block in memory to the new start of the free list and store the used up block
		temp.address = infoBlock.firstBlockOfInodeList;
		infoBlock.firstBlockOfInodeList = address.address;


		//read the free list from disk
		readInodeListFromDisk(infoBlock.firstBlockOfInodeList);
		//get the first pointer to an address
		getFirstFreeInodeAddress();
		//update the info block in disk with the one in memory
		updateInfoBlock();
		//get the first avaible free block address
		inodeFreeList->getValue(address);




	}
	//if the list is not empty we add a 0 value to the free list to remove the address
	//if the list was empty and we move to the next list we add the freed free list block to the top of the list
	inodeFreeList->insertItem(temp);
	inodeFreeList->next();
	//write the free list back to disk
	writeInodeListToDisk(infoBlock.firstBlockOfInodeList);

	freeAddress.address = address.address;
}

//update the information block
void Disk::updateInfoBlock()
{
	//read from disk the information block
	readFromDisk(infomationBlockAddress);
	//write the new info block to block
	writeToBlock(reinterpret_cast<char*>(&infoBlock), sizeof(Information_Block_Structure), 0);
	//write it back to disk
	writeToDisk(infomationBlockAddress);

}
//read the current dir in use
void Disk::readCurrentDir()
{

	readDir(currentDir);
}
//move to a given dir
bool Disk::moveToDir(int address)
{

	if (address >= INODE_SPACE || address < 0) return false;
	currentDir = address;

	return true;
}
//find a dir by name
bool Disk::findDir(char name[14], unsigned short int &address)
{
	bool found = false;
	Inode_Structure inode;
	Inode_Address_Structure dir;
	time_t now;


	//get the inode from disk
	readInode(currentDir, inode);
	//update the last modified and last acceesed time of the inode
	inode.access_time = inode.modificacion_time = time(&now);
	//check where the dir could be and save the disk location
	if (inode.direct_address0 != 0)
	{
		readFromDisk(inode.direct_address0);

		found = foundDir(name, address);
	}
	if (inode.direct_address1 != 0 && !found) {
		readFromDisk(inode.direct_address1);
		found = foundDir(name, address);
	}
	if (inode.direct_address2 != 0 && !found) {
		readFromDisk(inode.direct_address2);
		found = foundDir(name, address);
	}
	if (inode.direct_address3 != 0 && !found) {
		readFromDisk(inode.direct_address3);

		found = foundDir(name, address);
	}
	if (inode.direct_address4 != 0 && !found) {
		readFromDisk(inode.direct_address4);

		found = foundDir(name, address);
	}
	if (inode.direct_address5 != 0 && !found) {
		readFromDisk(inode.direct_address5);
		found = foundDir(name, address);

	}
	if (inode.direct_address6 != 0 && !found) {
		readFromDisk(inode.direct_address6);
		found = foundDir(name, address);

	}
	if (inode.direct_address7 != 0 && !found) {
		readFromDisk(inode.direct_address7);
		found = foundDir(name, address);

	}
	if (inode.direct_address8 != 0 && !found) {
		readFromDisk(inode.direct_address8);
		found = foundDir(name, address);

	}
	if (inode.direct_address9 != 0 && !found) {
		readFromDisk(inode.direct_address9);
		found = foundDir(name, address);

	}
	if (!found)
	{
		Block_Address_Structure temp;
		//read every block address in the indirect block and treat every block ad a block full of directory
		for (int i = 0; i > BLOCK_SIZE / sizeof(Block_Address_Structure); i++)
		{
			readFromDisk(inode.indirect_address);
			readFromBlock(reinterpret_cast<char*>(&temp), sizeof(Block_Address_Structure), sizeof(Block_Address_Structure)* i);
			if (temp.address == 0)
			{
				found = false;
				break;
			}
			readFromDisk(temp.address);
			if (found = foundDir(name, address)) break;
		}
	}

	//read the block in memeory

	//update the inode
	writeInode(currentDir, inode);
	//return that it was found or not
	return found;
}
//delete the dir in use
void Disk::deleteCurrentDir()
{

	Inode_Structure inode;

	char parentDir[14] = "..";
	unsigned short int parentAddress;
	unsigned short int inodeTemp = currentDir;
	unsigned short int inodesDeleted = 1;
	//read int the cur inode in use
	readInode(currentDir, inode);
	//find parent dir and get the location in dsk
	findDir(parentDir, parentAddress);

	//delete all the dirs in the inode
	if (inode.direct_address0 != 0)
	{
		appendToInodeListFromBlock(inode.direct_address0, inodesDeleted);
		appendToFreeList(inode.direct_address0);
	}
	if (inode.direct_address1 != 0)
	{
		appendToInodeListFromBlock(inode.direct_address1, inodesDeleted);
		appendToFreeList(inode.direct_address1);
	}
	if (inode.direct_address2 != 0)
	{
		appendToInodeListFromBlock(inode.direct_address2, inodesDeleted);
		appendToFreeList(inode.direct_address2);
	}
	if (inode.direct_address3 != 0)
	{
		appendToInodeListFromBlock(inode.direct_address3, inodesDeleted);
		appendToFreeList(inode.direct_address3);
	}
	if (inode.direct_address4 != 0)
	{
		appendToInodeListFromBlock(inode.direct_address4, inodesDeleted);
		appendToFreeList(inode.direct_address4);
	}
	if (inode.direct_address5 != 0)
	{
		appendToInodeListFromBlock(inode.direct_address5, inodesDeleted);
		appendToFreeList(inode.direct_address5);
	}
	if (inode.direct_address6 != 0)
	{
		appendToInodeListFromBlock(inode.direct_address6, inodesDeleted);
		appendToFreeList(inode.direct_address6);
	}
	if (inode.direct_address7 != 0)
	{
		appendToInodeListFromBlock(inode.direct_address7, inodesDeleted);
		appendToFreeList(inode.direct_address7);
	}
	if (inode.direct_address8 != 0)
	{
		appendToInodeListFromBlock(inode.direct_address8, inodesDeleted);
		appendToFreeList(inode.direct_address8);
	}
	if (inode.direct_address9 != 0)
	{
		appendToInodeListFromBlock(inode.direct_address9, inodesDeleted);
		appendToFreeList(inode.direct_address9);
	}
	if (inode.indirect_address != 0)
	{
		Block_Address_Structure temp;

		for (int i = 0; i < BLOCK_SIZE / sizeof(Block_Address_Structure); i++)
		{

			readFromDisk(inode.indirect_address);
			readFromBlock(reinterpret_cast<char*>(&temp), sizeof(Block_Address_Structure), sizeof(Block_Address_Structure)* i);
			if (temp.address == 0) break;


			appendToInodeListFromBlock(temp.address, inodesDeleted);
			appendToFreeList(temp.address);
		}

		appendToFreeList(inode.indirect_address);
	}

	//move to the parent dir
	moveToDir(parentAddress);
	//find the deleted dir in the parent inode and remove it

	findAndDelete(inodeTemp);
	//append the inode address delete to the inode free list
	appendToInodeList(inodeTemp);


}

//find dir by id this function im using it to delete a file in the dir so use with caution
bool Disk::findAndDelete(unsigned short int address)
{
	bool found = false;
	Inode_Structure inode;
	Inode_Address_Structure dir;
	time_t now;

	//get the block from disk
	readInode(currentDir, inode);
	//remove the space the inode to be remove uses
	inode.file_size = inode.file_size - sizeof(Inode_Address_Structure);
	inode.access_time = inode.modificacion_time = time(&now);

	if (inode.direct_address0 != 0)
	{
		readFromDisk(inode.direct_address0);

		found = foundAndDeleted(address, inode.direct_address0);
	}
	if (inode.direct_address1 != 0 && !found) {
		readFromDisk(inode.direct_address1);
		found = foundAndDeleted(address, inode.direct_address1);
	}
	if (inode.direct_address2 != 0 && !found) {
		readFromDisk(inode.direct_address2);
		found = foundAndDeleted(address, inode.direct_address2);
	}
	if (inode.direct_address3 != 0 && !found) {
		readFromDisk(inode.direct_address3);

		found = foundAndDeleted(address, inode.direct_address3);
	}
	if (inode.direct_address4 != 0 && !found) {
		readFromDisk(inode.direct_address4);

		found = foundAndDeleted(address, inode.direct_address4);
	}
	if (inode.direct_address5 != 0 && !found) {
		readFromDisk(inode.direct_address5);
		found = foundAndDeleted(address, inode.direct_address5);

	}
	if (inode.direct_address6 != 0 && !found) {
		readFromDisk(inode.direct_address6);
		found = foundAndDeleted(address, inode.direct_address6);

	}
	if (inode.direct_address7 != 0 && !found) {
		readFromDisk(inode.direct_address7);
		found = foundAndDeleted(address, inode.direct_address7);

	}
	if (inode.direct_address8 != 0 && !found) {
		readFromDisk(inode.direct_address8);
		found = foundAndDeleted(address, inode.direct_address8);

	}
	if (inode.direct_address9 != 0 && !found) {
		readFromDisk(inode.direct_address9);
		found = foundAndDeleted(address, inode.direct_address9);

	}
	if (!found)
	{
		Block_Address_Structure temp;

		for (int i = 0; i > BLOCK_SIZE / sizeof(Block_Address_Structure); i++)
		{
			readFromDisk(inode.indirect_address);
			readFromBlock(reinterpret_cast<char*>(&temp), sizeof(Block_Address_Structure), sizeof(Block_Address_Structure)* i);
			if (temp.address == 0)
			{
				found = false;
				break;
			}
			readFromDisk(temp.address);
			if (found = foundAndDeleted(address, temp.address)) break;
		}
	}


	writeInode(currentDir, inode);
	return found;
}

//append a freed up block addres to the list
void Disk::appendToFreeList(unsigned short int address)
{
	//point to the next free inode
	getFirstFreeInodeAddress();
	Block_Address_Structure temp{ address };
	//move one back
	freeList->prev();
	//insert the freed block in the list
	freeList->insertItem(temp);
	//update the list
	writeFreeListToDisk(infoBlock.firstBlockOfFreeList);
}

//this functions takes a block and appeends all dirs in the given block to the free list
void Disk::appendToInodeListFromBlock(unsigned short int address, unsigned short int &count)
{

	Inode_Address_Structure dir;
	Inode_Address_Structure nullDir;
	nullDir.name[0] = '@';
	nullDir.address = 0;
	for (int i = 2; i < BLOCK_SIZE / sizeof(Inode_Address_Structure); i++)
	{
		readFromDisk(address);
		readFromBlock(reinterpret_cast<char*>(&dir), sizeof(Inode_Address_Structure), sizeof(Inode_Address_Structure)*i);
		if (dir.name[0] == '@') continue;
		//if there is a dir in ehre then we increase the count of dirs deleted and append the inode address to the list
		count++;
		appendToInodeList(dir.address);
		if (!(writeInodeData(address, nullDir, sizeof(Inode_Address_Structure) * i))) cout << "could not write to Inode Daata" << endl;
	}
}

//append an inode address to the list od free inodes
void Disk::appendToInodeList(unsigned short int address)
{
	//get the first block avaiable in list
	getFirstFreeInodeAddress();
	Block_Address_Structure temp{ address };
	//move one back
	inodeFreeList->prev();
	//inser the free inode
	inodeFreeList->insertItem(temp);
	//write it back to disk
	writeInodeListToDisk(infoBlock.firstBlockOfInodeList);
}


bool Disk::createFile(unsigned int size , char name[14])
{
	if (size > 522 * BLOCK_SIZE)return false;
	Inode_Address_Structure temp;

	Block_Address_Structure address;
	Block_Address_Structure blockOfData;

	char tempBlock[1024];
	time_t now;

	char owner[14];
	//get the name of the current dir
	getNameofOwner(owner);
	//copy the name to the structure
	//set the name for the file
	for (int i = 0; i < 14; i++) temp.name[i] = name[i];

	getInodeAddress(temp);
	//get the first block of data



	Inode_Structure inode;
	//set the default size of the inode structure
	inode.file_size = size;
	inode.access_time = inode.modificacion_time = inode.creation_time = time(&now);
	for (int i = 0; i < 14; i++) inode.owner[i] = owner[i];
	//set the name of the owner



	//set the protection and count data
	inode.protection = 1;
	inode.count = 1;
	//set all addresd blocks as o
	inode.direct_address0 = 0;
	inode.direct_address1 = 0;
	inode.direct_address2 = 0;
	inode.direct_address3 = 0;
	inode.direct_address4 = 0;
	inode.direct_address5 = 0;
	inode.direct_address6 = 0;
	inode.direct_address7 = 0;
	inode.direct_address8 = 0;
	inode.direct_address9 = 0;
	inode.indirect_address = 0;

	srand(time(NULL));

	char c = 32;
		c += rand() % 223;


		// write random chars to a block one char at the time
	for (int i = 0; i < size ; i++)
	{
		unsigned short int location;
		char c = 32;
		if (i == size - 1) c = '~';

		else
		{
			c += rand() % 93;

			tempBlock[i % BLOCK_SIZE] = c;

		}
		
		 
		tempBlock[i % BLOCK_SIZE] = c;
		//check if we need a new block of data
		if (i % BLOCK_SIZE == 0 )
		{
			if (i != 0)
			{
				//write thje full block to disk
				writeToBlock(tempBlock, BLOCK_SIZE, 0);
				writeToDisk(location);
			}
			//get a new free block
			getFreeBlockAddress(blockOfData);
			

			

			int j = i / BLOCK_SIZE;
			//check where is the next free location in inode
			switch (j)
			{
			case 0:
				location = inode.direct_address0 = blockOfData.address;
				break;
			case 1:
				location =  inode.direct_address1 = blockOfData.address;
				break;
			case 2:
				location =  inode.direct_address2 = blockOfData.address;
				break;
			case 3:
				location = inode.direct_address3 = blockOfData.address;
				break;
			case 4:
				location = inode.direct_address4 = blockOfData.address;
				break;
			case 5:
				location = inode.direct_address5 = blockOfData.address;
				break;
			case 6:
				location = inode.direct_address6 = blockOfData.address;
				break;
			case 7:
				location = inode.direct_address7 = blockOfData.address;
				break;
			case 8:
				location = inode.direct_address8 = blockOfData.address;
				break;
			case 9:
				location = inode.direct_address9 = blockOfData.address;
				break;
			default:
				
				if (inode.indirect_address == 0)
				{
					inode.indirect_address = blockOfData.address;
					getFreeBlockAddress(blockOfData);
				}
				readFromDisk(inode.indirect_address);
				writeToBlock(reinterpret_cast<char*>(&blockOfData),	sizeof(Block_Address_Structure), sizeof(Block_Address_Structure)*(j -10));
				writeToDisk(inode.indirect_address);
				location = blockOfData.address;
				break;


			}
		}
		//if we reache the end of file write the block to disk
		else if (i == size - 1)
		{
			writeToBlock(tempBlock, BLOCK_SIZE, 0);
			writeToDisk(location);
		}
	}
	//write the inode to disk
	writeInode(temp.address, inode);
	//append the file address to cuurent directory
	appendToCurrentDir(temp);

	return true;

}
//check if the block given is full of Directories
bool Disk::checkIfDirBlockisFull(unsigned short int address)
{
	//read the block to disk
	readFromDisk(address);
	Inode_Address_Structure temp;
	//read thorugh the block until we find a @ meaning there is space for at least one directorie
	for (int i = 0; i < BLOCK_SIZE / sizeof(Inode_Address_Structure); i++)
	{
		readFromBlock(reinterpret_cast<char*>(&temp), sizeof(Inode_Address_Structure), sizeof(Inode_Address_Structure) * i);

		if (temp.name[0] == '@') return false;


	}

	return true;
}
//get the name of the current directoi
void Disk::getNameofOwner(char name[14])
{
	Inode_Structure inode;
	readInode(currentDir, inode);

	for (int i = 0; i < 14; i++) name[i] = inode.owner[i];

}
//check if we found the dir using the name
bool Disk::foundDir(char name[14], unsigned short int &address)
{
	Inode_Address_Structure dir;
	bool found = false;
	//check if any of the dirs in the inode match the name given
	for (int i = 0; i < BLOCK_SIZE / sizeof(Inode_Address_Structure); i++)
	{
		readFromBlock(reinterpret_cast<char*>(&dir), sizeof(Inode_Address_Structure), sizeof(Inode_Address_Structure)*i);
		if (dir.name[0] == '@') continue;
		found = true;
		for (int i = 0; i < 14; i++)
		{
			//
			if (name[i] != dir.name[i])
			{
				found = false;
				break;
			}

		}
		//if found break out of the loop and get the inode address in disk
		if (found)
		{
			address = dir.address;
			break;
		}
	}

	return found;
}

//find and delete a directorie using the directory address
bool Disk::foundAndDeleted(unsigned short int address, unsigned short int location)
{

	Inode_Address_Structure inodeAddress{ 0 ,'@' };
	Inode_Address_Structure dir;
	bool found = false;
	//check if any of the dirs in the inode match the name given
	for (int i = 0; i < BLOCK_SIZE / sizeof(Inode_Address_Structure); i++)
	{
		readFromBlock(reinterpret_cast<char*>(&dir), sizeof(Inode_Address_Structure), sizeof(Inode_Address_Structure)*i);
		if (dir.name[0] == '@') continue;
		found = true;

		if (dir.address != address) found = false;

		if (found)
		{
			//if teh directory is found we place a null directory in its place and write it back to disk


			for (int i = 0; i < 14; i++) dir.name[i] = inodeAddress.name[i];
			writeToBlock(reinterpret_cast<char*>(&dir), sizeof(Inode_Address_Structure), sizeof(Inode_Address_Structure)*i);
			writeToDisk(location);
			break;
		}
	}


	return found;
}
//fill a  block with emoty Dir
void Disk::fillBlockWithEmptyDirs(unsigned short int address)
{
	Inode_Address_Structure nullDir;
	nullDir.name[0] = '@';
	nullDir.address = 0;
	readFromDisk(address);
	//write to block an emoty dir till the end
	for (int i = 0; i < BLOCK_SIZE / sizeof(Inode_Address_Structure); i++)
	{
		writeToBlock(reinterpret_cast<char*>(&nullDir), sizeof(Inode_Address_Structure), sizeof(Inode_Address_Structure)* i);
	}


	writeToDisk(address);
}
//fille a block with empty blocks
void Disk::fillBlockWithNullBlocks(unsigned short int address)
{
	Block_Address_Structure nullBlock{ 0 };
readFromDisk(address);
//write null blocks till end of disk
for (int i = 0; i < BLOCK_SIZE / sizeof(Block_Address_Structure); i++)
	writeToBlock(reinterpret_cast<char*>(&nullBlock), sizeof(Block_Address_Structure), sizeof(Block_Address_Structure) * i);

writeToDisk(address);
}

//check if there is an empry dir somewher in the current directory before the total filesize of the inode is reached
bool Disk::checkIfThereIsAnEmptyDir(unsigned short int &location, Inode_Structure inode)
{
	bool found = false;

	//check thorugh all address in inode till we find an empty space or until we reached the last block in inode poiting to data

	if (inode.direct_address0 != 0)
	{
		if (found = !checkIfDirBlockisFull(inode.direct_address0)) location = 0;
	}
	if (inode.direct_address1 != 0 && !found && location > 1)
	{
		if (found = !checkIfDirBlockisFull(inode.direct_address0))location = 1;
	}
	if (inode.direct_address2 != 0 && !found && location > 2)
	{
		if (found = !checkIfDirBlockisFull(inode.direct_address0)) location = 2;
	}
	if (inode.direct_address3 != 0 && !found && location > 3)
	{
		if (found = !checkIfDirBlockisFull(inode.direct_address0)) location = 3;
	}
	if (inode.direct_address4 != 0 && !found && location > 4)
	{
		if (found = !checkIfDirBlockisFull(inode.direct_address0)) location = 4;
	}
	if (inode.direct_address5 != 0 && !found && location > 5)
	{
		if (found = !checkIfDirBlockisFull(inode.direct_address0)) location = 5;
	}
	if (inode.direct_address6 != 0 && !found && location > 6)
	{
		if (found = !checkIfDirBlockisFull(inode.direct_address0)) location = 6;
	}
	if (inode.direct_address7 != 0 && !found && location > 7)
	{
		if (found = !checkIfDirBlockisFull(inode.direct_address0)) location = 7;
	}
	if (inode.direct_address8 != 0 && !found && location > 8)
	{
		if (found = !checkIfDirBlockisFull(inode.direct_address0)) location = 8;
	}
	if (inode.direct_address9 != 0 && !found && location > 9)
	{
		if (found = !checkIfDirBlockisFull(inode.direct_address0))location = 9;
	}
	if (inode.indirect_address != 0 && !found && location > 10)
	{
		for (int i = 0; i < BLOCK_SIZE / sizeof(Block_Address_Structure); i++)
		{

			Block_Address_Structure blockOfDirs;
			readFromDisk(inode.indirect_address);

			readFromBlock(reinterpret_cast<char*>(&blockOfDirs), sizeof(Block_Address_Structure), sizeof(Block_Address_Structure) * i);

			if (blockOfDirs.address == 0 || location < blockOfDirs.address + 10) break;

			if (found = !checkIfDirBlockisFull(blockOfDirs.address)) location = blockOfDirs.address + 10;




		}
	}
	return found;
}


//read a files information and contents
void Disk::readFile(unsigned short int address)
{
	Inode_Structure inode;

	//read the files information and modify the acces time
	readInode(address, inode);

	time(&inode.access_time);
	

	cout << endl;
	cout << "file size \t" << inode.file_size << endl;
	cout << "creation time \t" << inode.creation_time << endl;
	cout << "last acessed \t" << inode.access_time << endl;
	cout << "last modified\t" << inode.modificacion_time << endl;
	cout << "owner \t\t" << inode.owner << endl;
	cout << "protection \t" << (int)(inode.protection) << endl;
	cout << "count \t\t" << (int)(inode.count) << endl;
	cout << "address 0 \t" << inode.direct_address0 << endl;
	cout << "address 1 \t" << inode.direct_address1 << endl;
	cout << "address 2 \t" << inode.direct_address2 << endl;
	cout << "address 3 \t" << inode.direct_address3 << endl;
	cout << "address 4 \t" << inode.direct_address4 << endl;
	cout << "address 5 \t" << inode.direct_address5 << endl;
	cout << "address 6 \t" << inode.direct_address6 << endl;
	cout << "address 7 \t" << inode.direct_address7 << endl;
	cout << "address 8 \t" << inode.direct_address8 << endl;
	cout << "address 9 \t" << inode.direct_address9 << endl;
	cout << "indirect  \t" << inode.indirect_address << endl;
	

	
	bool endOfFile;
	//read ther data stores in the blocks of data in inode till we reach the endOffile
	if (inode.direct_address0 != 0) endOfFile = readFileBlockData(inode.direct_address0);
	if (inode.direct_address1 != 0 && !endOfFile) endOfFile = readFileBlockData(inode.direct_address1);
	if (inode.direct_address2 != 0 && !endOfFile) endOfFile = readFileBlockData(inode.direct_address2);
	if (inode.direct_address3 != 0 && !endOfFile) endOfFile = readFileBlockData(inode.direct_address3);
	if (inode.direct_address4 != 0 && !endOfFile) endOfFile = readFileBlockData(inode.direct_address4);
	if (inode.direct_address5 != 0 && !endOfFile) endOfFile = readFileBlockData(inode.direct_address5);
	if (inode.direct_address6 != 0 && !endOfFile) endOfFile = readFileBlockData(inode.direct_address6);
	if (inode.direct_address7 != 0 && !endOfFile) endOfFile = readFileBlockData(inode.direct_address7);
	if (inode.direct_address8 != 0 && !endOfFile) endOfFile = readFileBlockData(inode.direct_address8);
	if (inode.direct_address9 != 0 && !endOfFile) endOfFile = readFileBlockData(inode.direct_address9);
	if (inode.indirect_address != 0 && !endOfFile)
	{
		Block_Address_Structure temp;

		for (int i = 0; i < BLOCK_SIZE / sizeof(Block_Address_Structure); i++)
		{
		
			readFromDisk(inode.indirect_address);
			readFromBlock(reinterpret_cast<char*>(&temp), sizeof(Block_Address_Structure), sizeof(Block_Address_Structure) * i);
			if (temp.address == 0) break;

			if (endOfFile = readFileBlockData(temp.address)) break;
		
		}
	

	}
	
	writeInode(address, inode);
}

//read the data of a block full of text
bool Disk::readFileBlockData(unsigned short int address)
{
	//read the file to memory
	readFromDisk(address);

	//read the block till we reache the end of teh block or the flag taht is the end of the file.
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		char c = block[i];

		if (c == '~')
		{
			cout << endl;
			return true;
		}

		cout << c;
	}

	cout << endl;


	return false;
}

//delete a file
void Disk::deleteFile(unsigned short int address)
{

	Inode_Structure inode;


	//read int the cur inode in use
	readInode(address, inode);
	
	//delete all the blocks in the Inode and append to the free list
	if (inode.direct_address0 != 0)
	{
		
		appendToFreeList(inode.direct_address0);
	}
	if (inode.direct_address1 != 0)
	{
	
		appendToFreeList(inode.direct_address1);
	}
	if (inode.direct_address2 != 0)
	{
	
		appendToFreeList(inode.direct_address2);
	}
	if (inode.direct_address3 != 0)
	{
	
		appendToFreeList(inode.direct_address3);
	}
	if (inode.direct_address4 != 0)
	{
	
		appendToFreeList(inode.direct_address4);
	}
	if (inode.direct_address5 != 0)
	{
	
		appendToFreeList(inode.direct_address5);
	}
	if (inode.direct_address6 != 0)
	{
	
		appendToFreeList(inode.direct_address6);
	}
	if (inode.direct_address7 != 0)
	{
		
		appendToFreeList(inode.direct_address7);
	}
	if (inode.direct_address8 != 0)
	{
		
		appendToFreeList(inode.direct_address8);
	}
	if (inode.direct_address9 != 0)
	{
		appendToFreeList(inode.direct_address9);
	}
	if (inode.indirect_address != 0)
	{
		Block_Address_Structure temp;

		for (int i = 0; i < BLOCK_SIZE / sizeof(Block_Address_Structure); i++)
		{

			readFromDisk(inode.indirect_address);
			readFromBlock(reinterpret_cast<char*>(&temp), sizeof(Block_Address_Structure), sizeof(Block_Address_Structure)* i);
			if (temp.address == 0) break;

			appendToFreeList(temp.address);
		}

		appendToFreeList(inode.indirect_address);
	}


	//delete the file from the current directory
	findAndDelete(address);
	//append the inode address to the inode free list
	appendToInodeList(address);


}bool Disk::findFile(char name[14], unsigned short int &address)
{
	bool found = false;
	Inode_Structure inode;
	Inode_Address_Structure dir;
	time_t now;


	//get the inode from disk
	readInode(currentDir, inode);
	//update the last modified and last acceesed time of the inode
	inode.access_time = inode.modificacion_time = time(&now);
	//check where the dir could be and save the disk location
	if (inode.direct_address0 != 0)
	{
		readFromDisk(inode.direct_address0);

		found = foundDir(name, address);
	}
	if (inode.direct_address1 != 0 && !found) {
		readFromDisk(inode.direct_address1);
		found = foundDir(name, address);
	}
	if (inode.direct_address2 != 0 && !found) {
		readFromDisk(inode.direct_address2);
		found = foundDir(name, address);
	}
	if (inode.direct_address3 != 0 && !found) {
		readFromDisk(inode.direct_address3);

		found = foundDir(name, address);
	}
	if (inode.direct_address4 != 0 && !found) {
		readFromDisk(inode.direct_address4);

		found = foundDir(name, address);
	}
	if (inode.direct_address5 != 0 && !found) {
		readFromDisk(inode.direct_address5);
		found = foundDir(name, address);

	}
	if (inode.direct_address6 != 0 && !found) {
		readFromDisk(inode.direct_address6);
		found = foundDir(name, address);

	}
	if (inode.direct_address7 != 0 && !found) {
		readFromDisk(inode.direct_address7);
		found = foundDir(name, address);

	}
	if (inode.direct_address8 != 0 && !found) {
		readFromDisk(inode.direct_address8);
		found = foundDir(name, address);

	}
	if (inode.direct_address9 != 0 && !found) {
		readFromDisk(inode.direct_address9);
		found = foundDir(name, address);

	}
	if (!found)
	{
		Block_Address_Structure temp;
		//read every block address in the indirect block and treat every block ad a block full of directory
		for (int i = 0; i > BLOCK_SIZE / sizeof(Block_Address_Structure); i++)
		{
			readFromDisk(inode.indirect_address);
			readFromBlock(reinterpret_cast<char*>(&temp), sizeof(Block_Address_Structure), sizeof(Block_Address_Structure)* i);
			if (temp.address == 0)
			{
				found = false;
				break;
			}
			readFromDisk(temp.address);
			if (found = foundDir(name, address)) break;
		}
	}

	if (found)
	{
		Inode_Structure temp;
		readInode(address, temp);
		if (temp.protection != 1) found = false;
	}

	//read the block in memeory

	//update the inode
	writeInode(currentDir, inode);
	//return that it was found or not
	return found;
}