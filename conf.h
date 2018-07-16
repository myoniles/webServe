#ifndef CONFOPTS
#define CONFOPTS

// Lets declare some global pointers
// This will eventually help phase out the constants above
int PORT;
char* ROOT;
char* CONF;
int SAVE;

void getConfOptions(){
	printf("-h:\tReturns a list of commands\n");
	printf("-l <filename>:\tSpecify a log file\n");
	printf("-c <filename>:\tSpecify a config file\n");
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

	fscanf(fille, "%d\n", &PORT);
	fscanf(fille, "%s\n", ROOT);
	fscanf(fille, "%s\n", LOGFILE);
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
	return;
}

#endif
