#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

struct dirs{
  char hdir[100];
  char cdir[100];
  char ddir[100];
};

int strcount(char *str, char *sub);

char *null_term(char *str);

int sep(char **str, char **src, char delim);

int exec_cd(char *line[], struct dirs *dir);

int exec_exit(char *line[]);

int exec_func(char *line[], struct dirs *dir);

int exec_all(char *line, struct dirs *dir);

#endif
