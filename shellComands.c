#include "shellFunctions.h"

int history_count = 0;
char *history[HISTORY_SIZE];

char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "pwd",
    "history",
    "grep",
    "find",
    "ls",
    "rm"};

int num_builtins() { return sizeof(builtin_str) / sizeof(char *); }

int (*builtin_func[])(char **) = {
    &dash_cd,
    &dash_help,
    &dash_exit,
    &dash_pwd,
    &dash_print_history,
    &dash_grep,
    &dash_find,
    &dash_ls,
    &dash_rm};

int dash_help(char **args)
{
    printf("Commands:\n");
    for (int i = 0; i < num_builtins(); i++)
    {
        printf("%s\n", builtin_str[i]);
    }
    return 1;
}

int dash_cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "cd: needs argument\n");
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("cd");
        }
    }
    return 1;
}

int dash_ls(char **args)
{
    DIR *d;
    struct dirent *dir;

    char *dir_name = ".";
    if (args[1] != NULL)
    {
        dir_name = args[1];
    }

    d = opendir(dir_name);
    if (d == NULL)
    {
        perror("ls");
        return 1;
    }

    while ((dir = readdir(d)) != NULL)
    {
        printf("%s\n", dir->d_name);
    }
    printf("\n");
    closedir(d);
    return 1;
}

int dash_exit(char **args)
{
    return 0;
}

int dash_pwd(char **args)
{
    char cwd[PATH_MAX];

    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s\n", cwd);
    }
    else
    {
        perror("pwd");
    }

    return 1;
}

int dash_print_history(char **args)
{
    for (int i = 0; i < history_count; i++)
    {
        printf("%d %s\n", i + 1, (char *)history[i]);
    }
    return 1;
}

int dash_find(char **args)
{

    if (args[1] == NULL || args[2] == NULL)
    {
        fprintf(stderr, "Usage: find <pattern> <directory>\n");
        exit(EXIT_FAILURE);
    }
    DIR *dir;
    struct dirent *entry;
    regex_t regex;

    if (regcomp(&regex, args[1], REG_EXTENDED | REG_NOSUB) != 0)
    {
        fprintf(stderr, "Compilation error\n");
        exit(EXIT_FAILURE);
    }

    dir = opendir(args[2]);
    if (dir == NULL)
    {
        perror("opendir");
        regfree(&regex);
        return 1;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (regexec(&regex, entry->d_name, 0, NULL, 0) == 0)
        {
            printf("%s\n", entry->d_name);
        }
    }

    closedir(dir);
    regfree(&regex);

    return 1;
}

int dash_grep(char **args)
{
    if (args[1] == NULL || args[2] == NULL)
    {
        fprintf(stderr, "Usage: grep <pattern> <filename>\n");
        return 1;
    }
    FILE *file;
    regex_t regex;
    char buffer[TK_BUFF_SIZE];

    if (regcomp(&regex, args[1], REG_EXTENDED | REG_NOSUB) != 0)
    {
        fprintf(stderr, "Compilation error\n");
        return 1;
    }

    file = fopen(args[2], "r");
    if (!file)
    {
        perror("file open error");
        regfree(&regex);
        return 1;
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        if (regexec(&regex, buffer, 0, NULL, 0) == 0)
        {
            printf("%s", buffer);
        }
    }

    regfree(&regex);
    fclose(file);
    return 1;
}

int dash_rm(char **args)
{
    const char *name = args[1];
    struct stat path_stat;
    struct dirent *entry;

    if (lstat(name, &path_stat) != 0)
    {
        perror("lstat");
        return -1;
    }

    if (S_ISREG(path_stat.st_mode) || S_ISLNK(path_stat.st_mode))
    {
        if (unlink(name) != 0)
        {
            perror("unlink");
            return -1;
        }
    }
    else if (S_ISDIR(path_stat.st_mode))
    {
        DIR *dir = opendir(name);
        if (!dir)
        {
            perror("opendir");
            return -1;
        }

        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            char fullpath[PATH_MAX];
            int len = snprintf(fullpath, sizeof(fullpath), "%s/%s", name, entry->d_name);
            if (len < 0 || len > sizeof(fullpath))
            {
                fprintf(stderr, "Path too long : %s/%s\n", name, entry->d_name);
                closedir(dir);
                return -1;
            }
            char *new_args[] = {args[0], fullpath, NULL};
            if (dash_rm(new_args) != 0)
            {
                closedir(dir);
                return -1;
            }
        }

        closedir(dir);

        if (rmdir(name) != 0)
        {
            perror("rmdir");
            return -1;
        }
    }
    return 0;
}
