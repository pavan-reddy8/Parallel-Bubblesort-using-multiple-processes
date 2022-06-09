#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h> 
#include <unistd.h>

#define MAX_COUNT 1000000 // Max integers to sort
#define MAX_NUM 100 // Generate numbers between 0 and MAX_NUM
#define P_MAX_COUNT 100
// Uncomment the following line to turn on debugging messages
//#define DEBUG


long * number;
int * swapCount;
int * flag;
int * pass;
int * sorted;
int * evenSwapCount;
int * oddSwapCount;


int N; // Number of integers to sort
int P;

// generate N random numbers between 0 and MAX_NUM
void generate_numbers()
{
	int i;

	srandom(time(NULL));

	for(i=0; i<N; i++) 
		number[i] = random()%MAX_NUM;
}

void print_numbers() 
{
	int i;

	for(i=0; i<N; i++) 
		printf("%ld ", number[i]);
	printf("\n");
}

void printflag() 
{
	int i;
	for(i=0; i<P; i++) 
		printf("flag:%ld,i:%d ", flag[i],i);
}
void printEvenSwapCount() 
{
	int i;

	for(i=0; i<P; i++) 
		printf("evenswap:%ld", evenSwapCount[i]);
}
void printOddSwapCount() 
{
	int i;

	for(i=0; i<P; i++) 
		printf("oddswap:%ld ", oddSwapCount[i]);
}

void printSwapCount() 
{
	int i;

	for(i=0; i<P; i++) 
		printf("swap:%ld ", swapCount[i]);
}


int compare_and_swap(int i, int j) 
{
#ifdef DEBUG
	fprintf(stderr,"i %d j %d\n", i, j);
#endif
	assert ( i<N );
	assert ( j<N );

	if( number[i] > number[j]) {
		long temp = number[i];
		number[i] = number[j];
		number[j] = temp;
		return 1;
	}

	return 0;
}

// even-odd pass bubbling from start to start+n
int bubble(int start, int n, int pass) 
{
	

	//fprintf(stderr, "start %d n %d pass %d\n", start, n, pass);


	int swap_count = 0;
	int next = start;

	assert (start < N-1); // bug if we start at the end of array

	if (pass) { // sort odd-even index pairs
		if ( !(next % 2) ) 
			next = next + 1;
	} else  { // sort even-odd index pairs
		if (next % 2) 
			next = next +1;
	}
	//fprintf(stderr, "start %d n %d pass %d\n", next, n, pass);
	while ( (next+1) < (start+n) ) {
		swap_count += compare_and_swap(next, next+1);
		//fprintf(stdout,"next:%d,",next);
		next+=2;
        //printf("\n");
	}
	

	return swap_count;
}




void bubble_sort() 
{
	int last_count, swap_count = N;
	int pass = 0;

#ifdef DEBUG
	
#endif

	do {
		last_count = swap_count;
		swap_count = bubble(0, N, pass); // 0 for single-process sorting
#ifdef DEBUG

#endif
		pass = 1-pass;
	} while((swap_count!=0) || (last_count != 0));

	
}

int create_shm(){
		key_t key;
       	int shmid; 

       /* make the key: */
       	if ((key = ftok("shared", 'X')) < 0) {
            perror("ftok");
            exit(1);
       	}

       /* create the shared memory segment: */
	   shmid = shmget(key,MAX_COUNT*4 ,0644 | IPC_CREAT );
       	if (shmid  < 0) {
       	     perror("shmgetN");
       	     exit(1);
       	}
		number = (long *)shmat(shmid, NULL, 0);
			if (number == (long *)(-1)) {
			perror("shmat");
			exit(1);
		}

       	if ((key = ftok("shared", 'Y')) < 0) {
            perror("ftok");
            exit(1);
       	}
	   shmid = shmget(key,P_MAX_COUNT*4 ,0644 | IPC_CREAT );
       	if (shmid  < 0) {
       	     perror("shmget");
       	     exit(1);
       	}
		flag = (int *)shmat(shmid,NULL, 0);
			if (flag == (int *)(-1)) {
			perror("shmat");
			exit(1);
		}
				if ((key = ftok("shared", 'H')) < 0) {
            perror("ftok");
            exit(1);
       	}
	   shmid = shmget(key,P_MAX_COUNT*4 ,0644 | IPC_CREAT );
       	if (shmid  < 0) {
       	     perror("shmget");
       	     exit(1);
       	}
		swapCount = (int *)shmat(shmid,NULL, 0);
			if (evenSwapCount == (int *)(-1)) {
			perror("shmat");
			exit(1);
		}

    
		if ((key = ftok("shared", 'A')) < 0) {
            perror("ftok");
            exit(1);
       	}
	   shmid = shmget(key,P_MAX_COUNT*4 ,0644 | IPC_CREAT );
       	if (shmid  < 0) {
       	     perror("shmget");
       	     exit(1);
       	}
		evenSwapCount = (int *)shmat(shmid,NULL, 0);
			if (evenSwapCount == (int *)(-1)) {
			perror("shmat");
			exit(1);
		}
       	if ((key = ftok("shared", 'Z')) < 0) {
            perror("ftok");
            exit(1);
       	}
	   shmid = shmget(key,P_MAX_COUNT*4 ,0644 | IPC_CREAT );
       	if (shmid  < 0) {
       	     perror("shmget");
       	     exit(1);
       	}
		oddSwapCount = (int *)shmat(shmid,NULL, 0);
			if (oddSwapCount == (int *)(-1)) {
			perror("shmat");
			exit(1);
		}
		if ((key = ftok("shared", 'C')) < 0) {
			perror("ftok");
			exit(1);
       	}
		shmid = shmget(key,4 ,0644 | IPC_CREAT );
       	if (shmid  < 0) {
       	     perror("shmget");
       	     exit(1);
       	}
		pass = (int *)shmat(shmid,NULL, 0);
		if (pass == ( int *)(-1)) {
			perror("shmat");
			exit(1);
		}

		if ((key = ftok("shared", 'J')) < 0) {
			perror("ftok");
			exit(1);
       	}
		shmid = shmget(key,4 ,0644 | IPC_CREAT );
       	if (shmid  < 0) {
       	     perror("shmget");
       	     exit(1);
       	}
		sorted = (int *)shmat(shmid,NULL, 0);
		if (sorted == ( int *)(-1)) {
			perror("shmat");
			exit(1);
		}



		

}
int sumFlag(){
	int sum=0;
	for (int i = 0; i < P; i++)
	{
		sum += flag[i];

	}
	return sum;
	
}

	


