/*
 * Implementing a simple pipe equivalent of 
 * man close |wc -c 
 */

#include<unistd.h> 
#include<stdio.h> 
#include<sys/wait.h>  
#include<stdlib.h>
#include<errno.h>
 
int main(void) { 

   /* Define space for pipe descriptors */ 
   int pipe_des [2]; 
 
   /* Define a child pid */ 
   pid_t cpid;


  if ( pipe(pipe_des) == -1 ) {
      printf("Error creating the pipe \n"); 
      exit(1); 
	}

	
  printf("Pipe has been created \n");     

  int len = 0;
  int i = 0; 
  int local_errno = errno ;

  len = sizeof(pipe_des) / sizeof(int) ;  

  while( i < len ) { 
	printf("pipe_desc[%d]: %d\n", i, pipe_des[i] );  
	i++ ; 	
  }


  /* Create a child */ 
  cpid = fork() ; 
  
  if( cpid == 0 ) { 
  	if ( close(1) == -1 ) { 
		local_errno = errno ;
		printf("Couldn't close stdout of child \n"); 
                printf("Close returned %d", local_errno); 
		exit(1);
	}
 	dup(pipe_des[1]); 
        close(pipe_des[0]);          /* Close unused read end */
        
	execlp("man", "man","close",  NULL) ; 
        //_exit(EXIT_SUCCESS); 	
        


  } else {  
	
       int status = 0 ; 
       if( close(0) == -1 ) { 
		local_errno = errno ;
		printf("Couldn't close stdin of parent \n"); 
                printf("Close returned %d \n", local_errno); 
		exit(1);
	}
       dup(pipe_des[0]); 
       printf("Output of our chain:\n ");       
       close(pipe_des[1]);	    /* Close unused write end */ 
       execlp("wc", "wc", "-c", NULL); 
      
   }  

   
}  
