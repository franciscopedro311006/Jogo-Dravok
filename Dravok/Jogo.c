#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

typedef enum {
    CIMA,
    BAIXO,
    ESQUERDA,
    DIREITA
} Direcao;

#define TAM 64

int mapa[8][20] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
    {1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};


bool dentro_area(int x, int y) {
    int player_w = 96;
    int player_h = 96;

    int tile_x = x / TAM;
    int tile_y = y / TAM;

    if (tile_x < 0 || tile_x >= 20 || tile_y < 0 || tile_y >= 8)
        return false;

    if (mapa[tile_y][tile_x] == 1)
        return false;

    return true;
}

typedef struct {
    int x;
    int y;
} Posicao;

typedef struct {
    Posicao pos;
    int frame;
    int linha;
    int vida;
    int dano;
    bool vivo;
    bool atacando;
} Personagem;
void ordenar_npcs(Personagem npcs[], int tamanho){

    for(int i = 0; i < tamanho - 1; i++){

        for(int j = 0; j < tamanho - 1 - i; j++){

            if(npcs[j].pos.x > npcs[j+1].pos.x){

                Personagem temp = npcs[j];
                npcs[j] = npcs[j+1];
                npcs[j+1] = temp;
            }
        }
    }
}
typedef struct {
    int x;
    int y;
    int largura;
    int altura;
} Hitbox;

Personagem npcs[2];

