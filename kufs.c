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

void display(char *fname) {

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

  // now lets try to see if a directory by the name already exists
  for (i=0; i<3; i++) {
    if (blocks[i]==0) continue;	// 0 means pointing at nothing

    readKUFS(blocks[i],(char *)_directory_entries);	// lets read a directory entry; notice the cast

    // so, we got four possible directory entries now
    for (j=0; j<4; j++) {
      if (_directory_entries[j].F=='0') continue;	// means unused entry

      e_inode = stoi(_directory_entries[j].MMM,3);	// this is the inode that has more info about this entry

      if (_inode_table[e_inode].TT[0]=='F') { // entry is for a directory; can't cd into a file, right?
        if (strncmp(fname,_directory_entries[j].fname, 252) == 0) {	// and it is the one we are looking for
          found = 1;	// VOILA
          break;
        }
      }
    }
    if (found) break; // no need to search more
  }

  if (found) {
    int x,y,z;
    char buffer[1024];
    _inode_entry fnode = _inode_table[e_inode];
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
    if(x==0 && y==0 && z==0){
      printf("File empty!");
    }
    printf("\n");
  }
  else {
    printf("%.252s: File not found.\n",fname);
  }
}

void create(char *fname) {

}

void rm(char *name) {

}
