# bishBOSH Shell (Late Date of Completion: 7 December 2021)
Successfully Implemented Features:
- Semicolon separation (with aforementioned quotation mark functionality).
- Path directory method execution (including cd and exit).
  - For `cd`, arguments `/`, `.`, `..`, `[child directory]`, simply `cd` and one-argument combinations of these arguments (e.g. `../../` are supported.
- Redirection and piping.

Bonus Features:
- Current directory shown in command line (not in reference to ~, unfortunately).
- Quotation marks to ignore delimiters (e.g. `cd 'fol der'`)
- Multiple (consecutive) semicolons and spaces are allowed (e.g. `ls ; ;;;  ;  ls   -l` executes `ls;ls -l`).
  - bash does not have exactly this semicolon functionality, but we thought it would be cool.
  - This applies for redirection and piping as well, HOWEVER there must be at least one space between the operators and surrounding arguments (e.g. `ls|wc` fails whereas `ls    |    wc` executes).

Unsuccessfully Attempted Features:
- Did not manage to immitate bash behaviour of showing current directory's relation to ~ directory.

Limitations:
- Current path has a maximum length of 100 chars.
- `>`, `>>`, `<` and `|` require blankspaces before and after them to function.

Known Bugs/Issues:
- For some reason, performing `ls | wc` outputs a character count 1 greater than bash (e.g. bishBOSH `ls | wc` could output `      6       6      49` when bash `ls | wc` would output `      6       6      48`) 

```
struct dirs{char hdir[100]; char cdir[100]; char ddir[100];};
// Holds information about the home, current and display directory.

int strcount(char *str, char *sub);
// Counts the total instances of sub within str (e.x. strcount("helllo", "ll") = 2).

char *null_term(char *str);
// Changes newline terminated strings into null terminated strings (particularly useful for fgets()).

char *combine_args(char *line[], int delim_i);
// Combines line into a char * and replaces the argument which was at line[delim_i] with 0, effectively splitting this memory space into two usable strings.
// delim_i is only ever passed the index of "|" in a given line, as this is only used for piping.

int sep(char **str, char **src, char delim);
// Mimics strsep (or, more accurately, strtok) functionality, but also allows quotations to cancel out deliminators.

int exec_cd(char *line[], struct dirs *dir);
// Runs cd commands.

int exec_exit(char *line[]);
// Runs exit command.

int exec_redir(char *line[], struct dirs *dir);
// Attempts to handle redirection and piping before calling exec_func if necessary.

int exec_func(char *line[], struct dirs *dir);
// Runs all other individual commands.

int exec_all(char *line, struct dirs *dir);
// Parses through entire input, separates each command into character pointer arrays and calls exec_func.
```
