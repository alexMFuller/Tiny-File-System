#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>

#include "tfs.h"
#include "io.h"
#define TOHEX(c) ((c) > 9 ? (c) - 10 + 'a' : (c) + '0')

// initializes the TFS in memory
void init() {

  // first block is root directory
  // - there is free space, and first free block is 01
  tfs[0][0] = 1;
  // - root dir contents and their locations are all 0
  for (int i = 1; i < 16; i++)
    tfs[0][i] = 0;

  // remaining blocks are free
  // each free block points to the next one,
  // except the last block which stores 255 in first byte
  for (int i = 1; i < 16; i++)
    tfs[i][0] = i == 15 ? 0xFF : i + 1;
    // free block data (beyond byte 0) is undefined (don't need to set)
}

char getLowerBitsAsHex(byte in){

	in = in & 0x0F;
	return TOHEX(in);
}
char getUpperBitsAsHex(byte in){

	in = in >> 4;

	return TOHEX(in);
}

int validateTFSPath(char* path){

	int lastWasAlpha = 1;
	int lastWasLower = 1;

	for(int i=0;i<strlen(path);i++){

		if(lastWasLower==0&&i<strlen(path)){
			return 0;
		}


		else if(lastWasAlpha == 1){
			if (path[i]!='/'){
				return 0;
			}
			lastWasAlpha = 0;
		}
		else{

			if(!(isalpha(path[i]))){
				return 0;

			}
			if(islower(path[i])){
				lastWasLower=0;
			}
			lastWasAlpha = 1;
		}

	}
	return 1;
}

int validateNewDir(char* path){

	int lastWasAlpha = 1;
	int pathLength = strlen(path);

	for(int i=0;i<pathLength;i++){

		if(lastWasAlpha == 1){
			if (path[i]!='/'){
				return 0;
			}
			lastWasAlpha = 0;
		}
		else{

			if(!(isalpha(path[i]))){
				return 0;

			}
			if(islower(path[i])){
				return 0;
			}
			lastWasAlpha = 1;

		}
		

	}
	

	return 1;
}


// display the TFS as columns of hex digits
void display() {
  // your code here
	  for(int i=0; i<16; i++) {
    // inner loop for column
    for(int j=0; j<16; j++) {

			unsigned char low = getLowerBitsAsHex(tfs[i][j]);
			unsigned char high = getUpperBitsAsHex(tfs[i][j]);

			printc((high));
			printc((low));
			
      write(STDOUT_FILENO, " " , 1);
    }
     write(STDOUT_FILENO, "\n" , 1);// new line
  }


}


//returns if dirMaking was successful
int makedir(char* path){
	
	//1. validate path by making sure its in the form  /A/B/C, and making sure B is in A, A is in the root.
	//2.  check if C already exists
	//3. create C at the index that tfs[0][0] holds (free space pointer)
	int dirNamePlace = 0;
	int iterator = 1;
	int dirToCheck = 0;
	int pathiterator = 1;
	
	if(validateNewDir(path)==0){
		return 2;
	}

	if(tfs[0][0]>16){
		return 0;
	}

	//if one path
	if(strlen(path)==2){
	while(dirNamePlace==0){
		
		if (tfs[0][iterator]==0){
			dirNamePlace=iterator;
		}
		if(tfs[0][iterator]==path[1]){
			return 0;
		}
		iterator++;
	}
	
	if( (dirNamePlace%2) == 1) {
		tfs[0][10+dirNamePlace/2+1] = (tfs[0][10+(dirNamePlace/2+1)]&0x0F) | (tfs[0][0]<< 4);
		tfs[tfs[0][0]][0]= tfs[0][0];
	}
	else{
		tfs[0][10+(dirNamePlace/2)] = (tfs[0][10+(dirNamePlace/2)]&0xF0) | (tfs[0][0]);
		tfs[tfs[0][0]][0] = tfs[0][0];
		
	}
	tfs[0][dirNamePlace] = path[1];
	tfs[0][0]++;
	return 1;
	}
	
	//If multiple file paths
	else{
		

		while(pathiterator<strlen(path)-1){
		
		if (iterator==10){
			return 0;
		}
		
		if(tfs[dirToCheck][iterator]==path[pathiterator]){
			
			if( (iterator%2) == 1) {
				
				dirToCheck = (tfs[dirToCheck][10+(iterator/2+1)]&0xF0)>>4;
			}
			else{
				dirToCheck = (tfs[dirToCheck][10+(iterator/2)]&0x0F);

			}
			
			pathiterator=pathiterator+2;
			iterator=0;
		}

		iterator++;
	}

	iterator=0;
	

	while(dirNamePlace==0){

		if (tfs[dirToCheck][iterator]==0){
				dirNamePlace=iterator;
		}
		if(tfs[dirToCheck][iterator]==path[pathiterator]){
			return -1;
		}

		
		iterator++;
	}

	if( (dirNamePlace%2) == 1) {
		tfs[dirToCheck][10+dirNamePlace/2+1] = (tfs[dirToCheck][10+(dirNamePlace/2+1)]&0x0F) | (tfs[0][0]<< 4);
		tfs[tfs[0][0]][0]= tfs[0][0];
	}
	else{
		tfs[dirToCheck][10+(dirNamePlace/2)] = (tfs[dirToCheck][10+(dirNamePlace/2)]&0xF0) | (tfs[0][0]);
		tfs[tfs[0][0]][0] = tfs[0][0];
	}
	
	tfs[dirToCheck][dirNamePlace] = path[pathiterator];
	tfs[0][0]++;
	return 1;
	
	}

	return 0;
}

	

