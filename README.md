# bishBOSH Shell (At the moment missing required features!!!!!)
Successfully Implemented Features:
- Semicolon separation (with aforementioned quotation mark functionality).
- Path directory method execution (including cd and exit).
  - For `cd`, arguments `/`, `.`, `..`, `[child directory]`, simply `cd` and one-argument combinations of these arguments are supported.

Bonus Features:
- Current directory shown in command line (not in reference to ~, unfortunately).
- Quotation marks to ignore delimiters (e.g. `cd 'fol der'`)
- Multiple (consecutive) semicolons and spaces are allowed (e.g. `ls ; ;;;  ;  ls   -l` executes `ls;ls -l`).
  - bash does not have exactly this semicolon functionality, but we thought it would be cool.

Unsuccessfully Attempted Features:
- Redirection and piping are not implemented (yet).

Known Bugs/Issues:
- Still no piping.
- Strange bug with name inconsistency. For example, running `rm hey` will actually execute `rm yey`.

```
struct dirs{char hdir[200]; char cdir[200]; char ddir[200];};
// Holds information about the home, current and display directory.

int strcount(char *str, char *sub);
// Counts the total instances of sub within str (e.x. strcount("helllo", "ll") = 2).

char *null_term(char *str);
// Changes newline terminated strings into null terminated strings (particularly useful for fgets()).

int sep(char **str, char **src, char delim);
// Mimics strsep (or, more accurately, strtok) functionality, but also allows quotations to cancel out deliminators.

int exec_cd(char *line[], struct dirs *dir);
// Runs cd commands.

int exec_exit(char *line[]);
// Runs exit command.

int exec_func(char *line[], struct dirs *dir);
// Runs all other individual commands (no redirection or piping support yet).

int exec_all(char *line, struct dirs *dir);
// Parses through entire input, separates each command into character pointer arrays and calls exec_func.
```
