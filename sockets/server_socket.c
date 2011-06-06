/* 
 * Create a socket that listens to a commandstring 
 * and executes it. 
 *
 * The exec output is directly forwarded to the open socket 
 * which should be read by the client which initiated the 
 * request 
 *
 * TODO: Error checking on the input 
 * 	 Find another way to buffer the exec output and send it 
 * 	 Add a loop to make it serve many clients  XXX DONE
 * 	 Enable executable path finding 
 * 	 Buffer checks 
 * 	 Improve error handling
 *
 */ 
#include<stdio.h> 
#include<unistd.h> 
#include<sys/socket.h> 
#include<sys/types.h> 
#include<sys/wait.h>
#include <netinet/in.h>
#include <errno.h>
#define BUFFER_SIZE 1024 

int main(void) { 

	int my_socket ;                 /* Socket desc */ 
	struct sockaddr_in in_name ;    /* holds the sa_family, etc.. */ 
	socklen_t in_name_len ;         /* socket length */
        unsigned short port = 12345;    /* Location */ 
	int bind_ret ; 			/* Bind() RC */
	int listen_ret ;		/* Listen() RC */  
	int accepted_con ;		/* Accpeted connection desc */ 
	ssize_t read_len = 0 ;          /* Bytes read on socket */	
	char read_buf [BUFFER_SIZE];    /* Holds the incoming command */
	char *p ; 			/* Location of the arg delimiter */
	char *path_string ; 		/* Executable path */
	char *arg_string ; 		/* Remains of args chopping  */
	char *command_arguments [BUFFER_SIZE] ;  /* arguments */
	int arg_iter;  			/* Start of the actual arg */ 
	int dup_out = 0 ;  		/* Return value of the dup call */
	int bck_log = 10 ; 		/* No of connections, behavior change in kernel 2.2 */	



	/* Create the socket */
	my_socket = socket(AF_INET, SOCK_STREAM, 0); 
	if (my_socket == -1 ) { 
		printf("socket error\n");
		exit(1);
	} 
 
        /* Define the name */ 
	memset(&in_name, 0 , sizeof(&in_name)) ; 
 
	in_name.sin_family = AF_INET;  
	in_name.sin_port = htons(port); 
	in_name.sin_addr.s_addr = INADDR_ANY ; 

	in_name_len = sizeof(struct sockaddr_in); 

	/* Bind the socket */ 
        bind_ret = bind(my_socket, (struct sockaddr *) &in_name, in_name_len);
	
	if (bind_ret == -1) { 
		printf("Bind error\n");
		close(my_socket); 
		exit(1);
	}

	/* Loop to serve many requests */ 
	for(;;) { 
		/* Mark the socket as passive */	
		printf("Listening to incoming requests\n");   
		listen_ret = listen( my_socket, bck_log);   	
		if (listen_ret) { 
			printf("listen error\n");
			close(my_socket); 
			exit(1);
		}

		/* Accept connections */ 
		accepted_con =  accept(my_socket, (struct sockaddr *) &in_name, 
					&in_name_len );   
		if (accepted_con == -1 ) { 
			printf("accept error\n"); 
			exit(10); 
		}
		else { 
			printf("Accepted a connection \n"); 
			read_len = recv(accepted_con,read_buf, BUFFER_SIZE,0) ; 
			if (read_len == -1 ) { 
				printf("Error reading the incoming data \n"); 
				exit(1); 
			}
			else {
				printf("Read data: %s \n", read_buf ) ; 
			
				/* From the read buffer separate the command part 
				 * and the arguments 
				 */ 
			
				p = strchr(read_buf,' '); /* Find the first location of a space */
				if (p) {
					 /* Terminate at the space and increment */
					*p++ ='\0'; 
					/* Pointer to new string from buf gets autocut */
					 path_string = strdup(read_buf); 
					/* Rest of the command string */
					 arg_string = strdup(p); 
				}

				printf("path : %s\n", path_string );  
				printf("args: %s\n", arg_string); 
				
				/* Construct the argument array */ 
				/* Convention: exec argv[0] has the name of calling*/ 
				command_arguments[0] = path_string;  
				arg_iter = 1; 	 /* Start of the actual arg */ 

				/* Split string and populate array */ 
				while( (p = strchr(arg_string, ' '))){
					*p++='\0';  /* Mark the end of the string */ 
					command_arguments[arg_iter] = strdup(arg_string);
					arg_string = strdup(p) ; /* Rest of the args */

					printf("command_arguments: %s\n ", command_arguments[arg_iter]);
					printf("arg_string : %s \n ", arg_string);
					arg_iter ++ ;
				}
			 
				pid_t servicing_child ; 
				servicing_child = fork() ; 
			
				if(servicing_child == 0 ) { 
					 if ( close(1) == -1 ) {
						 printf("Couldn't close stdout of child \n");
						 exit(1);
					 }

					printf(" Child servicing \n"); 
					dup_out = 0 ; 
					
					/* This will tie the output of the exec to the  socket */
					dup_out = dup(accepted_con) ;
						
					if(dup_out ==-1 ) { 
						printf("Dup not done \n"); 
						exit(1);
					}
						
					 int exec_ret = execv(path_string, command_arguments); 
					 if (exec_ret == -1 ) {
						 int local_err = errno ;
						 perror("exec not done \n");
						 printf("exec errno %d\n", local_err);
						 exit(666);
					 }
					fflush(stdout); 
					exit(0); 
					
				} 
				
				/* Parent */  
				else { 
					int status ; 
					waitpid(servicing_child,  &status,  WUNTRACED | WCONTINUED) ;
					printf("Data sent to the client \n"); 
					close(accepted_con); 
				}

			} 
		} 
	} // Loop for incoming connections	
}// End main 
