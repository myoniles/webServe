#ifndef LOGGING
#define LOGGING

#include <stdlib.h>
#include <stdio.h>

#define LOGFILE "mikeServe.log"

char* getDate(){
	time_t rawtime;
	struct tm* timeinfo;
	char* buff = (char*)calloc(50, sizeof(char));

	time (&rawtime);
	timeinfo = localtime ( &rawtime );
	strftime(buff, 50, "%a, %d %b %Y %H:%M:%S %Z", timeinfo);
	return buff;
}


void errormsg( char* msg, int status, int critical ) {
	fprintf(stderr, "%s\nStatus: %d\n", msg, status);
	if (critical){
		exit(EXIT_FAILURE);
	}
}

void logRequest(char* filename){
	FILE* fille = fopen(LOGFILE, "a+");
	if (fille == NULL){
		errormsg("No log file found and cannot create one.", 0 , 0);
		return;
	}
	char* time = getDate();

	fprintf(fille,"[%s]:\tREQUEST:%s\n", time, filename);

	// this project isn't too stressful.
	// I just work on it in my...
	free(time);

	fclose(fille);
	return;
}

void logError(char* msg){
	FILE* fille = fopen(LOGFILE, "a+");
	if (fille == NULL){
		errormsg("No log file found and cannot create one.", 0 , 0);
		return;
	}
	char* time = getDate();

	fprintf(fille,"[%s]:\tERROR:%s\n", time, msg);

	free(time);
	fclose(fille);
	return;
}
#endif
