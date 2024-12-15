#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unac.h>
#include <utf8proc.h>

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
        strncpy(texto, texto_sem_acento, comprimento_saida);
        texto[comprimento_saida] = '\0';
    }
    
    free(texto_sem_acento);
}

int main(){

    char texto[100];

    // Solicitar ao usuário que digite uma string
    printf("Digite um texto: ");
    fgets(texto, sizeof(texto), stdin);

    // Chama a função para preprocessar o texto
    preprocessar_texto(texto);

    // Exibir o texto modificado
    printf("Texto processado: %s\n", texto);

    return 0;
}