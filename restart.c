#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<unistd.h>
#include<assert.h>
#include "proc-self-maps.c"
#include<setjmp.h>

void read_file(){
char *filename="ckpt.dat";
  int fd;
  fd=open(filename,O_RDONLY);
  if (fd ==-1) {perror("open");}
  struct proc_maps_line segment;

int i;
int rc=-2;
int segmentsize=sizeof(struct proc_maps_line);

while ( rc !=0  && rc != EOF){
  rc=0; 
  while ( rc<segmentsize ){
  rc+=read(fd,&segment,segmentsize);
  printf("value of rc %d", rc);
  if (rc==-1){
   perror("read");
   exit(1);}
  if ( rc == EOF || rc==0 ){
    break; 
  
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
	/* Now I should read data from here */
	void *addr=mmap( segment.start , segment.data_size, PROT_READ | PROT_WRITE | PROT_EXEC ,
			MAP_FIXED | MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	int byte_count=0;

	//fseek( fp, segment.data_size, SEEK_CUR); 
	while ( byte_count < segment.data_size ){
         byte_count += read( fd, addr + byte_count, segment.data_size - byte_count);
	}
	int len=ftell(fp);
	printf("Current File location %d\n",len);

 }
else if (segment.is_register_context==1){
/*reading register context */
  printf(" %s %d \n",segment.name ,segment.is_register_context
           );
  FILE* fp=fdopen(fd,"r");
  int len=ftell(fp);
  //printf("Current File location %d\n",len);
 
/* Reading Old Register */
  printf("Reading Old Register Values \n");
  sigjmp_buf old_registers;

  //read(fd,&old_registers,sizeof(sigjmp_buf));
   int byte_count=0; 
   while ( byte_count < sizeof ( sigjmp_buf ) ){
    byte_count += read( fd, &old_registers + byte_count, sizeof( sigjmp_buf )- byte_count);
    }
   siglongjmp( old_registers, 1); 
  len=ftell(fp);
  printf("Current File location %d\n",len);
}
}}}
void restore(){
  struct proc_maps_line segment;
  int segmentsize = sizeof (struct proc_maps_line);
  int i = 0; 
  printf("hello\n");
 read_file();

}


void recursive(int levels){
  if (levels>0){
    recursive(levels-1); 
  }else{
     restore();       
  }
}
int main(int argc, char *argv[]){
  recursive(100000);   

}
