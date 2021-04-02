#define _GNU_SOURCE /* Required for 'constructor' attribute (GNU extension) */
#define _POSIX_C_SOURCE /*Required for sigsetmp/siglongjm[ */
#include <setjmp.h>
#include <stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>
#include "proc-self-maps.c" 
int main(int,char *[]);
sigjmp_buf env;
int print_current_process_memory(struct proc_maps_line proc_maps[]){
 printf(" *** Memory segments of Current process are :%s ***\n");   
  int i = 0;
  for (i = 0; i<1;/*proc_maps[i].start != NULL;*/ i++) {
    printf("%s (%c%c%c) %d %d\n"
           "  Address-range: %p - %p\n",
           proc_maps[i].name,
           proc_maps[i].rwxp[0], proc_maps[i].rwxp[1], proc_maps[i].rwxp[2],
	   proc_maps[i].is_register_context,
	   proc_maps[i].data_size,
           proc_maps[i].start, proc_maps[i].end);
  
   //printf("%s \n",proc_maps[i].name);
  }
return 0;
}

int read_current_process_memory(struct proc_maps_line proc_maps[]){ 
  assert(proc_self_maps(proc_maps)==0); 
  
return 0;
}


void signal_handler(int signal){

   printf("Signal Received %d\n",signal);

   if (signal==SIGUSR2){
        printf("SIGUSR RECEVED \n"); 

   }
siglongjmp(env,1);
}

void __attribute__((constructor))
my_constructor() {
	

  struct proc_maps_line proc_maps[1000];
 //Reading Memory Addresses  
  read_current_process_memory(proc_maps);
  print_current_process_memory(proc_maps);


  /*
  signal(SIGUSR2,&signal_handler);
  fprintf(stderr, "*************************************\n"
                    "***  This is my CPKT ***\n"
                    "*************************************\n");
    
  if (sigsetjmp(env,1)!=0){
    printf("\n\n *** RESTARTING ***\n"); 
  }

  */ 
   //exit(0);

}
