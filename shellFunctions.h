#ifndef SHELLFUNCTIONS_H
#define SHELLFUNCTIONS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>

#define TK_BUFF_SIZE 1024
#define TOK_DELIM " \t\r\n"

extern char *builtin_str[];
extern int (*builtin_func[]) (char **);

int dash_cd(char **args);
int dash_help(char **args);
int dash_exit(char **args);
int dash_ls(char **args);
int num_builtins();
int dash_execute(char **args);
char *read_line();
char **split_lines(char *line);
void loop();

#endif
