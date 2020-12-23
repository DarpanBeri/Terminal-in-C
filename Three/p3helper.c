/* p3helper.c
 * Darpan Beri
 * Program 3
 * CS570
 * John Carroll
 * SDSU
 * October 30 2020
 *
 * This is the only file you may change. (In fact, the other files should
 * be symbolic links to:
 *   ~cs570/Three/p3main.c
 *   ~cs570/Three/p3.h
 *   ~cs570/Three/Makefile
 *   ~cs570/Three/CHK.h    )
 *
 */
#include "p3.h"

/* You may put any semaphore (or other global) declarations/definitions here: */

sem_t jogger; // Jogger's "registry"
sem_t shooter; // Shooter's "registry"
sem_t gym_floor; // Access to gym_floor
int jogger_count = 0;
int shooter_count = 0;

/* General documentation for the following functions is in p3.h
 * Here you supply the code:
 */

// sem_init to init
// sem_post to increment
// sem_wait to reduce the number

void initstudentstuff(void) {

        CHK(sem_init(&jogger,LOCAL,1)); // Jogger's "registry"
        CHK(sem_init(&shooter,LOCAL,1)); // Shooter's "registry"
        CHK(sem_init(&gym_floor,LOCAL,1)); // gym floor's mutex, its to track if the floor is occcupied or not

}

void prolog(int kind) {

        /*
                Getting into the gym. If Jogger and joggers in gym, come on in.
                Else go to sleep / wait for it to empty out
                Vice Versa for Shooter
        */

        if(kind == JOGGER){
        
                // check if gym floor occupied or not:
                CHK(sem_wait(&jogger)); // Access granted to jogger's "registry"
                jogger_count = jogger_count + 1;
                if(jogger_count == 1){ // If first jogger, block access to gym_floor for shooters
                        CHK(sem_wait(&gym_floor));
                }
                CHK(sem_post(&jogger)); // Done the work that was needed by accessing the jogger's "registry"
                
                
        } else if (kind == SHOOTER){
        
                // check if gym floor occupied or not:
                CHK(sem_wait(&shooter)); // Access granted to shooter's "registry"
                shooter_count = shooter_count + 1;
                if(shooter_count == 1){ // If first shooter, block access to gym_floor for joggers
                        CHK(sem_wait(&gym_floor));
                }
                CHK(sem_post(&shooter)); // Done the work that was needed by accessing the shooter's "registry"
                
        }

}

void epilog(int kind) {

        /*
                Dealing with people done with their workout and leaving.
                When leaving, the last person says gym is empty.
        */

        if(kind == JOGGER){
                
                // Joggers jog, now they want to leave
                
                CHK(sem_wait(&jogger)); // Access granted to jogger's "registry"
                jogger_count = jogger_count - 1;
                if(jogger_count == 0){ // If last jogger, allow shooters to access the gym_floor
                        CHK(sem_post(&gym_floor));
                }
                CHK(sem_post(&jogger)); // Done the work that was needed by accessing the jogger's "registry"
                
        } else if (kind == SHOOTER){
                
                // Shooters shoot, now they want to leave
                
                CHK(sem_wait(&shooter)); // Access granted to shooter's "registry"
                shooter_count = shooter_count - 1;
                if(shooter_count == 0){ // If last shooter, allow joggers to access the gym_floor
                        CHK(sem_post(&gym_floor));
                }
                CHK(sem_post(&shooter)); // Done the work that was needed by accessing the shooter's "registry"
                
        }

}  

