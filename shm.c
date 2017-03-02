#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<sys/time.h>

int main(){
struct timeval start, end;

int dimension;
int smid;
unsigned int total = 0;
unsigned int element = 0;
pid_t pid;

printf("Input the matrix dimension: ");
scanf("%d",&dimension);
   /* Obtain a shared memory segment with the key 40 */
    int shma= shmget (40, dimension*dimension*sizeof (unsigned int), IPC_CREAT | 0666);
    if (shma < 0)
    {
        perror ("shmget");
        return 1;
    }

    /* Attach the segment as an int array */
    unsigned int *matrixA = shmat (shma, NULL, 0);
    if (matrixA < (unsigned int *) NULL)
    {
        perror ("shmat");
        return 1;
    }
     /* Obtain a shared memory segment with the key 41 */
    int shmb= shmget (41, dimension*dimension*sizeof (unsigned int), IPC_CREAT | 0666);
    if (shmb < 0)
    {
        perror ("shmget");
        return 1;
    }

    /* Attach the segment as an int array */
    unsigned int *matrixB = shmat (shmb, NULL, 0);
    if (matrixB < (unsigned int *) NULL)
    {
        perror ("shmat");
        return 1;
    }
       /* Obtain a shared memory segment with the key 42 */
    int shmc = shmget (42, dimension*dimension*sizeof (unsigned int), IPC_CREAT| 0666);
    if (shmc < 0)
    {
        perror ("shmget");
        return 1;
    }

    /* Attach the segment as an int array */
    unsigned int *matrixC = shmat (shmc, NULL, 0);
    if (matrixC < (unsigned int *) NULL)
    {
        perror ("shmat");
        return 1;
    }
   unsigned int k = 0;
for(int i = 0 ; i < dimension ;i ++){
	for(int j =0; j <dimension;j++){
		matrixA[i*dimension+j] = matrixB[i*dimension+j] = k ;		
		k++;
	}
}

int forktimes;
for(int numoffork = 1; numoffork <= 16;numoffork++){
	
	gettimeofday(&start, 0);

	int i, tmp = 0;
	for(forktimes = 1; forktimes <= numoffork; forktimes++){
	/////child
	pid = fork();
if(pid ==0){
	tmp = (dimension/numoffork)*(forktimes-1);
	
	if(!fork()){
	
	for(i = tmp ; i < (tmp+dimension/numoffork);i ++){
		for(int j = 0 ; j <dimension;j++){
                        for(int k = 0; k<dimension;k++){
			      element += matrixA[k+i*dimension]*matrixB[k*dimension+j];
				}
                        matrixC[i*dimension+j] = element;
                        element = 0;
			}
		   }
			exit(0);
		}
		else wait(NULL);
		exit(0);
}//child end
}//forktimes end

for(int g = 1 ;g<=forktimes;g++)
	wait(NULL);

 for(int i = 0 ; i < dimension ;i ++){
	for(int j =0; j <dimension;j++){
		total+= matrixC[i*dimension+j];
	        }
        }
   gettimeofday(&end, 0);
   int sec=end.tv_sec - start.tv_sec;
   int usec = end.tv_usec - start.tv_usec;
   printf("Multiplying matrices using %d processes\nelapsed %f sec, Checksum: %u\n",numoffork, (sec*1000+(usec/1000.0))/1000,total);
	total = 0;

}//numoffork end
 /* Detach the shared memory segment and delete its key for later reuse */
    	shmdt (matrixA);
 	shmdt (matrixB);
 	shmdt (matrixC);
        shmctl (shma, IPC_RMID, NULL);
 	shmctl (shmb, IPC_RMID, NULL);
	shmctl (shmc, IPC_RMID, NULL);

return 0;
}
