#include "jogo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TAM 64
int mapa[8][20] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
    {1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};
bool dentro_area(int x, int y) {

    int tile_x = x / TAM;
    int tile_y = y / TAM;

    if (tile_x < 0 || tile_x >= 20 || tile_y < 0 || tile_y >= 8)
        return false;

    if (mapa[tile_y][tile_x] == 1)
        return false;

    return true;
}
void ordenar_inventario(char inventario[][30], int tamanho) {

    for (int i = 0; i < tamanho - 1; i++) {

        for (int j = 0; j < tamanho - 1 - i; j++) {

            if (strcmp(inventario[j], inventario[j + 1]) > 0) {

                char temp[30];

                strcpy_s(temp, sizeof(temp), inventario[j]);
                strcpy_s(inventario[j], 30, inventario[j + 1]);
                strcpy_s(inventario[j + 1], 30, temp);
            }
        }
    }
}
int buscar_npc_mais_proximo(Personagem npcs[],
    int tamanho,
    Personagem player)
{
    int indice = -1;
    int menor_distancia = 999999;

    for (int i = 0; i < tamanho; i++) {

        int distancia =
            abs(player.pos.x - npcs[i].pos.x) +
            abs(player.pos.y - npcs[i].pos.y);

        if (distancia < menor_distancia) {

            menor_distancia = distancia;
            indice = i;
        }
    }

    return indice;
}
void adicionar_item(
    char (**inventario)[30],
    int* total_itens,
    char novo_item[])
{
    (*total_itens)++;

    *inventario = realloc(
        *inventario,
        (*total_itens) * sizeof(**inventario)
    );

    if (*inventario == NULL) {
        printf("Erro ao expandir inventario\n");
        exit(1);
    }

    strcpy_s(
        (*inventario)[*total_itens - 1],
        30,
        novo_item
    );
}