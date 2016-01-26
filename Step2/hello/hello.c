#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern ssize_t getline(char **lineptr, size_t *n, FILE *stream);

int main(){
  char* line=NULL;
  size_t len=0;
  ssize_t count=0;

  // Ask for the user name...
  while (1) {
    printf("What is your name? ");
    fflush(stdout);
    count = getline(&line,&len,stdin);
    if (count==-1) {
      printf("I am not hearing you, bye.\n");
      return;
    }
    if (count>1)
      break;
    free(line);
    line = NULL;
    len = 0;
  }
  line[count-1]='\0';
  printf("Hello %s, welcome.\n",line);
  
  // Now enter parrot mode...
  while(1){
    free(line);
    line = NULL;
    len = 0;
    if (count>1)
      printf("What say you? ");
    else 
      printf("I beg your pardon? ");
    fflush(stdout);
    count = getline(&line,&len,stdin);
    if (count==-1) {
      printf("I lost you, bye.\n");
      return;
    }
    if (count>1) {
      line[count-1]='\0';
      printf("You said: %s, very well.\n",line);
    }
    // getchar();
  }
}
