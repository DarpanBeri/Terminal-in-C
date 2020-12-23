/*

Name:           Darpan Beri
Professor:      John Carroll
Class:          CS 570
File:           p2.c
Description:    Source code for a basic linux shell which can execute legal commands and can do process redirection.

Exit Values:
        0 - Success
        1 - Unable to fork a child
        2 - Unable to execute a given command
        3 - Unable to overwrite existing file
        4 - Unable to locate input file
        5 - Unable to fork a grandchild
        6 - Child unable to locate input file after pipeing
        7 - Child could not exectue a given command after pipeing
        8 - Grandchild Unable to locate input file
        9 - Grandchild could not execute the given command
        
        10 - File already exists for pipeing
        11 - Cannot open file for pipeing
        12 - Output redirection failed with dup2
        13 - Cannot access input file while pipeing
        14 - Input redirection failed with dup2
        15 - Failed nested pipeline
        16 - Unable to fork in nested pipelines
        17 - dup2 failed in nested pipelines
        18 - Exec failed in nested pipelines
        19 - fopen failed in hereis
        20 - Unable to read temp file in hereis
        21 - hereisPtr not found
*/

#include "p2.h"

/* Global variables */
char* newArgv[MAXITEM]; // The list of arguments to be executed
int newArgC = 0; // Size of the new Arg list

int waitFlag = 1; // if 1, then wait else do not wait
int dollarFlag = FALSE; // Flag for dollar sign
int promptChangeFlag = FALSE; // Flag for changing prompt
int slashCaseFlag = FALSE; // Flag for environ HOME
int pipeFlag = 0; // Flag for Piping

char* infilePtr = NULL; // Pointer for the input file
char* outfilePtr = NULL; // Pointer for the output file

int pipeFilePtr = -1; // newArgv of pipe
int pipeIntArr[20]; // Changed from 2 to 20 due to having a possibilities of upto 10 pipes

int wordCount = 0; // Word Count for tracking pipes, used in parse
int location; // Gets locations of the pipes to exec
int pipeLocation[10]; // Location of pipes

//int metaCharFlag; // If 1, no meta char seen, else its 0. Set by getword()
//int tildeFlag;

char cwd[PATH_MAX]; //Path_MAX from <limits.h> . Will use for getcwd()
char *path; // For getcwd storage for prompt manipulation
char *path2; // Temp storage for path
char *bname; // Storage of string to manipulate prompt

int current = 0; // Tracks current pipeline

int usernameLookupFailFlag = FALSE;
int envVarFailFlag = FALSE;

int hereisFlag = FALSE;
int stopReadingFlag = FALSE;
char *hereisPtr;
char *tmpName = "TMPFILE";
FILE *hereisDoc = NULL;

