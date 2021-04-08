#define _GNU_SOURCE /* Required for 'constructor' attribute (GNU extension) */
#define _POSIX_C_SOURCE /*Required for sigsetmp/siglongjm[ */

#include<ucontext.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

int main(int,char *[]);

 //sigjmp_buf env;
ucontext_t context;

#define NAME_LEN 80
struct proc_maps_line {
  char *start;
  char *end;
  char rwxp[4];
  int read, write, execute; // Not used in this versoin of the code
  char name[NAME_LEN]; // for debutting only
  int is_register_context; //for register context only
  int data_size;
};



// Returns 0 on success
int match_one_line(int proc_maps_fd,
                   struct proc_maps_line *proc_maps_line, char *filename) {
  unsigned long int start, end;
  char rwxp[4];
  char tmp[10];
  int tmp_stdin = dup(0); // Make a copy of stdin
  dup2(proc_maps_fd, 0); // Duplicate proc_maps_fd to stain
  // scanf() reads _only_ from stdin.  It's a dup of proc_maps_fd (same struct file).
  int rc = scanf("%lx-%lx %4c %*s %*s %*[0-9 ]%[^\n]\n",
                 &start, &end, rwxp, filename);
  assert(fseek(stdin, 0, SEEK_CUR) == 0); 
  dup2(tmp_stdin, 0); // Restore original stdin; proc_maps_fd offset was advanced.
  close(tmp_stdin);
  if (rc == EOF || rc == 0) { 
    proc_maps_line -> start = NULL;
    proc_maps_line -> end = NULL;
    return EOF;
  }
  if (rc == 3) { // if no filename on /proc/self/maps line:
    strncpy(proc_maps_line -> name,
            "ANONYMOUS_SEGMENT", strlen("ANONYMOUS_SEGMENT")+1);
  } else {
    assert( rc == 4 );
    strncpy(proc_maps_line -> name, filename, NAME_LEN-1);
    proc_maps_line->name[NAME_LEN-1] = '\0';
  }
  proc_maps_line -> start = (void *)start;
  proc_maps_line -> end = (void *)end;
  proc_maps_line -> is_register_context=0;
  proc_maps_line -> data_size= end - start;
  memcpy(proc_maps_line->rwxp, rwxp, 4);
  return 0;
}






// This might cause scanf to call mmap().  If this is a problem, call it a second
//   time, and scanf() can re-use the previous memory, instead of mmap'ing more.
int proc_self_maps(struct proc_maps_line proc_maps[]) {
  // NOTE:  fopen calls malloc() and potentially extends the heap segment.
  int proc_maps_fd = open("/proc/self/maps", O_RDONLY);
  char filename [100]; // for debugging
  int i = 0;
  int rc = -2; // any value that will not terminate the 'for' loop.
  for (i = 0; rc != EOF; i++) {
    rc = match_one_line(proc_maps_fd, &proc_maps[i], filename);
#ifdef DEBUG
    if (rc == 0) {
      printf("proc_self_maps: filename: %s\n", filename); // for debugging
    }
#endif
  }
  close(proc_maps_fd);
  return 0;
}





 
 
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
  printf("inside read current process \n");
  assert(proc_self_maps(proc_maps)==0); 
  
return 0;
}


void signal_handler(int signal){
  
   printf("Signal Received %d\n",signal); 
   //ucontext_t context; // A context includes register values. i already defined this globally ask professor whether we can define this variable globally or not
   static int is_restart; //static local variables are stored in data segment	
   is_restart=0;
   getcontext(&context);
   if ( is_restart == 1) {

        //printf("going to read current memory\n");
	is_restart = 0;
 	return;
    }
   is_restart = 1; 
   struct proc_maps_line proc_maps[1000];
	//Reading Memory Addresses  
  
   read_current_process_memory(proc_maps);
   //print_current_process_memory(proc_maps);
   assert( save_process(proc_maps)==0); 
   printf("exit process \n");
   exit(1);	
}
int write_single_segment(int fd,struct proc_maps_line segment){
//writing contents of the struct file
  int rc=0;
  int sizeofmemory=sizeof(segment);
  //printf(" Writing this segment to file \n");
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
int save_register_context(int fd){
  
//writing contents of the struct file
  int rc=0;
  int sizeofmemory=sizeof(struct proc_maps_line);
  /*Creating Memory Segment for Register */
  struct proc_maps_line *register_maps_line=(struct proc_maps_line *)malloc(sizeof(struct proc_maps_line));
  register_maps_line -> start = (void *)&context;
  register_maps_line -> end = NULL;
  //strncpy(register_maps_line -> name, "REGISTER_SEGMENT",strlen("REGISTER_SEGMENT")+1);
  register_maps_line -> data_size = sizeof(context);  
  register_maps_line -> is_register_context = 1;
  //printf("Size of registers %d\n",register_maps_line -> data_size);
  //printf("Register Context Name %s", register_maps_line -> name);


	
	
  /*copying the contents of the struct file first */
  
  while (rc<sizeofmemory){
  rc +=write(fd,(char *)register_maps_line+rc,sizeofmemory-rc);
  if (rc<0){
    perror("write");
    exit(1);
   }
}
  assert(rc==sizeofmemory);

  
 //printf("Address of environment %p \n",&env);
 //printf("Start address is %p \n",register_maps_line->start);

  rc=0; 
  //printf(" Writing this Register segment to file \n");
  while (rc < register_maps_line -> data_size){
  rc +=  write(fd,(char *) register_maps_line -> start + rc, register_maps_line -> data_size - rc);
  if (rc<0){
    perror("write");
    exit(1);
  }
}
  printf(" Reigster Segment Written to Memory \n");
  FILE *fp=fdopen(fd,"w");	
  int len=ftell(fp);
  printf("Current file location after writing register  %d\n",len);
  return 0;
}



int save_process(struct proc_maps_line proc_maps[]){
  char *filename="ckpt.dat";
  int fd=open(filename,O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
  if (fd==-1){perror("open");} 
  int i=0;
  int counter=0;
  for (i=0; proc_maps[i].start!= NULL;i++){
  struct proc_maps_line segment=proc_maps[i];
  if (proc_maps[i].rwxp[0]=='-' && proc_maps[i].rwxp[1]=='-' && proc_maps[i].rwxp[2]=='-')
  {
  }else{
  //we don't want to write below segments to the memory 
  const char* vvar="[vvar]";
  const char *vdso="[vdso]";
  const char* vsyscall="[vsyscall]";
	
  if (strcmp(segment.name,vvar)==0 || strcmp(segment.name,vdso)==0 || strcmp(segment.name,vsyscall)==0){	 
  //do nothing since we don't want to write those three segments 
  }else {
	assert(write_single_segment(fd,segment)==0);
        FILE *fp=fdopen(fd,"w");	
	int len=ftell(fp);
	//printf("Current file location %d\n",len);
	counter++;
  }
}
  }
  
 /*save register context*/
  save_register_context(fd);
 
  FILE *fp=fdopen(fd,"w");	
  int len=ftell(fp);
  //printf("Current file location %d\n",len);
  
  close(fd);

 printf("Total memory segments %d\n",counter); 
 return 0;  
}

void __attribute__((constructor))
my_constructor() {

  signal(SIGUSR2,&signal_handler);
  fprintf(stderr, "*************************************\n"
                    "***  This is my CPKT ***\n"
                    "*************************************\n");
    

}
