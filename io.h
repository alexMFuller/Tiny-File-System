#include "tfs.h"

#define IN_SIZE 256

// prints a single character to standard out
void printc(char c);

// prints the given string to standard out
void prints(char* s);

// prints a newline to standard out
void println();

// reads up to "size" characters from standard input into dst array
// if standard input contains a newline, replaces it with the null character
// regardless, dst must be null-terminated after this is called
void inputs(char dst[], int size);