int import( char* fileName, char* tfsPath){

	if(tfs[0][0]>16){
		return 0;
	}
	
	if(validateTFSPath(tfsPath)==0){
		return 2;
	}
	int dirNamePlace = 0;
	int iterator = 1;
	int dirToCheck = 0;
	int pathiterator = 1;

	int file = open(fileName,O_RDWR,0777);

	if (file == -1) {
			return 0;
  }
	//handles single file path
	if(strlen(tfsPath)==2){

		while(dirNamePlace==0){
		
		if (tfs[0][iterator]==0){
			dirNamePlace=iterator;
		}
		if(tfs[0][iterator]==tfsPath[1]){
			return 0;
		}

		iterator++;

	}
	int size = lseek(file, 0, SEEK_END);
	lseek(file, 0, SEEK_SET);

	if(size==-1){
		return 0;
	}
	char* contents = malloc(254*sizeof(char));
	
	read(file,contents,size);
	close(file);

	if(size<=15){
		//handles basic files sizes
		tfs[dirNamePlace][0] = size;

		for(int i=0;i<strlen(contents);i++){
			
			tfs[tfs[0][0]][i+1] = contents[i];
		}

		if( (dirNamePlace%2) == 1) {
			tfs[0][10+dirNamePlace/2+1] = (tfs[0][10+(dirNamePlace/2+1)]&0x0F) | (tfs[0][0]<< 4);
		}
		else{
			tfs[0][10+(dirNamePlace/2)] = (tfs[0][10+(dirNamePlace/2)]&0xF0) | (tfs[0][0]);
			
		}
		tfs[0][dirNamePlace] = tfsPath[1];
		tfs[0][0]++;
	
		return 1;
	}

	else{
		//handles larger file sizes
		
		int bufferLocation = 0;
		
		
		
		if (size>(16-tfs[0][0])*16){
			return 7;
		}
		
		tfs[tfs[0][0]][0] = size;

		tfs[0][dirNamePlace] = tfsPath[1];

		if( (dirNamePlace%2) == 1) {
			tfs[0][10+dirNamePlace/2+1] = (tfs[0][10+(dirNamePlace/2+1)]&0x0F) | (tfs[0][0]<< 4);
			bufferLocation = (tfs[0][10+dirNamePlace/2+1]&0xF0)>>4;
		}
		else{
			tfs[0][10+(dirNamePlace/2)] = (tfs[0][10+(dirNamePlace/2)]&0xF0) | (tfs[0][0]);
			bufferLocation = (tfs[0][10+(dirNamePlace/2)]&0x0F);
		}

		
		int remainingBytes = size;
		int contentsIndex = 0;
		
		

		for(int i=11;i<16;i++){
			tfs[tfs[0][0]][i] = contents[contentsIndex];
			remainingBytes--;
			
			contentsIndex++;
		}
		tfs[0][0]++;

		for(int i = 0;remainingBytes>0;i++){
			
			if( (i%2) == 1) {
				tfs[bufferLocation][1+(i/2)] = (tfs[bufferLocation][1+(i/2)]) | (tfs[0][0]);
			}
			else{
				tfs[bufferLocation][1+(i/2)] = (tfs[bufferLocation][1+(i/2)]&0xF0) | (tfs[0][0]<< 4);
				
			}
			
			for (int j=0;j<16;j++){
				
				tfs[tfs[0][0]][j] = contents[contentsIndex++];
				remainingBytes--;
				if (remainingBytes==0){
					break;
				}
			}

			tfs[0][0]++;
			

		}
		
		return 1;
	}

	


	}

	

	else{
		//handles multiple file paths
		while(pathiterator<strlen(tfsPath)-1){
		
			if (iterator==10){
				return 0;
			}
		
			if(tfs[dirToCheck][iterator]==tfsPath[pathiterator]){
			
				if( (iterator%2) == 1) {
				
					dirToCheck = (tfs[dirToCheck][10+(iterator/2+1)]&0xF0)>>4;
				}
				else{
					dirToCheck = (tfs[dirToCheck][10+(iterator/2)]&0x0F);

				}
			
				pathiterator=pathiterator+2;
				iterator=0;
			}

			iterator++;
		}
		iterator=0;
		while(dirNamePlace==0){
		
		if (tfs[dirToCheck][iterator]==0){
			dirNamePlace=iterator;
		}
		if(tfs[dirToCheck][iterator]==tfsPath[pathiterator]){
			return -1;
		}

		
		iterator++;
	}


		int size = lseek(file, 0, SEEK_END);
		lseek(file, 0, SEEK_SET);

		if(size==-1){
			return 0;
		}
		char* contents = malloc(254*sizeof(char));
	
		read(file,contents,size);
		close(file);


		if(size<=15){
		//Bug with size placement
		//handles basic files sizes
		
		tfs[tfs[0][0]][0] = size;

		for(int i=0;i<strlen(contents);i++){
			
			tfs[tfs[0][0]][i+1] = contents[i];
		}

		if( (dirNamePlace%2) == 1) {
			tfs[dirToCheck][10+dirNamePlace/2+1] = (tfs[dirToCheck][10+(dirNamePlace/2+1)]&0x0F) | (tfs[0][0]<< 4);

		}
		else{
			tfs[dirToCheck][10+(dirNamePlace/2)] = (tfs[dirToCheck][10+(dirNamePlace/2)]&0xF0) | (tfs[0][0]);
		
		}
		
		tfs[dirToCheck][dirNamePlace] = tfsPath[pathiterator];
		tfs[0][0]++;
	
		return 1;
	}

	else{
		//handles larger file sizes
		
		int bufferLocation = 0;
		
		
		
		if (size>(16-tfs[0][0])*16){
			return 7;
		}
		
		tfs[tfs[0][0]][0] = size;

		tfs[dirToCheck][dirNamePlace] = tfsPath[pathiterator];

		if( (dirNamePlace%2) == 1) {
			tfs[dirToCheck][10+dirNamePlace/2+1] = (tfs[dirToCheck][10+(dirNamePlace/2+1)]&0x0F) | (tfs[0][0]<< 4);
			bufferLocation = (tfs[dirToCheck][10+dirNamePlace/2+1]&0xF0)>>4;
		}
		else{
			tfs[dirToCheck][10+(dirNamePlace/2)] = (tfs[dirToCheck][10+(dirNamePlace/2)]&0xF0) | (tfs[0][0]);
			bufferLocation = (tfs[dirToCheck][10+(dirNamePlace/2)]&0x0F);
		}

		
		int remainingBytes = size;
		int contentsIndex = 0;
		
		

		for(int i=11;i<16;i++){
			tfs[tfs[0][0]][i] = contents[contentsIndex];
			remainingBytes--;
			
			contentsIndex++;
		}
		tfs[0][0]++;

		for(int i = 0;remainingBytes>0;i++){
			
			if( (i%2) == 1) {
				tfs[bufferLocation][1+(i/2)] = (tfs[bufferLocation][1+(i/2)]) | (tfs[0][0]);
			}
			else{
				tfs[bufferLocation][1+(i/2)] = (tfs[bufferLocation][1+(i/2)]&0xF0) | (tfs[0][0]<< 4);
			}
			
			for (int j=0;j<16;j++){
				
				tfs[tfs[0][0]][j] = contents[contentsIndex++];
				remainingBytes--;
				if (remainingBytes==0){
					break;
				}
			}

			tfs[0][0]++;
			

		}
		
		return 1;
		

	}


	}




return 0;
}


