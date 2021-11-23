#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "io.h"

void printc(char c) {
  // your code here
	 write(STDOUT_FILENO, &c , 1);
}

void prints(char* s) {
  write(STDOUT_FILENO, s, strlen(s));
}

void println() {
  printc('\n');
}

// reads up to "size" characters from standard input into dst array
// if standard input contains a newline, replaces it with the null character
// regardless, dst must be null-terminated after this is called
void inputs(char dst[], int size) {
  // your code here
	read(STDIN_FILENO, dst , size);
	for(int i = 0;i<strlen(dst);i++){
		if (dst[i]=='\n'){
			dst[i]= 0;
		}
	}
	dst[strlen(dst)] = 0;
	
}

