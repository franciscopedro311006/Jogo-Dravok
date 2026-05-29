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
    int vida;
    int dano;
    bool vivo;
    bool atacando;
    char nome[50];
} Personagem;

void ordenar_npcs(Personagem npcs[], int tamanho) {

    for (int i = 0; i < tamanho - 1; i++) {

        for (int j = 0; j < tamanho - 1 - i; j++) {

            if (npcs[j].pos.x > npcs[j + 1].pos.x) {

                Personagem temp = npcs[j];
                npcs[j] = npcs[j + 1];
                npcs[j + 1] = temp;
            }
        }
    }
}

int buscar_npc_por_x(Personagem npcs[], int tamanho, int x) {

    for (int i = 0; i < tamanho; i++) {

        if (npcs[i].pos.x == x) {

            return i;
        }
    }

    return -1;
}



int main() {

    al_init();
    al_init_primitives_addon();
    al_install_keyboard();
    al_init_font_addon();
    al_init_image_addon();

    ALLEGRO_FONT* font = al_create_builtin_font();
    ALLEGRO_DISPLAY* display = al_create_display(1280, 720);

    Personagem* npcs = malloc(1 * sizeof(Personagem));

    if (npcs == NULL) {
        printf("Erro de memoria\n");
        return 1;
    }

    Personagem player = { {100, 300}, 0, 0 };
    player.vida = 100;
    player.dano = 20;
    player.vivo = true;
    player.atacando = false;

    int frame_delay = 0;
    int npc_x, npc_y ;

    npcs[0].pos.x = 800;
    npcs[0].pos.y = 160;

    strcpy_s(npcs[0].nome, sizeof(npcs[0].nome), "Velho");

    Personagem skeleton = { {600,300}, 0, 0 };
    skeleton.vida = 60;
    skeleton.dano = 10;
    skeleton.vivo = false;
    skeleton.atacando = false;

    ordenar_npcs(npcs, 1);

    npc_x = npcs[0].pos.x;
    npc_y = npcs[0].pos.y;

    //troquei o vetor de char enorme com o dialogo por um arquivo txt com o dialogo nele;
    char falas[100][200];
    FILE* file = fopen("dialogo.txt", "r");
    int total_falas = 0;

    if (file == NULL) {
        printf("Erro ao abrir dialogo.txt");
        return 1;
    }

    //Apaga o \n do final das frases e coloca \0
    while (fgets(falas[total_falas], 200, file) != NULL) {

        falas[total_falas][strcspn(falas[total_falas], "\n")] = 0;

        total_falas++;
    }

    fclose(file);


    int fala_atual = 0;

    int npc_frame = 0;

    bool em_dialogo = true;
    bool intro = true;

    bool npc_ativo = true;

    ALLEGRO_BITMAP* velho = al_load_bitmap("Velho.png");
    ALLEGRO_BITMAP* vila = al_load_bitmap("Casa.jpeg");

    ALLEGRO_BITMAP* player_parado = al_load_bitmap("PersonagemParado.png");
    ALLEGRO_BITMAP* player_andando = al_load_bitmap("PersonagemAndando.png");
    ALLEGRO_BITMAP* player_atacando = al_load_bitmap("PersonagemAtacando.png");

    ALLEGRO_BITMAP* skeleton_parado = al_load_bitmap("EsqueletoParado.png");
    ALLEGRO_BITMAP* skeleton_andando = al_load_bitmap("EsqueletoAndando.png");
    ALLEGRO_BITMAP* skeleton_atacando = al_load_bitmap("EsqueletoAtacando.png");

    if (!player_parado || !player_andando || !player_atacando) {
        printf("Erro ao carregar sprites do player\n");
        return 1;
    }

    if (!skeleton_parado || !skeleton_andando || !skeleton_atacando) {
        printf("Erro ao carregar sprites do esqueleto\n");
        return 1;
    }

    if (!velho) {
        printf("Erro ao carregar sprites do velho\n");
        return 1;
    }

    if (!vila) {
        printf("Erro ao carregar mapa da vila\n");
        return 1;
    }

    while (1) {

        ALLEGRO_KEYBOARD_STATE estado;
        al_get_keyboard_state(&estado);

        Direcao direcao;

        bool andando = false;

        int novo_x = player.pos.x;
        int novo_y = player.pos.y;

        //Movimenta o player
        if (!em_dialogo) {
            if (al_key_down(&estado, ALLEGRO_KEY_W)) { novo_y -= 5; andando = true; direcao = CIMA; }
            if (al_key_down(&estado, ALLEGRO_KEY_S)) { novo_y += 5; andando = true; direcao = BAIXO; }
            if (al_key_down(&estado, ALLEGRO_KEY_A)) { novo_x -= 5; andando = true; direcao = ESQUERDA; }
            if (al_key_down(&estado, ALLEGRO_KEY_D)) { novo_x += 5; andando = true; direcao = DIREITA; }
        }

        if (dentro_area(novo_x, player.pos.y)) {
            player.pos.x = novo_x;
        }

        if (dentro_area(player.pos.x, novo_y)) {
            player.pos.y = novo_y;
        }
        static bool ataque_antes = false;

        //Ataque do player
        if (al_key_down(&estado, ALLEGRO_KEY_SPACE)) {

            if (!ataque_antes) {

                player.atacando = true;

                if (skeleton.vivo && abs(player.pos.x - skeleton.pos.x) < 80 && abs(player.pos.y - skeleton.pos.y) < 80) {

                    skeleton.vida -= player.dano;

                    if (skeleton.vida <= 0) {

                        skeleton.vivo = false;
                        skeleton.vida = 60;

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

        //Recupera 20 da vida, como se fosse uma poção de cura
        if (al_key_down(&estado, ALLEGRO_KEY_Q)) {

            if (!cura_antes) {

                player.vida += 20;

                if (player.vida > 100)
                    player.vida = 100;

            }

            cura_antes = true;
        }
        else {

            cura_antes = false;
        }

        //Controla os frames do jogador, npc e inimigo 
        frame_delay++;

        if (frame_delay >= 8) {

            frame_delay = 0;

            if (andando || player.atacando)
                player.frame++;
            else
                player.frame = 0;

            if (skeleton.vivo)
                skeleton.frame++;
            else
                skeleton.frame = 0;

            npc_frame++;
            if (npc_frame > 3)
                npc_frame = 0;
        }


        //Trava o ataque para o dilaogo
        if (al_key_down(&estado, ALLEGRO_KEY_ESCAPE)) break;


        //Verificação de distancia entre o player e o npc
        bool perto = false;

        int npc_encontrado = buscar_npc_por_x(npcs, 1, 800);

        if (npc_encontrado != -1) {

            if (abs(player.pos.x - npcs[npc_encontrado].pos.x) < 40 && abs(player.pos.y - npcs[npc_encontrado].pos.y) < 40) {

                perto = true;
            }
        }

        //Inicia o dialogo com o npc
        static bool e_antes = false;

        if (perto && al_key_down(&estado, ALLEGRO_KEY_E)) {

            if (!e_antes && !intro && npc_ativo) {

                em_dialogo = true;

                fala_atual = 6;
            }

            e_antes = true;
        }

        //Controla o Dialogo com o npc
        static bool enter_antes = false;

        if (em_dialogo) {
            if (al_key_down(&estado, ALLEGRO_KEY_ENTER)) {
                if (!enter_antes) {
                    fala_atual++;
                    if (intro && fala_atual >= 6) {

                        em_dialogo = false;

                        intro = false;

                        fala_atual = 6;
                    }

                    else if (!intro && fala_atual >= total_falas) {

                        em_dialogo = false;

                        npc_ativo = false;

                        skeleton.vivo = true;

                        skeleton.pos.x = 120;
                        skeleton.pos.y = 300;
                    }
                }
                enter_antes = true;
            }
            else {
                enter_antes = false;
            }
        }
        
        //Algoritmo para fazer o inimigo perseguir
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

        //Controla o ataque/dano do inimigo
        if (cooldown_dano > 0)
            cooldown_dano--;

        if (skeleton.vivo && abs(player.pos.x - skeleton.pos.x) < 50 && abs(player.pos.y - skeleton.pos.y) < 50) {

            skeleton.atacando = true;
        }
        else {

            skeleton.atacando = false;
        }

        if (skeleton.vivo && abs(player.pos.x - skeleton.pos.x) < 50 && abs(player.pos.y - skeleton.pos.y) < 50 && cooldown_dano <= 0) {

            player.vida -= skeleton.dano;

            cooldown_dano = 60;

            if (player.vida <= 0) {

                printf("GAME OVER\n");

                break;
            }
        }
        al_clear_to_color(al_map_rgb(255, 255, 255));

        al_draw_scaled_bitmap(vila, 0, 0, 1536, 1024, 0, 0, 1280, 720, 0);

        ALLEGRO_BITMAP* sprite_player;

        if (player.atacando)
            sprite_player = player_atacando;

        else if (andando)
            sprite_player = player_andando;

        else
            sprite_player = player_parado;

        int frame_w;
        int total_frames;

        if (player.atacando) {
            frame_w = 184;
            total_frames = 6;
        }
        else if (andando) {
            frame_w = 188;
            total_frames = 8;
        }
        else {
            frame_w = 194;
            total_frames = 7;
        }

        int frame_h = 200;

        if (player.frame >= total_frames)
            player.frame = 0;

        al_draw_scaled_bitmap(sprite_player, player.frame * frame_w, 0, frame_w, frame_h, player.pos.x, player.pos.y, 128, 128, 0);

        int npc_w = 256;
        int npc_h = 341;

        al_draw_scaled_bitmap(velho, npc_frame * npc_w, 0, npc_w, npc_h, npc_x, npc_y, 80, 80, 0);

        if (skeleton.vivo) {

            ALLEGRO_BITMAP* sprite_skeleton;

            bool skeleton_andando_bool = false;

            if (abs(player.pos.x - skeleton.pos.x) > 10 || abs(player.pos.y - skeleton.pos.y) > 10) {

               skeleton_andando_bool = true;
           }

            if (skeleton.atacando)
                sprite_skeleton = skeleton_atacando;

            else if (skeleton_andando_bool)
                sprite_skeleton = skeleton_andando;

            else
                sprite_skeleton = skeleton_parado;

            int sk_frame_w;
            int sk_total_frames;

            if (skeleton.atacando) {
                sk_frame_w = 185;
                sk_total_frames = 6;
            }
            else if (skeleton_andando_bool) {
                sk_frame_w = 194;
                sk_total_frames = 8;
            }
            else {
                sk_frame_w = 193;
                sk_total_frames = 7;
            }

            int sk_frame_h = 200;

            if (skeleton.frame >= sk_total_frames)
                skeleton.frame = 0;

            al_draw_scaled_bitmap(sprite_skeleton, skeleton.frame * sk_frame_w, 0, sk_frame_w, sk_frame_h, skeleton.pos.x, skeleton.pos.y, 128, 128, 0);
        }

        if (perto) {
            al_draw_text(font, al_map_rgb(255, 255, 255), 100, 30, 0, "Pressione E");
        }

        if (em_dialogo) {
            al_draw_filled_rectangle(50, 450, 750, 580, al_map_rgb(0, 0, 0));
            al_draw_text(font, al_map_rgb(255, 255, 255), 60, 460, 0, falas[fala_atual]);
        }

        al_draw_filled_rectangle(20, 20, 20 + (player.vida * 2), 40, al_map_rgb(255, 0, 0));

        al_draw_rectangle(20, 20, 220, 40, al_map_rgb(255, 255, 255), 2);

        al_flip_display();
        al_rest(0.016);
    }

    free(npcs);
    al_destroy_bitmap(velho);
    al_destroy_bitmap(vila);

    al_destroy_bitmap(player_parado);
    al_destroy_bitmap(player_andando);
    al_destroy_bitmap(player_atacando);

    al_destroy_bitmap(skeleton_parado);
    al_destroy_bitmap(skeleton_andando);
    al_destroy_bitmap(skeleton_atacando);
    al_destroy_display(display);
    al_destroy_font(font);
    return 0;
}
