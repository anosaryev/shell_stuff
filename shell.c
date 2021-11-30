#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
//#include "shell.h"

#define DEBUG 0

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

int sep(char **str, char **src, char delim){
  strcpy(*str, "");
  if (DEBUG)
    printf("IN:\t\"%s\"\t\"%s\"\t'%c'\n", *str, *src, delim);

  int i;
  int q = 0;
  int n = 0;
  for (i = 0; *src && strlen(*src) && (*src)[i]; i ++){
    int b = ((*src)[i] == '\\' && ((*src)[i+1] == '"' || (*src)[i+1] == '\'') &&
             (!q || q == (*src)[i+1])); // valid quote cancellation?
    if ((*src)[i] == '\'' || (*src)[i] == '"' && (!q || q == (*src)[i]) && !n){
      q = (q)? 0 : (*src)[i];
      if (delim == ';')
        strncat(*str, *src+i, 1);
    }
    else if ((*src)[i] == delim && !q){
      if (strlen(*str))
        break;
    }else if (!b || delim == ';'){
      strncat(*str, *src+i, 1);
    }
    n = (b)? 1 : 0;
    if (DEBUG)
      printf("MID:\t\"%s\"\t\"%s\"\t'%c'\n", *str, *src, delim);
  }
  if (*src)
    *src = *src+i+1;
  strcat(*str, "\0");
  if (DEBUG)
    printf("OUT:\t\"%s\"\t\"%s\"\t'%c'\n", *str, *src, delim);
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

  if (DEBUG)
    printf("\n\tcom:\tline:\t';'\n\n");
  sep(&com, &line, ';');
  for (r = 0; (com && strlen(com)) || (line && strlen(line)); r ++){
    int s = strcount(com, " ")+1;
    char *argray[s];
    for (i = 0; i <= s; i ++)
      argray[i] = 0;
    if (DEBUG)
      printf("\n\targ:\tcom:\t' '\n\n");
    sep(&arg, &com, ' ');
    for (c = 0; (arg && strlen(arg)) || (com && strlen(com)); c ++){
      argray[c] = malloc(sizeof(arg)); // malloc(): corrupted top size if line too long
      strcpy(argray[c], arg);
      if (DEBUG)
        printf("\n\targ:\tcom:\t' '\n\n");
      sep(&arg, &com, ' ');
    }

    if (DEBUG){
      printf("\nArgs:\n{\n");
      for (i = 0; argray[i] && strlen(argray[i]); i ++)
        printf("  \"%s\"\n", argray[i]);
      printf("___\n");
      printf("  \"%s\"\n", argray[i]);
      printf("}\n\n");
    }

    for (i = 0; argray[i] && strlen(argray[i]); i++){}
    argray[i] = 0;
    if (argray[0])
      exec_func(argray);
    for (i = 0; argray[i]; i ++)
      free(argray[i]);


    if (DEBUG)
      printf("\n\tcom:\tline:\t';'\n\n");
    sep(&com, &line, ';');
  }
  if (arg)
    free(arg);
  return 0;
}

int main(){
  printf("Enter Command(s):\n");
  char line[100];
  fgets(line, 100, stdin);
  printf("\n");

  exec_all(null_term(line));
}