int totalswapCount(){
	int sum =0;
	for (int i = 0; i < P; i++)
	{   sum =sum +oddSwapCount[i] +evenSwapCount[i];
		//sum += swapCount[i];
	}
	return sum;
	
}


void worker(int i){
	//printf("childeren");
	int n = N/P+1;
	int s = i*(n-1);
	if(i == P-1){
		n = N-s;
		}
    
	while(!*sorted){
		if(*pass == 1) 
			while(*pass); //printf("odd flag:%d,i:",flag[i],i);
        if(*sorted) break;
		evenSwapCount[i] = bubble(s,n,0);
		flag[i]=1;

		if(*pass == 0) 
			while(!*pass);
		if(*sorted) break; //printf("even flag:%d,i:",flag[i],i);
		oddSwapCount[i] = bubble(s,n,1);
		flag[i]=1;
        
		
	}
	exit(0);
}

void parallel_bubblesort() {
	pid_t pids[P];
	*sorted = 0;
	*pass = 0;
	int count,lastcount = N;
	for(int i = 0; i < P; i++){
		swapCount[i]=N ;
		oddSwapCount[i]=N ;
		evenSwapCount[i]=N ;
		flag[i]=0;

		
	
		if((pids[i] = fork()) < 0){
		 	perror("fork");
		}

		 else if(pids[i] == 0){
			
			worker(i);
		}
		
	}
	do{	
		
		//printf("parent");
		
		lastcount = count;


		// printOddSwapCount();
		// printEvenSwapCount();
		// printflag();
		// print_numbers();
		
		while(sumFlag()!=P);
		
		(*pass) = 1 - (*pass);
		count = totalswapCount();
		for(int j =0 ;j<P;j++){
			
			flag[j]=0;
		}
		//printSwapCount();
		//printf("pass:%d\n",*pass)
		//print_numbers();

	}while( count!=0 || lastcount!=0);

	*sorted=1;
}

int main(int argc, char *argv[])
{	struct timeval current_time1 ,current_time2;
	create_shm();
	// N=100;
    // P=2;
    // int a=100;

	// for (int i = 0; i < N; i++)
    // {
    //     number[i] = a;
	// 	a--;
    // }
	if( argc != 3) {
		fprintf(stderr, "Usage: %s N\n", argv[0]);
		return 1;
	}

	N = strtol(argv[1], (char **)NULL, 10);
	P = strtol(argv[2], (char **)NULL, 10);
	if( (N < 2) || (N > MAX_COUNT) ) {
		fprintf(stderr, "Invalid N. N should be between 2 and %d.\n", MAX_COUNT);
		return 2;
	}
	if( (P < 1) || (P > MAX_COUNT) ) {
	fprintf(stderr, "Invalid N. N should be between 1 and %d.\n", MAX_COUNT);
	return 2;
	}
	generate_numbers();
	print_numbers();
	printf("Sorted array:\n");
	gettimeofday(&current_time1, NULL);
	parallel_bubblesort();
	gettimeofday(&current_time2, NULL);
	print_numbers();
	printf("parallel bubble sort time spent: micro sec:%ld\n",current_time2.tv_usec-current_time1.tv_usec);

	;

	return 0;
}

