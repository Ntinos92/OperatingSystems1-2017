#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/wait.h>
#include "myHeader.h"

union semun {
 int val;
 struct semid_ds *buf;
 unsigned short int *array;
};


int main( int argc, char* argv[] ){

	if(argc != 3){					//Checks for correct user input arguments.
		printf("Wrong number of arguments!\n");
	}

	int Status;
	int n = atoi(argv[2]);			//Number n
	int M = atoi(argv[1]);			//Number M
	int i = 0;
	int ArrayEls[M];					//Feeder array
	clock_t sharedTime;
	key_t key = 1234;
	pid_t* Pids;					//array for keeping pids
	FILE* file;

	
	sharedTime = clock();
	printf("Array Elements number is: %d \n""Consumers number is: %d \n", M, n);

	int random_value;
	for(i = 0; i < M; i++){				//Adds random numbers in the array
        	random_value = rand() % 20;
    		ArrayEls[i] = random_value;
	}

	printf("Array Elements are:");
	for(i = 0; i < M; i++){				//prints array

		printf("%d ",ArrayEls[i]);
	}	
	printf("\n");

	if( (Pids = malloc(n*sizeof(pid_t))) == NULL)	//array to keep pids
	{
		perror("Malloc failed ");
		exit(1);
	}


	sem_id  = Get_Semaphores (key);			//Creates 3 semaphores
	Init_Semaphores(sem_id);			//Initilizes semaphores to 1
	Init_and_Attach_Shared_Memory();		//Creates and attaches shared memory
	file = fopen("logFile.txt","w");
	if (file == NULL){
		printf("Error opening file!");
		exit(0);
	}
	

	for(i = 0; i<M; i++){
		sharedTime = clock()-sharedTime;
		Sem_DOWN(sem_id,0);
		arrEl->arrayElem = ArrayEls[i];				//feeder writes to shared memory using sempaphore "0"
		arrEl->timestamp = sharedTime;
		Sem_UP(sem_id,0);

			for(int j = 0; j<n; j++){
				if( i == 0){		//It will fork child processes on 1st loop only

				Pids[j]=fork();
				}
				if(Pids[j] < 0)
				{
					perror("Fork Failed ");
					exit(1);	
				}
				if(Pids[j] == 0){

					int childArray[M];
					float runAvg = 0.0;
					float runTime = 0.0;

					for(int k = 0; k<M; k++){			//Children created will read from shared memory using semaphore "0" and "1"

					Sem_DOWN(sem_id,0					//"0" for feeder synchronization
					Sem_DOWN(sem_id,1);					//"1" for children synchronization
					childArray[k] = arrEl->arrayElem;
					sharedTime = clock()-sharedTime;
					runTime = sharedTime - (arrEl->timestamp);
					runAvg = runAvg + runTime;
					Sem_UP(sem_id,1);
					Sem_UP(sem_id,0);

					}
					Sem_DOWN(sem_id,1);					//after filling their arrays, they print them in log file along with pid and running average
					for(i = 0; i<M; i++){
						fprintf(file, "%d  ",childArray[i]);
					}
					fprintf(file, "  %d %f \n", getpid(), runAvg/M);
					Sem_UP(sem_id,1);
					exit(0);
				}
			}

				
	}
	
	for(i=0; i < n; i++)
	{
		wait(&Status);			//Feeder waits for children
	}
	fclose(file);
	Destroy_Semaphores_Shared_Memory();
	free(Pids);
	return 0;
	
}

int Get_Semaphores(key_t key)
{
	int sem_id;
	if(( sem_id=semget(key, 2, 0666 | IPC_CREAT)) < 0)
	{
		perror(" semget "); 
		exit (1) ; 
	}
	printf("Creating a semaphore with ID: %d \n",sem_id );
	return sem_id;
}

void Init_Semaphores(int sem_id)					//Initializes semaphore and sets it to 1
{
	union semun mySems ;
	mySems.val=1;
	
	if(semctl(sem_id, 0, SETVAL, mySems) < 0)
	{
		perror(" Error semaphore 0 ");
		exit (1) ;
	}

	if(semctl(sem_id, 1, SETVAL, mySems) < 0)
	{
		perror(" Error semaphore 1 ");
		exit (1) ;
	}

	printf("Semaphores initialized and set to 1 successfully!\n");
}

void Init_and_Attach_Shared_Memory()			//Creates shared memory with size= our struct and attaches it
{
	shm_id = shmget(IPC_PRIVATE, sizeof(arrayElement), IPC_CREAT|0666);
	if(shm_id <0 ){
		perror("shmget() error");
		exit(EXIT_FAILURE);
	}
	

	arrEl = shmat(shm_id,(feederElement)0, 0);
	if(arrEl == (feederElement)-1){
        	perror("shmat() error");
        	exit(EXIT_FAILURE);
	}

	printf("Shared memory Initializes and attaches successfully! \n");
	
}

void Sem_UP(int sem_id , int SemNUM)
{
	struct sembuf UP={SemNUM, 1, 0}; 
	if( (semop(sem_id, &UP, 1)) < 0 ) 
	{
		printf("Semaphore %d --> ",SemNUM);
		perror("Error in  up ");
		exit(getpid());
	}
	
}

void Sem_DOWN(int sem_id , int SemNUM)
{
	struct sembuf DOWN={SemNUM, -1, 0};
	if( (semop(sem_id, &DOWN, 1)) < 0 ) 
	{
		printf("Semaphore %d --> ",SemNUM);
		perror("Error in down ");
		exit(getpid());
	}
}

void Destroy_Semaphores_Shared_Memory()
{
	struct shmid_ds Myshmid_ds;
	struct shmid_ds Myshmid_dss;
	
	if( shmdt(arrEl) < 0 )
	{
		perror("Error sto detach ");
		exit(1);
	}
	
	if( (shmctl(shm_id, IPC_RMID, &Myshmid_ds)) < 0 )
	{
		perror("Error Remove Shared Memory ");
		exit(1);
	}

	if( (semctl(sem_id, 0, IPC_RMID, 0)) < 0 )
	{
		perror("Error Remove Semaphores ");
		exit(1);
	}
}

