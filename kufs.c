//Written by Meric Melike Softa and Firtina Kucuk on 20.05.2016
//for the 3rd class project of Comp304 - Operating Systems

//necessary methods for the project are implemented
//here on top of those implemented in kufs.h

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

void display(char *);
void create(char *);
void rm(char *);
char fileContent[3072];

void display(char *fname) {
  //this function was mostly adapted from cd()
  //first we will find the file within current directory

  char itype;
  int blocks[3];
  _directory_entry _directory_entries[4];

  int i,j;
  int e_inode;

  char found=0;

  // read inode entry for current directory
  // in KUFS, an inode can point to three blocks at the most
  itype = _inode_table[CD_INODE_ENTRY].TT[0];
  blocks[0] = stoi(_inode_table[CD_INODE_ENTRY].XX,2);
  blocks[1] = stoi(_inode_table[CD_INODE_ENTRY].YY,2);
  blocks[2] = stoi(_inode_table[CD_INODE_ENTRY].ZZ,2);

  // its a directory; so the following should never happen
  if (itype=='F') {
    printf("Fatal Error! Aborting.\n");
    exit(1);
  }

  // now lets try to see if a file by the name already exists
  for (i=0; i<3; i++) {
    if (blocks[i]==0) continue;	// 0 means pointing at nothing

    readKUFS(blocks[i],(char *)_directory_entries);	// lets read a directory entry; notice the cast

    // so, we got four possible directory entries now
    for (j=0; j<4; j++) {
      if (_directory_entries[j].F=='0') continue;	// means unused entry

      e_inode = stoi(_directory_entries[j].MMM,3);	// this is the inode that has more info about this entry

      //difference from cd() starts here:
      if (_inode_table[e_inode].TT[0]=='F') { // entry is for a file; can't display a directory, right?
        if (strncmp(fname,_directory_entries[j].fname, 252) == 0) {	// and it is the one we are looking for
          found = 1;	// VOILA
          break;
        }
      }
    }
    if (found) break; // no need to search more
  }

  //this is the actual part of the code we wrote
  if (found) {
    int x,y,z;
    char buffer[1024];
    //get current inode entry, we'll need the block addresses from that
    _inode_entry fnode = _inode_table[e_inode];

    //read the blocks at xx, yy, and zz respectively
    //if they are meaningful print them on the screen
    //note that no "\n" is printed in between blocks
    //since they are continuous parts of a file
    if((x = stoi(fnode.XX, 2)) != 0){
      readKUFS(x, buffer);
      printf("%s", buffer);
    }
    if((y = stoi(fnode.YY, 2)) != 0){
      readKUFS(stoi(fnode.YY,2), buffer);
      printf("%s", buffer);
    }
    if((z = stoi(fnode.ZZ, 2)) != 0){
      readKUFS(stoi(fnode.ZZ,2), buffer);
      printf("%s", buffer);
    }

    //if we found the file but printed nothing
    if(x==0 && y==0 && z==0){
      printf("File empty!");
    }
    printf("\n");
  }
  else {
    printf("%.252s: File not found.\n",fname);
  }
}


