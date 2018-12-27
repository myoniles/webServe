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
#include <time.h>

#include "pageGen.h"
#include "log.h"
#include "conf.h"

//#define PORT 8080
//#define ROOT "root"
#define PROT "HTTP/1.0"
#define VERSION "0.1.0"
//#define LOGFILE "mikeServe.log"

/*
	If by chance you are Thomas, looking for a
	way to break this, best of luck!

	This program was written in vim not emacs.
*/


void sigintHandler(int sig_num) {
	signal(SIGINT, sigintHandler);
	// Using write because its Async safe
	// Even though this will exit, its just better this way
	write( 0 , "\nServer Shutting Down . . .\n", 28 );

	// Lets also unallocate some globals
	freeGlobals();
	exit(1);
}

void handle(int sockid) {
	int maxRequestLength = 2048;
	char buff[maxRequestLength + 1];
	int currLength = 0;
	char currChar = 0;
	char prevChar = 0;
	time_t start = time(0);

	// Get the request Store it in buff
	while (currLength < maxRequestLength && recv(sockid, &currChar, 1,0) > 0){
		// prevent any lorises
		// Or close requests after a connection was dropped
		if (difftime( time(0), start ) >= 15.0){
			sendHeader(sockid, "408 Request Timeout","text/html");
			genErrorPage(sockid, "408 Request Timeout");
		}


		if (currLength >= maxRequestLength){
			// Check for buffer overflow before writing
			sendHeader(sockid, "413 Payload Too Large", "text/html");
			genErrorPage(sockid, "413 Payload Too Large");
			return;
		} else if (currChar == '\r' && prevChar == '\n') {
			// Finish The message
			buff[currLength] = '\r';
			buff[++currLength] = '\n';
			buff[++currLength] = 0;
			break;
		}
		buff[currLength++] = currChar;
		prevChar = currChar;
	}

	if (!supportHTTPVersion(buff)){
		sendHeader(sockid, "505 HTTP Version Not Supported", "text/html");
		genErrorPage(sockid, "505 HTTP Version Not Supported");
		return;
	}

	// A wise man once said "We only support GET"
	if (!strncmp(buff, "GET", 3) == 0){
		// The request looks weird aka its not a get request
		sendHeader(sockid, "405 Method Not Allowed", "text/html" );
		genErrorPage (sockid, "405 Method Not Allowed");
		return;
	}

	// Check if the file exists
	char* requestPath = getRequestedFileName(buff);
	logRequest(requestPath);
	if (requestPath == NULL) {
		// Malformed request
		sendHeader(sockid, "400 Bad Request", "text/html");
		genErrorPage(sockid, "400 Bad Request");
		return;
	}


	if ( access(requestPath, F_OK) != -1) {
		// If it does return the thing
		char *contType = getContentType(buff);
		if (contType == NULL){
			sendHeader(sockid, "400 Bad Request", "text/html");
			genErrorPage(sockid, "400 Bad Request");
		} else {
			sendHeader(sockid, "200 OK", contType);
			fileDump(sockid, requestPath);
			free(contType);
		}
	} else {
		// If not, don't do the thing
		sendHeader(sockid, "404 Not Found", "text/html");
		genErrorPage(sockid, "404 Page Not Found");
	}
	free(requestPath);
}

int main (int argc, char** argv){
	int opt = 1;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	int sockid;
	int status;

	// Set initial values if none specified in either config file or command line
	PORT = 8080;
	LOGFILE = strdup("mikeServe.log");
	ROOT = strdup("root");
	CONF = strdup("mikeServe.conf");
	LAND = strdup("index.html");
	SAVE = 0;
	DAEMON = 0;

	// Configuration
	readConfFile();
	readTermConf(argc, argv);

	if (SAVE){
		writeConfFile();
	}

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

	if(DAEMON){
		daemon(1,0);
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

		handle( newSoc);
		shutdown(newSoc, 2);
	}
	status = close(sockid);
}
