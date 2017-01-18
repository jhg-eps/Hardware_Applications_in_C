#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <lirc/lirc_client.h>
#include <time.h>

void display_math_op(char * code);
void do_the_math(int r, int n1, int n2, char * code);
void numbers_from_strings(int nn[10], int r, int * n1, int * n2, int t);
void log_each_number(char * message, int value, int * t, int * buttonTimer, int nnn[10]);
void welcome_message();
void grab_the_numbers(int t, int * digit, int * buttonTimer, int n, char * code, int * n1, int * n2);
void check_button(int digit, int * t, int * buttonTimer, int n);

int main(int argc, char *argv[])
{
        struct lirc_config *config;

        if(lirc_init("lirc",1)==-1) {
            exit(EXIT_FAILURE);     // Initialize LIRC. Exit on Failure 
        }

        char *code;
        char *c;
		char *digit;
		char pidbuf[10];
		int n[10];
		int n1;
		int n2;
		
		int t;
	    int buttonTimer;   // timer for the space between remote button presses
    
        buttonTimer = millis();
        welcome_message();
		
		//Read the default LIRC config at /etc/lirc/lircd.conf  This is the config for your remote.
        if(lirc_readconfig(NULL,&config,NULL)==0)  { 
            //Do stuff while LIRC socket is open  0=open  -1=closed.
			printf("=======================================================\n");
			while(lirc_nextcode(&code)==0 && (!strstr(code,"BTN_TL"))) {            
                // code = NULL means nothing was returned from LIRC socket; wait for another button press
				if(code==NULL) continue;
				display_math_op(code);				
                grab_the_numbers(t, &digit, &buttonTimer, n, code, &n1, &n2);
                free(code);    // free the math operation code for the next iteration of the operation selection loop
			}
		}
    //Frees the data structures associated with config.
    lirc_freeconfig(config);
    //lirc_deinit() closes the connection to lircd and does some internal clean-up stuff.
    lirc_deinit();
    exit(EXIT_SUCCESS);
}

void welcome_message()
{
	printf("Welcome to the Raspberry Pi Remote Calculator!\n");
	printf("Use + for addition, - for subtraction, \n^ for multiply, and v for division.\n");
	printf("Press the D-Pad Center Button twice to \ngo to the next input number.\n");
	printf("To quit, press th D-Pad Center Button at\nthe Operator-choosing line.\n");    
}

void grab_the_numbers(int t, int * digit, int * buttonTimer, int n, char * code, int * n1, int * n2)
{
    int r = 0;
    int END = 0;
    if (millis() - *buttonTimer  > 400){   // make sure of 400 ms gap between recognized button presses
        for (r = 0; r < 2; r++) {       // loop through the variables we want to add multiply/divide/etc. 
t = 0;  
	 END = 0;                              // initialize size of each value (how many digits long it is)
	while((lirc_nextcode(digit)==0) && (END == 0)) {
		if(*digit==NULL) continue;
		if (millis() - buttonTimer  > 400){   // make sure of 400 ms gap between recognized button presses
			if (strstr(digit,"BTN_TL")) {
				printf("Preparing to Display Input\n");
				END = 1;
				*buttonTimer = millis();
			}
			 check_button(*digit, t, buttonTimer, n);
		}
	 free(digit);
	 }
	 numbers_from_strings(n, r, n1, n2, t);
	 do_the_math(r, *n1, *n2, code);
     }									// USER WANTS OPERATION END 
	}
}

void display_math_op(char * code)
{
	if(strstr(code,"KEY_PROG1")) printf("ready to process (+) operands\n");
	if(strstr(code,"KEY_PROG2")) printf("ready to process (-) operands\n");
	if(strstr(code,"KEY_PROG3")) printf("ready to process (x) operands\n");
	if(strstr(code,"KEY_PROG4")) printf("ready to process (/) operands\n");
}

void log_each_number(char * message, int value, int * t, int * buttonTimer, int nnn[10])
{
		printf("%s", message);
		nnn[*t] = value;
		*t += 1;
		*buttonTimer = millis();
}

void numbers_from_strings(int nn[10], int r, int * n1, int * n2, int t)
{
	int i = 0 ;
	int index = 0 ;
	printf("Input Digits: ");
	for (i = 0; i < (t); i++) printf("(%d)",nn[i]);
	printf("\n");
	const int length_n = sizeof(nn)/sizeof(*nn);
	char nstr[length_n * 10 ] ; // sized to accommodate worst case string length for 32 bit int of INT_MAX.
	for(i = 0; i < (t); i++ )
	{
	    index += sprintf( &nstr[index], "%d", nn[i] ) ; 
	}
	if (r == 0) {
		*n1 = atoi(nstr);
		printf( "\n %d \n", *n1) ;
	}
	if (r == 1) {
		*n2 = atoi(nstr);
		printf("\n %d \n", *n2);
	}
}

void do_the_math(int r, int n1, int n2, char * code)
{
	if (r == 1){
		printf("\nDoing the Math....\n");
		if (strstr(code, "KEY_PROG1")) {  // add
			printf("%d + %d = %d\n", n1, n2, n1 + n2);	
			printf("=======================================================\n");
		}

		if (strstr(code, "KEY_PROG2")) {   // subtract
			printf("%d - %d = %d\n", n1, n2, n1 - n2);
			printf("=======================================================\n");
		}

		if (strstr(code, "KEY_PROG3")) {    // multiply 
			printf("%d * %d = %d\n", n1, n2, n1*n2);
			printf("=======================================================\n");
		}

		if (strstr(code, "KEY_PROG4")) {   // divide
			printf("%d / %d = %f\n", n1, n2, (double)n1/(double)n2);
			printf("=======================================================\n");
		}
	}
	
}


void check_button(int digit, int * t, int * buttonTimer, int n)
{
	if (strstr(digit,"KEY_0"))	log_each_number("0 logged\n", 0, t, buttonTimer, n);
	if (strstr(digit,"KEY_1")) 	log_each_number("1 logged\n", 1, t, buttonTimer, n);
	if (strstr(digit,"KEY_2")) 	log_each_number("2 logged\n", 2, t, buttonTimer, n);
	if (strstr(digit,"KEY_3")) 	log_each_number("3 logged\n", 3, t, buttonTimer, n);
	if (strstr(digit,"KEY_4")) 	log_each_number("4 logged\n", 4, t, buttonTimer, n);
	if (strstr(digit,"KEY_5")) 	log_each_number("5 logged\n", 5, t, buttonTimer, n);
	if (strstr(digit,"KEY_6")) 	log_each_number("6 logged\n", 6, t, buttonTimer, n);
	if (strstr(digit,"KEY_7")) 	log_each_number("7 logged\n", 7, t, buttonTimer, n);
	if (strstr(digit,"KEY_8")) 	log_each_number("8 logged\n", 8, t, buttonTimer, n);
	if (strstr(digit,"KEY_9")) 	log_each_number("9 logged\n", 9, t, buttonTimer, n);
}
