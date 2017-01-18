BEGIN 477SERVER.C http://linuxhowtos.org/C_C++/socket.htm

/* A simple server in the internet domain using TCP
   The port number is passed as an argument
   This version runs forever, forking off a separate
   process for each connection
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void h_error(const char *msg)
{
	fputs(msg, stderr);
	exit(1);
}

int main(int argc, char *argv[])
{
 	int sockfd;	// file descriptor for the listening socket
 	int newsockfd;	// file descriptor for new socket opened when connection comes in.
 	int portnum;  	// port number to listen on.
 	int pid;        	// process id of child process created when another client connects to the socket.
 	socklen_t clilen;	// length of client's address information
 	struct sockaddr_in serv_addr;	// internet domain structs holding the address information of the client and server
 	struct sockaddr_in cli_addr;
 	char buffer[1024];    

 	if (argc < 2) {
     	fprintf(stderr,"ERROR, listening port number not provided!\n");
     	exit(1);
 	}
 	sockfd = socket(AF_INET, SOCK_STREAM, 0); // open our listening stream.
 	if (sockfd < 0)  {
    	h_error("ERROR, unable to open communication socket"); // make sure listening stream is OK
	}
 	bzero((char *) &serv_addr, sizeof(serv_addr)); // initialize the struct holding the server address info
                                         	 
 	portnum = atoi(argv[1]); // grab listening port number
 	serv_addr.sin_family = AF_INET; // server has internet family address
 	serv_addr.sin_addr.s_addr = INADDR_ANY;  // IP address of the host the server is running on
 	serv_addr.sin_port = htons(portnum); //put listening port number for the server in network byte order.
 	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
     	h_error("ERROR, unable to bind socket to host and port"); // bind process socket to localhost and port
}
 	listen(sockfd,5); // listen on port, allow 5 backlog connections maximum
 	clilen = sizeof(cli_addr); //find size of client address information struct
 	while (1) {   	// infinite loop so the server runs forever. MUAHAHAHA
    	 
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     // process blocks until client connects to server; that is, it
     // wakes up the process when a connection from a client has been successfully established. It returns a new file descriptor,    	   
     	// and all communication on this connection should be done using the new file descriptor"
     	if (newsockfd < 0) {
         	h_error("ERROR, unable to accept client connection"); // error check that connection was successful.
     }
     	pid = fork(); // if we did successfully listen, clone this process with its memory and progress up to this point.
     	if (pid < 0) {
         	error("ERROR, unable to fork to new process for latest client connection");  // make sure fork is OK
     }
     	if (pid == 0)  { // if  pid == 0, we are in the child process handling the new client request.
         	close(sockfd);  // child not interested in socket of parent
    	 
     	//read(newsockfd, buffer, sizeof(buffer)); // read from socket
     	//fputs(buffer, stdout);	// print socket contents from the client.
     	     	write(newsockfd, buffer, strlen(buffer));
         	play_game(newsockfd, buffer); // talk to the new socket; i.e, play and finish the game.
         	exit(0);
     	}
     	else close(newsockfd); // pid != 0, so we are in the parent. keep listening to the original socket file descriptor (sockfd)
 	} /* end of while */
 	close(sockfd);  // once while loop ends, close the very original socket file descriptor. We never reach here.
 	return 0; // Same as the close(sockfd) statement.
}

#include <stdio.h>
#include <stdlib.h>

void play_game(int sockfd, char * buffer)
{
//write(sockfd, buffer, strlen(buffer));
        char seqold[20] = "BBBBBBBBB";
        char sequin[20];
        char user_sym;
        char server_sym;
        char * code = “100”; // Defaults to 100
        int n = 0;

       while(atoi(code) < 200 || atoi(code) >= 400){
             read(sockfd, sequin, strlen(sequin));
	if(boardfull(sequin)){
		code = “500”;
		strcat(code,” “);
		strcat(code,sequin);   
		write(sockfd, sequin, strlen(sequin));
	}
	if(invalid(sequin)){
		code = “400”;
		continue;
	}
             if(cheat_checker(sequin,seqold)){
                        //printf(“Cheat has occurred; no change made\n”);
code = “401”;
                        continue;
             }
             code = win_checker(user_sym,sequin) + 100;
             if(code>=400) continue;
	sequin = server_move(sequin,user_sym,server_sym);
	code = win_checker(server_sym, sequin);
             replace(seqold,sequin);
	strcat(code,” “);
	strcat(code,sequin);
	n = write(sockfd,code,strlen(code));  // write the play to the client.
    	if (n < 0)   h_error("ERROR writing to socket");  
        }
}

