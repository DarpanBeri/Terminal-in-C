/*

Name:           Darpan Beri
Professor:      John Carroll
Class:          CS 570

*/

/* Include Files */
#include "getword.h"

int getword(char *w){

    int iochar;
    int prevChar;
    int length = 0;
    int returnNeg = 0; // For special case of '$asd' = -3 instead of 3
    iochar = getchar();
    
    while ( iochar == 32 || iochar == 9 ) iochar = getchar();
    
    while ( iochar != EOF ){
    
        if( iochar == 36 ){ // if '$' found
                w--;
                prevChar = *w;
                //printf("The prevChar is: %d \n", prevChar);
                w++;
                if( prevChar == 32 || prevChar == 9 || prevChar == 0){ // i.e. " $" or "^I$"
                        iochar = getchar();
                        if( iochar == 32 || iochar == EOF || iochar == 10 ){ // Found '$ '
                                ungetc(iochar, stdin); // Found ' ', put it back
                                //*w = iochar; // Add '$' to arr
                                //w++;
                                *w = 0;
                                return length;
                        }else{
                                returnNeg = 1;
                        }
                }
        }
    
        if( iochar == 9 ){ // If '^I', return 0
                *w = 0;
                if( returnNeg == 1 ){
                        return -length;
                }else{
                        return length;
                }
        }
        
        if( iochar == 32 ){ // If ' ', return 0
                *w = 0; // End a word
                if( returnNeg == 1 ){
                        return -length;
                }else{
                        return length;
                }
        }
        
        if( iochar == 10 ){ // If '\n', return 0
                if( length > 0 ){
                        ungetc(iochar, stdin);
                        *w = 0; // End a word
                        if( returnNeg == 1 ){
                                return -length;
                        }else{
                                return length;
                        }
                }else{
                        *w = 0; // End a word
                        if( returnNeg == 1 ){
                                return -length;
                        }else{
                                return length;
                        }
              }
        }
        
        if( iochar == 92 ){ // Found '\'
                iochar = getchar();
                if( iochar == 10 ){ // Special case, skip the '\'
                        *w = 0;
                        if( returnNeg == 1 ){
                                return -length;
                        }else{
                                return length; // Return the word
                        }
                }
                ungetc(iochar, stdin);
        }
       
        if( length != 0 ){
                w--;
                prevChar = *w;
                w++;
                if( iochar == prevChar ){
                        iochar = getchar();
                }
                else{
                        length++;
                        *w = iochar;
                        w++;
                        iochar = getchar();
                }
        }
        else{
        length++;
        *w = iochar;
        w++;
        
        iochar = getchar();
        }
    }
    if( length > 0 ){
        ungetc(iochar, stdin);
        *w=0;
        if( returnNeg == 1 ){
                return -length;
        }else{
                return length; // Return the word
        }               
    }else{
        *w=0;
        return -255; // Found 'EOF' return -255
    }
}