int export(char* tfsPath, char* fileName){

	if(validateTFSPath(tfsPath)==0){
		prints("bad path");
		println();
		return 2;
	}
	

	int dirNamePlace = 0;
	int iterator = 1;
	int dirToCheck = 0;
	int pathiterator = 1;

	

	if(strlen(tfsPath)==2){
		int file = open(fileName,O_RDWR, 0777);
		if (file==-1){
			file = open(fileName, O_CREAT | O_TRUNC | O_RDWR, 0777);
		}
		//handle case for base file path
		while(dirNamePlace==0){
	
			if (iterator==10){
				return 0;
			}
		
			if(tfs[0][iterator]==tfsPath[1]){
				dirNamePlace = iterator;
			}
		
		iterator++;
	}

	

	int size = 0;
	int row =0;
	
	if(size<15){
		
		if( (dirNamePlace%2) == 1) {
			row = ((tfs[0][10+(dirNamePlace/2+1)]&0xF0)>>4);
			size = tfs[row][0];
			
		}

		else{
			row = ((tfs[0][10+(dirNamePlace/2)]&0x0F));
			size = tfs[row][0];

		}

		for (int i=1;i<size;i++){
			
			int n = write(file, &tfs[row][i],1);
			if(n==-1){
				return 0;
			}

		}
		close(file);
	
	return 1;
	}
	else{
		//handle larger files
		
		
		if (size>(16-tfs[0][0])*16){
			return 7;
		}
		

		if( (dirNamePlace%2) == 1) {
			row = ((tfs[0][10+(dirNamePlace/2)]&0x0F));
			size = tfs[row][0];
		}
		else{
			row = ((tfs[0][10+(dirNamePlace/2)]&0x0F));
			size = tfs[row][0];
		}

		
		int remainingBytes = size;
		
		

		for(int i=11;i<16;i++){
			int n = write(file, &tfs[row][i],1);
			if(n==-1){
				return 0;
			}
			remainingBytes--;
			
		}
		row++;
		

		for(row=row;remainingBytes>0;row++){
			prints("test");
			for (int j=0;j<16;j++){
				int n = write(file, &tfs[row][j],1);
				if(n==-1){
					return 0;
				}
				remainingBytes--;
				if (remainingBytes==0){
					break;
				}
			}

			

		}
		close(file);
		return 1;



	}

	}
	else{
		//handles multiple file paths
		int file = open(fileName,O_RDWR, 0777);
		if (file==-1){
			file = open(fileName, O_CREAT | O_TRUNC| O_RDWR, 0777);
		}

		while(pathiterator<strlen(tfsPath)-1){
		
			if (iterator==10){
				
				println();
				return 0;
			}
		
			if(tfs[dirToCheck][iterator]==tfsPath[pathiterator]){
			
				if( (iterator%2) == 1) {
				
					dirToCheck = (tfs[dirToCheck][10+(iterator/2+1)]&0xF0)>>4;
				}
				else{
					dirToCheck = (tfs[dirToCheck][10+(iterator/2)]&0x0F);

				}
			
				pathiterator=pathiterator+2;
				iterator=0;
			}

			iterator++;
		}
		iterator=0;
		while(dirNamePlace==0){
		
			if (iterator==10){
					
					println();
					return 0;
				}

			if(tfs[dirToCheck][iterator]==tfsPath[pathiterator]){
				dirNamePlace = iterator;
			}
		
		iterator++;
		}

		int size = 0;
		int row =0;
		if(size<15){

		if( (dirNamePlace%2) == 1) {
			row = ((tfs[dirToCheck][10+(dirNamePlace/2+1)]&0xF0)>>4);
			size = tfs[row][0];

		}

		else{
			row = ((tfs[dirToCheck][10+(dirNamePlace/2)]&0x0F));
			size = tfs[row][0];

		}

		for (int i=1;i<size;i++){
			int n = write(file, &tfs[row][i] ,1);
			if(n==-1){
				
				return 0;
			}
		}

		close(file);
	
		return 1;
	}
	else{
		//handle larger files

		if (size>(16-tfs[0][0])*16){
			return 7;
		}
		

		if( (dirNamePlace%2) == 1) {
			row = ((tfs[dirToCheck][10+(dirNamePlace/2)]&0x0F));
			size = tfs[row][0];
		}
		else{
			row = ((tfs[dirToCheck][10+(dirNamePlace/2)]&0x0F));
			size = tfs[row][0];
		}

		
		int remainingBytes = size;
		
		

		for(int i=11;i<16;i++){
			int n = write(file, &tfs[row][i],1);
			if(n==-1){
				return 0;
			}
			remainingBytes--;
			
		}


		for(int i = 0;remainingBytes>0;i++){
			
			if( (i%2) == 1) {
				row = (tfs[row][1+(i/2)]) | (tfs[0][0]);
			}
			else{
				row = (tfs[row][1+(i/2)]&0xF0) | (tfs[0][0]<< 4);
			}
			
			for (int j=0;j<16;j++){
				int n = write(file, &tfs[row][j],1);
				if(n==-1){
					return 0;
				}
				remainingBytes--;
				if (remainingBytes==0){
					break;
				}
			}

			

		}
		close(file);
		return 1;

	}


		
	}

	return 4;
	
}

