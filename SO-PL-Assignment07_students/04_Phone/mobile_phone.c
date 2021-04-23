#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>


void phone();

int main(){

    if(fork() == 0){
        phone()
    }

    exit(0);
}

void phone(){
    pipe();
}