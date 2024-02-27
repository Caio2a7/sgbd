/*
=========================================================================================================
Arquivo: criar_tabela.c
Data modificação: 13 de dezembro de 2023
Descrição: O objetivo desse código é listar todos os dados do banco de dados.
=========================================================================================================
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "../../includes/utils.h"
#include "../../includes/utils/string_utils.h"
#include "../../includes/utils/file_utils.h"
#define TITULO "DADOS DA TABELA"

/*--------------------------------------------------------------------------------
Função: Printa na tela uma linha horizontal de delimitação formatadas para o tamanho dos caracters do banco.
Paramêtros: 
    -Array contendo o tamanho das colunas;
    -Quantidade de colunas;
*/
void mostrar_linha(int *tam_colunas, int qtd_colunas) {
    printf("+");
    for (int i = 0; i < qtd_colunas; i++) {
        for (int j = 0; j < tam_colunas[i] + 2; j++) {
            printf("-");
        }
        printf("+");
    }
    printf("\n");
}

/*--------------------------------------------------------------------------------
Função: Printa a tabela em si do banco de dados, utiliza a função mostrar linhas para
formatação.
    -Array contendo o nome das colunas;
    -Array contendo os tipos das colunas;
    -Quantidade de colunas;
*/
int * formatar_tab(char **nomes_colunas, int *tipos_colunas, int qtd_colunas) {
    int *tam_colunas = (int *)malloc(sizeof(int) * qtd_colunas);
    for (int i = 0; i < qtd_colunas; i++) {
        tam_colunas[i] = strlen(nomes_colunas[i])+9;
    }

    mostrar_linha(tam_colunas, qtd_colunas);
    printf("|");
    for (int i = 0; i < qtd_colunas; i++) {
        switch (tipos_colunas[i]) {
            case INT:
                sprintf(nomes_colunas[i], "%s(INT)", nomes_colunas[i]);
                printf(" %-*s |", tam_colunas[i], nomes_colunas[i]); 
                break;
            case FLOAT:
                sprintf(nomes_colunas[i], "%s(FLOAT)", nomes_colunas[i]);
                printf(" %-*s |", tam_colunas[i], nomes_colunas[i]); 
                break;
            case DOUBLE:
                sprintf(nomes_colunas[i], "%s(DOUBLE)", nomes_colunas[i]); 
                printf(" %-*s |", tam_colunas[i], nomes_colunas[i]);
                break;
            case CHAR:
                sprintf(nomes_colunas[i], "%s(CHAR)", nomes_colunas[i]); 
                printf(" %-*s |", tam_colunas[i], nomes_colunas[i]);
                break;
            case STRING:
                sprintf(nomes_colunas[i], "%s(STRING)", nomes_colunas[i]); 
                printf(" %-*s |", tam_colunas[i], nomes_colunas[i]);
                break;
            default:
                break;
        }
    }
    printf("\n");
    mostrar_linha(tam_colunas, qtd_colunas);
    return tam_colunas;
}

/*--------------------------------------------------------------------------------
Função: Realiza uma formatação correta dos dados na tabela.
    -Array contendo as linhas lidas na entrada do usuário;
    -Array contendo o tamanho das colunas;
    -Quantidade de colunas;
*/
void formatar_dados(char **tupla, int *tam_colunas, int qtd_colunas){
    printf("|");
    for (int j = 0; j < qtd_colunas; j++) {
        printf(" %-*s |", tam_colunas[j], tupla[j]);
    }
    printf("\n");
    mostrar_linha(tam_colunas, qtd_colunas);
}

/*--------------------------------------------------------------------------------
Função: principal a qual irá realizar todas as operações de listagem e que chamará as outras funções principal.
*/
int listar_dados(){
    FILE *arquivo;
    char banco_nome[STRING_MAX_SIZE];

    // Solicita ao usuário o nome do Banco de Dados
    printf("Digite o nome do Banco de Dados: ");
    fgets(banco_nome, STRING_MAX_SIZE, stdin);
    int string_lenght = strlen(banco_nome);
    if(banco_nome[string_lenght-1] == '\n'){
        banco_nome[string_lenght-1] = '\0';
    }

    // Remove o caractere de nova linha, se existir
    arquivo = abrir_arquivo(banco_nome, 'r');
    
    // Verifica se o arquivo foi aberto corretamente
    if(arquivo == NULL){
        perror("Erro ao abrir o banco de dados. ");
        return 0;
    }

    char buffer[BUFFER_MAX_SIZE];
    fgets(buffer, sizeof(buffer), arquivo);

    int pos = 0;
    int aux = 1;
    int *coluna_tipos = (int *)malloc(sizeof(int) * aux);
    // Processa a primeira linha para determinar os tipos de dados das colunas
    while(sscanf(buffer+pos, "%d;", &coluna_tipos[aux-1]) == 1){
        pos += strcspn(buffer + pos, ";") + 1;
        aux++;
        coluna_tipos = realloc(coluna_tipos, sizeof(int) * aux * 2);
    }

    fgets(buffer, sizeof(buffer), arquivo);

    pos = 0;
    aux = 1;
    char **coluna_nomes = (char **)malloc(sizeof(char *) * aux);
    coluna_nomes[aux-1] = (char *)malloc(sizeof(char) * STRING_MAX_SIZE);

     // Processa a segunda linha para obter os nomes das colunas
    while(sscanf(buffer + pos, "%[^;\n];", coluna_nomes[aux-1]) == 1){
        pos += strcspn(buffer + pos, ";") + 1;
        aux ++;
        coluna_nomes = realloc(coluna_nomes, sizeof(char *) * 2 * aux);
        coluna_nomes[aux-1] = (char *)malloc(sizeof(char) * STRING_MAX_SIZE);
    }

    int qtd_colunas = aux-1;
    int *coluna_tam = formatar_tab(coluna_nomes, coluna_tipos, qtd_colunas);

    // Itera sobre as linhas do arquivo, extraindo e formatando as tuplas
    while(fgets(buffer, sizeof(buffer), arquivo) != NULL){
        pos = 0;
        aux = 1;
        char **tupla = (char **)malloc(sizeof(char *) * aux);
        tupla[aux-1] = (char *)malloc(sizeof(char) * STRING_MAX_SIZE);
        // Processa a linha para obter os valores da tupla
        while(sscanf(buffer + pos, "%[^;\n];", tupla[aux-1]) == 1){
            pos += strcspn(buffer + pos, ";") + 1;
            aux++;
            tupla = realloc(tupla, sizeof(char *) * aux * 2);
            tupla[aux-1] = (char *)malloc(sizeof(char) * STRING_MAX_SIZE);
        }
        formatar_dados(tupla, coluna_tam, qtd_colunas);
        free(tupla);
    }   
    free(coluna_nomes);
    free(coluna_tam);
    free(coluna_tipos);
    fclose(arquivo);
    return 0;
}