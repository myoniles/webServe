// A project is worth the amount of packages it requires
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>

#include "pageGen.h"

#define PORT 8080
#define ROOT "root"
#define PROT "HTTP/1.0"
#define VERSION "0.1.0"

/*
	If by chance you are Thomas, looking for a
 	way to break this, best of luck!

	This program was written in vim not emacs.
*/

void errormsg( char* msg, int status, int critical ) {
	fprintf(stderr, "%s\nStatus: %d\n", msg, status);
	if (critical){
		exit(EXIT_FAILURE);
	}
}

void sigintHandler(int sig_num) {
	signal(SIGINT, sigintHandler);
	// Using write because its Async safe
	// Even though this will exit, its just better this way
	write( 0 , "\nServer Shutting Down . . .\n", 28 );
	exit(1);
}

void handle(int sockid) {
	int maxRequestLength = 2048;
	char buff[maxRequestLength + 1];
	int currLength = 0;
	char currChar = 0;
	char prevChar = 0;

	// Get the request Store it in buff
	while (currLength < maxRequestLength && recv(sockid, &currChar, 1,0) > 0){
		if (currChar == '\r' && prevChar == '\n') {
			// Finish The message
			buff[currLength] = '\r';
			buff[++currLength] = '\n';
			buff[++currLength] = 0;
			break;
		}
		buff[currLength++] = currChar;
		prevChar = currChar;
	}

	// TODO Process request
	int method;

	// A wise man once said "We only support GET"
	if (strncmp(buff, "GET", 3) == 0){
		method = 1;
	} else {
		// The request looks weird aka its not a get request
		sendHeader(sockid, "400 Bad Request", "text/html" );
		genErrorPage (sockid, "400 Bad Request");
	}

	sendHeader(sockid, "200 OK", "text/html");
	char* requestPath = getRequestedFileName(buff);
	fileDump(sockid, requestPath);


	free(requestPath);
	shutdown(sockid, 2);
}

int main (){
	int opt = 1;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	int sockid;
	int status;

	// Set up the sigint handler as to ensure the socket is closed
	signal(SIGINT, sigintHandler);

	// Create the socket
 	if ( (sockid = socket(AF_INET, SOCK_STREAM,0))  == 0) {
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

	while ( 1 ){
		// Listen to on PORT
		if ((status=listen(sockid, 3) < 0 )){
			errormsg("I couldn't hear you if I wanted to...", status, 1);
		}
		
		int newSoc;
		// Accept incoming requests
		if ( (newSoc=accept(sockid, (struct sockaddr*)&address,(socklen_t*)&addrlen )) < 0) {
			errormsg("I object!", status, 0);
		}

		handle( newSoc ); 

	}
	status = close(sockid);
}
