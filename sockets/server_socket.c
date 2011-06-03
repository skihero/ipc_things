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
 * 	 Add a loop to make it serve many clients  
 *
 */ 
#include<stdio.h> 
#include<unistd.h> 
#include<sys/socket.h> 
#include<sys/types.h> 
#include<sys/wait.h>
#include <netinet/in.h>
#define BUFFER_SIZE 1024 

int main(void) { 
 
	struct sockaddr_in in_name ; /* holds the sa_family, etc.. */ 
	socklen_t in_name_len ;
        unsigned short port = 12345;  
	ssize_t read_len = 0 ; 	
 	
	/* Create the socket */
	int my_socket = socket(AF_INET, SOCK_STREAM, 0); 
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
        int bind_ret = bind(my_socket, (struct sockaddr *) &in_name, in_name_len);
	
	if (bind_ret == -1) { 
		printf("Bind error\n");
		exit(1);
	}

        /* No of connection */ 
	int bck_log = 10 ; /* behavior change in kernel 2.2 */	

	/* Mark the socket as passive */	
	printf("Listening to incoming requests\n");   
        int listen_ret = listen( my_socket, bck_log);   	
	if (listen_ret) { 
	 	printf("listen error\n");
		exit(1);
	}

	/* Accept connections */ 
        int accepted_con =  accept(my_socket, (struct sockaddr *) &in_name, 
				&in_name_len );   
	if (accepted_con == -1 ) { 
		printf("accept error\n"); 
		exit(10); 
	}
	else { 
		char read_buf [BUFFER_SIZE];
		printf("Accepted a connection \n"); 
		read_len = recv(accepted_con,read_buf, BUFFER_SIZE,0) ; 
		if (read_len == -1 ) { 
			printf("Error reading the incoming data \n"); 
			exit(1); 
		}
		else {
			printf("Read data: %s \n", read_buf ) ; 
		        /* chomp goes here 
                         			*/ 

		        pid_t servicing_child ; 
			servicing_child = fork() ; 
			if(servicing_child == 0 ) { 
				 if ( close(1) == -1 ) {
					 printf("Couldn't close stdout of child \n");
					 exit(1);
				 }

				printf(" Child servicing \n"); 
				int dup_out = 0 ; 
				
				// This will tie the output of the exec to the 
				// socket 
				dup_out = dup(accepted_con) ;
					
				if(dup_out ==-1 ) { 
					printf("Dup not done \n"); 
					exit(1);
				}
			
				//execlp("man", "man",read_buf,  NULL) ; 	
			        // exec'ng every command that comes from a socket is a bad idea.
				
				
				execlp(read_buf, read_buf, NULL); 
				exit(0); 
				
			} 
			
			/* Parent */  
			else { 
	
			/*	printf("parent \n"); 
				char buf[BUFFER_SIZE]; // = "hi"; 
				ssize_t to_send = sizeof(buf); 
				ssize_t tried = 0;  
			*/
				int status ; 
				waitpid(servicing_child,  &status,  WUNTRACED | WCONTINUED) ;

	
				/* fill up the buf with the data from exec*/ 
			/*		strcpy(buf, "hi") ;
				
				tried =	send(accepted_con, buf, to_send, 0); 
				if(to_send != tried ) { 
					 printf(" Error sending data \n");
					 exit(1); 
				}
			*/
				printf("Data sent to the client \n"); 
				close(	accepted_con); 
				
				
			}
			close(my_socket);	
			exit(0); 	

		} 
	} 
	
	
	
	/* Fork */ 
		/* Serve the request */ 
			/* parse the command */ 
			   /* Fork for exec */ 
				/* child */ 
				   /* dupe the pipe */ 
				   /* execvp */
				/* parent */ 
				   /* out the exec */ 
	/* parent */  
	
	} 
