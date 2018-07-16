#ifndef CONFOPTS
#define CONFOPTS

// Lets declare some global pointers
// This will eventually help phase out the constants above
int PORT;
char* ROOT;
char* CONF;
int SAVE;

void freeGlobals(){
	free(ROOT);
	free(LOGFILE);
	free(CONF);
}


void getConfOptions(){
	printf("-h:\tReturns a list of commands\n");
	printf("-l <filename>:\tSpecify a log file\n");
	printf("-c <filename>:\tSpecify a config file\n");
	printf("-s:\tSave configuartion options / Generate a mikeServe.conf file\n");
	freeGlobals();
	exit(0);
}

void readTermConf(int argc, char** argv){
	int conf;
	while ((conf = getopt(argc, argv, "hsl:c:")) != -1 ){
		switch (conf){
			case 'h':
				getConfOptions();
				break;
			case 'l':
				LOGFILE = strdup(optarg);
				break;
			case 'c':
				CONF = strdup(optarg);
				break;
			case 's':
				SAVE = 1;
				break;
		}
	}
}

void readConfFile(){
	FILE* fille = fopen(CONF, "r");
	if(fille == NULL){
		errormsg("mikeServe.conf not found. Please specify a config file using the \n", 1, 1);
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

	fclose(fille);
	return;
}

#endif
