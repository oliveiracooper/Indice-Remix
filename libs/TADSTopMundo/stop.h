#ifndef TIPOSTOP_H
#define TIPOSTOP_H

typedef struct tipoStop tipoStop;

tipoStop * criarStop(char* arquivoStop);

int verificaStop(tipoStop* stop, char* arquivoStop);
int retornaComparacoesTotaisSTOP(tipoStop* stop);
int retornaNumBuscasSTOP(tipoStop* stop);

#endif