int cheat_checker(char sequin[], char seqold[])
// Returns 1 if a character is changed illegally, or 0 otherwise
{
        int i;
        int moved=-1;
        for(i=0;i<9;i++){
                if(seqold[i]!="B" && seqold[i]!=sequin[i]){ // A non-blank has changed
                        printf("Cheat detected.\n");
                        printf("Because space %d was \"%c\" and not \"%c\".\n",i,seqold[i],sequin[i]);
                        return 1; // We have a cheater on our hands.
                }
                else if(sequin[i]!="B"){
                        if(moved!=-1){
                                printf("Cheated by changing both spaces %d and %d.\n",moved,i);
                                return 1; // Cheating creatively
                        }
                        else moved = i;
                }
        }
        return 0; // No cheating has occurred.
}

void replace(char to_replace[], char replacer[])
// Copies over the first string with the second
{
        int i;
        for(i=0;i<9;i++){
                to_replace[i]=replacer[i];
        }
}

char *win_checker(char to_check, char sequence[])
// Returns 1 if the specified character is in win condition, or 0 otherwise
{
        if(sequence[0]==sequence[1]==sequence[2]==to_check){
                return "200"; // Row 1 win
        } else if(sequence[3]==sequence[4]==sequence[5]==to_check){
                return "201"; // Row 2 win
        } else if(sequence[6]==sequence[7]==sequence[8]==to_check){
                return "202"; // Row 3 win
        } else if(sequence[0]==sequence[3]==sequence[6]==to_check){
                return "203"; // Col 1 win
        } else if(sequence[1]==sequence[4]==sequence[7]==to_check){
                return "204"; // Col 2 win
        } else if(sequence[2]==sequence[5]==sequence[8]==to_check){
                return "205"; // Col 3 win
        } else if(sequence[0]==sequence[4]==sequence[8]==to_check){
                return "206"; // L to R diag win
        } else if(sequence[2]==sequence[4]==sequence[6]==to_check){
                return "207"; // R to L diag win
        } else return "100";  // No win
}

char *server_move(char after_user[],char use_sym,char serv_sym)
// returns a 9 char string that will constitute the board after server has moved
{
        char *out = after_user;
        int here;
        if(here = intellemove(after_user,serv_sym)){
                out[here-1] = serv_sym; // go for the win
        }
        else if(here = intellemove(after_user,use_sym)){
                out[here-1] = serv_sym; // go on the defensive
        }
        else{
                while(after_user[here] != "B"){ // generates number from 0 to 8
                        here = rand()%9;
                }
                out[here] = serv_sym; // go somewhere
        }
}

int intellemove(char useq[],char opsym)
{
        // This function goes space by space determining if making a move in
        // that space is appropriate
        int move;
        if(useq[1]==useq[2]==opsym || useq[4]==useq[8]==opsym || useq[5]==useq[6]==opsym){
                move = 1;
        }
        else if(useq[0]==useq[2]==opsym || useq[4]==useq[7]==opsym){
                move = 2;
        }
        else if(useq[0]==useq[1]==opsym || useq[4]==useq[6]==opsym || useq[5]==useq[8]==opsym){
                move = 3;
        }
        else if(useq[0]==useq[6]==opsym || useq[4]==useq[5]==opsym){
                move = 4;
        }
        else if(useq[0]==useq[8]==opsym || useq[2]==useq[6]==opsym){
                move = 5;
        }
        else if(useq[1]==useq[7]==opsym || useq[3]==useq[5]==opsym){
                move = 5;
        }
        else if(useq[3]==useq[4]==opsym || useq[2]==useq[8]==opsym){
                move = 6;
        }
        else if(useq[0]==useq[3]==opsym || useq[4]==useq[2]==opsym || useq[7]==useq[8]==opsym){
                move = 7;
        }
        else if(useq[1]==useq[4]==opsym || useq[6]==useq[8]==opsym){
                move = 8;
        }
        else if(useq[0]==useq[4]==opsym || useq[2]==useq[5]==opsym || useq[6]==useq[7]==opsym){
                move = 9;
        }
        else move = 0;
}

int boardfull(char * board)
// returns one if the passed board contains no "B", and zero otherwise
{
        int i;
        for(i=0;i<9;i++){
                if(board[i] == "B") return 1;
        }
        return 0;
}

int invalid(char *seq)
// returns 1 if the input string is not valid
{
	int i;
	for(i=0;i<9;i++){
		if(seq[i]!=”B” && seq[i]!=”X” && seq[i]!=”O”) return 1;
	}
	return 0;
}