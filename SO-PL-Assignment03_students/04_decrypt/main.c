#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

FILE *txt_file, *words_file;

struct crypt{
    char word[50];
    char chave[100];
} words[6];


void decrypt(char line[300], int line_num);
void find_words(char line[300], int line_num);
int replace(char *line, char *b, int index);


int main(int argc, char *argv[]){
    if (argc != 2){
        printf("Erro - No file was passed in the parameters!\n");
        exit(-1);
    }

    words_file = fopen("decrypt.txt", "r");
    txt_file = fopen(argv[1], "r");
    char buffer_words[300] = "";
    char buffer[300] = "";
    int line = 1;
    int i = 0;

    //load the words from decrypt.txt
    while((fgets(buffer_words, 300, (FILE*)words_file)) != NULL){
        char *ptr = strtok(buffer_words, " ");
        strcpy(words[i].word, ptr); //words[i].word = ptr;
        ptr = strtok(NULL, " ");
        strncpy(words[i].chave, ptr, 32);
        i++;
    }

    //Ler o texto do ficheiro text.txt
    while((fgets(buffer, 300, (FILE*)txt_file)) != NULL){
        decrypt(buffer, line);
        //find_words(buffer, line);
        line++;
    }
    printf("\n");

    fclose(words_file);
    fclose(txt_file);
    return 0;
}

//Cretes a process for each line, to decrypt
void decrypt(char line[300], int line_num){
    if(fork() == 0){ //child
        find_words(line, line_num);
        //exit(0);
    } else { //parent
        wait(NULL);
        exit(0);
    }

}


void find_words(char line[300], int line_num){
    int a = 0;
    char window[33] = "";

    strncpy(window, line + a, 32);
    while(strlen(window) == 32){
        for(int k = 0; k < 6; k++){
            if(strcmp(window, words[k].chave) == 0){
                int aux = replace(line, words[k].word, a);
                if (a - aux < 0){
                    a = 0;
                }
                break;
            }
        }
        a++;
        strncpy(window, line + a, 32);
    }
    printf("Line %d: ", line_num);
    printf("%s", line);
    
}


int replace(char *line, char *b, int index){
    char aux[300] = "";
    char new_line[300] = "";

    strncpy(new_line, line, index);
    strcat(new_line, b);
    strncpy(aux, line + index + 32, 300);
    strcat(new_line, aux);

    strcpy(line, new_line);

    return strlen(b);
}
