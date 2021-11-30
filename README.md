# bishBOSH Shell (At the moment, a deeply flawed construction!)
Successfully Implemented Features:
- Quotation marks to ignore delimiters (e.x. `cd 'fol der'`)
- Current directory shown in command line (unfortunately not in reference to home)
- Semicolon separation is implemented, but imperfect

Unsuccessfully Attempted Features:
- Redirection and piping are not implemented (yet).
- Directory in the command line does not try to stem from home directory.

Known Bugs/Issues:
- `cd` cannot `cd` into a parent directory (e.x. `cd ..`)
- Still no redirection / piping (these features will hopefully come late)
- Argument parsing is prone to error due to quote functionality.
  - `argray` and `com` have occasionally overlapped (e.x. with `DEBUG = 1`, `ls; echo hello; echo how are you; ls -l` demonstrates overlap, breaks either way).
  - If input is exceptionally long (i.e. `                                                 ls`, `malloc(): corrupted top size` error may occur.

```
struct dirs{char hdir[100]; char cdir[100]; char ddir[100];};
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
