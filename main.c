#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>


#include "tfs.h"
#include "io.h"

#define WHITESPACE " \t\r\n"

int main() {

  int done = 0;
  char* tfs_file = NULL;

  do {

    // prompt is current open file, or none
    char* prompt = tfs_file ? tfs_file : "(no file)";

    // print the prompt, then get a line of input from the user
    prints(prompt);
    prints("> ");
    char line[IN_SIZE];
    inputs(line, IN_SIZE);

    // flag denoting we changed the open TFS, and need to save it to the file
    // after this command has been processed
    int altered = 0;

    char* command = strtok(line, WHITESPACE); // user's command
    char* arg1 = strtok(NULL, WHITESPACE);
		char* arg2 = strtok(NULL, WHITESPACE);
		//char* arg3 = strtok(NULL, WHITESPACE);
		// first argument
    // TODO: some commands need one more argument

    if (!strcmp("exit", command)) {
      done = 1;
    }
    else if (!strcmp("display", command)) {
      // most commands don't make sense if there's no open file
      if (!tfs_file) {
        prints("No file open.");
        println();
        continue;
      }
      display(); // display the TFS as columns of hex digits
    }
    else if (!strcmp("create", command)) {
      if (!arg1) {
        prints("Need a filepath.");
        println();
        continue;
      }
      int temp = open(arg1, O_RDONLY);
      if (temp != -1) {
        prints("File already exists: ");
        prints(arg1);
        println();
        close(temp); // close inadvertently opened file
        continue;
      }
      free(tfs_file); // free previous string (if any)
      tfs_file = strdup(arg1); // save path for prompt and writing
      init(); // initialize new TFS
      altered = 1; // flag for saving to file
    }

    else if (!strcmp("open", command)) {
      if (!arg1) {
        prints("Need a filepath.");
        println();
        continue;
      }
      int temp = open(arg1, O_RDONLY);
      if (temp == -1) {
        prints("Error opening file: ");
        prints(arg1);
        println();
        continue;
      }
      // load TFS from new file (previous file already saved)
      read(temp, (char*)tfs, 256);
      close(temp);
      free(tfs_file); // free previous string
      tfs_file = strdup(arg1); // save path for prompt and writing
    }
		else if (!strcmp("mkdir", command)) {
      if (!arg1) {
        prints("Need a filepath.");
        println();
        continue;
      }
      // load TFS from new file (previous file already saved)
      int i = makedir(arg1);
			if(i!=1){
				prints("Could not make Directory: ");
				prints(arg1);
        println();
			}
			else{
				altered = 1;
			}
			
			
    }
		else if (!strcmp("import", command)) {

      if (!arg1) {
        prints("Need a filepath.");
        println();
        continue;
      }
			if (!arg2) {
        prints("Need a tfs file name.");
        println();
        continue;
      }
      // load TFS from new file (previous file already saved)
		
			
      int i = import(arg1,arg2);
			if (i==7){
				prints("TFS file too big: ");
				prints(arg1);
        println();
			}
			if(i!=1){
				prints("Could not make tfs file: ");
				prints(arg1);
        println();
			}
			else{
				altered = 1;
			}
			
			
    }
		else if (!strcmp("export", command)) {

      if (!arg1) {
        prints("Need a tfs file name.");
        println();
        continue;
      }
			if (!arg2) {
        prints("Need a filepath.");
        println();
        continue;
      }
      // load TFS from new file (previous file already saved)
      int i = export(arg1,arg2);
			if(i!=1){
				
				prints("Could not make file: ");
				prints(arg2);
        println();
			}
			else{
				altered = 1;
			}
			
			
    }
		else if (!strcmp("ls", command)) {
			
      if (!arg1) {
				char n[] = "root";
        arg1 = n;
      }

      // load TFS from new file (previous file already saved)
      int i = ls(arg1);
			if(i!=1){
				
				prints("Could not read dir: ");
				prints(arg1);
        println();
			}
			
			
    }
    // TODO: more commands



    else {
      prints("Command not recognized: ");
      prints(command);
      println();
    }

    if (altered) {
      // save changes to file
		int file = open(tfs_file,O_WRONLY, 0777);
		if (file==-1){
			open(tfs_file, O_CREAT | O_TRUNC | O_WRONLY, 0777);
		}

		write(file,(char*)tfs, 256);

		close(file);
      // TODO: your code here
    }

  } while (!done);

  return 0;
}