//following function is mostly adapted from md() fn in kufs.h
void create(char *fname) {

  	char itype;
  	int blocks[3];
  	_directory_entry _directory_entries[4];

  	int i,j;

  	int empty_dblock=-1,empty_dentry=-1;
  	int empty_ientry;

  	// non-empty name
  	if (strlen(fname)==0) {
  		printf("Usage: create <file name>\n");
  		return;
  	}

  	// do we have free inodes
  	if (free_inode_entries == 0) {
  		printf("Error: Inode table is full.\n");
  		return;
  	}

  	// read inode entry for current directory
  	// in KUFS, an inode can point to three blocks at the most
  	itype = _inode_table[CD_INODE_ENTRY].TT[0];
  	blocks[0] = stoi(_inode_table[CD_INODE_ENTRY].XX,2);
  	blocks[1] = stoi(_inode_table[CD_INODE_ENTRY].YY,2);
  	blocks[2] = stoi(_inode_table[CD_INODE_ENTRY].ZZ,2);

  	// its a directory; so the following should never happen
  	if (itype=='F') {
  		printf("Fatal Error! Aborting.\n");
  		exit(1);
  	}

  	// now lets try to see if the name already exists
  	for (i=0; i<3; i++) {
  		if (blocks[i]==0) { 	// 0 means pointing at nothing
  			if (empty_dblock==-1) empty_dblock=i; // we can later add a block if needed
  			continue;
  		}

  		readKUFS(blocks[i],(char *)_directory_entries); // lets read a directory entry; notice the cast

  		// so, we got four possible directory entries now
  		for (j=0; j<4; j++) {
  			if (_directory_entries[j].F=='0') { // means unused entry
  				if (empty_dentry==-1) { empty_dentry=j; empty_dblock=i; } // AAHA! lets keep a note of it, just in case we have to create the new directory
  				continue;
  			}

  			if (strncmp(fname,_directory_entries[j].fname, 252) == 0) { // compare with user given name
  					printf("%.252s: Already exists.\n",fname);
  					return;
  			}
  		}

  	}
  	// so file name is new

  	// if we did not find an empty directory entry and all three blocks are in use; then no new files can be made
  	if (empty_dentry==-1 && empty_dblock==-1) {
  		printf("Error: Maximum directory entries reached.\n");
  		return;
  	}
  	else { // otherwise
  		if (empty_dentry == -1) { // Great! didn't find an empty entry but not all three blocks have been used
  			empty_dentry=0;

  			if ((blocks[empty_dblock] = getBlock())==-1) {  // first get a new block using the block bitmap
  				printf("Error: Disk is full.\n");
  				return;
  			}

  			writeKUFS(blocks[empty_dblock],NULL);	// write all zeros to the block (there may be junk from the past!)

  			switch(empty_dblock) {	// update the inode entry of current dir to reflect that we are using a new block
  				case 0: itos(_inode_table[CD_INODE_ENTRY].XX,blocks[empty_dblock],2); break;
  				case 1: itos(_inode_table[CD_INODE_ENTRY].YY,blocks[empty_dblock],2); break;
  				case 2: itos(_inode_table[CD_INODE_ENTRY].ZZ,blocks[empty_dblock],2); break;
  			}
  		}


  		// NOTE: all error checkings have already been done at this point!!
  		// time to put everything together


  		empty_ientry = getInode();	// get an empty place in the inode table which will store info about blocks for this new file

  		readKUFS(blocks[empty_dblock],(char *)_directory_entries);	// read block of current directory where info on this new file will be written
  		_directory_entries[empty_dentry].F = '1';			// remember we found which directory entry is unused; well, set it to used now
  		strncpy(_directory_entries[empty_dentry].fname,fname,252);	// put the name in there
  		itos(_directory_entries[empty_dentry].MMM,empty_ientry,3);	// and the index of the inode that will hold info inside this directory
  		writeKUFS(blocks[empty_dblock],(char *)_directory_entries);	// now write this block back to the disk

      //our actual code starts at this point

      printf("Max 3072 characters: hit ESC-ENTER to end\n");
      char c;
      int i = 0;
      while (1) { //Read user input until ESC+Enter is pressed
            c = getchar();
            if (c == 27) { //ASCII code for ESC
              break;
            }
            fileContent[i]=c; //save that in char array fileContent
            i++;
      }
      int len;
      int x;
      char adr[2];
      len = strlen(fileContent);

      //lets start saving the information for the file
  		strncpy(_inode_table[empty_ientry].TT, "FI",2); //this is a file, so "FI" goes to inode table
      if(len>0){ //if the user actually typed something, assign a new block for the data
        x = getBlock();
      } else{ x=0;}
      itos(adr, x, 2);
  		strncpy(_inode_table[empty_ientry].XX, adr,2);
      if(len>1024){//if the input was greater than 1K, assign another
        x = getBlock();
      } else{ x=0;}
      itos(adr, x, 2);
  		strncpy(_inode_table[empty_ientry].YY, adr,2);
      if(len>2048){//if the input was greater than 2K, assign another
        x = getBlock();
      } else{ x=0;}
      itos(adr, x, 2);
  		strncpy(_inode_table[empty_ientry].ZZ, adr,2);

      _inode_entry fnode = _inode_table[empty_ientry];
      int blocks[3];
      blocks[0] = stoi(fnode.XX,2);
      blocks[1] = stoi(fnode.YY,2);
      blocks[2] = stoi(fnode.ZZ,2);

     char substr[1024];
     for(i=0;i<3;i++){
       strncpy(substr, fileContent+(i*1024), 1024);
       if(blocks[i]!=0){ //write to block only if it is assigned, i.e. non-zero
         writeKUFS(blocks[i], substr); //we are updating the contents in 3 parts
       }
    }

      printf("%d bytes saved.\n", len);
  	  writeKUFS(BLOCK_INODE_TABLE, (char *)_inode_table);	// phew!! write the inode table back to the disk
  	}

}

void rm(char *name) {

}
