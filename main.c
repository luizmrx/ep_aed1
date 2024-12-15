#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unac.h>
#include <utf8proc.h>

#define TAMANHO 10000

int numero_total_palavras;
int quantidade = 0;

// Estrutura para o Array de Palavras
typedef struct PalavraArray {
    char *palavra; //Armazena a palavra
    int contagem;   //Armazena a quantidade de vezes que a palavra apareceu
    int *linhas;    //Armazena o número da linha em que a palavra apareceu
    char **linhas_texto;    //Armazena a linha inteira onde a palavra apareceu
    int quantidade_linhas;  //Armazena a quantidade total de linhas que a palavra apareceu
} PalavraArray;

PalavraArray* array_palavras = NULL;
int tamanho_array = 0;//Note que o total de palavras indexadas no array também será o tamanho dele.

int buscar_array(PalavraArray *array, int tamanho, char *palavra) {
    for (int i = 0; i < tamanho; i++) {
        if (strcmp(array[i].palavra, palavra) == 0) {
            return i; // Retorna o índice onde a palavra foi encontrada
        }
    }
    return -1; // Palavra não encontrada
}

void inserir_array(char *palavra, int numero_linha, char *linha_texto, int quantidade) {
    int idx = buscar_array(array_palavras, tamanho_array, palavra);
    
    if (idx == -1) {
        // Palavra não encontrada, adicionar nova palavra ao array
        array_palavras = (PalavraArray *)realloc(array_palavras, sizeof(PalavraArray) * (tamanho_array + 1));
        idx = tamanho_array; //Novas palavras são inseridas no final do array
        array_palavras[idx].palavra = strdup(palavra);
        array_palavras[idx].contagem = 1;
        array_palavras[idx].quantidade_linhas = 1;
        array_palavras[idx].linhas = (int *)malloc(sizeof(int));
        array_palavras[idx].linhas_texto = (char **)malloc(sizeof(char *));
        array_palavras[idx].linhas[0] = numero_linha;
        array_palavras[idx].linhas_texto[0] = strdup(linha_texto);
        tamanho_array++;
    } else{
        // Palavra já existe, atualizar a contagem e linhas
        array_palavras[idx].contagem++;
        //Verifica se a palavra está na mesma linha. Como o sistema percorre linha a linha, desde da primeira até a última, podemos verificar apenas a última linha em que a palavra surgiu
        if(array_palavras[idx].linhas[array_palavras[idx].quantidade_linhas-1]!=numero_linha){
            array_palavras[idx].linhas = (int *)realloc(array_palavras[idx].linhas, sizeof(int) * (array_palavras[idx].quantidade_linhas + 1));
            array_palavras[idx].linhas[array_palavras[idx].quantidade_linhas] = numero_linha;
            array_palavras[idx].quantidade_linhas++;
            array_palavras[idx].linhas_texto = (char **)realloc(array_palavras[idx].linhas_texto, sizeof(char *) * (array_palavras[idx].quantidade_linhas));
            array_palavras[idx].linhas_texto[array_palavras[idx].quantidade_linhas - 1] = strdup(linha_texto);
        } 
            
    }

}

// Estrutura para armazenar palavras e suas ocorrências na árvore AVL
typedef struct NoAVL {
    char *palavra;
    int contagem;
    int *linhas;
    char **linhas_texto; // Linhas inteiras
    char **palavras_linha; // Armazenar todas as palavras da linha
    int quantidade_linhas;
    int quantidade_palavras_linha;
    int altura; // Altura do nó
    struct NoAVL *esquerda, *direita;
} NoAVL;

// Função para obter a altura de um nó
int altura(NoAVL *no) {
    if (no == NULL) return 0;
    return no->altura;
}

// Função para obter o fator de balanceamento
int fator_balanceamento(NoAVL *no) {
    if (no == NULL) return 0;
    return altura(no->esquerda) - altura(no->direita);
}

// Função para realizar rotação à direita
NoAVL* rotacionar_direita(NoAVL *y) {
    NoAVL *x = y->esquerda;
    NoAVL *T2 = x->direita;
    
    // Rotacionando
    x->direita = y;
    y->esquerda = T2;

    // Atualizando alturas
    y->altura = 1 + (altura(y->esquerda) > altura(y->direita) ? altura(y->esquerda) : altura(y->direita));
    x->altura = 1 + (altura(x->esquerda) > altura(x->direita) ? altura(x->esquerda) : altura(x->direita));

    return x;
}

