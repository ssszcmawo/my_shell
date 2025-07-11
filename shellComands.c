#include "shellFunctions.h"
char *builtin_str[] = {
    "cd",
    "help"
    "exit"
};


int num_builtins(){return sizeof(builtin_str) / sizeof(char *);}


int dash_help(char **args){
    printf("Commands:\n");
    for(int i = 0; i < num_builtins(); i++){
        printf("%s\n",builtin_str[i]);
    }
    return 1;
}

int dash_cd(char **args){
    if(args[1] == NULL){
        fprintf(stderr,"cd: needs argument");
    }else{
        if(chdir(args[1]) != 0){
            perror("cd");
        }
    }
    return 1;
}

int dash_ls(char **args){
    DIR *d;
    struct dirent *dir;

    char *dir_name = ".";
    if(args[1] != NULL){
        dir_name = args[1];
    }
    
    d = opendir(dir_name);
    if(d == NULL){
        perror("ls");
        return 1;
    }

    while((dir = readdir(d)) != NULL){
        printf("%s",dir->d_name);
    }
    printf("\n");
    closedir(d);
    return 0;
    
}

int dash_exit(char **args){
    return 0;
}

int (*builtin_func[]) (char **) = {&dash_cd,&dash_help,&dash_exit};