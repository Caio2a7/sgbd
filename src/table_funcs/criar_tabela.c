/*
=========================================================================================================
Arquivo: criar_tabela.c
Data modificação: 13 de dezembro de 2023
Descrição: O objetivo desse código é criar e formatar corretamente o banco de dados requerido pelo usuário.
           Requisita o nome do banco, tipos das colunas e o nome de cada uma, formatando duas linhas no arquivo para tal.
           A fucionalidade de adicionar dados no banco é realizado pelo código criar_linhas.c
=========================================================================================================
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include "../../includes/utils.h"
#include "../../includes/utils/string_utils.h"
#include "../../includes/utils/file_utils.h"

/*--------------------------------------------------------------------------------
Função: salva todos os dados escaneados das colunas no arquivo do banco de dados.
Paramêtros: 
    -Ponteiro para a variável heterogênea onde armazenamos as colunas do banco de dados;
    -Quantidade de colunas;
*/
int salvar_coluna(Tabela *banco_de_dados, int colunas_qtd) {   
    FILE *arquivo;
    arquivo = abrir_arquivo(banco_de_dados->tabela_nome, 'w'); 
    if (arquivo == NULL) return -1; 
    
    for(int i = 0; i < colunas_qtd; i++){
        //Escrevemos a primeira linha do banco, a qual será dos tipos das colunas
        fprintf(arquivo, "%d;", banco_de_dados->colunas[i].coluna_tipo); 
    }
    fprintf(arquivo, "\n");
    for(int i = 0; i < colunas_qtd; i++){
        //Escrevemos a segunda linha dos títulos das coluna
        fprintf(arquivo, "%s;", banco_de_dados->colunas[i].coluna_nome); 
    }

    fclose(arquivo);
    return 0;
}
/*--------------------------------------------------------------------------------
Função: define o tipo da coluna.
Paramêtros: 
    -Tipo da coluna que o usuário requisitou;
    -Ponteiro para a variável heterogênea onde armazenamos os dados do banco de dados;
    -Índice da coluna;
*/
void definir_tipo_coluna(int enum_type, Tabela *banco_de_dados, int aux){
    switch(enum_type){
        case INT:
            banco_de_dados->colunas[aux].coluna_tipo = INT;
            break;
        case FLOAT:
            banco_de_dados->colunas[aux].coluna_tipo = FLOAT;
            break;
        case DOUBLE:
            banco_de_dados->colunas[aux].coluna_tipo = DOUBLE;
            break;
        case CHAR:
            banco_de_dados->colunas[aux].coluna_tipo = CHAR;
            break;
        case STRING:
            banco_de_dados->colunas[aux].coluna_tipo = STRING;
            break;
        default:
            printf("~Digite um valor válido\n");
    }
}

