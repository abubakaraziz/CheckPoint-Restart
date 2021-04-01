#define _GNU_SOURCE /* Required for 'constructor' attribute (GNU extension) */
#define _POSIX_C_SOURCE /*Required for sigsetmp/siglongjm[ */
#include <setjmp.h>
#include <stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>

int main(int,char *[]);
sigjmp_buf env;
void signal_handler(int signal){
siglongjmp(env,1);
}

void __attribute__((constructor))
my_constructor() {
   signal(SIGINT,&signal_handler);
   fprintf(stderr, "*************************************\n"
                    "***  This is my CPKT ***\n"
                    "*************************************\n");
   
   
  
  if (sigsetjmp(env,1)!=0){
    printf("\n\n *** RESTARTING ***\n"); 
  }


   
   fprintf(stderr,   "*** What integer should we test? ");
   fflush(stdout);
   char buf[100]={'\0'}; //null characters at end of string
   read(0,buf,sizeof(buf)-1);
   char *argv[]={"UNKNOWN", "12345", NULL};
   argv[0]=getenv("TARGET");
   argv[1]=buf;
   main(/* argc */2, argv);

   exit(0);

}
