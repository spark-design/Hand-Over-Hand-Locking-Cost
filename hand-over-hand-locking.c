/* Sources: 
https://www.usenix.org/system/files/login/articles/login_fall20_14_kelly.pdf
https://man7.org/linux/man-pages/man0/pthread.h.0p.html
https://pubs.opengroup.org/onlinepubs/7908799/xsh/systime.h.html
https://man7.org/linux/man-pages/man3/pthread_self.3.html
*/
#include <stdio.h>
#include <pthread.h> // for pthread_mutex_t, pthread_mutex_init, pthread_mutex_lock, pthread_mutex_unlock, pthread_self, pthread_t, pthread_create, pthread_join
#include <sys/time.h>
#include <stdlib.h>
#include<string.h>
#define integers 10000

// struct and INIT for non HOH
// basic node structure
typedef struct __node_t { // Code found in figure 29.7 of the book
	int              key;
	struct __node_t *next;
} node_t;

// basic list structure
typedef struct __list_t { // Code found in figure 29.7 of the book
	node_t            *head;
	pthread_mutex_t    lock;
} list_t;

void List_Init(list_t *L) {// Code found in figure 29.8 of the book
	L->head = NULL;
	pthread_mutex_init(&L->lock, NULL);
}

void List_Insert(list_t *L, int key) {// Code found in figure 29.8 of the book

	// synchronization not needed
	node_t *new = malloc(sizeof(node_t));
	if (new == NULL) {
		perror("malloc");
		return;
	}
	new->key = rand();
	
	// just lock critical section
	pthread_mutex_lock(&L->lock);
	new->next = L->head;
	L->head = new;
	pthread_mutex_unlock(&L->lock);
}


int List_Lookup(list_t *L, int key) {// Code found in figure 29.8 of the book
	int rv = -1;
	pthread_mutex_lock(&L->lock);
	node_t *curr = L->head;
	while (curr) {
		if (curr->key == key) {
			rv = 0;
			break;
		}
		curr = curr->next;
	}
	pthread_mutex_unlock(&L->lock);
	return rv; // now both success and failure
}

// Struct and INIT for HOH
// basic node structure
typedef struct __node_t_HOH {
        pthread_mutex_t lock;
        int                  key;
        struct __node_t_HOH *next;
} node_t_HOH;

// basic list structure
typedef struct __list_t_HOH {
        node_t_HOH        *head;
        pthread_mutex_t   lockHOH;
} list_t_HOH;

void List_Init_HOH(list_t_HOH *L) {
        L->head = NULL;
        pthread_mutex_init(&L->lockHOH, NULL);
}
 
void List_Insert_HOH(list_t_HOH *L, int key) {// Code found in figure 29.8 of the book then modified
  // synchronization not needed
        node_t_HOH *new = malloc(sizeof(node_t_HOH));
        if (new == NULL){
	    perror("malloc");
	    return;
	   }
        new->key = rand();
        pthread_mutex_init(&new->lock, NULL);

        // just lock critical section
        pthread_mutex_lock(&L->lockHOH);
        new->next = L->head;
        L->head = new;
        pthread_mutex_unlock(&L->lockHOH);
}


int List_Lookup_HOH(list_t_HOH *L, int key) {// Code found in figure 29.8 of the book then modified
       int rv = -1;
       node_t_HOH *curr = L->head;
       if (!curr)
          return rv;
       pthread_mutex_lock(&curr->lock);
       while (curr) {
           if (curr->key == key) {
               rv = 0;
               pthread_mutex_unlock(&curr->lock);
               break;
         }
         pthread_mutex_t *pos = &curr->lock;
         curr = curr->next;
         if (curr)
            pthread_mutex_lock(&curr->lock);
         pthread_mutex_unlock(pos);
        }
        return rv; // now both success and failure
}


// Non HOH Workloads 
void * load1(void *L) {
	for (int i = 0; i < (integers/2); i++) { // for loop that gets random numbers for the insert. integers gets divided by 2 since you will be inserting in the list twice.
	int random = rand();
	List_Insert(L, random);}
	return 0;}



void * load2p1(void *L) 
{
	for (int i = 0; i < (integers); i++) { // for loop that gets random numbers for the insert
	int random = rand();
	List_Insert(L, random);}
	return 0;
	}
void * load2p2(void *L) 
{
	for (int i = 0; i < (integers); i++) { // for loop that gets random numbers for the lookup
	int random = rand();
	List_Lookup(L, random);}
	return 0;
	}


void * load3(void *L) 
{
	for (int i = 0; i < (integers); i++) { // for loop that gets random numbers for the lookup
	int random = rand();
	List_Lookup(L, random);}
	return 0;
	}
	
	
// HOH workloads
void * load4(void *L) {
	for (int i = 0; i < (integers/2); i++) { // for loop that gets random numbers for the insert. integers gets divided by 2 since you will be inserting in the list twice.
	int random = rand();
	List_Insert_HOH(L, random);}
	return 0;}



void * load5p1(void *L) 
{
	for (int i = 0; i < (integers); i++) { // for loop that gets random numbers for the insert
	int random = rand();
	List_Insert_HOH(L, random);}
	return 0;
	}
void * load5p2(void *L) 
{
	for (int i = 0; i < (integers); i++) { // for loop that gets random numbers for the lookup
	int random = rand();
	List_Lookup_HOH(L, random);}
	return 0;
	}


