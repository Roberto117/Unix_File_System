
#include <fstream>
#include <time.h>
#include "Inode_Address_Structure.h"
#include "Information_Block_Structure.h"
#include "Inode_Structure.h"
#include "FreeList.h"

#include "FreeList.h"
#include <stdint.h>

#ifndef Disk_h
#define Disk_h

class Disk
{
public:
	//initiate the disk
	explicit Disk();
	//delete the disk
	~Disk();

	void reset();
	//get the size of one block
	int getBlockSize();
	//get the total size of the block in bytes
	int getTotalSize();
	//get the size of a single inode
	int getInodeSize();
	bool findFile(char[14], unsigned short int &);

	//read an address from inode
	void readInodeAddresses(int);
	//write to an inode
	bool writeInode(int, Inode_Structure);
	//write addresses to the inode structure
	bool writeInodeData(int address, Inode_Address_Structure, int = 0);

	//write Data to a block
	bool writeData(int);
	//read an inode to memory
	bool readInode(int, Inode_Structure &);
	//read a dir from inode
	bool readDir(int address);

	bool findDir(char[14], unsigned short int &);
	bool findAndDelete(unsigned short int);
	bool moveToDir(int);
	void readCurrentDir();
	void readFile(unsigned short int );
	bool readFileBlockData(unsigned short int address);
	void printAllFreeBlocks();
	//reset the freelist
	void createFreeList();
	//reset the inode list
	void createInodeList();
	//print the list in memory
	void printCurrentFreeList();
	void printCurrentInodeList();
	bool createFile(unsigned int, char[14]);
	void deleteFile(unsigned short int);

	//get the size of the free list in memory
	int getSizeOfFreeList();
	//get the next free block in free list
	void getFreeBlockAddress(Block_Address_Structure &);
	//point to the next free block in memory
	void getfirstFreeBlock();
	//get the next free inode in the list


	void getInodeAddress(Inode_Address_Structure &);
	//point to the next inode in mermory
	void getFirstFreeInodeAddress();
	//get a freeListblock in memory
	void printFreeListBlock(int);
	void printInodeListBlock(int);

	//get the address to the first free list block
	void getFreeListFirstBlockAddress();

	//create a dir
	bool createDir(char[14]);
	void deleteCurrentDir();
	void deleteDir(unsigned short int );
	void deleteFileFromBlock(unsigned short int);
private:

	void appendToCurrentDir(Inode_Address_Structure &);
	void appendToFreeList(unsigned short int);
	void appendToInodeListFromBlock(unsigned short int, unsigned short int &);
	void appendToInodeList(unsigned short int);
	void getNameofOwner(char[14]);
	//read a block from disk to memory

	bool readFromDisk(int);

	bool checkIfThereIsAnEmptyDir(unsigned short int&, Inode_Structure);
	bool checkIfDirBlockisFull(unsigned short int);
	//write a block from memory to disk
	bool writeToDisk(int);

	void createRootDir(char[14]);

	

	void updateInfoBlock();

	void fillBlockWithEmptyDirs(unsigned short int);
	void fillBlockWithNullBlocks(unsigned short int);

	//write to block in memory
	bool writeToBlock(char*, int, int);

	bool foundDir(char[14], unsigned short int &);
	bool foundAndDeleted(unsigned short int, unsigned short int);

	//read from block in memory
	bool readFromBlock(char*, int, int);
	//write the freelist block to disk
	bool writeFreeListToDisk(int);
	//read free list block from disk
	bool readFreeListFromDisk(int);
	//write an inode list to disk
	bool writeInodeListToDisk(int);
	//read inode list block from disk
	bool readInodeListFromDisk(int);
	//create the disk fil wioth 16mb of memory
	void createDiskFile();
	// free list in memory
	FreeList *freeList;
	//functions to control the disk
	std::fstream * disk;


	//constants for the disk
	const int TOTAL_SIZE = 16777216;
	const int TOTAL_BLOCK_SIZE = 16384;
	const int BLOCK_SIZE = 1024;
	const int INODE_SPACE = 4096;
	const int NUMBER_OF_INODES = 65536;
	unsigned int infomationBlockAddress = 16383;

	unsigned short int currentDir;

	//information block where important data for the disk is stored
	Information_Block_Structure infoBlock;
	// memory block
	char  block[1024];
	//inode list ion memory
	FreeList * inodeFreeList;
	//enum for types for data

	enum types { DIR = 0, TXT };
	//buffer to read and write to data

	char BLOCK_SIZE_BUFFER[1024];


};

#endif
