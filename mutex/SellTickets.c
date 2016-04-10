#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/syscall.h>
#include <string.h>
#define NUM_TICKETS 35
#define NUM_SELLERS 4
#define NAME_LEN 10

pthread_t tid[NUM_SELLERS];
pthread_mutex_t lock;
pid_t parent_tid;

static int numTickets = NUM_TICKETS;
int ticket_counter[NUM_SELLERS];
char thread_name[NUM_SELLERS][NAME_LEN];

void *SellTickets(void *arg){
	bool done=false;
	int numSoldBythisthread = 0;
	int i;

       
        while(!done){
		/*imagine a different counter different from other counters attending to 
		different cusomer needs */
		sleep(1); //simulation of work
		
		pthread_mutex_lock(&lock);
		
		if(numTickets == 0)
			done=true;
		else{
			numTickets--;
			numSoldBythisthread++;

			pid_t tid = syscall(SYS_gettid);
			for(i=1;i<NUM_SELLERS+1;i++){
				if(tid == (parent_tid + i)){
					//printf("**Lock held by %s**\n",thread_name[i-1]);
					printf("%s sold one (%d left)\n",thread_name[i-1],numTickets);
					ticket_counter[i-1]=numSoldBythisthread;
				}

			}
		
		}
		
		pthread_mutex_unlock(&lock);
		
 	
	}
	
	return NULL;


}

int main(){
    int i = 0;
    int err;
    char name[NAME_LEN];

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

    parent_tid = syscall(SYS_gettid);
    //printf("**tid of main thread:%d=%d**\n",i,parent_tid);

    while(i<4){
	err=pthread_create(&(tid[i]),NULL,&SellTickets,NULL);
	if(err!=0)
		printf("\ncan't create thread :[%s]", strerror(err));
	
	sprintf(name,"Seller #%d", i);
        strcpy(thread_name[i],name);

	pthread_setname_np((tid[i]),thread_name[i]);
	
        pthread_getname_np((tid[i]),thread_name[i],NAME_LEN);
        printf("%s starts selling\n",thread_name[i]);
	i++;

    }



    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);
    pthread_join(tid[3], NULL);

    for(i=0;i<4;i++){
    	pthread_getname_np((tid[i]),thread_name[i],NAME_LEN);
    	printf("%s is closed\n",thread_name[i]);
    }

    int rem=NUM_TICKETS;
    for(i=0;i<4;i++){
    	printf("%s sold %d tickets\n",thread_name[i],ticket_counter[i]);
	rem = rem-ticket_counter[i];
    }

    if(rem == 0 )
	printf("All done!\n");
    
    pthread_mutex_destroy(&lock);

    return 0;
}