int parse(char* w){
	int tempCount;
        char* buffer[MAXITEM]; // Storage of words to form a line to be executed
	int snafu = FALSE;
;
        ;int bufferCount = 0; // Size of the line so far
	char *name;
        int c; // Return value from getword()
        char * oldW = w; // oldW to reset w post parse
        int i; // Counter for the for loop
        int wordCount = 0;
        
        
	struct passwd *pwd;
	char *inLoopTilde[MAXITEM];
	const char delimiter[2] = "/";
	char *token;	
	char tempTilde[MAXITEM];
	int tildeCount = 0;
	
	int tildeCatcher[MAXITEM];
	int bufferI = 0;
	
	char *envVariable;
	
	int dollarCatcher[MAXITEM];
	char *inLoopDollar[MAXITEM];
	int dollarCount = 0;
	int bufferi = 0;
	int dollarFlag = FALSE;
        
        // Using a infinite loop to get all words from getword()
        for(;;){
                c = getword(w);
                
                if( c == -255 ) return -1; // EOF reached
                else if (c == 0) break; // Newline reached
                
                buffer[bufferCount++] = w; // Storing of the word
                
                if (tildeFlag){
                        tildeFlag = FALSE;
                        snafu = TRUE;
                        inLoopTilde[tildeCount++] = w;
                        tildeCatcher[bufferI++] = bufferCount - 1;
                }
                
                ////////////// HEREIS STUFF TO BE ADDED ///////
                // wordCount for HEREIS
                if( c < 0 ){ // $ case where it returns a neg return value
                        c = abs(c);
                        dollarFlag = TRUE;
                        inLoopDollar[dollarCount++] = w;
                        dollarCatcher[bufferi++] = bufferCount - 1;
                }
                w += c + 1; // Incrementing the pointer to the word
        }
        int j = 0;
        int k = 0;
        // Parsing the stdin input
        for(i=0; i < bufferCount; i++){
        
                if( dollarFlag && dollarCatcher[k] == i ){
                        
                        
                        envVariable = getenv(inLoopDollar[k]);
                        if( envVariable == -1 || envVariable == NULL){
                                fprintf(stderr, "%s: Undefined variable.\n", inLoopDollar[k]);
                                envVarFailFlag = TRUE;
                        }
                        if( outfilePtr != NULL ){ // Not sure about this
                                outfilePtr = envVariable;
                                wordCount++;
                        } else{
                                wordCount++;
                                newArgv[newArgC++] = envVariable;
                        }
                        k++;
                        continue;
                }

                if( snafu && tildeCatcher[j] == i){
                                
                                name = inLoopTilde[j];
                                pwd = getpwnam(name);
                                //printf("Name is: %s \n", name);
                                if(pwd == NULL){
			                token = strtok(name, delimiter);
			                pwd = getpwnam(token);
			                //printf("pwd in if is: %s \n", pwd);
			                if(pwd == NULL){

				                perror("Couldn't find username directory");

				                usernameLookupFailFlag = TRUE; 

			                }
			                else{
				                /* Copying the 6th field of password struct from /etc/passwd
				                 * and putting them together into one string called tempTilde
				                 * using strncat.
				                 */
				                strncpy(tempTilde,pwd->pw_dir,MAXITEM - 1);
				                token = strtok(NULL, delimiter);
				                strncat(tempTilde, "/", MAXITEM - strlen(tempTilde) - 1);
				                strncat(tempTilde, token, MAXITEM -strlen(tempTilde) - 1);
				                wordCount++;
				                newArgv[newArgC++] = tempTilde;
			                }
			        }else{
			                wordCount++;
			                newArgv[newArgC++] = pwd->pw_dir;
                                        
			        }
			        
			        j++;
			        continue;
                        }
                //}
        
                if( strcmp(buffer[i], "<") == 0 && metaCharFlag == 0){
                
                        infilePtr = buffer[i+1]; // Setting the next word as the input
                        i++; // Proceeding to the next word
                        
                }else if( strcmp(buffer[i], ">") == 0 && metaCharFlag == 0){
                        
                        outfilePtr = buffer[i+1]; // Setting the next word as the output file
                        i++; // Proceeding to the next word
                        
                }else if( strcmp(buffer[i], "|") == 0 && metaCharFlag == 0){
                        
                        if( pipeFlag == 0 ){
                                location = wordCount;
                        } else{
                                location = wordCount + pipeFlag;
                        }
                        pipeLocation[pipeFlag++] = location;
                        
                        // pipeFilePtr = newArgC+1; // Set the pipe process to the next word - p2 method
                        
                        newArgv[newArgC++] = NULL;          
                
                }else if( i+1 == bufferCount && strcmp(buffer[i], "&") == 0 && metaCharFlag == 0){
                        
                        waitFlag = 0;
                        
                }else if(strcmp(buffer[i], "$") == 0){

                        newArgv[newArgC++] = NULL;
                        
                }else if( strcmp(buffer[i], "<<") == 0 ){
                        
                        if( hereisFlag ){
                                return -1;
                        }
                        
                        hereisFlag = TRUE;
                        hereisPtr = buffer[i+1];
                        if( hereisPtr == NULL ){
                                perror("hereisPtr not found");
                                return -1;
                        }else{
                                int tempLen = strlen(hereisPtr);
                                hereisPtr[tempLen] = '\n';
                                hereisPtr[tempLen + 1] = '\0';
                        }
                }else{
                        wordCount++;
                        newArgv[newArgC++] = buffer[i];
                        
                }
        }
        if(hereisFlag && hereisPtr != NULL){
                stopReadingFlag = TRUE;
        }
        w = oldW; // Resetting w
        newArgv[newArgC] = NULL; // Null terminate the array for processing
        return 1; // Everything went swell, return 1
}

void myHandler(int signum){
}

