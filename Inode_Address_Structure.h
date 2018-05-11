#pragma pack(2)

#ifndef Inode_Address_Structure_h
#define Inode_Address_Structure_h
//strucutre for an address stored in the inode
struct Inode_Address_Structure {
	//inode address 
	short unsigned int address;
	//name for the inode
	char name[14] = {0};
};
#endif //Inode_Address_Structure_h
