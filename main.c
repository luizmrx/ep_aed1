#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unac.h>
#include <utf8proc.h>
#define TAMANHO 10000

// Estrutura para armazenar palavras e suas ocorrências na árvore AVL
typedef struct NoAVL {
    char *palavra;
    int contagem;
    int *linhas;
    char **linhas_texto; // Armazenar as linhas inteiras
    int quantidade_linhas;
    int altura; // Altura do nó
    struct NoAVL *esquerda, *direita;
} NoAVL;

// Funções auxiliares para a árvore AVL

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
NoAVL* criar_no_avl(char *palavra, int numero_linha, char *linha_texto) {
    NoAVL *novo_no = (NoAVL *)malloc(sizeof(NoAVL));
    novo_no->palavra = strdup(palavra);
    novo_no->contagem = 1;
    novo_no->quantidade_linhas = 1;
    novo_no->linhas = (int *)malloc(sizeof(int));
    novo_no->linhas_texto = (char **)malloc(sizeof(char *));
    novo_no->linhas[0] = numero_linha;
    novo_no->linhas_texto[0] = strdup(linha_texto);
    novo_no->esquerda = novo_no->direita = NULL;
    novo_no->altura = 1;
    return novo_no;
}

// Função para inserir uma palavra na árvore AVL
NoAVL* inserir_avl(NoAVL *no, char *palavra, int numero_linha, char *linha_texto) {
    // 1. Realizar a inserção normal em uma árvore binária de busca
    if (no == NULL) {
        return criar_no_avl(palavra, numero_linha, linha_texto);
    }

    if (strcmp(palavra, no->palavra) < 0) {
        no->esquerda = inserir_avl(no->esquerda, palavra, numero_linha, linha_texto);
    } else if (strcmp(palavra, no->palavra) > 0) {
        no->direita = inserir_avl(no->direita, palavra, numero_linha, linha_texto);
    } else {
        // Se a palavra já existe, aumentar o contador e adicionar a linha
        no->contagem++;
        no->linhas = (int *)realloc(no->linhas, sizeof(int) * (no->quantidade_linhas + 1));
        no->linhas[no->quantidade_linhas] = numero_linha;
        no->quantidade_linhas++;

        // Adicionar a linha de texto
        no->linhas_texto = (char **)realloc(no->linhas_texto, sizeof(char *) * (no->quantidade_linhas));
        no->linhas_texto[no->quantidade_linhas - 1] = strdup(linha_texto);
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
        if (ispunct(*p)) {
            *p = ' '; // Substitui pontuação por espaço
        } else {
            *p = tolower(*p); // Converte para minúscula
        }
        p++;
    }

    // Usar unac_string para remover os acentos
    char *texto_sem_acento = NULL;
    size_t comprimento_saida = 0;
    int resultado = unac_string("UTF-8", texto, strlen(texto), &texto_sem_acento, &comprimento_saida);
    
    if (resultado == 0) {
        // Substituir o texto original pela versão sem acento
        strcpy(texto, texto_sem_acento);
        free(texto_sem_acento);  // Liberar memória alocada
    } else {
        printf("Erro ao remover acentos.\n");
    }
}

// Função principal
int main(int argc, char *argv[]) {
    FILE *entrada;
    char *linha;
    char *copia_ponteiro_linha;
    int numero_linha = 0;
    int tamanho_maximo_linha = TAMANHO;
    clock_t inicio, fim;
    double tempo_carga;
    NoAVL *arvore_avl = NULL;

    if (argc < 2) {
        printf("Por favor, forneça um arquivo de entrada.\n");
        return 1;
    }

    entrada = fopen(argv[1], "r");
    if (entrada == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    // Aloca memória para a linha
    linha = (char *)malloc(tamanho_maximo_linha * sizeof(char));

    // Início do tempo de carga
    inicio = clock();

    while (fgets(linha, tamanho_maximo_linha, entrada)) {
        numero_linha++;
        copia_ponteiro_linha = strdup(linha);
        preprocessar_texto(copia_ponteiro_linha);

        char *palavra = strtok(copia_ponteiro_linha, " ");
        while (palavra != NULL) {
            arvore_avl = inserir_avl(arvore_avl, palavra, numero_linha, linha);
            palavra = strtok(NULL, " ");
        }

        free(copia_ponteiro_linha);
    }

    // Fim do tempo de carga
    fim = clock();
    tempo_carga = ((double)(fim - inicio)) / CLOCKS_PER_SEC * 1000;  // Tempo em milissegundos

    fclose(entrada);

    // Exibir as informações da árvore AVL
    printf("Tempo de carga: %.2f ms\n", tempo_carga);

    // Menu de busca
    char palavra_busca[100];
    while (1) {
        printf("\nDigite uma palavra para buscar (ou 'sair' para encerrar): ");
        fgets(palavra_busca, sizeof(palavra_busca), stdin);
        palavra_busca[strcspn(palavra_busca, "\n")] = '\0';  // Remover o '\n' no final

        if (strcmp(palavra_busca, "sair") == 0) {
            break;
        }

        preprocessar_texto(palavra_busca);
        NoAVL *resultado = buscar_avl(arvore_avl, palavra_busca);

        if (resultado != NULL) {
            printf("Palavra: %s\n", resultado->palavra);
            printf("Ocorrências: %d\n", resultado->contagem);
            printf("Linhas onde aparece:\n");
            for (int i = 0; i < resultado->quantidade_linhas; i++) {
                printf("Linha %d: %s", resultado->linhas[i], resultado->linhas_texto[i]);
            }
        } else {
            printf("Palavra não encontrada.\n");
        }
    }

    return 0;
}