// Função para realizar rotação à esquerda
NoAVL* rotacionar_esquerda(NoAVL *x) {
    NoAVL *y = x->direita;
    NoAVL *T2 = y->esquerda;
    
    // Rotacionando
    y->esquerda = x;
    x->direita = T2;

    // Atualizando alturas
    x->altura = 1 + (altura(x->esquerda) > altura(x->direita) ? altura(x->esquerda) : altura(x->direita));
    y->altura = 1 + (altura(y->esquerda) > altura(y->direita) ? altura(y->esquerda) : altura(y->direita));

    return y;
}

// Função para balancear a árvore
NoAVL* balancear(NoAVL *no) {
    int balanceamento = fator_balanceamento(no);

    // Se o nó estiver desbalanceado para a esquerda
    if (balanceamento > 1) {
        // Se a subárvore esquerda estiver desbalanceada para a direita
        if (fator_balanceamento(no->esquerda) < 0) {
            no->esquerda = rotacionar_esquerda(no->esquerda);
        }
        return rotacionar_direita(no);
    }

    // Se o nó estiver desbalanceado para a direita
    if (balanceamento < -1) {
        // Se a subárvore direita estiver desbalanceada para a esquerda
        if (fator_balanceamento(no->direita) > 0) {
            no->direita = rotacionar_direita(no->direita);
        }
        return rotacionar_esquerda(no);
    }

    return no;
}

// Função para criar um novo nó
NoAVL* criar_no_avl(char *palavra, int numero_linha, char *linha_texto, char **palavras_linha, int quantidade_palavras_linha) {
    NoAVL *novo_no = (NoAVL *)malloc(sizeof(NoAVL));
    novo_no->palavra = strdup(palavra);
    novo_no->contagem = 1;
    novo_no->quantidade_linhas = 1;
    novo_no->quantidade_palavras_linha = quantidade_palavras_linha;
    novo_no->linhas = (int *)malloc(sizeof(int));
    novo_no->linhas_texto = (char **)malloc(sizeof(char *));
    novo_no->palavras_linha = palavras_linha;  // Armazenar todas as palavras da linha
    novo_no->linhas[0] = numero_linha;
    novo_no->linhas_texto[0] = strdup(linha_texto); // Armazenar a linha original
    novo_no->esquerda = novo_no->direita = NULL;
    novo_no->altura = 1;
    return novo_no;
}

// Função para inserir uma palavra na árvore AVL
NoAVL* inserir_avl(NoAVL *no, char *palavra, int numero_linha, char *linha_texto, char **palavras_linha, int quantidade_palavras_linha) {
    // 1. Realizar a inserção normal em uma árvore binária de busca
    if (no == NULL) {
        return criar_no_avl(palavra, numero_linha, linha_texto, palavras_linha, quantidade_palavras_linha);
    }

    if (strcmp(palavra, no->palavra) < 0) {
        no->esquerda = inserir_avl(no->esquerda, palavra, numero_linha, linha_texto, palavras_linha, quantidade_palavras_linha);
    } else if (strcmp(palavra, no->palavra) > 0) {
        no->direita = inserir_avl(no->direita, palavra, numero_linha, linha_texto, palavras_linha, quantidade_palavras_linha);
    } else {
        no->contagem++;
        //Verifica se a palavra está na mesma linha. Como o sistema percorre linha a linha, desde da primeira até a última, podemos verificar apenas a última linha em que a palavra surgiu
        if((no->linhas[no->quantidade_linhas-1])!=numero_linha){
            //printf("%d %d\n", *no->linhas, numero_linha);
            // Se a palavra já existe, aumentar o contador e adicionar a linha
            no->linhas = (int *)realloc(no->linhas, sizeof(int) * (no->quantidade_linhas + 1));
            no->linhas[no->quantidade_linhas] = numero_linha;
            no->quantidade_linhas++;

            // Adicionar a linha de texto
            no->linhas_texto = (char **)realloc(no->linhas_texto, sizeof(char *) * (no->quantidade_linhas));
            no->linhas_texto[no->quantidade_linhas - 1] = strdup(linha_texto);
        }
        return no;
    }

    // 2. Atualizar a altura do nó
    no->altura = 1 + (altura(no->esquerda) > altura(no->direita) ? altura(no->esquerda) : altura(no->direita));

    // 3. Balancear a árvore e retornar o nó balanceado
    return balancear(no);
}

// Função para buscar uma palavra na árvore AVL
NoAVL* buscar_avl(NoAVL *no, char *palavra) {
    if (no == NULL) {
        return NULL;
    }

    if (strcmp(palavra, no->palavra) < 0) {
        return buscar_avl(no->esquerda, palavra);
    } else if (strcmp(palavra, no->palavra) > 0) {
        return buscar_avl(no->direita, palavra);
    } else {
        return no;  // Palavra encontrada
    }
}

