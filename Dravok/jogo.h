#ifndef JOGO_H
#define JOGO_H
#include<stdbool.h>
typedef enum {
    ESQUERDA,
    DIREITA
} Direcao;
typedef struct {
    int x;
    int y;
} Posicao;
typedef struct {
    Posicao pos;
    int frame;
    int vida;
    int dano;
    bool vivo;
    bool atacando;
    char nome[50];
} Personagem;
<<<<<<< HEAD
typedef struct {

    char falas[100][200];

    int inicio;

    int fim;

} FilaDialogo;
=======
>>>>>>> 47100fc39b37891aa7217314a180c1a49c088e7f
bool dentro_area(int x, int y);
void ordenar_inventario(char inventario[][30], int tamanho);
int buscar_npc_mais_proximo(
    Personagem npcs[],
    int tamanho,
    Personagem player
);
void adicionar_item(
    char (**inventario)[30],
    int* total_itens,
    char novo_item[]
);
<<<<<<< HEAD

void enfileirar(FilaDialogo* fila, char texto[]);
void desenfileirar(FilaDialogo* fila);
char* fala_atual(FilaDialogo* fila);

=======
>>>>>>> 47100fc39b37891aa7217314a180c1a49c088e7f
#endif