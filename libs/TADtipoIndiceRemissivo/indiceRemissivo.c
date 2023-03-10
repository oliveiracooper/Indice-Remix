#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include "indiceRemissivo.h"
#include "../DicDinamico/Dic.Dinamico.h"
#include "../TrataStrings/TrataStrings.h"
#include "../TADSTopMundo/stop.h"
#include "../ListaEncadeada/ListaEncadeada.h"
#include "../DicEstatico/tipoDicionario.h"

typedef struct pagOcorre{    
    int pagina;
    int ocorrencias;
    double pontuation;
}pagOcorre;

struct InfoDic{
    int tamVetOcorrencias;    
    int ocorrenciasTotal;
    int paginasTotal; // Total de paginas que apareceu a palavra
    pagOcorre* vetDeOcorrencias;
};


InfoDic* criarInfoDic(){    
    InfoDic* tipox = malloc(sizeof(InfoDic));
    tipox->tamVetOcorrencias = 500;    
    tipox->vetDeOcorrencias = malloc(sizeof(pagOcorre) * tipox->tamVetOcorrencias);
    tipox->ocorrenciasTotal = 1;
    tipox->paginasTotal = 1;    
    return tipox;
}

void mostrarVetorOcorrencias(InfoDic* blocoDeInfo){
   int x = 0;
   pagOcorre* aux = blocoDeInfo->vetDeOcorrencias;  
    while(x<5){
       printf("página: %d", aux[x].pagina);       
       printf("página: %d", aux[x].ocorrencias);
   }    
   printf("ocorrencias totais = %d", blocoDeInfo-> ocorrenciasTotal);
   printf("paginas totais: %d", blocoDeInfo->paginasTotal);  
   return;                                                               
   }
///FALTA criar a funcao INSERE ORDENADAMENT
pagOcorre* criarPagOcorre(int ocorrenciasXX, int paginaXX){    
    pagOcorre* x = malloc(sizeof(pagOcorre));
    x-> pagina = paginaXX;   
    x-> ocorrencias = ocorrenciasXX;
    return x;
}


typedef struct docs{
    int pagDoc;
    int Palavras_in_Doc;
}docs;

struct tipoIndiceRemissivo{
    TDicDinamic* Dicionario_do_livro;
    ListaE * listWithWords;
    docs* registraDocs;
    int ocupacaoRegistraDocs;
};

int _cmpValues(void * a, void * b){

    char * aa = a;
    char * bb = b;

    return strcmp(aa,bb);
}


