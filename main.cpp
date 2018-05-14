

#include <iostream>
#include <fstream>
#include "Inode_Structure.h"
#include "Inode_Address_Structure.h"
#include "Disk.h"
#include "DLList.hpp"
#include "FreeList.h"
#include <string>
#include <sstream>
using namespace std;

FreeList *freeList;
Disk * disk;

bool getCommand();
//strings to handle input
string createDir = "createDir";

string moveDir = "moveDir";

string readDir = "readDir";
string deleteDir = "deleteDir";

string createFile = "createFile";

string readFile = "readFile";

string deleteFile = "deleteFile";


string EXIT = "EXIT";


int main()
{

	bool ext = false;
	disk = new Disk();
	
	//only do this to reset everything
	char documents[14] = "documents";
	char test[14] = "file1";
	char name[14] = "robert";

	unsigned short int a;

	
	//only do this to reset everything
	/*
	disk->reset();

	disk->createDir(name);
	disk->readCurrentDir();
	disk->findDir(name, a);
	disk->moveToDir(a);
	disk->createDir(documents);
	disk->findDir(documents, a );
	disk->moveToDir(a);
	disk->readCurrentDir();
	disk->createFile( 80, test);
	disk->readCurrentDir();
	disk->findDir(test, a);
	disk->readFile(a);
	disk->deleteFile(a);
	disk->readCurrentDir();
	
	*/

	/*

	//test indirect address
	for (int i = 0; i < 1280; i++)
	{
	if (i == 0) ++test[1];

	else if (i % 10 == 0)
	{
	++test[0];
	test[1] = '0';
	}
	else ++test[1];

	if (i == 574)
	{
	cout << endl;
	}


	cout << i << endl;
	disk->createDir(test);
	}
	*/

	//start of program



	while (ext = getCommand());



	//disk->printAllFreeBlocks();
}

bool getCommand()
{
	char name[14]{ 0 };
	char c = 0;
	string input;
	string buffer;
	string command;
	//get the commanda from console
	getline(cin, input);
	stringstream stream(input);

	stream >> command;

	//the user wants to create a Dir get the desire name for input and create a dir in the disk
	if (command == createDir)
	{


		stream >> name;
		//create the dir
		disk->createDir(name);
	}
	//the user wants to move to a dir somwhere in disk
	else if (command == moveDir)
	{
		
		//check if we must go deeper in the tree
		bool more = true;

		unsigned short int address;
		while (more)
		{
			char name[14] = { 0 };
			for (int i = 0; i < 14; i++)
			{
				//reached the end of the command
				if (stream.rdbuf()->in_avail() == 0)
				{
					more = false;
					break;
				}

				stream >> c;
				//if we encounter '/' then the user wants to go deeper in the tree and we break and reapeat the process
				if (c == '/') break;

				//get the name od dir the user wants to go into
				name[i] = c;
				//reached the end of the input without encountering '/' therefore we have reached the end of the desired address
				if (i == 13) more = false;
			}


			//find the dir in the current dir
			if (!disk->findDir(name, address))
			{
				cout << "No such directory" << endl;
				more = false;

			}
				
			//move to the dir
			else disk->moveToDir(address);


		}

	}
	// read the current dir in memory
	else if (command == readDir)
	{
		disk->readCurrentDir();
	}
	//delete the current dir
	else if (command == deleteDir)
	{
		disk->deleteCurrentDir();
		cout << "current file deleted<" << endl;
	}
	//create a text file with trhe desired size in bytes
	else if (command == createFile)
	{
		unsigned  int size;
		//get the name of the file
		stream >> name;
		//get the size  the text file will be in bytes
		stream >> size;
		//createthe file 
		if (!disk->createFile(size, name))
			cout << "File size to Big" << endl;
		
		
	}
	//read a file in the current directory
	else if (command == readFile)
	{
		unsigned short int address;
		//get file name
		stream >> name;
		//find the file in the current dir
		if (!disk->findFile(name, address))
			cout << "no such file" << endl;
		//read the dir
		else disk->readFile(address); 
		
	}
	//delete a file in the current directory
	else if (command == deleteFile)
	{
		unsigned short int address;
		//get file name
		stream >> name;
		//find file
		if (!disk->findFile(name, address))
			cout << "No such File" << endl;

		//delete file
		else  disk->deleteFile(address);
	}
	else cout << "Incorrect command" << endl;
	//clear the stream
	while (stream.rdbuf()->in_avail() != 0) stream >> c;
	//check if user wants to exit
	if (command == EXIT) return false;

	return true;
}