// Função para pré-processamento do texto (ignorando pontuação, convertendo para minúsculas e removendo acentos)
void preprocessar_texto(char *texto) {
    char *p = texto;
    while (*p) {
        // Substituir o apóstrofo por um espaço
        if (*p == '\'') {
            *p = ' ';
        }
        // Substitui a pontuação por um espaço
        else if (ispunct(*p)) {
            *p = ' '; 
        } 
        // Converte caracteres para minúscula
        else {
            *p = tolower(*p);
        }
        p++;
    }

    int i = 0, j = 0;
    
    while (texto[i] != '\0') {
        // Verifica se o caractere é alfanumérico (letra ou número)
        if (isalnum(texto[i])) {
            texto[j++] = texto[i];  // Mantém o caractere alfanumérico
        }
        i++;
    }
    
    texto[j] = '\0';  // Adiciona o terminador de string ao final

    // Usar unac_string para remover os acentos
    char *texto_sem_acento = NULL;
    size_t comprimento_saida = 0;
    int resultado = unac_string("UTF-8", texto, strlen(texto), &texto_sem_acento, &comprimento_saida);
    
    if (resultado == 0) {
        // Substituir o texto original pela versão sem acento
        strncpy(texto, texto_sem_acento, comprimento_saida);
        texto[comprimento_saida] = '\0';
    }
    
    free(texto_sem_acento);
}

// Função para processar cada linha do texto e inseri-la na árvore AVL
void preprocessar_texto_e_inserir(char *linha, int numero_linha, NoAVL **arvore_avl, bool array) {
    char *palavra;
    int quantidade_palavras_linha = 0;
    char **palavras_linha = (char **)malloc(sizeof(char *) * TAMANHO);
    char *linha_texto = strdup(linha);

    // Tokenização da linha em palavras
    preprocessar_texto(linha);
    palavra = strtok(linha, " \0");
    while (palavra != NULL) {
        palavras_linha[quantidade_palavras_linha] = strdup(palavra);
        quantidade_palavras_linha++;
        numero_total_palavras++;
        palavra = strtok(NULL, " \0");
    }

    if(!array){
        // Inserir cada palavra na árvore AVL
        for (int i = 0; i < quantidade_palavras_linha; i++) {
            // Preprocessar palavra para inserção
            preprocessar_texto(palavras_linha[i]);
            *arvore_avl = inserir_avl(*arvore_avl, palavras_linha[i], numero_linha, linha_texto, palavras_linha, quantidade_palavras_linha);
        }
    }else{
        // Inserir cada palavra no array
        for (int i = 0; i < quantidade_palavras_linha; i++) {
            // Preprocessar palavra para inserção
            preprocessar_texto(palavras_linha[i]);
            inserir_array(palavras_linha[i], numero_linha, linha_texto, quantidade_palavras_linha);
        }
    }
    

    // Liberar memória alocada para palavras
    for (int i = 0; i < quantidade_palavras_linha; i++) {
        free(palavras_linha[i]);
    }
    free(palavras_linha);
}

//Função para determinar o total de nós da árvore
int no_arvore(NoAVL* no){

    if(no!=NULL){
        no_arvore(no->esquerda);
        quantidade++;
        no_arvore(no->direita);
    }
    return 0;

}

int visualizar_arvore(NoAVL* no){

    if(no!=NULL){
        visualizar_arvore(no->esquerda);
        printf("%s\n", no->palavra);
        visualizar_arvore(no->direita);
    }
    return 0;
}