int main() {

  

    al_init();
    al_init_primitives_addon();
    al_install_keyboard();
    al_init_font_addon();
    al_init_image_addon();

    ALLEGRO_FONT* font = al_create_builtin_font();
    ALLEGRO_DISPLAY* display = al_create_display(1280, 720);

    Personagem player = { {100, 300}, 0, 0 };
    player.vida = 100;
    player.dano = 20;
    player.vivo = true;
    player.atacando = false;

    int frame_delay = 0;

    int npc_x = 800, npc_y = 160;

    Personagem skeleton = { {600,300}, 0, 0 };
    skeleton.vida = 60;
    skeleton.dano = 10;
    skeleton.vivo = true;
    skeleton.atacando = false;

    npcs[0].pos.x = 800;
    npcs[0].pos.y = 160;

    npcs[1].pos.x = 400;
    npcs[1].pos.y = 200;

    int* numeros = malloc(10 * sizeof(int));
    for (int i = 0; i < 10; i++) numeros[i] = i;
    free(numeros);

    Personagem* npc_ptr = malloc(sizeof(Personagem));
    npc_ptr->pos.x = 500;
    npc_ptr->pos.y = 200;
    free(npc_ptr);

    //troquei o vetor de char enorme com o dialogo por um arquivo txt com o dialogo nele;
    char falas[100] [200];
    FILE* file = fopen("dialogo.txt", "r");
    int total_falas = 0;

    if (file == NULL) {
        printf("Erro ao abrir dialogo.txt");
        return 1;
    }

    while (fgets(falas[total_falas], 200, file) != NULL) {

        falas[total_falas][strcspn(falas[total_falas], "\n")] = 0;

        total_falas++;
    }

    fclose(file);

   
    int fala_atual = 0;

    int npc_frame = 0;
    int npc_delay = 0;
    int npc_linha = 0;

    bool em_dialogo = false;
    bool npc_ativo = true;

    ALLEGRO_BITMAP* player_bitmap = al_load_bitmap("Personagem.png");
    ALLEGRO_BITMAP* velho = al_load_bitmap("Velho.png");
    ALLEGRO_BITMAP* vila = al_load_bitmap("Casa.jpeg");
    ALLEGRO_BITMAP* esqueleto_bitmap = al_load_bitmap("Esqueleto.png");
    

    while (1) {

        ALLEGRO_KEYBOARD_STATE estado;
        al_get_keyboard_state(&estado);

        Direcao direcao; 

        bool andando = false;

        Hitbox hit_player = {player.pos.x + 50, player.pos.y + 25, 35, 65};

        int novo_x = player.pos.x;
        int novo_y = player.pos.y;

        if (!em_dialogo) {
            if (al_key_down(&estado, ALLEGRO_KEY_W)) { novo_y -= 5; andando = true; player.linha = 2; direcao = CIMA; }
            if (al_key_down(&estado, ALLEGRO_KEY_S)) { novo_y += 5; andando = true; player.linha = 0; direcao = BAIXO; }
            if (al_key_down(&estado, ALLEGRO_KEY_A)) { novo_x -= 5; andando = true; player.linha = 1; direcao = ESQUERDA; }
            if (al_key_down(&estado, ALLEGRO_KEY_D)) { novo_x += 5; andando = true; player.linha = 3; direcao = DIREITA; }
        }

        if (dentro_area(novo_x, player.pos.y)) {
            player.pos.x = novo_x;
        }

        if (dentro_area(player.pos.x, novo_y)) {
            player.pos.y = novo_y;
        }
        static bool ataque_antes = false;

if (al_key_down(&estado, ALLEGRO_KEY_SPACE)) {

    if (!ataque_antes) {

        player.atacando = true;

        if (skeleton.vivo &&
            abs(player.pos.x - skeleton.pos.x) < 80 &&
            abs(player.pos.y - skeleton.pos.y) < 80) {

            skeleton.vida -= player.dano;

            printf("Esqueleto tomou dano! Vida: %d\n",
                   skeleton.vida);

            if (skeleton.vida <= 0) {

                skeleton.vivo = false;
                skeleton.vida = 60;

                printf("Esqueleto derrotado!\n");
            }
        }
    }

    ataque_antes = true;
}
else {

    ataque_antes = false;
    player.atacando = false;
}
        static bool cura_antes = false;

if (al_key_down(&estado, ALLEGRO_KEY_Q)) {

    if (!cura_antes) {

        player.vida += 20;

        if (player.vida > 100)
            player.vida = 100;

        printf("Curado! Vida: %d\n",
               player.vida);
    }

    cura_antes = true;
}
else {

    cura_antes = false;
}

        npc_delay++;
        if (npc_delay > 30) {
            npc_frame++;
            npc_delay = 0;
            if (npc_frame > 3) npc_frame = 0;
        }

        if (andando) {
            frame_delay++;
            if (frame_delay > 15) {
                player.frame++;
                frame_delay = 0;
                if (player.frame > 3) player.frame = 0;
            }
        }
        else {
            player.frame = 0;
        }

        if (al_key_down(&estado, ALLEGRO_KEY_ESCAPE)) break;

        bool perto = false;

        if (npc_ativo && abs(player.pos.x - npc_x) < 40 && abs(player.pos.y - npc_y) < 40) {
            perto = true;
        }

        static bool e_antes = false;

        if (perto && al_key_down(&estado, ALLEGRO_KEY_E)) {
            if (!e_antes) em_dialogo = true;
            e_antes = true;
        }
        else {
            e_antes = false;
        }

        static bool enter_antes = false;

        if (em_dialogo) {
            if (al_key_down(&estado, ALLEGRO_KEY_ENTER)) {
                if (!enter_antes) {
                    fala_atual++;
                    if (fala_atual >= total_falas) {
                        em_dialogo = false;
                        fala_atual = 0;
                        skeleton.vivo = true;
                        //npc_ativo = false;
                    }
                }
                enter_antes = true;
            }
            else {
                enter_antes = false;
            }
        }
        if (skeleton.vivo) {

    if (player.pos.x < skeleton.pos.x)
        skeleton.pos.x -= 2;

    if (player.pos.x > skeleton.pos.x)
        skeleton.pos.x += 2;

    if (player.pos.y < skeleton.pos.y)
        skeleton.pos.y -= 2;

    if (player.pos.y > skeleton.pos.y)
        skeleton.pos.y += 2;
}
   static int cooldown_dano = 0;

if (cooldown_dano > 0)
    cooldown_dano--;

if (skeleton.vivo &&
    abs(player.pos.x - skeleton.pos.x) < 50 &&
    abs(player.pos.y - skeleton.pos.y) < 50 &&
    cooldown_dano <= 0) {

    player.vida -= skeleton.dano;

    cooldown_dano = 60;

    printf("Player tomou dano! Vida: %d\n",
           player.vida);

    if (player.vida <= 0) {

        printf("GAME OVER\n");

        break;
    }
}
        al_clear_to_color(al_map_rgb(255, 255, 255));

        al_draw_scaled_bitmap(vila, 0, 0, 1536, 1024, 0, 0, 1280, 720, 0);

        int frame_w = 256;
        int frame_h = 256;

        al_draw_scaled_bitmap(player_bitmap, player.frame * frame_w, player.linha * frame_h, frame_w, frame_h, player.pos.x, player.pos.y, 96, 96, 0);

        al_draw_rectangle(hit_player.x, hit_player.y, hit_player.x + hit_player.largura, hit_player.y + hit_player.altura, al_map_rgb(255, 0, 0), 2);

        int npc_w = 256;
        int npc_h = 341;

        al_draw_scaled_bitmap(velho, npc_frame * npc_w, npc_linha * npc_h, npc_w, npc_h, npc_x, npc_y, 80, 80, 0);
        if (skeleton.vivo) {

    al_draw_scaled_bitmap(
        esqueleto_bitmap,
        0,
        0,
        256,
        256,
        skeleton.pos.x,
        skeleton.pos.y,
        96,
        96,
        0
    );
}

        if (perto) {
            al_draw_text(font, al_map_rgb(255, 255, 255), 100, 30, 0, "Pressione E");
        }

        if (em_dialogo) {
            al_draw_filled_rectangle(50, 450, 750, 580, al_map_rgb(0, 0, 0));
            al_draw_text(font, al_map_rgb(255, 255, 255), 60, 460, 0, falas[fala_atual]);
        }
        
        al_draw_filled_rectangle(
    20,
    20,
    20 + (player.vida * 2),
    40,
    al_map_rgb(255, 0, 0)
);

al_draw_rectangle(
    20,
    20,
    220,
    40,
    al_map_rgb(255,255,255),
    2
);

        al_flip_display();
        al_rest(0.016);
    } 
    
al_destroy_bitmap(player_bitmap);
al_destroy_bitmap(velho);
al_destroy_bitmap(vila);
al_destroy_bitmap(esqueleto_bitmap);

al_destroy_font(font);
    
    al_destroy_display(display);
    return 0;
}
