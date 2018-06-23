#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <dirent.h>

#define PORT 8080
#define ROOT "root"

/*
	If by chance you are Thomas, looking for a
 	way to break this, good luck!
	This program was written in vim not emacs
*/

void errormsg( char* msg, int status, int critical ) {
	fprintf(stderr, "%s\nStatus: %d\n", msg, status);
	if (critical){
		exit(EXIT_FAILURE);
	}
}

void handle(int sockid, DIR* root) {
	int maxRequestLength = 512;
	char buff[maxRequestLength + 1];
	int currLength = 0;
	char currChar = 0;
	char prevChar = 0;

	// Get the request Store it in buff
	while (currLength < maxRequestLength && read (sockid, &currChar, 1) > 0){
		if (currChar == '\n' && prevChar == '\r') {
			break;
		}

		buff[currLength++] = currChar;
		prevChar = currChar;
	}
	buff[--currLength] = 0;

	printf("%s\n", buff);
	close(sockid);
}

int main (){
	int opt = 1;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	int sockid;
	int status;
	DIR* dir = opendir( ROOT );

	// Create the socket
 	if ( (sockid = socket(PF_INET, SOCK_STREAM,0))  == 0) {
		errormsg("Socket failure", sockid, 1);
		int status = close(sockid);
	}

	// set socket options
	if( (status = setsockopt(sockid, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) < 0 ){
		errormsg("choosing options failed", status, 0);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	// Bind the socket to PORT
	if ( (status=bind(sockid, (struct sockaddr*)&address, sizeof(address))) < 0) {
		errormsg("could not bind", status, 1);
		int status = close(sockid);
	}

	// Listen to on PORT
	if ((status=listen(sockid, 3) < 0 )){
		errormsg("I couldn't hear you if I wanted to...", status, 1);
	}

	int newSoc;
	// Accept incoming requests
	if ( (newSoc=accept(sockid, (struct sockaddr*)&address,(socklen_t*)&addrlen )) < 0) {
		errormsg("I object!", status, 0);
	}

	handle( newSoc, dir);

	status = close(sockid);


}