tipoIndiceRemissivo * criarIndice(char*nomeArquivo, void*stopMundo){

    tipoStop * stop = stopMundo;

    tipoIndiceRemissivo* IndiceRe_atual = malloc(sizeof(tipoIndiceRemissivo));

    IndiceRe_atual->Dicionario_do_livro = criarDicDinamic(197);
    IndiceRe_atual->listWithWords = criarListaEncadeada(&_cmpValues);
    IndiceRe_atual->registraDocs = malloc(sizeof(docs)*2);
    IndiceRe_atual->ocupacaoRegistraDocs= 0;

    FILE* Arquivo_atual = fopen(nomeArquivo, "r");
    char* palavraLida = malloc(sizeof(char)*150); 
    int NumPagina_atual=-1;
    int OcorrenciasNa_pagina=0; 
    //docs* registraDocs = malloc(sizeof(docs)*2);
    int tamRegistraDocs = 2;
    //int ocupacaoRegistraDocs = 0;

    while(fscanf(Arquivo_atual, "%s", palavraLida)==1){

        if(strcmp(palavraLida, "PA")==0){
            NumPagina_atual+= 1;
            if(NumPagina_atual>=tamRegistraDocs){
                tamRegistraDocs *= 2;
                IndiceRe_atual->registraDocs = realloc(IndiceRe_atual->registraDocs,sizeof(docs)*tamRegistraDocs);
            }
            IndiceRe_atual->registraDocs[NumPagina_atual].pagDoc = NumPagina_atual;
            IndiceRe_atual->registraDocs[NumPagina_atual].Palavras_in_Doc = 0;
            IndiceRe_atual->ocupacaoRegistraDocs +=1;
        }
        else{
            LimpaString(palavraLida);
            if(strlen(palavraLida) == 0) continue;
            if(verificaStop(stopMundo, palavraLida)!=0) continue;
            
            removeSpecialCharacters((unsigned char*)palavraLida);
            
            
            LowerString((unsigned char*)palavraLida);
            
            if(strlen(palavraLida) <= 2) continue;
            
            InfoDic* endPalavra = buscarDicDinamico(IndiceRe_atual->Dicionario_do_livro, palavraLida);
            
            if(endPalavra==NULL){
                InfoDic* infoPalavra = criarInfoDic();
                infoPalavra->vetDeOcorrencias[0].ocorrencias = 1;
                infoPalavra->vetDeOcorrencias[0].pagina = NumPagina_atual;

                inserirLE_por_conteudo(IndiceRe_atual->listWithWords,palavraLida);

                inserirDicDinamico(IndiceRe_atual->Dicionario_do_livro, palavraLida, infoPalavra);
                
            }
            else{
                endPalavra->ocorrenciasTotal ++;
                pagOcorre* VInfo = endPalavra->vetDeOcorrencias;
                if((VInfo[(endPalavra->paginasTotal)-1].pagina)!= NumPagina_atual){
                    // // Se o Número de página atual não está no vetor, temos q adicionar ele.
                    if(endPalavra->tamVetOcorrencias <= endPalavra->paginasTotal){
                        endPalavra->tamVetOcorrencias *= 2;
                        endPalavra->vetDeOcorrencias = realloc(endPalavra->vetDeOcorrencias, sizeof(pagOcorre)*endPalavra->tamVetOcorrencias);
                    }
                    VInfo[endPalavra->paginasTotal].pagina= NumPagina_atual;
                    VInfo[endPalavra->paginasTotal].ocorrencias = 1;
                    endPalavra->paginasTotal++;

                }
                else{
                    // a pagina atual já existe no vetorOcorrencias, sua ocorrencia soma + 1
                    VInfo[(endPalavra->paginasTotal)-1].ocorrencias ++;
                }
            }
            IndiceRe_atual->registraDocs[NumPagina_atual].Palavras_in_Doc+= 1; /// contabiliza o numero de palavras total no documento

        }

        palavraLida = malloc(sizeof(char)*150);
    }
    fclose(Arquivo_atual);

    return IndiceRe_atual;
}

void calculePontuation(InfoDic * infomation,docs * documentos, int totDocs, int tam){
    int i=0;
    for( i=0; i < tam; i++){
        // vet =  16/400/ [ ]
        // tam é o tamanho de vet odocrrencias, ou seja, a quantidade de páginas que a palavra apareceu
        int ocorrenciaPalavra = infomation->vetDeOcorrencias[i].ocorrencias;

        int page = infomation->vetDeOcorrencias[i].pagina;

        int palavrasTotalDoDocumento = documentos[page].Palavras_in_Doc;

        int documentosQAparece = tam;
        infomation->vetDeOcorrencias[i].pontuation = idefDF(ocorrenciaPalavra,palavrasTotalDoDocumento,documentosQAparece,totDocs);
    }

}

const int cmpQsor(const void * a,const void *b){

    const pagOcorre * aa = a;
    const pagOcorre * bb = b;
    // mudamos o sinal de comparacao para que fique em ordem decrescente
    if((aa->pontuation) < (bb->pontuation)){
        return 1;
    }
    else if((aa->pontuation) > (bb->pontuation)){
        return -1;
    }
    else{
        return 0;
    }
}

void* searchElement(tipoIndiceRemissivo * index, char * key){
    removeSpecialCharacters(key);
    LimpaString(key);
    LowerString(key);
    
    InfoDic * retorno = buscarDicDinamico(index->Dicionario_do_livro, key);
   
    int i;
   
    if(retorno == NULL){
        printf("%s-- \n", key);
        printf("Palavra nao encontrada \n\n");
        return NULL;
    } 

    calculePontuation(retorno, index->registraDocs,index->ocupacaoRegistraDocs,retorno->paginasTotal);
    
    qsort(retorno->vetDeOcorrencias,retorno->paginasTotal,sizeof(pagOcorre),&cmpQsor);

    printf("%s-- \n", key);
    for( i=0; (i<retorno->paginasTotal)&&(i<5); i++){
        //printf("{ %d, %d,p: %lf  }\n", retorno->vetDeOcorrencias[i].ocorrencias, retorno->vetDeOcorrencias[i].pagina, retorno->vetDeOcorrencias[i].pontuation);
        printf("pagina: %d, ", retorno->vetDeOcorrencias[i].pagina+1);
        printf("pontuacao da pagina: %lf, \n", retorno->vetDeOcorrencias[i].pontuation);
    }
    printf("\n");
    //printf(" ocorrenciaaaaas totallllll %d", retorno->ocorrenciasTotal);
    
}
/*void* TF_IDF(tipoIndiceRemissivo* index, char* key){

    InfoDic* retorno = buscarDicDinamico(index->Dicionario_do_livro, key);
    docs* registradocs_atual = index->registraDocs;

    
}*/

