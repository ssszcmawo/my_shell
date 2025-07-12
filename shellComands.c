#include "shellFunctions.h"
char *builtin_str[] = {
    "cd",
    "help"
    "exit"
    "pwd"
    "history"
};


int num_builtins(){return sizeof(builtin_str) / sizeof(char *);}


int (*builtin_func[]) (char **) = {
    &dash_cd,
    &dash_help,
    &dash_exit,
    &dash_pwd,
    &dash_print_history};


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


int dash_pwd(){
    char cwd[PATH_MAX];

    if(getcwd(cwd,sizeof(cwd)) != NULL){
        printf("%s",cwd);
    }else{
        perror("pwd");
    }
}

void dash_print_history(){
    for(int i = 0; i < history_count;i++){
        printf("%d %s\n",i + 1,history[i]);
    }
}

int dash_find(char *pattern,DIR *dir_path){
    DIR *dir;
    struct dirent *entry;
    regex_t regex;
    char **file_list = NULL;
    int file_count = 0;

    if (regcomp(&regex, pattern, REG_EXTENDED | REG_NOSUB) != 0) {
        fprintf(stderr, "Compilation error\n");
        return NULL;
    }

    dir = opendir(dir_path);
    if (dir == NULL) {
        perror("opendir");
        regfree(&regex);
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (regexec(&regex, entry->d_name, 0, NULL, 0) == 0) {
        
            file_list = realloc(file_list, (file_count + 1) * sizeof(char *));
            file_list[file_count] = strdup(entry->d_name); 
            file_count++;
        }
    }

    closedir(dir);
    regfree(&regex);

    return file_list;
}

int dash_grep(char *pattern,FILE *filename){
    FILE *file;
    regex_t regex;
    int buffersize = 256;
    char *buffer = malloc(buffersize * sizeof(char *));
    char *read;

    if (regcomp(&regex, pattern, REG_EXTENDED | REG_NOSUB) != 0) {
        fprintf(stderr, "Compilation error\n");
        return NULL;
    }

    file = fopen(filename,"r");
    if(!file){
        perror("file open error");
        return;
    }

    while((fgets(buffer,sizeof(buffer),file)) != NULL){
        if(regexec(&regex,buffer,0,NULL,0) == 0){
            printf("%s",buffer);
        }
    }

    regfree(&regex);
    fclose(file);
}






