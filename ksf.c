#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>

// A user-inputted value specifying the total number of engines
int engineNum;

// A user-inputted value specifying the total number of fuselages	
int fuselageNum;
	
// A user-inputted value specifying the total number of fuel tanks
int fuelTankNum;
	
// A user-inputted value specifying the total number of Kerbals
int kerbalNum;

// NOTE: ADD DESCRIPTION
enum state {ENTERING_FOR_ASSEMBLY, ASSEMBLING, DISASSEMBLING};
enum state kerbalState;

// NOTE: ADD DESCRIPTION
pthread_mutex_t entrance;

pthread_cond_t af;
pthread_condattr_t af_attr;

/* 
Function Purpose: 
	To display a message to the user if they pass invalid values as parameters when initally executing this program.
*/
void print_input_error()
{
	printf("An input-based error occurred. Be sure to enter four integers between 1 and 255.\n");
	printf("Try to input a command as follows:\n");
	printf("./[compiled_name] 1 2 3 4\n");
}


/*
Function Purpose:
	Test a particular kerbal (pthread) value.
NOTE: 
	Maybe we need the infinite loop to be in main?
	Shouldn't this cause an infinite number of each thread to be created?
	printf() causing them all to be dumped together? Or are they actuallly being synchronized because of pthread_join()?
*/
void* begin_sequence(void* kerbalID)
{
	kerbalState = ENTERING_FOR_ASSEMBLY;
	while(1)
	{
		//ENTERING BUILDING
		pthread_mutex_lock(&entrance);
		if(kerbalState == ENTERING_FOR_ASSEMBLY)
		{
		
			pthread_mutex_unlock(&entrance);		
		}
		
		else if (kerbalState == ASSEMBLING)
		{
			
		}
		
		else if(kerbalState == DISASSEMBLING)
		{

		} 
		
	}
}

int main(int argc, char* argv[])
{
	// Check to see if enough parameters were passed to main()
	if(argc != 5)
	{
		print_input_error();
		return -1;
	}

	// Assign the user-inputted values to their respective global variable
	engineNum = atoi(argv[1]);
	fuselageNum = atoi(argv[2]);
	fuelTankNum = atoi(argv[3]);
	kerbalNum = atoi(argv[4]);

	// A counter used in `for-loops`
	long i;

	// Check to see if acceptable values were passed to main()
	if( !((engineNum > 0) && (engineNum < 256) 
		&& (fuselageNum > 0) && (fuselageNum < 256)
		&& (fuelTankNum > 0) && (fuelTankNum < 256)
		&& (kerbalNum > 0) && (kerbalNum < 256)) )
	{
		print_input_error();
		return -1;
	}

	//NOTE: ADD DESCRIPTION
	pthread_cond_init(&af, &af_attr);

	// An array will be used to hold all of the kerbals (pthreads)
	pthread_t kerbalArray [kerbalNum];

	for (i = 0; i < kerbalNum; i++)
	{	
		pthread_create(&kerbalArray[i], NULL, begin_sequence, (void*) i);
	}

	//	pthread_join(kerbalArray[0], NULL);
	// NOTE: For some reason, we need this 'join' otherwise there is no output.
	// As far as I understand it, we shouldn't need join.		
	for(i = 0; i < kerbalNum; i++)
	{
		if(pthread_join(kerbalArray[i], NULL) != 0)
		{
			printf("DEADLOCK DETECTED.\n");
			return -1;
		}
	}

	return 0;
}
