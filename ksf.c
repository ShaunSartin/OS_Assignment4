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

// NOTE: ADD DESCRIPTION
pthread_mutex_t entrance;

pthread_cond_t af;
pthread_condattr_t af_attr;

sem_t door;

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
	enum state kerbalState = ENTERING_FOR_ASSEMBLY;
	while(1)
	{
		//ENTERING BUILDING
		if(kerbalState == ENTERING_FOR_ASSEMBLY)
		{
			printf("Kerbal %ld is trying entering assembly mutex\n", (long) kerbalID);
			//pthread_mutex_lock(&entrance);
			printf("Kerbal %ld grabbed entering assembly mutex\n", (long) kerbalID);
			printf("Kerbal %ld trying semaphore\n", (long) kerbalID);
			sem_wait(&door);
			printf("Kerbal %ld got semaphore\n", (long) kerbalID);
			if((engineNum > 0) && (fuselageNum > 0) && (fuelTankNum > 0))
			{
				printf("Kerbal %ld: has entered the building.\n", (long) kerbalID);
				kerbalState = ASSEMBLING;
			}
			else
			{
			        sem_post(&door);
				printf("Kerbal %ld: is waiting to enter the assembly building.\n", (long) kerbalID);
                                sleep(1);
			}
			printf("Kerbal %ld is unlocking entering assembly mutex\n", (long) kerbalID);
			//pthread_mutex_unlock(&entrance);
			printf("Kerbal %ld unlocked entering assembly mutex\n", (long) kerbalID);
		}

		else if (kerbalState == ASSEMBLING)
		{
			printf("Kerbal %ld is tryiung assembly mutex\n", (long) kerbalID);
			pthread_mutex_lock(&entrance);
			printf("Kerbal %ld grabbed assembly mutex\n", (long) kerbalID);
			engineNum -= 1;
			fuselageNum -= 1;
			fuelTankNum -= 1;
			printf("Kerbal %ld posting semaphore\n", (long) kerbalID);
			sem_post(&door);
			printf("Kerbal %ld: has finished assembling a rocket. (%d Engine(s) %d Fuselage(s) %d Fuel Tank(s) remaining)\n", 
				(long int) kerbalID, engineNum, fuselageNum, fuelTankNum);
			printf("Kerbal %ld is unlocking assembly mutex\n", (long) kerbalID);
			pthread_mutex_unlock(&entrance);
			printf("Kerbal %ld unlocked assembly mutex\n", (long) kerbalID);
			printf("Changing Kerbal %ld's state to DISASSEMBLING\n", (long) kerbalID);
			kerbalState = DISASSEMBLING;
			sleep(5);
			printf("Kerbal %ld woke up.\n", (long) kerbalID);	
		}
		
		else if(kerbalState == DISASSEMBLING)
		{
			printf("Kerbal %ld is now beginning the disassembling process\n", (long) kerbalID);
			pthread_mutex_lock(&entrance);	
			printf("Kerbal %ld grabbed disassemble mutex\n", (long) kerbalID);
			engineNum += 1;
			fuselageNum += 1;
			fuelTankNum += 1;
			printf("Kerbal %ld: has finished disassembling a rocket. (%d Engine(s) %d Fuselage(s) %d Fuel Tank(s) remaining)\n", 
				(long int) kerbalID, engineNum, fuselageNum, fuelTankNum);
			printf("Kerbal %ld released grabbed disassemble mutex\n", (long) kerbalID);
			pthread_mutex_unlock(&entrance);
			kerbalState = ENTERING_FOR_ASSEMBLY;
			sleep(1);
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

	// ADD DESCRIPTION
	sem_init(&door, 0, 1);

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
