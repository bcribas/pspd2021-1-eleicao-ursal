#include <stdio.h>
#include <stdlib.h>

int main (){
    int senadores, depFederal, depEstadual, presidente;
    int votoValido = 0, votoInvalido = 0;
    int voto;
    
    int senador = malloc(sizeof(int));
    int federal = malloc(sizeof(int));
    int estadual = malloc(sizeof(int));
    int presidente = malloc(sizeof(int));
    
    scanf("%d %d %d", &senador, &depFederal, &depEstadual);

    while(EOF){
        scanf("%d", &voto);
        
        if(voto < 0){
            votoInvalido++;
        } else {
            votoValido++;
            if(voto > 9 && voto > 100){
                
            } else if(voto > 99 && voto > 1000){

            } else if(voto > 999 && voto > 10000){

            } else if(voto > 9999 && voto > 100000) {

            }
        }
    }


    return 0;
}