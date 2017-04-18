#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

//Function Purpose: To display a message to the user if they pass invalid values as parameters when initally executing this program.
void print_input_error()
{
	printf("An input-based error occurred. Be sure to enter four integers between 1 and 255.\n");
	printf("Try to input a command as follows:\n");
	printf("./[compiled_name] 1 2 3 4\n");
}


int main(int argc, char* argv[])
{
	if(argc != 5)
	{
		print_input_error();
		return -1;
	}


	// A user-inputted value specifying the total number of engines
	int engineNum = atoi(argv[1]);

	// A user-inputted value specifying the total number of fuselages
	int fuselageNum = atoi(argv[2]);
	
	// A user-inputted value specifying the total number of fuel tanks
	int fuelTankNum = atoi(argv[3]);
	
	// A user-inputted value specifying the total number of Kerbals
	int kerbalNum = atoi(argv[4]);

	if( !((engineNum > 0) && (engineNum < 255) 
		&& (fuselageNum > 0) && (fuselageNum < 255)
		&& (fuelTankNum > 0) && (fuelTankNum < 255)
		&& (kerbalNum > 0) && (kerbalNum < 255)) )
	{
		print_input_error();
		return -1;
	}

	printf("%d, %d, %d, %d\n", engineNum, fuselageNum, fuelTankNum, kerbalNum);

	return 0;
}
