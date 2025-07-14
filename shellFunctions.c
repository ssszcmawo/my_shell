#include "shellFunctions.h"

#define ARGS_NUM 64

char *read_line()
{
    int buffsize = TK_BUFF_SIZE;
    int position = 0;

    char *buffer = malloc(sizeof(char) * buffsize);
    int c;

    if (!buffer)
    {
        fprintf(stderr, "dash: Allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        c = getchar();
        if (c == EOF || c == '\n')
        {
            buffer[position] = '\0';
            return buffer;
        }
        else
        {
            buffer[position] = c;
        }
        position++;
        if (position >= buffsize)
        {
            buffsize += 1024;
            buffer = realloc(buffer, buffsize);
            if (!buffer)
            {
                fprintf(stderr, "dash: Allocation error");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **split_lines(char *line)
{
    int buffsize = TK_BUFF_SIZE;
    int position = 0;

    char **tokens = malloc(buffsize * sizeof(char *));
    char *token;

    if (!tokens)
    {
        fprintf(stderr, "dash: Allocation error");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELIM);
    while (token != NULL)
    {
        tokens[position] = token;
        position++;
        if (position >= buffsize)
        {
            buffsize += TK_BUFF_SIZE;
            tokens = realloc(tokens, buffsize * sizeof(char *));

            if (!tokens)
            {
                fprintf(stderr, "dash: Allocation error");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

int dash_execute(char **args)
{
    pid_t cpid;
    int status;
    if (args[0] == NULL)
        return 1;

    for (int i = 0; i < num_builtins(); i++)
    {
        if (strcmp(args[0], builtin_str[i]) == 0)
        {
            if (strcmp(args[0], "cd") == 0)
            {
                return dash_cd(args);
            }
            return (*builtin_func[i])(args);
        }
    }

    cpid = fork();

    if (cpid == 0)
    {
        if (execvp(args[0], args) < 0)
        {
            printf("dash: command not found");
            exit(EXIT_FAILURE);
        }
    }
    else if (cpid < 0)
    {
        printf("Error forking");
        exit(EXIT_FAILURE);
    }
    else
    {
        waitpid(cpid, &status, WUNTRACED);
    }

    return 1;
}

void add_to_history(char *comand)
{
    if (history_count < HISTORY_SIZE)
    {
        history[history_count++] = strdup(comand);
    }
    else
    {
        free(history[0]);
        for (size_t i = 1; i < HISTORY_SIZE; i++)
        {
            history[i - 1] = history[i];
        }
        history[history_count++] = strdup(comand);
    }
}

void parse_comands(char *line, char **args)
{
    int i = 0;
    char *token = strtok(line, TOK_DELIM);
    while (token != NULL)
    {
        args[i++] = token;
        token = strtok(NULL, TOK_DELIM);
    }
    args[i] = NULL;
}

void handle_pipe(char *line)
{
    int pipefd[2];
    pid_t pid1, pid2;

    char *left_cmd = strtok(line, "|");
    char *right_cmd = strtok(NULL, "|");

    if (left_cmd == NULL || right_cmd == NULL)
    {
        fprintf(stderr, "Invalid pipe syntax\n");
        return;
    }

    char *args1[ARGS_NUM];
    char *args2[ARGS_NUM];

    parse_comands(left_cmd, args1);
    parse_comands(right_cmd, args2);

    pipe(pipefd);

    pid1 = fork();
    if (pid1 == 0)
    {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        execvp(args1[0], args1);
        perror("execvp left");
        exit(1);
    }

    pid2 = fork();
    if (pid2 == 0)
    {
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[1]);
        close(pipefd[0]);
        execvp(args2[0], args2);
        perror("execvp rigth");
        exit(1);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

void loop()
{
    char *line;
    char **args;
    int status = 1;

    do
    {
        printf(">");
        line = read_line();
        if (strchr(line, '|') != NULL)
        {
            handle_pipe(line);
            free(line);
            continue;
        }
        args = split_lines(line);
        status = dash_execute(args);
        free(line);
        free(args);
    } while (status);
}
