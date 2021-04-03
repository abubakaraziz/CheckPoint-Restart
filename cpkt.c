#define _GNU_SOURCE /* Required for 'constructor' attribute (GNU extension) */
#define _POSIX_C_SOURCE /*Required for sigsetmp/siglongjm[ */
#include <setjmp.h>
#include <stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
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
int print_memory_segment(struct proc_maps_line segment){
    printf("%s (%c%c%c) %d %d\n"
           "  Address-range: %p - %p\n",
           segment.name,
           segment.rwxp[0], segment.rwxp[1], segment.rwxp[2],
	   segment.is_register_context,
	   segment.data_size,
           segment.start, segment.end);
  
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
int write_single_segment(int fd,struct proc_maps_line segment){
//writing contents of the struct file
int rc=0;
int sizeofmemory=sizeof(segment);
printf(" Writing this segment to file \n");
print_memory_segment(segment);

/*copying the contents of the struct file first */
 
while (rc<sizeofmemory){
rc +=write(fd,(char *)&segment+rc,sizeofmemory-rc);
if (rc<0){
  perror("write");
  exit(1);
}
}
assert(rc==sizeofmemory);

/*copying data contents in struct now*/
rc=0;
/* double check condition if it should be less or equal to */
while (rc < segment.data_size){
rc +=write(fd,(char *) segment.start + rc, segment.data_size - rc);
if (rc<0){
  perror("write");
  exit(1);
}

}
assert(rc == segment.data_size);
return 0; //means successfully written one segment
}

int save_process(struct proc_maps_line proc_maps[]){
  char *filename="ckpt.dat";
  int fd=open(filename,O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
  if (fd==-1){perror("open");} 
  int i=0;
  for (i=0; proc_maps[i].start != NULL;i++){
  struct proc_maps_line segment=proc_maps[i];
  //printf(" permission %c\n",proc_maps[i].rwxp[0]);
  if (proc_maps[i].rwxp[0]=='-' && proc_maps[i].rwxp[1]=='-' && proc_maps[i].rwxp[2]=='-')
  {
  }else{
  //printf(" sr %s\n",segment.name);
  //we don't want to write below segments to the memory 
  const char* vvar="[vvar]";
  const char *vdso="[vdso]";
  const char* vsyscall="[vsyscall]";
	
  if (strcmp(segment.name,vvar)==0 || strcmp(segment.name,vdso)==0 || strcmp(segment.name,vsyscall)==0){	 
  //do nothing since we don't want to write those three segments 
  }else {
	assert(write_single_segment(fd,segment)==0);
  }
}
  }
  close(fd);
}

void __attribute__((constructor))
my_constructor() {
	

  struct proc_maps_line proc_maps[1000];
 //Reading Memory Addresses  
  read_current_process_memory(proc_maps);
  //print_current_process_memory(proc_maps);
 //
  assert( save_process(proc_maps)==0);

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
exit(0);
}
