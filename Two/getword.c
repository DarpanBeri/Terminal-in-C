/*

Name:           Darpan Beri
Professor:      John Carroll
Class:          CS 570
File:           getword.c
Description:    A lexical analyzer based on the given specifications
                based off the prompt in getword.h
*/

/* Include Files */
#include "getword.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int metaCharFlag;
int tildeFlag;

int getword(char *w){

        /*
        * The getword() function gets one word from the input stream.
        * It returns -255 if end-of-file is encountered, otherwise it returns the number of characters in the word.
        * There is exception to this based off metacharacters detected by getword().
        */

    int iochar; // Integer representation of a char from stdin
    int prevChar; // Integer representation of a char used to store the previously processed char
    int length = 0; // Integer to track the length of the word
    int returnNeg = 0; // Binary variable to track for special case of encountering a'$'

    iochar = getchar();

    while ( iochar == 32 || iochar == 9 ) iochar = getchar(); // Skips leading tabs and blanks
    
    while ( iochar != EOF ){
    
        if( iochar == 36 ){ // if '$' found
                // Check if '$' is part of an existing word or a beginning of a new word
                // The prevChar method is a legacy method from p0
                w--;
                prevChar = (int)*w;
                w++;
                if( prevChar == 32 || prevChar == 9 || prevChar == 0){ // i.e. If prevChar is " $" or "^I$" or NULL
                        iochar = getchar(); // Looking ahead of the '$'
                        if( iochar == 32 || iochar == EOF || iochar == 10 ){ // Found Space / EOF / Line Feed after the '$'
                                (void)ungetc(iochar, stdin); // Put the upcoming char back to stdin be processed next go around
                                *w = (char)0;
                                //metaCharFlag = 0; Not needed
                                return length;
                        }else{
                                returnNeg = 1;
                        }
                }
        }
        
        if( iochar == 126 ){ // If '~' encountered
                // Check if the metachar is part of an existing word or a beginning of a new word
                
                if( length == 0 && returnNeg == 0 ){
                        iochar = getchar();
                        tildeFlag = 1;
                        
                        
                        if( iochar == -1 ){ // Special case when error is encountered
                                // Standard template to return getword()
                                *w = (char)0; // NULL to terminate the word
                                if( returnNeg == 1 ){
                                        return -length; // Special case
                                }else{
                                        return length;
                                }
                        }
                }
                /*
                if( length == 0 && returnNeg == 0){
                        //char *home;
                        char *home = getenv("HOME"); // Acquire the address of the $HOME directory
                        size_t len = strlen(home); // Find the length of the address of $HOME
                        int temp; // Temp variable for copying over the address into the getword's "word"
                        int count = 0; // Count for the purpose of the for loop
                        
                        // Check if the full length of the address can be added on to the "word" of getword()
                        if( length+len > STORAGE - 2 ){ // If unable to fit,
                                // Do 2 main operations,
                                // Add all the possible chars to getword()'s word
                                // Put the rest of the address of $HOME in stdin to be processed in the next go of getword()
                                int difference = length + (int)len - STORAGE - 2;
                                // Add all possible chars to the word
                                while( length != STORAGE - 2 ){
                                        temp = (int)*home;
                                        length++;
                                        *w = (char)temp;
                                        w++;
                                        home++;
                                        count++;
                                }
                                
                                count = 0; // reset count
                                // Add rest of the remaining chars to stdin
                                while( count != difference ){
                                        temp = (int)*home;
                                        (void)ungetc(temp, stdin); // Put the char back to be processed next round
                                        home++;
                                        count++;
                                }
                                
                                // Standard template to return getword()
                                *w = (char)0; // NULL to terminate the word
                                if( returnNeg == 1 ){
                                        return -length; // Special case
                                }else{
                                        return length;
                                }
                                
                        }else{ // If the length of $HOME can be filled in the word
                                while( count != (int)len){
                                        temp = (int)*home;
                                        length++;
                                        *w = (char)temp;
                                        w++;
                                        home++;
                                        count++;
                                }
                        }
                        iochar = getchar(); // Move on to the next char from '~'
                        if( iochar == -1 ){ // Special case when error is encountered
                                // Standard template to return getword()
                                *w = (char)0; // NULL to terminate the word
                                if( returnNeg == 1 ){
                                        return -length; // Special case
                                }else{
                                        return length;
                                }
                        }
                }*/
        }
        
        if( iochar == 60 ){ // If '<' encountered
                // check if part of an existing word
                if( length == 0 ){
                        // Check if next char is also '<'
                        iochar = getchar(); // seeking forward
                        if( iochar == 60 ){ // If '<<', which is another metachar
                                // return the '<<'
                                length = length + 2;
                                *w = (char)iochar;
                                w++;
                                *w = (char)iochar;
                                w++;
                                
                                // Standard template to return getword()
                                *w = (char)0; // NULL to terminate the word
                                
                                if( returnNeg == 1 ){
                                        return -length; // Special case
                                }else{
                                        return length;
                                }
                        }else{
                                (void)ungetc(iochar, stdin); // put the seeked char back to stdin to be processed next go around
                                // Return '<'
                                iochar = 60;
                                length++;
                                *w = (char)iochar;
                                w++;
                                
                                // Standard template to return getword()
                                *w = (char)0; // NULL to terminate the word
                                metaCharFlag = 0;
                                if( returnNeg == 1 ){
                                        return -length; // Special case
                                }else{
                                        return length;
                                }
                        }
                }else{ // Part of an existing word
                        // Check if next char is also '<'
                        iochar = getchar(); // Seeking forward
                        if( iochar == 60 ){ // If '<<'
                                /*
                                "<<" terminates the word
                                */
                                
                                // Put the '<<' back in the input stream to be processed as one whole metachar
                                (void)ungetc(iochar, stdin); // Two time for < each
                                (void)ungetc(iochar, stdin);
                                
                                // Standard template to return getword()
                                *w = (char)0; // NULL to terminate the word
                                
                                if( returnNeg == 1 ){
                                        return -length; // Special case
                                }else{
                                        return length;
                                }
                        }else{
                                (void)ungetc(iochar, stdin); // Put the seeked char back to stdin to be processed next go around
                                iochar = 60;
                                (void)ungetc(iochar, stdin); // Put the '<' back to the stdin to be processed next go around
                                
                                // Standard template to return getword()
                                *w = (char)0; // NULL to terminate the word
                                if( returnNeg == 1 ){
                                        return -length; // Special case
                                }else{
                                        return length;
                                }
                        }
                }
        }
        
        if( iochar == 62 ){ // If '>' found
                // Check if part of a word
                if( length == 0 ){
                        length++;
                        *w = (char)iochar;
                        w++;
                        
                        // Standard template to return getword()
                        *w = (char)0; // NULL to terminate the word
                        metaCharFlag = 0;
                        if( returnNeg == 1 ){
                                return -length; // Special case
                        }else{
                                return length;
                        }
                }else{ // part of a word
                        (void)ungetc(iochar, stdin); // Put it back to stdin to be processed again next call
                        
                        // Standard template to return getword()
                        *w = (char)0; // NULL to terminate the word
                        if( returnNeg == 1 ){
                                return -length; // Special case
                        }else{
                                return length;
                        }
                }
        }
        
        if( iochar == 124 ){ // If '|' found
                // Check if part of a word
                if( length == 0 ){
                        length++;
                        *w = (char)iochar;
                        w++;
                        
                        // Standard template to return getword()
                        *w = (char)0; // NULL to terminate the word
                        metaCharFlag = 0;
                        if( returnNeg == 1 ){
                                return -length; // Special case
                        }else{
                                return length;
                        }
                }else{ // part of a word
                        (void)ungetc(iochar, stdin); // Put it back to stdin to be processed again next call
                        
                        // Standard template to return getword()
                        *w = (char)0; // NULL to terminate the word
                        if( returnNeg == 1 ){
                                return -length; // Special case
                        }else{
                                return length;
                        }
                }
        }
        
        if( iochar == 38 ){ // If '&' found
                // Check if part of a word
                if( length == 0 ){
                        length++;
                        *w = (char)iochar;
                        w++;
                        
                        // Standard template to return getword()
                        *w = (char)0; // NULL to terminate the word
                        metaCharFlag = 0;
                        if( returnNeg == 1 ){
                                return -length; // Special case
                        }else{
                                return length;
                        }
                }else{ // part of a word
                        (void)ungetc(iochar, stdin); // Put it back to stdin to be processed again next call
                        
                        // Standard template to return getword()
                        *w = (char)0; // NULL to terminate the word
                        if( returnNeg == 1 ){
                                return -length; // Special case
                        }else{
                                return length;
                        }
                }
        }
        
        if( iochar == 9 ){ // If '^I', return 0
                // Standard template to return getword()
                *w = (char)0; // NULL to terminate the word
                if( returnNeg == 1 ){
                        return -length; // Special case
                }else{
                        return length;
                }
        }
        
        if( iochar == 32 ){ // If ' ', return 0
                // Standard template to return getword()
                *w = (char)0; // NULL to terminate the word
                if( returnNeg == 1 ){
                        return -length; // Special case
                }else{
                        return length;
                }
        }
        
        if( iochar == 10 ){ // If '\n', return 0
                // Check if its connected to a word
                if( length > 0 ){ // Part of an existing word 
                        (void)ungetc(iochar, stdin); // Put the line feed back to be processed next round
                        
                        // Standard template to return getword()
                        *w = (char)0; // NULL to terminate the word
                        if( returnNeg == 1 ){
                                return -length; // Special case
                        }else{
                                return length;
                        }
                }else{ // Not part of an existing word
                        // Standard template to return getword()
                        *w = (char)0; // NULL to terminate the word
                        if( returnNeg == 1 ){
                                return -length; // Special case
                        }else{
                                return length;
                        }
              }
        }
        
        if( iochar == 92 ){ // Found '\'
        /*
        When "\" precedes a metacharacter, that metacharacter is treated like most other characters.
        */
                iochar = getchar(); // Skip to the next character
                if( iochar == 10 ){ // Special case where the word ends with '\', skip the '\'
                        // Standard template to return getword()
                        *w = (char)0; // NULL to terminate the word
                        if( returnNeg == 1 ){
                                return -length; // Special case
                        }else{
                                return length;
                        }
                }
        }
        
        // Add the char to the word in getword()
        length++;
        *w = (char)iochar;
        w++;
        
        if( length > STORAGE - 2 ){ //Buffer overflow check
                // Standard template to return getword()
                *w = (char)0; // NULL to terminate the word
                if( returnNeg == 1 ){
                        return -length; // Special case
                }else{
                        return length;
                }
        }
        iochar = getchar(); // Seek to the next char
    }
    
    if( length > 0 ){ // If part of an existing word
    
        (void)ungetc(iochar, stdin); // put the char back to the stdin to be processed in the next round
        // Standard template to return getword()
        *w = (char)0; // NULL to terminate the word
        if( returnNeg == 1 ){
                return -length; // Special case
        }else{
                return length;
        }               
    }else{
        *w = (char)0; // NULL to terminate the word
        return -255; // Found 'EOF' return -255
    }
}
