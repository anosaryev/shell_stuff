#include "shell.h"

#define DEBUG 1

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

char *combine_args(char *line[], int delim_i){
  int i;
  int si = -1;
  for (i = 0; line[i]; i ++){
    si += 1 + strlen(line[i]);
  }
  if (si == -1)
    si = 0;
  char *command = malloc(si);
  for (i = 0; line[i]; i ++){
    if (i != delim_i)
      strcat(command, line[i]);
    else
      strcat(command, "\0");
    strcat(command, " ");
  }
  return command;
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
    int b = ((*src)[i] == '\\' && (!q || q == (*src)[i+1]) &&
	     ((*src)[i+1] == '"' || (*src)[i+1] == '\'')); // valid cancellation?
    if (((*src)[i] == '\'' || (*src)[i] == '"') && (!q || q == (*src)[i]) && !n){
      q = (q)? 0 : (*src)[i]; // valid quote initiation or termination?
      if (delim == ';')
        strncat(*str, *src+i, 1);
    }
    else if ((*src)[i] == delim && !q){ //@ delim?
      if (strlen(*str))
        break;
    }else if (!b || delim == ';'){
      strncat(*str, *src+i, 1);
    }
    n = b;
    if (DEBUG)
      printf("MID:\t\"%s\"\t\"%s\"\t'%c'\n", *str, *src, delim);
  }

  if (i == strlen(*src))
    strcpy(*src, "");
  else{
    char temp[strlen(*src+1)];
    strcpy(temp, *src+i);
    strcpy(*src, temp);
  }
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
      strcpy(line[1], line[1]+1);
    }else if(!strncmp(line[1], "../", 3) || !strncmp(line[1], "..\0", 3)){
      if (strcmp(dir->cdir, "/")){

	char *sp = strrchr(dir->cdir, '/');
        *sp = '\0';
        if (dir->cdir == ""){
          memset(dir->cdir, '\0', 100);
          strcpy(dir->cdir, "/");
        }
        b = chdir(dir->cdir);
      }
      strcpy(line[1], (line[1][2] == '/')? line[1]+3 : line[1]+2);
    }else if(!strncmp(line[1], "./", 2) || !strncmp(line[1], ".", 2))
      strcpy(line[1], line[1]+2);
    else{
      strcat(dir->cdir, "/");
      strncat(dir->cdir, line[1], 1 + (strchr(line[1], '/'))? strchr(line[1], '/') - line[1] : strlen(line[1]));
      strcpy(line[1], (strchr(line[1], '/'))? strchr(line[1], '/')+1 : "");
      b = chdir(dir->cdir);
    }
  }
  if (b == -1){
    printf("Error %d: %s\n", errno, strerror(errno));
    return -1;
  }
  getcwd(dir->cdir, 100);
  strcpy(dir->ddir, strrchr(dir->cdir, '/')+1);

  if (DEBUG)
    printf("line[1] = \"%s\" at %p; cdir = \"%s\"\n", line[1], line[1], dir->cdir);

  if (line[1] && line[1][0])
    return exec_cd(line, dir);
  return 0;
}

int exec_exit(char *line[]){
  exit(0);
}

int exec_redir(char *line[], struct dirs *dir){
  int i;
  for (i = 0; line[i]; i ++){
    if (!strcmp(line[i], "<")){

      int stdin_dup = dup(STDIN_FILENO);
      int fd1 = open(line[i+1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
      dup2(fd1, STDIN_FILENO);
      char *op = line[i];
      line[i] = 0;
      exec_func(line, dir);
      line[i] = op;
      dup2(stdin_dup, STDIN_FILENO);
      return 0;

    }else if (!strcmp(line[i], ">")){

      int stdout_dup = dup(STDOUT_FILENO);
      int fd1 = open(line[i+1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
      dup2(fd1, STDOUT_FILENO);
      char *op = line[i];
      line[i] = 0;
      exec_func(line, dir);
      line[i] = op;
      dup2(stdout_dup, STDOUT_FILENO);
      return 0;

    }else if (!strcmp(line[i], ">>")){

      int stdout_dup = dup(STDOUT_FILENO);
      int fd1 = open(line[i+1], O_CREAT | O_WRONLY | O_APPEND, 0644);
      dup2(fd1, STDOUT_FILENO);
      char *op = line[i];
      line[i] = 0;
      exec_func(line, dir);
      line[i] = op;
      dup2(stdout_dup, STDOUT_FILENO);
      return 0;

    }else if (!strcmp(line[i], "|")){
      char *from = combine_args(line, i);
      char *to = from + i + 1;
      FILE *read = popen(from, "r");
      FILE *write = popen(to, "w");
      char temp[strlen(from)];
      dup2(fileno(read), fileno(write));
      to = 0;
      free(from);
      return 0;
    }
  }
  exec_func(line, dir);
  return 0;
}

int exec_func(char *line[], struct dirs *dir){
  if (!strcmp(line[0], "cd")){
    if (DEBUG)
      printf("line[1] = \"%s\" from %p to %p; cdir = \"%s\"\n", line[1], line[1], line[1]+strlen(line[1]), dir->cdir);
    return exec_cd(line, dir);
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
  int si = strlen(line);
  int el = strcount(line, " ")+1;
  char *cmd[el];
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
    for (i = 0; argray[i] && strlen(argray[i]); i ++){
      cmd[i] = malloc(sizeof(argray[i]));
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
      exec_redir(cmd, dir);
    }

    for (i = 0; cmd && cmd[i] && strcmp(cmd[i], ""); i ++)
      free(cmd[i]);

    if (line && strlen(line)){
      if (DEBUG)
        printf("\n\tcom:\tline:\t';'\n\n");
      sep(&com, &line, ';');
    }
  }

  if (arg)
    free(arg);
  if (com){
    free(com);
  }
  return 0;
}

int main(){
  struct dirs dir;
  getcwd(dir.hdir, 200);
  printf("hdir: \"%s\"\n", dir.hdir);
  getcwd(dir.cdir, 200);
  strcpy(dir.ddir, strrchr(dir.cdir, '/')+1);

  while (1){
    printf("bish@BOSH:%s¢ ", dir.ddir);
    char line[100];
    fgets(line, 100, stdin);
    if(strcmp(line, "\n"))
      exec_all(null_term(line), &dir);
  }
}