// Função principal
int main(int argc, char *argv[]) {
    FILE *entrada;
    char *linha;
    int numero_linha = 0;
    int tamanho_maximo_linha = TAMANHO;
    clock_t inicio, fim;
    clock_t inicio_busca, fim_busca;
    double tempo_carga;
    double tempo_busca;
    NoAVL *arvore_avl = NULL;
    numero_total_palavras=0;

    // Verificar número de argumentos
    if (argc < 3) {
        printf("Erro!!! A entrada deve ser composta por: ./main <arquivo.txt> <arvore|lista>\n");
        return 1;
    }
    entrada = fopen(argv[1], "r");
    if (entrada == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    //Captura o nome do arquivo txt
    char* nome_arquivo_txt = argv[1];
    //Captura a escolha de busca
    char* tipo_de_busca = argv[2];

    //Verifica que a escolha de busca é uma das opções válidas
    if ((strcmp(argv[2], "arvore") != 0)&&(strcmp(argv[2], "lista") != 0)){
        printf("Busca incorreta. Escolha entre arvore ou lista.\n");
        return 1;
    }

    // Aloca memória para a linha
    linha = (char *)malloc(tamanho_maximo_linha * sizeof(char));

    // Início do tempo de carga
    inicio = clock();

    // Processar o arquivo linha por linha
    while (fgets(linha, tamanho_maximo_linha, entrada)) {
        numero_linha++;
        if(strcmp(argv[2], "arvore") == 0)preprocessar_texto_e_inserir(linha, numero_linha, &arvore_avl, false);
        else if(strcmp(argv[2], "lista") == 0)preprocessar_texto_e_inserir(linha, numero_linha, &arvore_avl, true);
    }

    // Fim do tempo de carga
    fim = clock();
    tempo_carga = ((double)(fim - inicio)) / CLOCKS_PER_SEC * 1000;  // Tempo em milissegundos

    fclose(entrada);

    //Informações iniciais na tela
    printf("Arquivo: '%s'\n", nome_arquivo_txt);
    printf("Tipo de indice: '%s'\n", tipo_de_busca);
    printf("Numero de linhas no arquivo: %d\n", numero_linha);
    printf("Total de palavras indexadas: ");
    if(strcmp(argv[2], "lista") == 0) printf("%d \n", tamanho_array);
    if(strcmp(argv[2], "arvore") == 0){
        no_arvore(arvore_avl);
        printf("%d \n", quantidade);
        printf("Altura da arvore: %d\n", altura(arvore_avl));
    } 
    printf("Tempo de carga do arquivo e construcao do indice: %.2f ms\n", tempo_carga);

    // Menu de busca
    char palavra_busca[100];
    char palavra_busca1[50];
    char palavra_busca2[50];
    while (1) {
        
        printf("\n> ");
        fgets(palavra_busca, sizeof(palavra_busca), stdin);
        palavra_busca[strcspn(palavra_busca, "\n")] = '\0';  // Remover o '\n' no final
        sscanf(palavra_busca, "%s %s", palavra_busca1, palavra_busca2);

        //Verifica se o usuario quer sair antes de analisar o arquivo
        if (strcmp(palavra_busca1, "fim") == 0) {
            break;
        }

        // Verifica se o comando esta correto e, em caso positivo, procura a palavra de acordo com o metodo escolhido
        if (strcmp(palavra_busca1, "busca") != 0)
        {
            printf("Opcao invalida!\n");
        }else if (strcmp(palavra_busca1, "busca") == 0)
        {
            preprocessar_texto(palavra_busca2);
            if (strcmp(argv[2], "arvore") == 0) {
                //Inicio do tempo de busca
                inicio_busca=clock();

                NoAVL *resultado = buscar_avl(arvore_avl, palavra_busca2);
                if (resultado != NULL) {
                    printf("Existem %d ocorrências da palavra '%s' na(s) seguinte(s) linha(s):\n", resultado->contagem, resultado->palavra);
                    for (int i = 0; i < resultado->quantidade_linhas; i++) {
                        printf("%05d: %s", resultado->linhas[i], resultado->linhas_texto[i]);
                    }
                    //Função utilizada para vericar todos os nós da árvore de maneira ordenada. Caso queira visualizá-la, apenas descomente a seguinte função.
                    //visualizar_arvore(arvore_avl);
                    

                } else {
                    printf("Palavra '%s' não encontrada.\n", palavra_busca2);
                }

                //Fim do tempo de busca
                fim_busca=clock();
                tempo_busca = ((double)(fim_busca - inicio_busca)) / CLOCKS_PER_SEC * 1000;  // Tempo em milissegundos

                printf("\nTempo de busca: %.2f ms\n", tempo_busca);

            } else if (strcmp(argv[2], "lista") == 0) {

                //Inicio do tempo de busca
                inicio_busca=clock();

                int idx = buscar_array(array_palavras, tamanho_array, palavra_busca2);
                if (idx != -1) {
                    // Palavra encontrada no array
                    printf("Existem %d ocorrências da palavra '%s' na(s) seguinte(s) linha(s): \n", array_palavras[idx].contagem, array_palavras[idx].palavra);
                    for (int i = 0; i < array_palavras[idx].quantidade_linhas; i++) {
                        printf("%05d: %s", array_palavras[idx].linhas[i], array_palavras[idx].linhas_texto[i]);
                    }

                    //Usamos essa parte para verificar a formação do array completo. Caso também queira ver como o array está organizado, descomente essa parte do código e faça uma busca de uma palavra que está presente no array.
                    /*
                    for (int i = 0; i < tamanho_array; i++)
                    {
                        printf("%s\n", array_palavras[i].palavra);
                    }
                    */
                    
                } else {
                    printf("Palavra '%s' não encontrada.\n", palavra_busca2);
                }

                //Fim do tempo de busca
                fim_busca=clock();
                tempo_busca = ((double)(fim_busca - inicio_busca)) / CLOCKS_PER_SEC * 1000;  // Tempo em milissegundos

                printf("\nTempo de busca: %.2f ms\n", tempo_busca);
            }
        }
        
        
        
    }

    return 0;
}
