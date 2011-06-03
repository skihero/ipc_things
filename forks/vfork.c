/* Using vfork 
 * This is used to create new processes 
 * which shares the page tables and the execution flow of the 
 * parent  
 * 
 * vfork also comes with a lot of warnings about it 
 * having undefined behavior if it modifies anything other
 * than the pid_t variables 
 *
 */ 

#include <unistd.h>
#include <stdio.h>
#include <string.h> 
#include <sys/wait.h>

int main (void) {

        pid_t p;
        char string_array[] = "parent_string";

        printf("Original program, pid=%d\n", getpid());
	
        p = vfork(); /* See the man page for the complete list of quirks */ 

        if (p == 0) {
                printf("In child process, pid=%d, ppid=%d\n",
                        getpid(), getppid());

		printf("Parent string in child=%s\n", string_array) ; 
		printf("changing parent string... \n"); 
		strcat(string_array, "abc"); 

		printf("Parent string in child changed =%s\n", string_array) ; 

                /* This is to unsuspend the parent flow 
 		   which is waiting for the vfork flow to finish  */
		_exit(0) ; 

        } else {
                printf("In parent, pid=%d, fork returned=%d\n",
                        getpid(), p);

	        /* The parent flow waits because of the vfork */
		printf("Parent string=%s\n", string_array) ; 
        }
}

