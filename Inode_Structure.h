#ifndef Inode_Structure_h
#define Inode_Structure_h
#pragma pack(2)
#include <time.h>
#include <stdint.h>
//inode structures
struct Inode_Structure {

	//store the file size
	unsigned short int file_size;

	// 3 times varibables to store when the data was created when it was accesed and when it was modified
	time_t creation_time;
	time_t access_time;
	time_t modificacion_time;
	//cahr to store the name of the owner
	char owner[14];
	//protection for the inode
	unsigned char protection;
	//how many inode are linked to this inode
	unsigned char count;
	//10 addresss to store data blocks
	unsigned short int direct_address0;
	unsigned short int direct_address1;
	unsigned short int direct_address2;
	unsigned short int direct_address3;
	unsigned short int direct_address4;
	unsigned short int direct_address5;
	unsigned short int direct_address6;
	unsigned short int direct_address7;
	unsigned short int direct_address8;
	unsigned short int direct_address9;
	//indirect block to store pointes to block of data
	unsigned short int indirect_address;
};

#endif Inode_Structure_h