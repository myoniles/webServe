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
	while ((conf = getopt(argc, argv, "dhsftc:l:i:")) != -1 ){
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

	char buff[50];
	int buffInt;

	if (fscanf(fille, "%d\n", &buffInt) > 0){
		PORT = buffInt;
	}
	if (	fscanf(fille, "%s\n", buff) > 0 ){
		free(ROOT);
		ROOT = strdup(buff);
	}
	if (	fscanf(fille, "%s\n", buff) > 0 ){
		free(LOGFILE);
		LOGFILE = strdup(buff);
	}
	if (	fscanf(fille, "%s\n", buff) > 0 ){
		free(LAND);
		LAND = strdup(buff);
	}

	fclose(fille);
}

void writeConfFile(){
	FILE* fille = fopen(CONF, "w+");

	if (fille == NULL){
		errormsg("Cannot create new conf file. Your options will not be saved!\n", 0, 0);
		return;
	}
	fprintf(fille, "%d\n", PORT);
	fprintf(fille, "%s\n", ROOT);
	fprintf(fille, "%s\n", LOGFILE);
	fprintf(fille, "%s\n", LAND);

	fclose(fille);
	return;
}

#endif
