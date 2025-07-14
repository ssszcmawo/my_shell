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
#include <linux/limits.h>
#include <regex.h>
#include <sys/stat.h>

#define TK_BUFF_SIZE 1024
#define TOK_DELIM " \t\r\n"
#define HISTORY_SIZE 10

extern char *builtin_str[];
extern int (*builtin_func[])(char **);
extern char *history[HISTORY_SIZE];
extern int history_count;

int dash_cd(char **args);
int dash_help(char **args);
int dash_exit(char **args);
int dash_ls(char **args);
int dash_pwd(char **args);
int dash_print_history(char **args);
int dash_find(char **args);
int dash_grep(char **args);

int num_builtins();
int dash_execute(char **args);
char *read_line();
char **split_lines(char *line);
void add_to_history(char *command);
void parse_comands(char *line, char **args);
void handle_pipe(char *line);
void loop();

#endif
