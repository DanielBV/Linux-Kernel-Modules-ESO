
#include <stdio.h>
#include <unistd.h>
#define BUF_LEN 200	

static char msg[BUF_LEN];

int main(void){
 
    FILE* fichero;
    char c;


    sprintf(msg, "Nuevo mensaje para el buffer\n");


    fichero = fopen("/dev/CharEcho","w");

    fputs(msg, fichero);

    fclose(fichero);

}