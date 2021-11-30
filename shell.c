#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "shell.h"

int strcount(char *str, char *sub){
  int i;
  int c = 0;
  for (i = 0; i < strlen(str) - strlen(sub); i ++){
    if (!strncmp(str+i, sub, strlen(sub)))
      c ++;
  }
  return c;
}

char *null_term(char *str){
  if (str[strlen(str)-1] == '\n')
    str[strlen(str)-1] = 0;
  return str;
}


// custom strsep with specific quote feature
int sep(char **str, char **src, char delim){
  strcpy(*str, "");
  printf("IN:\tstr:\tsrc:\tdelim:\n\t\"%s\"\t\"%s\"\t\"%c\"\n\n", *str, *src, delim);

  int i;
  int q = 0;
  int n = 0;
  for (i = 0; *src && (*src)[i]; i ++){
    int b = ((*src)[i] == '\\' && ((*src)[i+1] == '"' || (*src)[i+1] == '\'') &&
             (!q || q == (*src)[i+1])); // are we at a valid quote cancellation?
    if ((*src)[i] == '\'' || (*src)[i] == '"' && (!q || q == (*src)[i]) && !n){
      q = (q)? 0 : (*src)[i];
      if (delim == ';')
        strncat(*str, *src+i, 1);
    }
    else if ((*src)[i] == delim && !q){
      if (strlen(*str))
        break;
    }else if (!b || delim == ';'){
      //printf("%d, %d\n", (*src)[i] == delim, !q);
      strncat(*str, *src+i, 1);
    }
    n = (b)? 1 : 0;
    printf("MID:\tstr:\tsrc:\tdelim:\n\t\"%s\"\t\"%s\"\t\"%c\"\n\n", *str, *src, delim);
  }
  if (*src)
    *src = *src+i+1;
  strcat(*str, "\0");
  printf("OUT:\tstr:\tsrc:\tdelim:\n\t\"%s\"\t\"%s\"\t\"%c\"\n\n", *str, *src, delim);
  return 0;
}

int exec_cd(char **line){
  return 0;
}

int exec_exit(char **line){
  exit(0);
}

int exec_func(char *line[]){
  if (!strcmp(line[0], "cd"))
    exec_cd(line);
  else if (!strcmp(line[0], "exit"))
    exec_exit(line);

  int pid = fork();
  if (!pid){
    if (execvp(line[0], line) == -1){
      printf("Error %d: %s\n", errno, strerror(errno));
      exit(-1);
    }
    exit(0);
  }
  waitpid(pid, 0, 0);
  return 0;
}


int exec_all(char *line){
  char *arg = malloc(sizeof(line));
  char *com = malloc(sizeof(line));
  int i, r, c;

  sep(&com, &line, ';');
  for (r = 0; (com && strlen(com)) || (line && strlen(line)); r ++){
    int s = strcount(com, " ");
    char *argray[s];
    for (i = 0; i <= s; i ++)
      argray[i] = 0;
    sep(&arg, &com, ' ');
    for (c = 0; (arg && strlen(arg)) || (com && strlen(com)); c ++){
      printf("Oi! \"%s\"\n", arg);
      /*if (!strcmp(arg, "")){
        sep(&arg, &com, ' ');
        c --;
        continue;
      */
      //printf("Ahoy! %ld > %ld\n", sizeof(arg), strlen(arg));
      //printf("Hopefully %d > %d\n", s, c);
      argray[c] = malloc(strlen(arg)); // error here
      //printf("Alright!\n");
      strcpy(argray[c], arg);
      sep(&arg, &com, ' ');
    }

    printf("\nArgs:\n{\n");
    for (i = 0; argray[i] && strlen(argray[i]); i ++)
      printf("  \"%s\"\n", argray[i]);
    printf("___\n");
    printf("  \"%s\"\n", argray[i]);
    printf("}\n\n");

    for (i = 0; argray[i] && strlen(argray[i]); i++){}
    argray[i] = 0;
    if (argray[0])
      exec_func(argray);
    for (i = 0; argray[i]; i ++)
      free(argray[i]);

    printf("ye\n");
    sep(&com, &line, ';');
  }
  //printf("arg: \"%s\"\t%p\t%ld\n", arg, &arg, sizeof(arg));
  //printf("com: \"%s\"\t%p\t%ld\n", com, &com, sizeof(com));
  //free(arg);
  //printf("arg done\n");
  //free(com);
  //printf("com done\n");
  return 0;
}

int main(){
  printf("Enter Command(s):\n");
  char line[100];
  fgets(line, 100, stdin);
  printf("\n");

  /*
  printf("{\n");
  int i;
  for (i = 0; i < strlen(line)-1; i ++){
    printf("  '%c'\n", line[i]);
  }
  printf("}\n\n");
  */

  exec_all(null_term(line));
}