/*--------------------------------------------------------------------------------
Função: criar o banco de dados e recolhe os requisitos do usuário para o banco. 
*/
int criar_banco_de_dados() {
    //Declaro um ponteiro para a variável do tipo tabela que representa o banco de dados
    //nessa variável que alocaremos o título e tipo das colunas para depois salvar no banco de dados real arquivo
    Tabela *banco_de_dados = (Tabela *)malloc(sizeof(Tabela));
    
    printf("$Digite o nome do banco de dados: ");
    fgets(banco_de_dados->tabela_nome, STRING_MAX_SIZE, stdin);
    remover_quebra_linha(banco_de_dados->tabela_nome);
    
    //Fazemos uma verificação se o nome do banco a ser criado já existe ou não
    //Usa a biblioteca dirent.c para tratamento de diretórios e arquivos 
    DIR *dir;
    struct dirent *entrada;
    int tabela_existe = 0;

    dir = opendir(DB_PATH);
    if(dir == NULL){
        perror("Erro ao abrir o diretório.");
        return -1;
    }
    
    char database_filename[STRING_MAX_SIZE];
    sprintf(database_filename, "%s.txt", banco_de_dados->tabela_nome);

    while ((entrada = readdir(dir)) != NULL){
        if(strcmp(entrada->d_name, database_filename) == 0){
            printf("O banco de dados já existe, digite outro nome.\n");
            return -1;
        }
    }
    //Fim da verificação

    //Aqui faremos a alocação para a quantidade de colunas inicial do banco de dados
    int aux = 1; //usaremos de índice para as colunas
    banco_de_dados->colunas = (Coluna *)malloc(sizeof(Coluna) * 2 * aux);
    
    //Alocando o título para a CHAVE PRIMÁRIA, coluna obrigatória
    char key_input[STRING_MAX_SIZE];
    printf("Nome da coluna de chave primária: ");
    fgets(key_input, STRING_MAX_SIZE, stdin);
    int tamanho_string = strlen(key_input);
    remover_quebra_linha(key_input);

    banco_de_dados->colunas[aux - 1].coluna_tipo = INT; //definindo a chave primário como tipo INT
    banco_de_dados->colunas[aux - 1].coluna_nome = (char *)malloc(sizeof(char) * (tamanho_string + 1));//alocando memória para o titulo
    strcpy(banco_de_dados->colunas[aux - 1].coluna_nome, key_input); //setando o título da coluna no banco

    //Aqui faremos a alocação para o restante das colunas, com sua quantidade sendo delimitada pelo 'fim'
    //digitado pelo usuário
    do {
        printf("Nome da %dº coluna(digite 'fim' para finalizar): ", aux);
        //Formatação da string
        char input[STRING_MAX_SIZE];
        fgets(input, STRING_MAX_SIZE, stdin);
        upper_string(input);
        remover_quebra_linha(input);
        tamanho_string = strlen(input);
        if (strcmp(input, "FIM") == 0) {
            break;
        }
        //Alocando memória para o titulo
        banco_de_dados->colunas[aux].coluna_nome = (char *)malloc(sizeof(char) * (tamanho_string + 1));
        if (banco_de_dados->colunas[aux].coluna_nome == NULL) {
            fprintf(stderr, "Memory allocation for coluna_nome failed\n");
            exit(EXIT_FAILURE);
        }
        strcpy(banco_de_dados->colunas[aux].coluna_nome, input);

        //Agora definiremos o tipo da coluna
        printf("$Tipo da %dº coluna(int, float, double, char, str): ", aux);
        char tipo_entrada[STRING_MAX_SIZE];
        if (fgets(tipo_entrada, sizeof(tipo_entrada), stdin) == NULL) {
            fprintf(stderr, "Erro ao ler a entrada\n");
            exit(EXIT_FAILURE);
        }

        upper_string(tipo_entrada);
        remover_quebra_linha(tipo_entrada);

        //Lógica para interpretar a string digitada pelo usuário para determinar o seu tipo
        //Para saber o que cada número indica vá em utils e olhe a estrutura Colunatipos
        int enum_type = (strcmp(tipo_entrada, "INT") == 0) ? 0 :
                (strcmp(tipo_entrada, "FLOAT") == 0) ? 1 :
                (strcmp(tipo_entrada, "DOUBLE") == 0) ? 2 :
                (strcmp(tipo_entrada, "CHAR") == 0) ? 3 :
                (strcmp(tipo_entrada, "STR") == 0) ? 4 : -1;

        definir_tipo_coluna(enum_type, banco_de_dados, aux);  

        //Aqui aumentamos a quantidade de colunas +1 para dar prosseguimento ao loop
        aux++;
        banco_de_dados->colunas = realloc(banco_de_dados->colunas, sizeof(Coluna) * (aux + 1));
        if (banco_de_dados->colunas == NULL) {
            fprintf(stderr, "Memory reallocation failed\n");
            exit(EXIT_FAILURE);
        }
    } while (1);
    //Salvando os dados no arquivo do banco de dados
    salvar_coluna(banco_de_dados, aux);
    printf("TABELA CRIADA.\n");
    for (int i = 0; i < aux; i++) {
        free(banco_de_dados->colunas[i].coluna_nome);
    }
    free(banco_de_dados->colunas);
    free(banco_de_dados);
}
