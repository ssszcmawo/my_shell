#include "shellFunctions.h"


char *read_line(){
    int buffsize = 1024;
    int position = 0;

    char *buffer = malloc(sizeof(char) * buffsize);
    int c;

    if(!buffer){
        fprintf(stderr,"dash: Allocation error\n");
        exit(EXIT_FAILURE);
    }

    while(1){
        c = getchar();
        if(c == EOF || c == '\n'){
            buffer[position] = '\0';
            return buffer;
        }else{
            buffer[position] = c;
        }
        position++;
        if(position >= buffsize){
            buffsize += 1024;
            buffer = realloc(buffer,buffsize);        
        if(!buffer){
            fprintf(stderr,"dash: Allocation error");
            exit(EXIT_FAILURE);
        }

    }  
 }
}

char **split_lines(char *line){
    int buffsize = TK_BUFF_SIZE;
    int position = 0;

    char **tokens = malloc(buffsize * sizeof(char *));
    char *token;

    if(!tokens){
        fprintf(stderr,"dash: Allocation error");
        exit(EXIT_FAILURE);
    }

    token = strtok(line,TOK_DELIM);
    while(token != NULL){
        tokens[position] = token;
        position++;
        if(position >= buffsize){
            buffsize += TK_BUFF_SIZE;
            tokens = realloc(tokens,buffsize * sizeof(char *));

            if(!tokens){
                fprintf(stderr,"dash: Allocation error");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL,TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

int dash_execute(char **args){
    pid_t cpid;
    int status;
    if(args[0] == NULL)return 1;

    for (int i = 0; i < num_builtins(); i++)
    {
        if(strcmp(args[0],builtin_str[i]) == 0){
            if(strcmp(args[0],"cd") == 0){
                return dash_cd(args);
            }
            return (*builtin_func[i])(args);
        }
    }

    cpid = fork();

    if(cpid == 0){
        if(execvp(args[0],args) < 0){
            printf("dash: command not found");
            exit(EXIT_FAILURE);}

        }else if(cpid < 0){
            printf("Error forking");
            exit(EXIT_FAILURE);
        }else{
            waitpid(cpid,&status,WUNTRACED);
        }
    
    return 1;
}


void loop(){
    char *line;
    char **args;
    int status = 1;

    do{
        printf(">");
        line = read_line();
        args = split_lines(line);
        status = dash_execute(args);
        free(line);
        free(args);
    }while(status);
}


