/*
 * Client that connect to a socket and 
 * passes a command string that the server 
 * executes under its context and returns 
 * the output
 *
 * Usage: progname commandstring 
 * TODO: Input cleaning 
 * 	 Decide on buffer size
 */


#include<stdio.h> 
#include<unistd.h> 
#include<sys/socket.h> 
#include<sys/types.h> 

#include <netinet/in.h>

#define  BUFFER_SIZE 1024

int main(int argc, char * argv[] ) { 

       printf("argc val %d \n", argc ); 
       if (argc < 2) {
		fprintf(stderr, "Usage: %s commandstring\n", argv[0]);
                exit(1);
		
       }
       
	 
	struct sockaddr_in in_name ; /* holds the sa_family, etc.. */ 
	socklen_t in_name_len; 
        unsigned short port = 12345;  
	ssize_t sent_data_len ; 
	ssize_t tried_len; 
 	
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

	/* Connect to the socket */ 
        int connect_ret= connect(my_socket, (struct sockaddr *) &in_name, in_name_len);
	
	if (connect_ret == -1) { 
		printf("Connect error\n");
		exit(1);
	}
	else {
		//char buf[BUFFER_SIZE] = "hi";  
		char buf[BUFFER_SIZE];
		int i = 1; 
	        printf("argc  val: %d\n", argc);
  		for(i; i<argc-1; i++) {
			strcpy(buf,  argv[i] );  
			printf("i val: %d\n", i);
		}

		sent_data_len = sizeof(buf) ; 
		tried_len = send(my_socket, buf, sizeof(buf), 0) ; 

		/* Check if we sent all the data */ 
		if (sent_data_len != tried_len) { 
			printf(" Error sending data \n"); 
			exit(1); 
		}
		else { 
			printf("Data sent to server \n"); 	
			char read_buf [BUFFER_SIZE] ; 
			ssize_t read_len ; 
			
			/* Read the exec status of our command */
			read_len = recv(my_socket, read_buf, BUFFER_SIZE, 0 ) ; 

			if(read_len == -1 ) { 
				printf("Error reading the data from server\n"); 
				exit(10); 
			}
		        printf("Data recieved from server %s", read_buf); 	
		
			close(my_socket) ; 
			exit(0);
		}
	}
 		
} 
