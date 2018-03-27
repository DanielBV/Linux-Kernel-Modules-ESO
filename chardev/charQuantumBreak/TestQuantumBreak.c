
#include <stdio.h>
#include <unistd.h>
#define BUF_LEN 200	

static char msg[BUF_LEN];

int main(void){
    /**
     *   Opens the quantumbreak char device and executes a infinite loop.
     *    If scheduler throttling is disabled, it will block.
     * 
     */
 
    FILE* fichero;
    char c;


    sprintf(msg, "Nuevo mensaje para el buffer\n");


    fichero = fopen("/dev/quantumBreak","r");

    while(1);

    fclose(fichero);

}