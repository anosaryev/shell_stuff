#include "shell.h"

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
  if (*str && strlen(*str))
    memset(*str, 0, sizeof(*str));
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

  if (*src){
    if (i == strlen(*src))
      *src = "";
    else
      *src = *src+i+1;
    strcat(*str, "\0");
  }else
    *str = 0;

  if (DEBUG)
    printf("OUT:\t\"%s\"\t\"%s\"\t'%c'\n", *str, *src, delim);
  return 0;
}

int exec_cd(char *line[], struct dirs *dir){
  int b;
  if (line[1] == 0){
    b = chdir(dir->hdir);
  }else{
    if (line[1][0] == '/'){
      b = chdir("/");
      line[1] = line[1]+1;
    }else if(!strncmp(line[1], "../", 3) || !strncmp(line[1], "..\0", 3)){
      char *sp = strrchr(dir->cdir, '/');
      *sp = '\0';
      if (dir->cdir == ""){
        memset(dir->cdir, '\0', 100);
        strcpy(dir->cdir, "/");
      }
      b = chdir(dir->cdir);
      line[1] = (line[1][2] == '/')? line[1]+3 : line[1]+2;
    }else if(!strncmp(line[1], "./", 2) || !strncmp(line[1], ".", 2))
      line[1] = line[1]+2;
    else{
      strcat(dir->cdir, "/");
      strcat(dir->cdir, strsep(&line[1], "/"));
      b = chdir(dir->cdir);
    }
  }
  if (b == -1){
    printf("Error %d: %s\n", errno, strerror(errno));
    return -1;
  }
  getcwd(dir->cdir, 100);
  strcpy(dir->ddir, strrchr(dir->cdir, '/')+1);

  if (line[1] && line[1][0])
    return exec_cd(line, dir);
  return 0;
}

int exec_exit(char *line[]){
  exit(0);
}

int exec_func(char *line[], struct dirs *dir){
  if (!strcmp(line[0], "cd")){
    exec_cd(line, dir);
    return 0;
  }
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


int exec_all(char *line, struct dirs *dir){
  int si = sizeof(line);
  int el = strcount(line, " ")+1;
  char argray[el][si];
  memset(argray, 0, el*si);
  strcpy(argray[el+1], "\0");
  char *arg = malloc(si);
  char *com = malloc(si);
  int i, r, c;

  if (DEBUG)
    printf("\n\tcom:\tline:\t';'\n\n");
  sep(&com, &line, ';');
  for (r = 0; (com && strlen(com)) || (line && strlen(line)); r ++){
    memset(argray, 0, el*(si+1));
    if (DEBUG)
      printf("\n\targ:\tcom:\t' '\n\n");
    sep(&arg, &com, ' ');
    for (c = 0; (arg && strlen(arg)) || (com && strlen(com)); c ++){
      strcpy(argray[c], arg);

      if (DEBUG)
        printf("\n\targ:\tcom:\t' '\n\n");
      sep(&arg, &com, ' ');
    }
    char *cmd[el];
    if (c > 1) // i have absolutely no clue why argray[c] sometimes becomes 2 here.
      strcpy(argray[c], "");

    for (i = 0; argray[i] && strlen(argray[i]); i ++){
      cmd[i] = malloc(sizeof(argray));
      strcpy(cmd[i], argray[i]);
    }
    cmd[i] = malloc(1);
    cmd[i] = 0;

    if (DEBUG){
      printf("\nArgs:\n{\n");
      for (i = 0; cmd[i] && strcmp(cmd[i], ""); i ++)
        printf("  \"%s\"\n", cmd[i]);
      printf("___\n");
      printf("  \"%s\"\n", cmd[i]);
      printf("}\n\n");
    }

    if (cmd[0]){
      exec_func(cmd, dir);
    }

    for (i = 0; cmd[i] && strcmp(cmd[i], ""); i ++)
      free(cmd[i]);

    if (line && strlen(line)){
      com = malloc(si);
      if (DEBUG)
        printf("\n\tcom:\tline:\t';'\n\n");
      sep(&com, &line, ';');
    }
  }

  if (arg)
    free(arg);
  //if (com)
    //free(com);
  return 0;
}

int main(){
  struct dirs dir;
  getcwd(dir.hdir, 200);
  //printf("hdir: \"%s\"\n", dir.hdir);
  getcwd(dir.cdir, 200);
  strcpy(dir.ddir, strrchr(dir.cdir, '/')+1);

  while (1){
    printf("bish@BOSH:%s¢ ", dir.ddir);
    char line[100];
    fgets(line, 100, stdin);

    exec_all(null_term(line), &dir);
  }
}
