#ifndef PAGE_GEN_H
#define PAGE_GEN_H
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
#include <time.h>

#include "log.h"

#define PROT "HTTP/1.0"
#define VERSION "0.1.0"

#define wsockid(str) if (send(sockid, str, strlen(str), 0) != strlen(str)) exit(1);

char* ROOT;
char* LAND;

void sendHeader(int sockid, char* status, char* contentType,int fileSize ){
	// hard coding is like sausage; its a grind and you might
	// not want to have any if you knew how it was made.
	wsockid(PROT);
	wsockid(" ");
	wsockid(status);
	wsockid("\r\n");
	wsockid("Server: MikeServe/0.1.0\r\n");
	wsockid("Date: ");
	char* date = getDate();
	wsockid(date);
	wsockid("\r\n");
	wsockid("Content-Language: en_US\r\n");
	wsockid("Content-Type: ");
	wsockid(contentType);
	wsockid("\r\n");
	if (fileSize >= 0) {
		// Done exclusively because pregenerated pages are not able to be measured
		char size[10];
		snprintf(size,10, "%d", fileSize);
		wsockid("Content Length ");
		wsockid(size);
	}
	wsockid("\r\n");
	wsockid("Connection: close\r\n\r\n\r\n");
	// Opposite of "Save the Date" letters"
	free(date);
	return;
}

void genErrorPage(int sockid, char* error){
	// See comment in SendHeader()
	wsockid("<html>\r\n<head>\r\n<title>");
	wsockid(error);
	wsockid("</title>\r\n</head>\r\n<body>\r\n<h1>\r\n");
	wsockid(error);
	wsockid("</h1>\r\n<hr>\r\n<p>MikeServe version ");
	wsockid(VERSION);
	wsockid("</p>\r\n</body></html>\r\n");

	logError(error);
	return;
}

int getFileSize(char* filename){
	FILE* fille = fopen(filename,"r");
	if (fille == NULL){
		return 1;
	}

	// Get the file size for
  fseek(fille, 0L, SEEK_END);
  int size = ftell(fille);

	fclose(fille);
	return size;
}

int fileDump (int sockid, char* filename){
	FILE* fille = fopen(filename,"r");
	if (fille == NULL){
		return 1;
	}

	char c;
	int count;
	// This does not use the macro wsockid because
	// strlen behaves strangely with single chars
  while((count=fread(&c,1,1,fille)) != 0){
    if(write(sockid, &c, 1) != 1){
      perror("write");
      return 1;
    }
  }

	fclose(fille);
	return 0;
}

char* getRequestedFileName(char* full){
	int numChars = 0;
	char* needle = strchr(full,'/');
	if( needle == NULL){
		return NULL;
	}	else {
		// We can ignore the '/' character
		needle++;
	}

	while (*needle != ' ' && *needle != '\r'){
		needle++;
		numChars++;
	}

	// requesting the landing page
	if ( numChars == 0){
		needle = (char*) malloc(strlen(ROOT) + strlen(LAND));
		strcpy(needle, ROOT);
		strcat(needle, "/");
		strcat(needle, LAND);
	} else {
		// 1 is added to offset the '/'
		needle = (char*) malloc(numChars + strlen(ROOT));
		strcpy(needle, ROOT);
		strncat(needle, strchr(full, '/'), numChars +1 );
	}
	return needle;
}

int supportHTTPVersion(char* full){
	char* needle = strstr(full, "HTTP/1.");
	if( needle == NULL){
		return -1;
	} else if (*(needle + 8) != '1' || *(needle + 8) != '0' ){
		return 1;
	} else {
		return 0;
	}
}

char* getContentType(char* full){
	char* name = getRequestedFileName(full);
	char* ending = strchr( name, '.');
	char* toReturnType;

	// images
	if (strcmp(ending, ".png") == 0 ){
		toReturnType = strdup("image/png");
	} else if (strcmp(ending, ".jpg") == 0 || strcmp(ending, ".jpeg") == 0 ) {
		toReturnType = strdup("image/jpeg");
	} else if (strcmp(ending, ".gif") == 0 ) {
		toReturnType = strdup("image/gif");
	} else if (strcmp(ending, ".css") == 0 ) { // text
		toReturnType = strdup("text/css");
	} else if (strcmp(ending, ".csv") == 0 ) {
		toReturnType = strdup("text/csv");
	} else if (strcmp(ending, ".html") == 0 ) {
		toReturnType = strdup("text/html");
	} else if (strcmp(ending, ".xml") == 0 ) {
		toReturnType = strdup("text/xml");
	} else {
		toReturnType = strdup("text/plain");
	}
	free(name);
	return toReturnType;
}

void teaTime( int sockid){
	wsockid("<html>\r\n<head>\r\n<title>");
	wsockid("418 I'm a teapot.");
	wsockid("</title>\r\n</head>\r\n<body>\r\n<h1>\r\n");
	wsockid("418 I'm a teapot.");
	wsockid("</h1>\r\n<hr>\r\n<p>MikeServe version ");
	wsockid(VERSION);
	wsockid("</p><p>The server hosting the webpage you requested is currently being used a teapot.</p>");
	wsockid("</p>\r\n</body></html>\r\n");
	return;
}

#endif