int ls(char* tfsPath){

	if(validateNewDir(tfsPath)==0&& strcmp("root",tfsPath) !=0 ){
		return 2;
	}

	int dirNamePlace = 0;
	int iterator = 1;
	int dirToCheck = 0;
	int pathiterator = 1;
	

	//if root
	if(strcmp("root",tfsPath) ==0){

		
		println();

		for (int i=1;i<11;i++){
			printc(i+47);
			prints(" | Letter: ");
			printc(tfs[0][i]);
			prints(" | Root: ");
			if( (i%2) == 1) {
				printc(getUpperBitsAsHex(tfs[0][10+(i/2+1)]));
			}
			else{
				printc(getLowerBitsAsHex(tfs[0][10+(i/2)]));
			}
			println();
		}
		
		return 1;
	}

	//if one path
	if(strlen(tfsPath)==2){
		while(dirNamePlace==0){
		
			if (iterator==10){
				return 0;
			}
			if(tfs[0][iterator]==tfsPath[1]){
				dirNamePlace=iterator;
			}
			iterator++;
		}

		int row =0;
		
		if( (dirNamePlace%2) == 1) {
			row = ((tfs[0][10+(dirNamePlace/2+1)]&0xF0)>>4);

		}
		else{
			row = ((tfs[0][10+(dirNamePlace/2)]&0x0F));
		
		}

		for (int i=1;i<11;i++){
			printc(i+47);
			prints(" | Letter: ");
			printc((tfs[row][i]));
			prints(" | Root: ");
			if( (i%2) == 1) {
				printc(getUpperBitsAsHex(tfs[row][10+(i/2+1)]));
				
			}
			else{
				printc(getLowerBitsAsHex(tfs[row][10+(i/2)]));
			}
			println();
		}
	return 1;

	}
	else{
		//handle multiple paths
			while(pathiterator<strlen(tfsPath)-1){
		
			if (iterator==10){
				return 0;
			}
		
			if(tfs[dirToCheck][iterator]==tfsPath[pathiterator]){
			
				if( (iterator%2) == 1) {
				
					dirToCheck = (tfs[dirToCheck][10+(iterator/2+1)]&0xF0)>>4;
				}
				else{
					dirToCheck = (tfs[dirToCheck][10+(iterator/2)]&0x0F);

				}
			
				pathiterator=pathiterator+2;
				iterator=0;
			}

			iterator++;
		}
		iterator=0;
		while(dirNamePlace==0){
		
			if (iterator==10){
					return 0;
				}

			if(tfs[dirToCheck][iterator]==tfsPath[pathiterator]){
				dirNamePlace = iterator;
			}
		
		iterator++;
		}

		int row =0;
		
		if( (dirNamePlace%2) == 1) {
			row = ((tfs[dirToCheck][10+(dirNamePlace/2+1)]&0xF0)>>4);

		}
		else{
			row = ((tfs[dirToCheck][10+(dirNamePlace/2)]&0x0F));
		
		}

		for (int i=1;i<11;i++){
			printc(i+47);
			prints(" | Letter: ");
			printc((tfs[row][i]));
			prints(" | Root: ");
			if( (i%2) == 1) {
				printc(getUpperBitsAsHex(tfs[row][10+(i/2+1)]));
				
			}
			else{
				printc(getLowerBitsAsHex(tfs[row][10+(i/2)]));
			}
			println();
		}


		return 1;
	}

return 0;
}



// TODO: many more functions, to handle other commands