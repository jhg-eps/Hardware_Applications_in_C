#include <stdio.h>
#include <string.h>
#include <errno.h>

const int STR_SIZE = 1024;
const int HELP_MESSAGE = 784;

void inv_file_message();
void no_file_message();
void final_action(int f_action, const int HELP_MESSAGE);
int find_f_position(int argc, char * argv[]);
void choose_output(int f_position, char * result, char * file_to_write);
void prepare_model_info(char str_buf[STR_SIZE], int f_position, char * file_to_write);
int prepare_proc_info(char str_buf[STR_SIZE], int f_position, char * file_to_write);
void count_procs_name_models(int f_position, char * file_to_write, FILE * fp_cpu, char * watch_string);
void determine_the_args(int argc, char * argv[], int f_position, char * file_to_write, FILE * fp_cpu);
int determine_f_action(int argc, char * argv[], FILE * fp_cpu);
void help_message();

int main(int argc, char * argv[])
{
	int i = 0;
  	int f_action = 0;
  	FILE * fp_cpu;

   	// make sure that the user is using the program correctly.
    if ((argc == 1) || (strcmp(argv[1],"-h") == 0)) {
  	help_message();  // print the help message if user does $ ./cpuinfo or $ ./cpuinfo -h [...OPTIONAL]
  	} else {                                                              
  		f_action = determine_f_action(argc, argv, fp_cpu);  
		final_action(f_action, HELP_MESSAGE);
	}
	return(0);
}

void help_message()
{
   printf("\n*****************************************************************\n");
   printf("Usage: ./cpuinfo [OPTIONS]...[-f FILE]...\n");
   printf("./cpuinfo can list the model names of each processor, \nhow many processors there are, and can print to stdout or FILE.\n");
   printf("\n******************** OPTIONS ************************************\n");
   printf("\n  -p,           list how many processors the system has\n");
   printf("  -m,           list each processor with its model name\n");
   printf("  -f,           redirect output to the file FILE\n");
   printf("  -mp, -pm      does -p and -m\n");
   printf("  -h,           print this helpful message!\n");
   printf("\nNote that running just ./cpuinfo will print this message as well.\nThe option -f should always be the right-most option.\n");
   printf("If no FILE is specified, output is directed to stdout (the screen).\n");
   printf("*******************************************************************\n\n");
}

int determine_f_action(int argc, char * argv[], FILE * fp_cpu)
{
	int f_position = 0;
	
	if (strcmp(argv[argc - 1],"-f") == 0) {
		return EPERM;  // User put last option as -f without a file. We need to call no_file_message (WORKS)
	} 
	
	f_position = find_f_position(argc, argv);

	if ((f_position < (argc - 2)) && (f_position > 0)) {
		return HELP_MESSAGE;         // there are at least two strings after -f. not acceptable. We call help_message (WORKS)
	}

	if (f_position == 1) {
		return HELP_MESSAGE;    // the person has ./cpuinfo -f FILE at this point. They forgot to include some commands!
	}

	// If we haven't returned at this point, it's time to parse the command line arguments.
	determine_the_args(argc, argv, f_position, argv[argc -1], fp_cpu);  // RETURN VALUE????
	return 0;
}

void determine_the_args(int argc, char * argv[], int f_position, char * file_to_write, FILE * fp_cpu)
{
	int k = 0;
    int yescow = 0;
    char cowstring[500] = "cowsay -f tux ";
	for (k = 0; k < argc; k++) {
 		if (strcmp(argv[k], "-p") == 0) {
 	  		count_procs_name_models(f_position, file_to_write, fp_cpu, "processor");
 	  	} 
 		else if (strcmp(argv[k],"-m") == 0) {
 			count_procs_name_models(f_position, file_to_write, fp_cpu, "model name");
		}
		else if ((strcmp(argv[k],"-pm") == 0) || (strcmp(argv[k],"-mp") == 0)) {
			count_procs_name_models(f_position, file_to_write, fp_cpu, "processor");
			count_procs_name_models(f_position, file_to_write, fp_cpu, "model name");             // allow -mp / -pm as legitimate commands.
		}
}

void count_procs_name_models(int f_position, char * file_to_write, FILE * fp_cpu, char * watch_string)
{
	const int STR_SIZE = 1024;
   	char str_buf[STR_SIZE];
   	char * block = ": ";
   	char * result;
   	if (strcmp(watch_string,"processor") == 0){ 
		prepare_proc_info(str_buf, f_position, file_to_write);
   	}
   	if (strcmp(watch_string, "model name") == 0) {
   		prepare_model_info(str_buf, f_position, file_to_write);
   	}
}

int prepare_proc_info(char str_buf[STR_SIZE], int f_position, char * file_to_write)
{                             
	char * result = (char *)malloc(STR_SIZE*sizeof(char));
   	FILE * fp_cpu = fopen("/proc/cpuinfo" , "r");   
   	if(fp_cpu == NULL) {
		perror("Error opening file\n");
      	return(-1);
    }
	int procs = 0;
	while (fgets(str_buf, STR_SIZE, fp_cpu) != NULL ) {
		if (strstr(str_buf, "processor") != NULL) {
     		procs += 1;
      	}
    }  
    sprintf(result, "Number of procs: %d\n", procs);
    choose_output(f_position, result, file_to_write);
    free(result);
} 	

void prepare_model_info(char str_buf[STR_SIZE], int f_position, char * file_to_write)
{

	char * block = ": ";
	char * print_from = 0;
	int models = 0;

	char * result = (char *)malloc(STR_SIZE*sizeof(char));
   	FILE * fp_cpu = fopen("/proc/cpuinfo" , "r");   
   	if(fp_cpu == NULL) {
		perror("Error opening file\n");
    }
	
	while (fgets(str_buf, STR_SIZE, fp_cpu) != NULL ) {
    	if (strstr(str_buf, "model name") != NULL) {
      		print_from = strstr(str_buf, block);
      		sprintf(result, "Model %d is %s", models, print_from);
      		choose_output(f_position, result, file_to_write);
      		models += 1;
      	}
    } 
    free(result); 
} 	

void choose_output(int f_position, char result[], char * file_to_write)
{
	FILE * fp_to_write;
    if (f_position == 0) {
   		fputs(result,stdout);              // -f FILE never specified so just print to screen
   	} 
   	if (f_position > 0) {
   		fp_to_write = fopen(file_to_write, "a");            // test that we can open file
   		if(fp_to_write != NULL) {                                                  
			fputs(result,fp_to_write);                    // write to that file 
		} 
		if (fp_to_write == NULL) {
			void inv_file_message(); // give failure notice if fopen failed
		}
		fclose(fp_to_write);
	}
}

int find_f_position(int argc, char * argv[])
{
	int k = 0;
	int position = 0;
	for(k = 0; k < argc; k++) {
		if(strcmp(argv[k], "-f") == 0) {
			position = k;
		}
	}
	return position;
}

void final_action(int f_action, const int HELP_MESSAGE)
{
	switch (f_action){
		case EPERM:
			no_file_message();
		break;
		case ENOENT:
			inv_file_message();
		break;
		default:
			// IDK
		break;
	}
	if (f_action == HELP_MESSAGE) {
		help_message();
	}
}

void no_file_message()
{
	printf("\nYou have specified -f but no file afterward.\nPlease specify a file FILE.\n\n");
}

void inv_file_message()
{
	printf("\nThe file specified is not valid. It may not have the proper permissions, be corrupted, etc.\n");
}
