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
enum state {ENTERING_FOR_ASSEMBLY, ASSEMBLING, LAUNCHPAD, FLIGHT, ENTERING_FOR_DISASSEMBLY, DISASSEMBLING};

// NOTE: ADD DESCRIPTION
pthread_mutex_t partModification;
pthread_mutex_t launch;
pthread_mutex_t entrance;

pthread_cond_t af;
pthread_condattr_t af_attr;

sem_t door;
sem_t launchpad;

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
	
	// NOTE: ADD DESCRIPTION
	int doorFlag = 0;
	int launchpadFlag = 0;	

	while(1)
	{
		if(kerbalState == ENTERING_FOR_ASSEMBLY)
		{
			//printf("Kerbal %ld: trying door semaphore\n", (long) kerbalID);
			while((sem_trywait(&door) != 0) && (doorFlag == 0))
			{
				printf("Kerbal %ld: is waiting to enter the assembly building.\n", (long) kerbalID); 
				doorFlag = 1;
			}

			doorFlag = 0;

			pthread_mutex_lock(&entrance);
			//printf("Kerbal %ld: got door semaphore\n", (long) kerbalID);
			if((engineNum > 0) && (fuselageNum > 0) && (fuelTankNum > 0))
			{
				printf("Kerbal %ld: has entered the building.\n", (long) kerbalID);
				pthread_mutex_unlock(&entrance);
				kerbalState = ASSEMBLING;
			}
			else
			{
				pthread_mutex_unlock(&entrance);
				// Need this sem_post() to cover the case where no parts remain,
				// 	causing this else-statement to execute, thus not causing the sem_post() in ASSEMBLING
				//	to never execute.
				//printf("Kerbal %ld: is attempting to post door semaphore.\n", (long) kerbalID);
			        sem_post(&door);
				//printf("Kerbal %ld: has posted the door semaphore.\n" (long) kerbalID);
				// ^ SPAM PROBLEM
                                sleep(1);
			}
		}

		else if (kerbalState == ASSEMBLING)
		{
			//printf("Kerbal %ld: is trying assembly mutex\n", (long) kerbalID);
			pthread_mutex_lock(&partModification);
			//printf("Kerbal %ld: grabbed assembly mutex\n", (long) kerbalID);
			engineNum -= 1;
			fuselageNum -= 1;
			fuelTankNum -= 1;
			//printf("Kerbal %ld: posting door semaphore\n", (long) kerbalID);
			sem_post(&door);
			//printf("Kerbal %ld: has posted the door semaphore.\n", (long) kerbalID);
			printf("Kerbal %ld: has finished assembling a rocket. (%d Engine(s) %d Fuselage(s) %d Fuel Tank(s) remaining)\n", 
				(long int) kerbalID, engineNum, fuselageNum, fuelTankNum);
			//printf("Kerbal %ld: is unlocking assembly mutex\n", (long) kerbalID);
			pthread_mutex_unlock(&partModification);
			//printf("Kerbal %ld: unlocked assembly mutex\n", (long) kerbalID);
			//printf("Changing Kerbal %ld's state to LAUNCHPAD\n", (long) kerbalID);
			kerbalState = LAUNCHPAD;
			sleep(5);
			//printf("Kerbal %ld: woke up.\n", (long) kerbalID);	
		}

		else if (kerbalState == LAUNCHPAD)
		{
			
			//printf("Kerbal %ld: is trying launchpad mutex\n", (long) kerbalID);
			while((sem_trywait(&launchpad) != 0) && (launchpadFlag == 0))
			{
				printf("Kerbal %ld: Waiting at the launchpad\n", (long) kerbalID);
				launchpadFlag = 1;
			}
			
			launchpadFlag = 0;
			pthread_mutex_lock(&launch);
		
			printf("Kerbal %ld: Arriving at the launchpad\n", (long) kerbalID);

			printf("Kerbal %ld: Launching in ... 10\n", (long) kerbalID);
			sleep(1);
			printf("Kerbal %ld: Launching in ... 9\n", (long) kerbalID);
			sleep(1);
			printf("Kerbal %ld: Launching in ... 8\n", (long) kerbalID);
			sleep(1);
			printf("Kerbal %ld: Launching in ... 7\n", (long) kerbalID);
			sleep(1);
			printf("Kerbal %ld: Launching in ... 6\n", (long) kerbalID);
			sleep(1);
			printf("Kerbal %ld: Launching in ... 5\n", (long) kerbalID);
			sleep(1);
			printf("Kerbal %ld: Launching in ... 4\n", (long) kerbalID);
			sleep(1);
			printf("Kerbal %ld: Launching in ... 3\n", (long) kerbalID);
			sleep(1);
			printf("Kerbal %ld: Launching in ... 2\n", (long) kerbalID);
			sleep(1);
			printf("Kerbal %ld: Launching in ... 1\n", (long) kerbalID);
			sleep(1);
			printf("Kerbal %ld: Liftoff\n", (long) kerbalID);
			
			pthread_mutex_unlock(&launch);
			//printf("Kerbal %ld: is unlocking launchpad mutex\n", (long) kerbalID);
			sem_post(&launchpad);
			//printf("Kerbal %ld: unlocked launchpad mutex\n", (long) kerbalID);
			kerbalState = FLIGHT;
			//printf("Changed Kerbal %ld's state to FLIGHT\n", (long) kerbalID);
		}


		else if (kerbalState == FLIGHT)
		{
			sleep(5);
			printf("Kerbal %ld: Landed\n", (long) kerbalID);
			kerbalState = ENTERING_FOR_DISASSEMBLY;
		}

		
		// Since there is no limit on how many kerbals can be in the assembly building, I don't think we need this.	
		else if (kerbalState == ENTERING_FOR_DISASSEMBLY)
		{
			kerbalState = DISASSEMBLING;
		}
	
		else if(kerbalState == DISASSEMBLING)
		{
			//printf("Kerbal %ld: is now trying the disassemble mutex\n", (long) kerbalID);
			pthread_mutex_lock(&partModification);	
			//printf("Kerbal %ld: grabbed disassemble mutex\n", (long) kerbalID);
			engineNum += 1;
			fuselageNum += 1;
			fuelTankNum += 1;
			printf("Kerbal %ld: has finished disassembling a rocket. (%d Engine(s) %d Fuselage(s) %d Fuel Tank(s) remaining)\n", 
				(long int) kerbalID, engineNum, fuselageNum, fuelTankNum);
			//printf("Kerbal %ld: released disassemble mutex\n", (long) kerbalID);
			pthread_mutex_unlock(&partModification);
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
	sem_init(&launchpad, 0, 1);

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