int main(int argc, char* argv[])
{
        int cPid; // Pid of a child
        int gcPid; // Pid of a grandchild
        
        int inputFile; // File Descriptor for input file
        int outputFile; // File Descriptor for output file
        int inFFlags = O_RDONLY; // Read only flags for input files
        int outFFlags = O_WRONLY | O_CREAT; // Write only and create flags for output files
        
        int c; // Return value from parse
        char s[STORAGE*MAXITEM]; // Storage for a command
        
        (void) setpgid(0,0); // getting p2 into its own group
        (void) signal(SIGTERM, myHandler); // Catching SIGTERM
        
        

        for(;;){ // Looping the prompt and terminal
                newArgC = 0; // Resetting of newArgC for next command
                waitFlag = 1;
                infilePtr = NULL;
                outfilePtr = NULL;
                pipeFilePtr = -1;
                
                metaCharFlag = 1;
                
                pipeFlag = 0;
  
  ///////////////////// Prompt Manipulation //////////////////////////            
                if( promptChangeFlag ){
                        if( slashCaseFlag ){
                                (void) printf("/");
                                slashCaseFlag = FALSE;
                        } else {
                                (void) printf(bname);
                        }
                        (void) printf(":570: "); // Issues prompt
                
                }else{
                        (void) printf(":570: "); // Issues prompt
                }
                
                if(stopReadingFlag){
                        break;
                }
                
                c = parse(s); // Parse the input from stdin
                
                if( newArgC == 0 && c == -1){ // if (first word is EOF) break;
                        break;
                }
                
                if( c == 0 ){ // If parse returns an error
                        perror("Syntax Error");
                        continue;
                }
                
                if( newArgC == 0 ) continue; // if (line is empty) continue;
///////////////////// 'cd' command //////////////////////////

                // handling builtins (cd) and continue, or:
                if( strcmp(newArgv[0], "cd") == 0 ){ // if cd command
                        promptChangeFlag = TRUE;
                
                        if( newArgC == 1 ){ // No arguments
                                if( chdir(getenv("HOME")) == -1 ){ // if chdir fail
                                
                                        perror("Unable to change directory"); // chdir failed
                                
                                }
                                
                                path = getcwd(cwd, sizeof(cwd)); // Get absolute pathname of current directory
                                if( path != NULL ){
                                        path2 = strdup(path);
                                        bname = basename(path2); // For p4 specifications
                                } else{
                                        perror("getcwd() Failed");
                                }
                                
                        }else if( newArgC > 2 ){ // Too many arguments for cd
                                
                                perror("Too many arguments for cd command"); // chdir failed

                        }else{ // legal number of argumend of cd
                                if( chdir(newArgv[1]) == -1 ){ // if chdir failed
                                        
                                        perror("Unable to change directory"); // chdir failed
                                        continue;
                                
                                }
                                
                                path = getcwd(cwd, sizeof(cwd)); // Get absolute pathname of current directory
                                if( path != NULL ){
                                        path2 = strdup(path);
                                        if (strcmp(path2, "/") == 0){
                                                bname = path2;
                                        }else{
                                        bname = basename(path2); // For p4 specifications
                                        }
                                } else{
                                        perror("getcwd() Failed");
                                }
                                
                        }
                        continue;
                }
                
///////////////////// 'environ' command //////////////////////////                
                
                if( strcmp(newArgv[0], "environ") == 0 ){
                        if( newArgC == 2 ){ // environ and 1 argument
                                
                                char *environment = getenv(newArgv[1]);
                                if( environment == NULL ){
                                        (void) printf("\n");
                                } else {
                                        (void) printf("%s\n",environment);
                                }
                        
                        } else if( newArgC == 3 ){ // environ and 2 argument
                                if( strcmp(newArgv[2], "/") == 0 ) {
                                        slashCaseFlag = TRUE;
                                        if(setenv(newArgv[1],"/",1) == -1){
                                                perror("setenv() failed to set the environment variable");
                                        }
                                } else if(setenv(newArgv[1],newArgv[2],1) == -1){
                                        perror("setenv() failed to set the environment variable");
                                }
                        } else {
                                perror("Wrong number of arguments to environ");
                        }
                        continue;
                }
                
                if( usernameLookupFailFlag ){
                        usernameLookupFailFlag = FALSE;
                        continue;
                }
                
                if( envVarFailFlag ){
                        envVarFailFlag = FALSE;
                        continue;
                }
                
                ///////////////// PIPING /////////
                
                (void) fflush(stdout); // Cleaning up all output and errors before fork
                (void) fflush(stderr);
                
                ////// CHILD CREATED ///////
                if (-1 == (cPid = (int) fork())){ // Forking a child and detecting error
                        perror("Unable to fork");
                        exit(1); // Unable to fork child
                } else if(0 == cPid){ // The child succeds this if statement
                        ///// Do redirection stuff char-chee-zero ////
                        
                        if ( outfilePtr != NULL ) {
                                if( file_exists(outfilePtr) == 0 ){
                                        perror("File already exists");
                                        exit(10);
                                }
                                
                                inputFile = open(outfilePtr, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR, S_IWUSR);
                                
                                if( inputFile < 0 ){
                                        perror("Cannot open file");
                                        exit(11);
                                }
                                
                                if( dup2(inputFile, STDOUT_FILENO) < 0 ){
                                        perror("Output redirection failed with dup2");
                                        exit(12);
                                }
                                close(inputFile);
                        }
                        
                        if (infilePtr != NULL){
                                inputFile = open(infilePtr, O_RDONLY);
                                
                                if( inputFile < 0 ){
                                        perror("Cannot access input file");
                                        exit(13);
                                }
                                
                                if( dup2(inputFile, STDIN_FILENO) < 0 ){
                                        perror("Input redirection failed with dup2");
                                        exit(14);
                                }
                                close(inputFile);
                        }
                        /////// HERERIS STUFF/////////
                        
                        if(hereisFlag){
                                
                                char myString[MAXITEM] = "";
                                char iochar = NULL;
                                int tempI = 0;
                                int delimiterLine = 1;
                                FILE *filePtr1;
                                int FDIn; // Used to replace stdin
                                int tempLine = 1;
                                hereisDoc = fopen(tmpName, "w+");
                                if(hereisDoc == NULL){
                                        perror("fopen failed in hereis");
                                        exit(19);
                                }
                                
                                while(iochar != -1){
                                        iochar = getchar();
                                        myString[tempI] = iochar;
                                        tempI++;
                                }
                                myString[tempI]='\0';
                                fputs(myString, hereisDoc);
                                fflush(hereisDoc);
                                
                                //Calc the delimiterLine
                                FILE *fp;
                                int lineNum = 1;
                                char temp[512] = ""; // Dont needed i think, remove in debugging
                                char *line = NULL;
                                ssize_t nread;
                                size_t len = 0;
                                
                                if( (fp = fopen(tmpName, "r")) == NULL ){
                                        perror("Unable to read temp file in hereis");
                                        exit(20);
                                }
                                while( (nread = getline(&line, &len, fp)) != -1 ){
                                        if( (strcmp(line, hereisPtr)) == 0 ){
                                                break;
                                        }
                                        lineNum++;
                                }
                                
                                if( fp ){
                                        fclose(fp);
                                }
                                
                                delimiterLine = lineNum;
                                
                                rewind(hereisDoc);
                                filePtr1 = fopen("replica", "w");
                                
                                iochar = NULL;
                                
                                while(iochar != -1){
                                
                                        iochar = getc(hereisDoc);
                                        if(iochar == '\n'){
                                                tempLine++;
                                        }
                                        
                                        if(tempLine != delimiterLine){
                                                putc(iochar, filePtr1);
                                        }
                                }
                                
                                if( hereisDoc ){
                                        fclose(hereisDoc);
                                }
                                
                                if( filePtr1 ){
                                        fclose(filePtr1);
                                }
                                
                                remove(tmpName);
                                rename("replica", tmpName);
                                
                                FDIn = open(tmpName, O_RDONLY);
                                dup2(FDIn, 0);
                                close(FDIn);
                                
                        }
                        stopReadingFlag = TRUE;
                        
                        if(pipeFlag){ // Initiate pipeing
                                pipe(pipeIntArr);
                                
                                (void) fflush(stdout);
                                (void) fflush(stderr);
                                ////// GC CREATED ///////
                                gcPid = fork(); // Grandkid forked
                                
                                if(gcPid == -1){
                                        perror("Unable to fork a grandchild");
                                        exit(5);
                                }else if(gcPid == 0){ // Grandchild code
                                        if(pipeFlag > 1){
                                        
                                                //nested pipeline code
                                                pid_t greatGCPID; // pid of great grand child(rens)
                                                int offset = 1; // Offsets required for pipes
                                                int i; // For for loop
                                                int startFd = 0; // To get the read end of a pipe
                                                
                                                for( i = pipeFlag - 1; i > 0; i-- ){
                                                        startFd = (2*offset); // Grab the read end
                                                        if( pipe((pipeIntArr + (2 * offset))) == -1 ){
                                                                perror("Failed nested pipeline");
                                                                exit(15);
                                                        }
                                                        
                                                        (void) fflush(stdout);
                                                        (void) fflush(stderr);

                                                        if((greatGCPID = fork()) == -1){
                                                                perror("Unable to fork in nested pipelines");
                                                                exit(16);
                                                        }else if (greatGCPID != 0){
                                                        
                                                                if(dup2(pipeIntArr[startFd - 1], STDOUT_FILENO) < 0){
                                                                        perror("dup2 failed in nested pipelines");
                                                                        exit(17);
                                                                }
                                                                
                                                                if(dup2(pipeIntArr[startFd], STDIN_FILENO) < 0){
                                                                        perror("dup2 failed in nested pipelines");
                                                                        exit(17);
                                                                }
                                                                (void) close(pipeIntArr[startFd - 2]); // Close the read end of inherited pipe
			                                        (void) close(pipeIntArr[startFd - 1]); // Close the write end of inherited pipe
		                                                (void) close(pipeIntArr[startFd]); // Close the read end of own pipe
			                                        (void) close(pipeIntArr[startFd + 1]); // Close the write end of own pipe
			                                        current = pipeLocation[i - 1];
			                                        
			                                        
			                                        
			                                        if(execvp(newArgv[current + 1], newArgv + (current + 1)) == -1){
			                                                
				                                        perror("line 364 Exec failed in nested pipelines");
				                                        exit(18);	
	                                                        }
                                                        }else{
                                                                
                                                                if(i - 1 == 0){
				                                        if(dup2(pipeIntArr[startFd + 1], STDOUT_FILENO) < 0){
					                                        perror("dup2 failed in nested pipelines");
                                                                                exit(17);
				                                        }
				                                        
				                                        (void) close(pipeIntArr[startFd + 1]);
				                                        (void) close(pipeIntArr[startFd]);

				                                        if(execvp(newArgv[0], newArgv) == -1){

                                                                                perror("line 379 Exec failed in nested pipelines");
				                                                exit(18);
				                                        }
			                                        }	
			                                        offset++;
                                                        }
                                                }
                                        
                                        }else{
                                                
                                                (void) dup2(pipeIntArr[1], STDOUT_FILENO); // Redirect the output to stdout
                                                (void) close(pipeIntArr[1]); // Close pipe's reference
                                                (void) close(pipeIntArr[0]);
                                                
                                                if( execvp(newArgv[0], newArgv) == -1 ){
                                                        fprintf(stderr, "Grandchild could not execute the %s command\n", newArgv[0]);
                                                        exit(9);
                                                }
                                        }
                                }else{ // Child's code
                                
                                        current = pipeLocation[pipeFlag - 1];
                                        
                                        (void) dup2(pipeIntArr[0], STDIN_FILENO); // Redirect input to stdin
                                        (void) close(pipeIntArr[0]); // Close pipe's refrence
                                        (void) close(pipeIntArr[1]);
                                        ///////// EXEC HEREIS SECTION //////////
                                        if( execvp(newArgv[current + 1], newArgv + (current + 1)) == -1 ){
                                                fprintf(stderr, "Child could not execute the %s command\n", newArgv[0]);
                                                exit(7);
                                        }
                                }
                        }
                        if(hereisFlag){
                                int tempLen = strlen(*newArgv);
                                newArgv[tempLen-2] = '\0';
                        }
                        // No pipeline so execute code as normal
                        if( execvp(newArgv[0], newArgv) == -1 ){
                                fprintf(stderr, "Could not execute the %s command\n", newArgv[0]);
                                exit(2);
                        }
                        
                        ////// Background check and waiting children to die stuff ///////
                        if( waitFlag == 0 && infilePtr == NULL ){
                                inputFile = open("/dev/null", inFFlags, S_IRUSR | S_IWUSR);
                                (void) dup2(inputFile, STDIN_FILENO);
                                (void) close(inputFile);
                                waitFlag = 1;
                        }
                        
                        exit(0); // The child completed its task and exits successfully
                }else{ // The parent
                        if(waitFlag == 1){
                                for(;;){ // Wait for the child
                                        pid_t currentPid;
                                        currentPid = wait(NULL);
                                        if( currentPid == cPid ) break;
                                }
                        }else{
                                (void) printf("%s [%d]\n", newArgv[0], cPid);
                        }
                }
                
                if(outfilePtr != NULL){
		        outfilePtr = NULL;
	        }
	        if(pipeFlag){
		        pipeFlag = FALSE;
	        }
	        if(infilePtr != NULL){
		        infilePtr = NULL;
	        }
	        /*
	        if(stopReadingFlag){
	                stopReadingFlag = FALSE;
	        }*/
        }
   
   (void) killpg(getpgrp(), SIGTERM);// Terminates any children that are still running.
   printf("p2 terminated.\n");// MAKE SURE this printf comes AFTER killpg
   exit(0);
   return 0;
}
