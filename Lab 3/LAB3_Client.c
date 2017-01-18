// BEGIN 477_TTT_CLIENT.c Code adapted from http://linuxhowtos.org/C_C++/socket.htm
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void h_error(const char *msg)
{
	fputs(msg, stderr);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd;	// socket file descriptor for remote connection
	int portnum; 	// port number to communicate through
	int n;       	// variable to hold read/write return values
	struct sockaddr_in serv_addr;  // internet domain structs holding the address information of the client and server
	struct hostent *server;
	char buffer[1024];          	// general buffer for sending messages to and receiving messages from the socket
 
	if (argc < 3) {  // make sure the user is using the program correctly
   	fprintf(stderr,"Usage of %s is: %s hostname port\n", argv[0], argv[0]);
   	exit(0);
	}

	printf("Welcome to the Tic-Tac-Toe Client! The play-character format is as follows: X = <X>, O = <O>, blanks are <B>.\n");
	printf("Put down your letters in this layout: XBXBBBOOO, where XBX is the first row, BBB the second, and OOO the third.\n");

	portnum = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // create internet family socket which is a continuous data stream, rather than a datagram stream.

	if (sockfd < 0) {
    	h_error("ERROR, unable to open socket");  // make sure socket opened OK
	}

	server = gethostbyname(argv[1]);   // self-explanatory (not really necessary since we are using IP addresses here)
 
	if (server == NULL) {
    	fprintf(stderr,"ERROR, specified host not found\n"); // error check to see if server host was found.
    	exit(0);
	}
    
	bzero((char *) &serv_addr, sizeof(serv_addr)); // zero buffer of the server address information struct.
	serv_addr.sin_family = AF_INET;       	// server has internet family address
	bcopy((char *)server->h_addr,
     	(char *)&serv_addr.sin_addr.s_addr,	// fill in the serv_addr fields.
     	server->h_length);
	serv_addr.sin_port = htons(portnum); //put listening port number for the server in network byte order.
    
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
    	h_error("ERROR, could not connect to the desired host and port.");  // error check for socket connection attempt.
	}

	while( code == 100 ) {
    	printf("Please enter your Tic-Tac-Toe board layout: ");  // prompt user to enter
// the board information
    	bzero(buffer,256);   // zero the buffer which will hold the 
// Tic-Tac-Toe character set.
    	fgets(buffer,255,stdin); 	// grab the 9 Tic-Tac-Toe characters to be 
// played from the user.
	play_game(sockfd, buffer);
	grab numbers from string 
	}
	close(sockfd);  // game is complete, shut down the client.
	return 0;	// return success
}
