
#include <stdio.h>
#include <unistd.h>
#define BUF_LEN 200	

static char msg[BUF_LEN];

int main(int argc, char *argv[]){
 
    FILE* fichero;
    char c;

    if (argc!=2){
        fprintf(stderr,"./TestCharEcho Mensaje \n");
        return -1;
    }

    fichero = fopen("/dev/CharEcho","w");
    fputs(argv[1], fichero);

    fclose(fichero);

}