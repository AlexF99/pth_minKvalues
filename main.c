#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_SIZE 120

float input[MAX_SIZE];

typedef struct
{
    float key;
    int val;
} pair_t;

void verifyOutput(const float *Input,
                  const pair_t *Output, // pair_t é o tipo de um par (v,p)
                  int nTotalElmts,
                  int k)
{
    // codigo da verificacao a ser incluido por voce
    // voce deve verificar se o conjunto de pares de saida está correto
    // e imprimir uma das mensagens abaixo
    int ok = 1;

    // inserir aqui o codigo da verificacao
    // uma implementação possível para o verificador seria
    // (nao precisa seguir essa descrição, voce pode fazer outro método
    //  de verificação)
    //
    // 1) Criar um vetor I de pares (chave, valor)
    //    os os elementos de I devem ser copias
    //    de cada valor (v,p) do vetor de entrada Input
    //    (ou seja, cada valor v que veio da posição p da entrada)
    // 2) Ordenar o vetor I em ordem crescente,
    //    obtendo-se um outro vetor Is (ordenado em ordem crescente de chaves)
    //    usando um algoritmo de ordenação do tipo (chave, valor)
    //    (por exemplo ordenação da stdlib, caso exista)
    // 3) Para cada par (ki,vi) pertencente ao vetor Output
    //      procurar a chave ki dentre K primeiros elementos
    //      de Is.
    //      Se a chave Ki estiver em Is com valor val==vi continue
    //      senão faça ok = 0 e reporte o erro abaixo

    if (ok)
        printf("\nOutput set verifyed correctly.\n");
    else
        printf("\nOutput set DID NOT compute correctly!!!\n");
}

int main(int argc, char const *argv[])
{
    /* code */

    int inputSize = 0;
    int nTotalElements = 0;
    for (int i = 0; i < nTotalElements; i++)
    {

        int a = rand(); // Returns a pseudo-random integer
                        //    between 0 and RAND_MAX.
        int b = rand(); // same as above

        float v = a * 100.0 + b;
        int p = inputSize;

        // inserir o valor v na posição p
        input[p] = v;
        ++inputSize;
    }

    // verifyOutput( Input, Output, nTotalElements, k );
    return 0;
}
