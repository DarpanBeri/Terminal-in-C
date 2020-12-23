/*

Name:           Darpan Beri
Professor:      John Carroll
Class:          CS 570
File:           p2.h
Description:    Header file for the program in p2.c

*/

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <pwd.h>
#include <libgen.h>
#include "getword.h"

#define MAXITEM 100 /* max number of words per line */
#define TRUE 1;
#define FALSE 0;
#define PATH_MAX        4096    /* # chars in a path name including nul */

extern int metaCharFlag; // For communicating with getword()
extern int tildeFlag;

int parse(char* w);
/*
int parse();

Arguments:      Char pointer from main
Returns:        1 if successfully parsed, 0 in case of syntax error and -1 incase of EOF
Description:    Parses input from stdin to be processed for the basic terminal created in p2.c
*/

void myHandler(int signum);
/*
void myHandler;

Arguments:      int signum
Returns:        Nothing, its a void
Description:    A handler for SIGTERM
*/

int file_exists(const char *file_name){
	return access(file_name, F_OK);
}
/*
int file_exists

Argument:        const char pointer
Returns:         int with the return value of access
Description:     Checks for file's existence
*/