void * load6(void *L) 
{
	for (int i = 0; i < (integers); i++) { // for loop that gets random numbers for the lookup
	int random = rand();
	List_Lookup_HOH(L, random);}
	return 0;
	}

int main(int argc, char *argv[]) 
{
	srand(time(NULL)); // Completed a program in c before this course to get override involving linked list randomization. From this I learned about rand()
	struct timeval begin1, begin2, begin3, begin4, begin5, begin6, terminate1, terminate2, terminate3, terminate4, terminate5, terminate6; // initialize variables
	pthread_t p1, p2; // figure 26.6 had useful code for thread creation
	list_t_HOH L;
	List_Init_HOH(&L); // Workload 1 of non hand-over-hand
	list_t_HOH L2;
	List_Init_HOH(&L2); // Workload 2 of non hand-over-hand
	list_t_HOH L3;
	List_Init_HOH(&L3); // Workload 3 of non hand-over-hand
	list_t L4;
	List_Init(&L4); // Workload 1 of hand-over-hand 
	list_t L5;
	List_Init(&L5); // Workload 2 of hand-over-hand 
	list_t L6;
	List_Init(&L6); // Workload 3 of hand-over-hand 
	
	
	printf("Non hand-over-hand workloads");
	// Workload 1 start
	gettimeofday(&begin1, 0); // start time
	
	// Thread code taken from 26.6
        // launches two threads
	pthread_create(&p1, 0, load1, &L);
	pthread_create(&p2, 0, load1, &L);
	
	// join waits for the threads to finish
	pthread_join(p1, NULL);
	pthread_join(p2, 0);
	

	gettimeofday(&terminate1, 0); // end time
	// Workload 1 finish
	
	// Calculate: Got code from cost of syscall
	printf("\nWorkload 1 took %f ms",(float) (((terminate1.tv_sec - begin1.tv_sec)* 1000000) + (terminate1.tv_usec - begin1.tv_usec)));
	
	
	
	// Workload 2
	gettimeofday(&begin2, 0); // start time
	
	// Thread code taken from 26.6
	// launches two threads
	pthread_create(&p1, 0, load2p1, &L2);
	pthread_create(&p2, 0, load2p2, &L2);
	
	// join waits for the threads to finish
	pthread_join(p1, 0);
	pthread_join(p2, 0);
	
	gettimeofday(&terminate2, 0); // end time
	// Workload 2 finish
	
	// Calculate: Got code from cost of syscall
	printf("\nWorkload 2 took %f ms",(float) (((terminate2.tv_sec - begin2.tv_sec)* 1000000) + (terminate2.tv_usec - begin2.tv_usec)));
	


	// Workload 3
	gettimeofday(&begin3, 0); // start time

	// Thread code taken from 26.6
	// launches two threads
	pthread_create(&p1, 0, load3, &L3);
	pthread_create(&p2, 0, load3, &L3);
	
	// join waits for the threads to finish
	pthread_join(p1, 0);
	pthread_join(p2, 0);
	

	gettimeofday(&terminate3, 0); // end time
	// Workload 3 finish
	
	// Calculate: Got code from cost of syscall (calculations)
	printf("\nWorkload 3 took %f ms\n",(float) (((terminate3.tv_sec - begin3.tv_sec)* 1000000) + (terminate3.tv_usec - begin3.tv_usec)));
	
	
	
	// Non hand-over-hand
	printf("\nHand-over-hand workloads");
	// Workload 1 start
	gettimeofday(&begin4, 0); // start time
	
	// Thread code taken from 26.6
        // launches two threads
	pthread_create(&p1, 0, load4, &L4);
	pthread_create(&p2, 0, load4, &L4);
	
	// join waits for the threads to finish
	pthread_join(p1, NULL);
	pthread_join(p2, 0);
	

	gettimeofday(&terminate4, 0); // end time
	// Workload 1 finish
	
	// Calculate: Got code from cost of syscall
	printf("\nWorkload 1 took %f ms",(float) (((terminate4.tv_sec - begin4.tv_sec)* 1000000) + (terminate4.tv_usec - begin4.tv_usec)));
	
	
	// Workload 2
	gettimeofday(&begin5, 0); // start time
	
	// Thread code taken from 26.6
	// launches two threads
	pthread_create(&p1, 0, load5p1, &L5);
	pthread_create(&p2, 0, load5p2, &L5);
	
	// join waits for the threads to finish
	pthread_join(p1, 0);
	pthread_join(p2, 0);
	
	gettimeofday(&terminate5, 0); // end time
	// Workload 2 finish
	
	// Calculate: Got code from cost of syscall
	printf("\nWorkload 2 took %f ms",(float) (((terminate5.tv_sec - begin5.tv_sec)* 1000000) + (terminate5.tv_usec - begin5.tv_usec)));
	

	// Workload 3
	gettimeofday(&begin6, 0); // start time

	// Thread code taken from 26.6
	// launches two threads
	pthread_create(&p1, 0, load6, &L6);
	pthread_create(&p2, 0, load6, &L6);
	
	// join waits for the threads to finish
	pthread_join(p1, 0);
	pthread_join(p2, 0);
	

	gettimeofday(&terminate6, 0); // end time
	// Workload 3 finish
	
	// Calculate: Got code from cost of syscall (calculations)
	printf("\nWorkload 3 took %f ms\n",(float) (((terminate6.tv_sec - begin6.tv_sec)* 1000000) + (terminate6.tv_usec - begin6.tv_usec)));
}
