#ifndef CONFOPTS
#define CONFOPTS

// Lets declare some global pointers
// This will eventually help phase out the constants above
int PORT;
int DAEMON;
char* CONF;
char* LAND;
int SAVE;
char SERVE_TYPE;
int DEBUG;

void freeGlobals(){
	free(ROOT);
	free(LOGFILE);
	free(CONF);
	free(LAND);
}


void getConfOptions(){
	printf("-d:\tRun this as a daemon\n");
	printf("-f:\tFork a process to handle each request\n");
	printf("-h:\tReturns a list of commands\n");
	printf("-s:\tSave configuartion options / Generate a mikeServe.conf file\n");
	printf("-t:\tcreate a thread to handle each request\n");
	// TODO
	//printf("-p <number of threads>:\tcall on a pool of threads of size <n> to handle each request\n");
	printf("-c <filename>:\tSpecify a config file\n");
	printf("-i <filename>:\tSpecify a landing page\n");
	printf("-l <filename>:\tSpecify a log file\n");
	freeGlobals();
	exit(0);
}

void readTermConf(int argc, char** argv){
	int conf;
	while ((conf = getopt(argc, argv, "dghsftc:l:i:")) != -1 ){
		switch (conf){
			case 'h':
				getConfOptions();
				break;
			case 'l':
				free(LOGFILE);
				LOGFILE = strdup(optarg);
				break;
			case 'c':
				free(CONF);
				CONF = strdup(optarg);
				break;
			case 'i':
				free(LAND);
				LAND = strdup(optarg);
			case 's':
				SAVE = 1;
				break;
			case 'd':
				DAEMON = 1;
				break;
			case 'g':
				DEBUG = 1;
				break;
			case 'f':
				SERVE_TYPE = 'f';
				break;
			case 't':
				SERVE_TYPE = 't';
				break;
		}
	}
}

void readConfFile(){
	FILE* fille = fopen(CONF, "r");
	if(fille == NULL){
		errormsg("mikeServe.conf not found. Please specify a config file\n", 1, 1);
		// This will exit the program
	}
	int MAXLINE = 50;

	char linebuff[MAXLINE];
	char ansbuff[MAXLINE];
	int intbuff;

	while(!feof(fille)){
		fgets(linebuff, MAXLINE, fille);
		if (sscanf( linebuff, "PORT: %d\n", &intbuff) > 0){
			PORT = intbuff;
		}
		if (sscanf( linebuff, "ROOT: %s\n", ansbuff) > 0){
			free(ROOT);
			ROOT = strdup(ansbuff);
		}
		if (sscanf( linebuff, "LOG FILE: %s\n", ansbuff) > 0){
			free(LOGFILE);
			LOGFILE = strdup(ansbuff);
		}
		if (sscanf( linebuff, "LANDING PAGE: %s\n", ansbuff) > 0){
			free(LAND);
			LAND = strdup(ansbuff);
		}
	}

	fclose(fille);
}

void writeConfFile(){
	FILE* fille = fopen(CONF, "w+");

	if (fille == NULL){
		errormsg("Cannot create new conf file. Your options will not be saved!\n", 0, 0);
		return;
	}
	fprintf(fille, "PORT: %d\n", PORT);
	fprintf(fille, "ROOT: %s\n", ROOT);
	fprintf(fille, "LOG FILE: %s\n", LOGFILE);
	fprintf(fille, "LANDING PAGE: %s\n", LAND);

	fclose(fille);
	return;
}

#endif
