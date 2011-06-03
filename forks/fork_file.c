/*
 * Use a file as a means of communication b/w 
 * processes 
 */ 


#include <unistd.h>
#include <stdio.h>
#include <strings.h> 
#include <sys/wait.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <fcntl.h>


int main (void) { 
	
	pid_t  p ; 
	int status, fd_in, fd_read ; 
	char buf[1024]  ; 

	memset(buf, 0, 1024) ; 

	/* open a few descriptors */ 

	fd_in = open("/tmp/shared_file", O_WRONLY|O_CREAT, S_IRWXU); 
	fd_read = open("/tmp/shared_file", O_RDONLY); 

	sprintf(buf,"parentstuff"); 
	write(fd_in, buf, strlen(buf)) ; 

	/* Fork this process */ 
	p = fork() ; 

	if( p == 0 ) { 
		/* Child goes here */ 
		
		printf("child %d sleeping...zz\n", getpid()); 
      		sleep(3); 

		printf("buf: %s  \n", buf); 
		printf("child id: %d \n", getpid() );  
		
		sprintf(buf,"childstuff"); 
		write(fd_in, buf, strlen(buf)) ; 

		printf("buf: %s  \n", buf); 
		close(fd_in); 
	} 

	else { 

	     printf("parent id: %d \n", getpid() );  
	     int w = waitpid(p, &status,  WUNTRACED | WCONTINUED) ; 
	       
	     memset(buf, 0 ,1024 ) ;  

	     /* open the file and read the contents */ 
	     printf("After wait\n");    
	     printf("wait value %d\n", w);
	     printf("status ret %d\n", status);  

	     /* Read the shared file */ 
	     while (read(fd_read, buf, 2) > 0 ) { 
	       	printf("%s", buf ); 
		} 

		printf("End \n"); 

	} 

} 
