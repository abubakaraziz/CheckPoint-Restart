#include<setjmp.h>
#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include "proc-self-maps.c"

int main(){

  char *filename="ckpt.dat";
  int fd;
  fd=open(filename,O_RDONLY);
  if (fd ==-1) {perror("open");}
  struct proc_maps_line segment;

int i;
int rc=-2;
int segmentsize=sizeof(struct proc_maps_line);
for (i=0;i<27;i++){
  rc=0; 
  while (rc<segmentsize){
  rc+=read(fd,&segment,segmentsize);
  if (rc==-1){
   perror("read");
   exit(1);}
  }
  if ( segment.is_register_context == 0) {
  printf("%s (%c%c%c) %d %d\n"
           "  Address-range: %p - %p\n",
           segment.name,
           segment.rwxp[0], segment.rwxp[1], segment.rwxp[2],
	   segment.is_register_context,
	   segment.data_size,
           segment.start, segment.end);
  FILE* fp=fdopen(fd,"r");
  fseek( fp, segment.data_size, SEEK_CUR); 
  int len=ftell(fp);
  printf("Current File location %d\n",len);

 }
else if (segment.is_register_context==1){
/*reading register context */
  printf(" %s %d \n",segment.name ,segment.is_register_context
           );
  FILE* fp=fdopen(fd,"r");
  int len=ftell(fp);
  printf("Current File location %d\n",len);
 
/* Reading Old Register */
  printf("Reading Old Register Values \n");
  sigjmp_buf old_registers;
  read(fd,&old_registers,sizeof(sigjmp_buf));
  
  len=ftell(fp);
  printf("Current File location %d\n",len);
}

}
}
