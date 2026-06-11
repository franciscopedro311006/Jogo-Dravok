#include "jogo.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

int main() {

    al_init();
    al_init_primitives_addon();
    al_install_keyboard();
    al_init_font_addon();
    al_init_image_addon();

    ALLEGRO_FONT* font = al_create_builtin_font();
    ALLEGRO_DISPLAY* display = al_create_display(1280, 720);

    int total_npcs = 1;

    int total_itens = 5;

    char (*inventario)[30] = malloc(total_itens * sizeof(*inventario));

    if (inventario == NULL) {
        printf("Erro de memoria\n");
        return 1;
    }

    Personagem* npcs = malloc(total_npcs * sizeof(Personagem));

    if (npcs == NULL) {
        printf("Erro de memoria\n");
        return 1;
    }

    strcpy_s(inventario[0], 30, "Pocao");
    strcpy_s(inventario[1], 30, "Espada");
    strcpy_s(inventario[2], 30, "Arco");
    strcpy_s(inventario[3], 30, "Escudo");
    strcpy_s(inventario[4], 30, "Maca");

    Personagem player = { {100, 300}, 0, 0 };
    player.vida = 100;
    player.dano = 20;
    player.vivo = true;
    player.atacando = false;

    int frame_delay = 0;
    int npc_x, npc_y;

    npcs[0].pos.x = 800;
    npcs[0].pos.y = 160;

    strcpy_s(npcs[0].nome, sizeof(npcs[0].nome), "Velho");

    Personagem skeleton = { {600,300}, 0, 0 };
    skeleton.vida = 60;
    skeleton.dano = 10;
    skeleton.vivo = false;
    skeleton.atacando = false;

    npc_x = npcs[0].pos.x;
    npc_y = npcs[0].pos.y;

    //troquei o vetor de char enorme com o dialogo por um arquivo txt com o dialogo nele;
    FILE* file = fopen("dialogo.txt", "r");

    FilaDialogo dialogo;

    dialogo.inicio = 0;

    dialogo.fim = 0;

    if (file == NULL) {
        printf("Erro ao abrir dialogo.txt");
        return 1;
    }

    //Apaga o \n do final das frases e coloca \0
    char linha[200];

    while (fgets(linha,200,file)) {

        linha[strcspn(linha,"\n")] = 0;

        enfileirar(&dialogo,linha);
    }

    fclose(file);

    Direcao direcao = DIREITA;

    Direcao direcao_skeleton = DIREITA;


    int npc_frame = 0;

    bool em_dialogo = true;
    bool intro = true;
    bool inventario_aberto = false;
    bool npc_ativo = true;

    ALLEGRO_BITMAP* velho = al_load_bitmap("Velho.png");
    ALLEGRO_BITMAP* vila = al_load_bitmap("Casa.jpeg");

    ALLEGRO_BITMAP* player_parado = al_load_bitmap("PersonagemParado.png");
    ALLEGRO_BITMAP* player_andando_esquerda = al_load_bitmap("PersonagemAndandoEsquerda.png");
    ALLEGRO_BITMAP* player_andando_direita = al_load_bitmap("PersonagemAndandoDireita.png");
    ALLEGRO_BITMAP* player_atacando_direita = al_load_bitmap("PersonagemAtacandoDireita.png");
    ALLEGRO_BITMAP* player_atacando_esquerda = al_load_bitmap("PersonagemAtacandoEsquerda.png");

    ALLEGRO_BITMAP* skeleton_parado = al_load_bitmap("EsqueletoParado.png");
    ALLEGRO_BITMAP* skeleton_andando_esquerda = al_load_bitmap("EsqueletoAndandoEsquerda.png");
    ALLEGRO_BITMAP* skeleton_andando_direita = al_load_bitmap("EsqueletoAndandoDireita.png");
    ALLEGRO_BITMAP* skeleton_atacando_direita = al_load_bitmap("EsqueletoAtacandoDireita.png");
    ALLEGRO_BITMAP* skeleton_atacando_esquerda = al_load_bitmap("EsqueletoAtacandoEsquerda.png");

    if (!player_parado || !player_andando_direita || !player_andando_esquerda || !player_atacando_direita || !player_atacando_esquerda) {
        printf("Erro ao carregar sprites do player\n");
        return 1;
    }

    if (!skeleton_parado || !skeleton_andando_direita || !skeleton_andando_esquerda || !skeleton_atacando_direita || !skeleton_atacando_esquerda) {
        printf("Erro ao carregar sprites do esqueleto\n");
        return 1;
    }

    if (!velho) {
        printf("Erro ao carregar sprites do velho\n");
        return 1;
    }

    if (!vila) {
        printf("Erro ao carregar mapas\n");
        return 1;
    }

    while (1) {

        ALLEGRO_KEYBOARD_STATE estado;
        al_get_keyboard_state(&estado);

        bool andando = false;

        int novo_x = player.pos.x;
        int novo_y = player.pos.y;

        //Movimenta o player
        if (!em_dialogo) {
            if (al_key_down(&estado, ALLEGRO_KEY_W)) { novo_y -= 5; andando = true; }
            if (al_key_down(&estado, ALLEGRO_KEY_S)) { novo_y += 5; andando = true; }
            if (al_key_down(&estado, ALLEGRO_KEY_A)) { novo_x -= 5; andando = true; direcao = ESQUERDA; }
            if (al_key_down(&estado, ALLEGRO_KEY_D)) { novo_x += 5; andando = true; direcao = DIREITA; }
        }

        if (dentro_area(novo_x, player.pos.y)) {
            player.pos.x = novo_x;
        }

        if (dentro_area(player.pos.x, novo_y)) {
            player.pos.y = novo_y;
        }

        static bool i_antes = false;

        if (al_key_down(&estado, ALLEGRO_KEY_I)) {

            if (!i_antes) {

                inventario_aberto = !inventario_aberto;
            }

            i_antes = true;
        }
        else {

            i_antes = false;
        }

        static bool o_antes = false;

        if (inventario_aberto &&
            al_key_down(&estado, ALLEGRO_KEY_O)) {

            if (!o_antes) {

                ordenar_inventario(inventario, total_itens);
            }

            o_antes = true;
        }
        else {

            o_antes = false;
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

                        adicionar_item(
                            &inventario,
                            &total_itens,
                            "Machado"
                        );

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

        //Recupera 20 da vida, como se fosse uma poÃ§Ã£o de cura
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

        //Fecha o jogo com esc
        if (al_key_down(&estado, ALLEGRO_KEY_ESCAPE)) break;


        //VerificaÃ§Ã£o de distancia entre o player e o npc
        bool perto = false;

        int npc_encontrado = buscar_npc_mais_proximo(npcs, total_npcs, player);

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

                dialogo.inicio = 6;
            }

            e_antes = true;
        }

        //Controla o Dialogo com o npc
        static bool enter_antes = false;

        if (em_dialogo) {
            if (al_key_down(&estado, ALLEGRO_KEY_ENTER)) {
                if (!enter_antes) {
                    desenfileirar(&dialogo);
                    if (intro && dialogo.inicio >= 6) {

                        em_dialogo = false;

                        intro = false;

                        dialogo.inicio = 6;
                    }

                    if (!intro && dialogo.inicio >= dialogo.fim) {

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


        if (player.atacando) {

            if (direcao == DIREITA)
                sprite_player = player_atacando_direita;
            else
                sprite_player = player_atacando_esquerda;
        }
        else if (andando) {

            if (direcao == ESQUERDA)
                sprite_player = player_andando_esquerda;
            else
                sprite_player = player_andando_direita;
        }
        else {

            sprite_player = player_parado;
        }



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

        if (inventario_aberto) {

            al_draw_filled_rectangle(300, 100, 700, 500, al_map_rgb(40, 40, 40));

            al_draw_text(font, al_map_rgb(255, 255, 255), 320, 120, 0, "Inventario");

            for (int i = 0; i < total_itens; i++) {

                al_draw_text(font, al_map_rgb(255, 255, 255), 320, 170 + i * 30, 0, inventario[i]);
            }

            al_draw_text(font, al_map_rgb(255, 255, 0), 320, 420, 0, "Pressione O para ordenar");
        }

        if (skeleton.vivo) {

            ALLEGRO_BITMAP* sprite_skeleton;

            bool skeleton_andando_bool = false;

            if (abs(player.pos.x - skeleton.pos.x) > 10 || abs(player.pos.y - skeleton.pos.y) > 10) {

                skeleton_andando_bool = true;
            }

            if (player.pos.x < skeleton.pos.x)
                direcao_skeleton = ESQUERDA;
            else
                direcao_skeleton = DIREITA;

            if (skeleton.atacando) {

                if (direcao_skeleton == DIREITA)
                    sprite_skeleton = skeleton_atacando_direita;
                else
                    sprite_skeleton = skeleton_atacando_esquerda;
            }
            else if (skeleton_andando_bool) {

                if (direcao_skeleton == ESQUERDA)
                    sprite_skeleton = skeleton_andando_esquerda;
                else
                    sprite_skeleton = skeleton_andando_direita;
            }
            else {

                sprite_skeleton = skeleton_parado;
            }

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
            al_draw_text(font, al_map_rgb(255, 255, 255), 800, 150, 0, "Pressione E");
        }

        if (em_dialogo) {
            al_draw_filled_rectangle(50, 450, 750, 580, al_map_rgb(0, 0, 0));
            al_draw_text(font, al_map_rgb(255, 255, 255), 60, 460, 0, fala_atual(&dialogo));
        }

        al_draw_filled_rectangle(20, 20, 20 + (player.vida * 2), 40, al_map_rgb(255, 0, 0));

        al_draw_rectangle(20, 20, 220, 40, al_map_rgb(255, 255, 255), 2);

        al_flip_display();
        al_rest(0.016);
    }

    free(npcs);
    free(inventario);
    al_destroy_bitmap(velho);
    al_destroy_bitmap(vila);

    al_destroy_bitmap(player_parado);
    al_destroy_bitmap(player_andando_direita);
    al_destroy_bitmap(player_andando_esquerda);

    al_destroy_bitmap(player_atacando_direita);
    al_destroy_bitmap(player_atacando_esquerda);

    al_destroy_bitmap(skeleton_parado);
    al_destroy_bitmap(skeleton_andando_direita);
    al_destroy_bitmap(skeleton_andando_esquerda);

    al_destroy_bitmap(skeleton_atacando_direita);
    al_destroy_bitmap(skeleton_atacando_esquerda);
    al_destroy_display(display);
    al_destroy_font(font);
    return 0;
}