void mostraIndiceRemissivo(tipoIndiceRemissivo* index){

    ListaE * lista = index->listWithWords;

    char* retorno = removeInicioListaEncadeada(lista);

    while (retorno){
        // printf("%s\n",retorno);
        searchElement(index,retorno);
        retorno = removeInicioListaEncadeada(lista);    
    }
    
}

void* searchElementARQUIVO(tipoIndiceRemissivo * index, char * key, FILE* nomeArquivo){
    removeSpecialCharacters(key);
    LimpaString(key);
    LowerString(key);
    
    InfoDic * retorno = buscarDicDinamico(index->Dicionario_do_livro, key);
   
    int i;
   
    if(retorno == NULL) return NULL;

    calculePontuation(retorno, index->registraDocs,index->ocupacaoRegistraDocs,retorno->paginasTotal);
    
    qsort(retorno->vetDeOcorrencias,retorno->paginasTotal,sizeof(pagOcorre),&cmpQsor);
    fprintf(nomeArquivo, "%s-- \n", key);
    for( i=0; (i<retorno->paginasTotal)&&(i<5); i++){
        //printf("{ %d, %d,p: %lf  }\n", retorno->vetDeOcorrencias[i].ocorrencias, retorno->vetDeOcorrencias[i].pagina, retorno->vetDeOcorrencias[i].pontuation);
        fprintf(nomeArquivo, "pagina: %d, ", retorno->vetDeOcorrencias[i].pagina+1);
        fprintf(nomeArquivo, "pontuacao da pagina: %lf, \n", retorno->vetDeOcorrencias[i].pontuation);
    }
    fprintf(nomeArquivo, "\n");
    //printf(" ocorrenciaaaaas totallllll %d", retorno->ocorrenciasTotal);
    
}



void mostraIndiceRemissivoARQUIVO(tipoIndiceRemissivo* index, FILE* nomearquivo){
    ListaE * lista = index->listWithWords;

    char* retorno = removeInicioListaEncadeada(lista);

    while (retorno){
        // printf("%s\n",retorno);
        searchElementARQUIVO(index,retorno, nomearquivo);
        retorno = removeInicioListaEncadeada(lista);    
    }
}

int returnIndexComparation(tipoIndiceRemissivo* index){
    TDicDinamic* atual = index->Dicionario_do_livro;
    int comps = retornaComparacoesTotaisDinamic(atual);
    return comps;
}

int returnIndexBusca(tipoIndiceRemissivo* index){
    TDicDinamic* atual = index->Dicionario_do_livro;
    int comps = retornaNumBuscasDinamic(atual);
    return comps;
}

int passouFatorCargaIndex(tipoIndiceRemissivo* index){
    TDicDinamic * atual = index->Dicionario_do_livro; 
    int passouCarga = retornaPassouCarga(atual);
    return passouCarga; 
}

int passouTamMaior(tipoIndiceRemissivo*index){
    TDicDinamic*atual = index->Dicionario_do_livro;
    int tamMaior = retornaMaior(atual); 
    return tamMaior; 
}

int tamVetDiclivro(tipoIndiceRemissivo* index){
    TDicDinamic*atual = index->Dicionario_do_livro;
    int tamVetLivro = retornaTamVetor(atual); 
    return tamVetLivro;
}

int quantiChavesDicLivro(tipoIndiceRemissivo* index){
    TDicDinamic*atual = index->Dicionario_do_livro;
    int quantChaves = retornaQuantiChaves(atual); 
    return quantChaves;
}

int posiOcupadasDicLivro(tipoIndiceRemissivo* index){
    TDicDinamic*atual = index->Dicionario_do_livro;
    int posiOcupada = retornaPosiOcupadas(atual);
    return posiOcupada;
}

double mediasPorListaDicLivro(tipoIndiceRemissivo* index){
    TDicDinamic*atual = index->Dicionario_do_livro;
    double mediasPorLista = mediasPorListas(atual);
    return mediasPorLista;
}

int ContReHashDicLivro(tipoIndiceRemissivo* index){
    TDicDinamic*atual = index->Dicionario_do_livro;
    int contReHas = retornaContReHash(atual);
    return contReHas; 
}
