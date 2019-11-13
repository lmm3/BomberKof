#include "lib/client.h"
#include "Exemplo_Heitor/libAllegro/AllegroCore.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
 
#define TABWIDTH 15
#define TABHEIGHT 11
#define ALTURA 480
#define LARGURA 640

enum MYKEYS {
   KEY_W, KEY_S, KEY_A, KEY_D, KEY_SPACE
};

typedef struct{
    int isValid;
    char move;
}pacote_server;

typedef struct{
    char tab[TABHEIGHT][TABWIDTH];
    int bombFlag;
    int xBomb;
    int yBomb;
    int bombRange;
    int lifes;
    int kills;
    int isInvicible[4];
    int deaths;
    char direction[4];
}pacote_client;


void printTab(char tab[][TABWIDTH], int l, int d, int k)
{
    int i, j;
    for(i = 0; i < TABHEIGHT; i++){
        for(j = 0; j < TABWIDTH; j++)
            printf("%c" ,tab[i][j]);

        printf("\n");
    }

    printf("LIFES: %d\nKILLS: %d\nDEATHS: %d\n" ,l ,k, d);
}

int validMove(char move)
{
    if(move == ' ' || move == 'w' || move == 'a' || move == 's' || move == 'd')
        return 1;
    else
        return 0;
}  

void manipular_entrada(ALLEGRO_EVENT evento, char * str)
{
    if (evento.type == ALLEGRO_EVENT_KEY_CHAR)
    {
        if (strlen(str) <= 16)
        {
            char temp[] = {evento.keyboard.unichar, '\0'};
            if (evento.keyboard.unichar == ' ')
            {
                strcat(str, temp);
            }
            else if (evento.keyboard.unichar >= '0' &&
                     evento.keyboard.unichar <= '9')
            {
                strcat(str, temp);
            }
            else if (evento.keyboard.unichar >= 'A' &&
                     evento.keyboard.unichar <= 'Z')
            {
                strcat(str, temp);
            }
            else if (evento.keyboard.unichar >= 'a' &&
                     evento.keyboard.unichar <= 'z')
            {
                strcat(str, temp);
            }
            else if(evento.keyboard.unichar == '.')
            {
                strcat(str, temp);
            }
        }
 
        if (evento.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && strlen(str) != 0)
        {
            str[strlen(str) - 1] = '\0';
        }
       
    }
}

void exibir_texto_centralizado(char * str, ALLEGRO_FONT * font)
{
    if (strlen(str) > 0)
    {
        al_draw_text(font, al_map_rgb(255, 255, 255), LARGURA / 2,
                     (ALTURA - al_get_font_ascent(font)) / 2,
                     ALLEGRO_ALIGN_CENTRE, str);
    }
}

char * ConectarAoServer(ALLEGRO_EVENT evento, ALLEGRO_DISPLAY * janela, ALLEGRO_FONT *font){
   char str[20];
   bool sair = false;
   bool concluido = false;

   strcpy(str, "");
   ALLEGRO_BITMAP * image2 = NULL;
   image2 = al_load_bitmap("conectarServer.png");

   al_register_event_source(fila_eventos, al_get_display_event_source(janela));
   al_register_event_source(fila_eventos, al_get_keyboard_event_source());

   while (!sair)
    {
        while (!al_is_event_queue_empty(fila_eventos))
        {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);
 
            if (!concluido)
            {
                manipular_entrada(evento, str);
 
                if (evento.type == ALLEGRO_EVENT_KEY_DOWN && evento.keyboard.keycode == ALLEGRO_KEY_ENTER)
                {
                    concluido = true;
                }
            }
 
            if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
                sair = true;
            }
        }
 
        al_draw_bitmap(image2, 0, 0, 0);
 
        
        exibir_texto_centralizado(evento,font);
 
        al_flip_display();
    }
 
    
    al_destroy_bitmap(image2);
    return str;

}


int main (void)
{	
    int i, j;
    ALLEGRO_DISPLAY *janela = NULL;
    ALLEGRO_BITMAP *tiles = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_FONT *font = NULL;
    ALLEGRO_BITMAP *image1 = NULL;
    bool done = false;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    al_init();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_init_primitives_addon();
    al_install_keyboard();
    if(!al_install_keyboard)
        printf("Deu merda no teclado\n");
    if(!al_install_mouse)
	printf("Deu merda no mouse");
    ALLEGRO_KEYBOARD_STATE keystate;

    janela = al_create_display(LARGURA, ALTURA);
    al_set_window_title(janela, "BomberKOF");
    tiles = al_load_bitmap("folha.png");
    image1 = al_load_bitmap("layout.png");
    font = al_load_ttf_font("Minecraft.ttf", 16,0);
    if(!tiles)
        printf("Deu merda\n");
    char server_IP[] = "172.20.4.21";
    connectToServer(server_IP);
    pacote_server dados;
    pacote_client recebido;
    timer = al_create_timer(1.0 / 60);
    event_queue = al_create_event_queue();

    if(!event_queue) {
      fprintf(stderr, "failed to create event_queue!\n");
      al_destroy_display(janela);
      al_destroy_timer(timer);
      return -1;
   }
    char server_IP[] = conectarAoServer(evento,janela,font);
    connectToServer(server_IP);
    pacote_server dados;
    pacote_client recebido;
    al_register_event_source(event_queue, al_get_display_event_source(janela));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_clear_to_color(al_map_rgb(0,0,0));
    al_flip_display();
    al_start_timer(timer);
    char move = '.';
    int keyboard_timer = 0;
    enum {UP, DOWN, LEFT, RIGHT, SPACE};
    char dir[] = {0, 0, 0, 0, 0};
    while(!done) 
    {
        ALLEGRO_EVENT events;
        al_wait_for_event(event_queue, &events);
        recvMsgFromServer(&recebido, DONT_WAIT);

        if(recebido.bombFlag == 1)
        {
            printf("A bomb has exploded\n");
            printf("Range: %d, x: %d, y: %d\n" ,recebido.bombRange, recebido.xBomb, recebido.yBomb);
            
        }

        if(events.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch(events.keyboard.keycode)
            {
                case ALLEGRO_KEY_ESCAPE:
                    done = true;
            }
        }
	if(events.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
	    break;
	}
        if(events.type == ALLEGRO_EVENT_KEY_DOWN){
            if(events.keyboard.keycode == ALLEGRO_KEY_W) dir[UP] = 1;
            al_rest(0.001);
            if(events.keyboard.keycode == ALLEGRO_KEY_S) dir[DOWN] = 1;
            al_rest(0.001);
            if(events.keyboard.keycode == ALLEGRO_KEY_A) dir[LEFT] = 1;
            al_rest(0.001);
            if(events.keyboard.keycode == ALLEGRO_KEY_D) dir[RIGHT] = 1;
            al_rest(0.001);
            if(events.keyboard.keycode == ALLEGRO_KEY_SPACE) dir[SPACE] = 1;
            al_rest(0.001);
        }
        else if(events.type == ALLEGRO_EVENT_KEY_UP){
            if(events.keyboard.keycode == ALLEGRO_KEY_W) dir[UP] = 0;
            if(events.keyboard.keycode == ALLEGRO_KEY_S) dir[DOWN] = 0;
            if(events.keyboard.keycode == ALLEGRO_KEY_A) dir[LEFT] = 0;
            if(events.keyboard.keycode == ALLEGRO_KEY_D) dir[RIGHT] = 0;
            if(events.keyboard.keycode == ALLEGRO_KEY_SPACE) dir[SPACE] = 0;
        }
        if(events.type == ALLEGRO_EVENT_TIMER)
        {
            //al_get_keyboard_state(&keystate);
                if(dir[UP] == 1)
                    move = 'w';
                else if(dir[DOWN] == 1)
                    move = 's';
                else if(dir[LEFT] == 1)
                    move = 'a';
                else if(dir[RIGHT] == 1)
                    move = 'd';
                else if(dir[SPACE] == 1)
                    move = ' ';
            redraw = true;
        }
        if(redraw && al_is_event_queue_empty(event_queue)){
            //printTab(recebido.tab, recebido.lifes, recebido.deaths, recebido.kills);
            redraw = false;
            al_clear_to_color(al_map_rgb(0,0,0));
		    al_draw_bitmap(image1, 0,0,0);
            al_draw_textf(font, al_map_rgb(0,0,0), 80+10+32+20, 32-8,ALLEGRO_ALIGN_CENTRE, "%d", recebido.lifes);
	       al_draw_textf(font, al_map_rgb(0,0,0), 160+80+10+32+20, 32-8,ALLEGRO_ALIGN_CENTRE, "%d", recebido.kills);
            al_draw_textf(font, al_map_rgb(0,0,0), 160 +160+80+10+32+20, 32-8,ALLEGRO_ALIGN_CENTRE, "%d", recebido.deaths);
    	    
            for (i = 0; i < TABHEIGHT ; i++)
            {
                for (j = 0; j < TABWIDTH; j++)
                {
                    if(recebido.tab[i][j] == '|'){ // BLOCO INQUEBRAVEL
                        al_draw_bitmap_region(tiles, 64, 288, 32, 32, 80+(32*j), 64+(32*i), 0);
                    }
                    else if (recebido.tab[i][j] == ' '){ // CHÃO
                        al_draw_bitmap_region(tiles, 0, 288, 32, 32, 80+(32*j), 64+(32*i), 0);
                        if(recebido.tab[i-1][j] == '|' || recebido.tab[i-1][j] == '}')
                            al_draw_bitmap_region(tiles, 32, 288, 32, 32, 80+(32*j), 64+(32*i), 0);
                    }
                    else if (recebido.tab[i][j] == '}'){
                        al_draw_bitmap_region(tiles, 96, 288, 32, 32, 80+(32*j), 64+(32*i), 0);
                    }
                    else if (recebido.tab[i][j] == 'v'){
                        al_draw_bitmap_region(tiles, 0, 288, 32, 32, 80+(32*j), 64+(32*i), 0);

                        switch(recebido.direction[0]){
                            case 's':
                                if(!recebido.isInvicible[0]) al_draw_bitmap_region(tiles, 0, 17, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                else al_draw_bitmap_region(tiles, 128, 17, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                break;
                            case 'w':
                                if(!recebido.isInvicible[0]) al_draw_bitmap_region(tiles, 32, 17, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                else al_draw_bitmap_region(tiles, 128+32, 17, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                break;
                            case 'd':
                                if(!recebido.isInvicible[0]) al_draw_bitmap_region(tiles, 64, 17, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                else al_draw_bitmap_region(tiles, 128+64, 17, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                break;
                            case 'a':
                                if(!recebido.isInvicible[0]) al_draw_bitmap_region(tiles, 96, 17, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                else al_draw_bitmap_region(tiles, 128+96, 17, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                break;
                            default:
                                al_draw_bitmap_region(tiles, 0, 17, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                break;
                        }
                        
                    }
                    else if (recebido.tab[i][j] == 'w'){
                        al_draw_bitmap_region(tiles, 0, 288, 32, 32, 80+(32*j), 64+(32*i), 0);
                        if(recebido.tab[i-1][j] == '|' || recebido.tab[i-1][j] == '}')
                            al_draw_bitmap_region(tiles, 32, 288, 32, 32, 80+(32*j), 64+(32*i), 0);

                        switch(recebido.direction[1]){

                            case 's':
                                if(!recebido.isInvicible[1]) al_draw_bitmap_region(tiles, 0, 17+64, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                else al_draw_bitmap_region(tiles, 128, 17, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                break;
                            case 'w':
                                if(!recebido.isInvicible[1]) al_draw_bitmap_region(tiles, 32, 17+64, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                else al_draw_bitmap_region(tiles, 128+32, 17, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                break;
                            case 'd':
                                if(!recebido.isInvicible[1]) al_draw_bitmap_region(tiles, 64, 17+64, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                else al_draw_bitmap_region(tiles, 128+64, 17, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                break;
                            case 'a':
                                if(!recebido.isInvicible[1]) al_draw_bitmap_region(tiles, 96, 17+64, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                else al_draw_bitmap_region(tiles, 128+96, 17, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                break;
                            default:
                                al_draw_bitmap_region(tiles, 0, 17+64, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                break;
                        }
                        
                    }
                    else if (recebido.tab[i][j] == 'x'){
                        al_draw_bitmap_region(tiles, 0, 288, 32, 32, 80+(32*j), 64+(32*i), 0);
                        if(recebido.tab[i-1][j] == '|' || recebido.tab[i-1][j] == '}')
                            al_draw_bitmap_region(tiles, 32, 288, 32, 32, 80+(32*j), 64+(32*i), 0);

                        switch(recebido.direction[2]){

                            case 's':
                                if(!recebido.isInvicible[2]) al_draw_bitmap_region(tiles, 0, 17+128, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                else al_draw_bitmap_region(tiles, 128, 17, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                break;
                            case 'w':
                                if(!recebido.isInvicible[2]) al_draw_bitmap_region(tiles, 32, 17+128, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                else al_draw_bitmap_region(tiles, 128+32, 17, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                break;
                            case 'd':
                                if(!recebido.isInvicible[2]) al_draw_bitmap_region(tiles, 64, 17+128, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                else al_draw_bitmap_region(tiles, 128+64, 17, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                break;
                            case 'a':
                                if(!recebido.isInvicible[2]) al_draw_bitmap_region(tiles, 96, 17+128, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                else al_draw_bitmap_region(tiles, 128+96, 17, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                break;
                            default:
                                al_draw_bitmap_region(tiles, 0, 17+128, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                break;
                        }
                        
                    }
                    else if (recebido.tab[i][j] == 'y'){
                        al_draw_bitmap_region(tiles, 0, 288, 32, 32, 80+(32*j), 64+(32*i), 0);
                        if(recebido.tab[i-1][j] == '|' || recebido.tab[i-1][j] == '}')
                            al_draw_bitmap_region(tiles, 32, 288, 32, 32, 80+(32*j), 64+(32*i), 0);

                        switch(recebido.direction[3]){

                            case 's':
                                if(!recebido.isInvicible[3]) al_draw_bitmap_region(tiles, 0, 17+192, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                else al_draw_bitmap_region(tiles, 128, 17, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                break;
                            case 'w':
                                if(!recebido.isInvicible[3]) al_draw_bitmap_region(tiles, 32, 17+192, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                else al_draw_bitmap_region(tiles, 128+32, 17, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                break;
                            case 'd':
                                if(!recebido.isInvicible[3]) al_draw_bitmap_region(tiles, 64, 17+192, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                else al_draw_bitmap_region(tiles, 128+64, 17, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                break;
                            case 'a':
                                if(!recebido.isInvicible[3]) al_draw_bitmap_region(tiles, 96, 17+192, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                else al_draw_bitmap_region(tiles, 128+96, 17, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                break;
                            default:
                                al_draw_bitmap_region(tiles, 0, 17+192, 32, 62-16, 80+(32*j), (64+(32*i))-13, 0);
                                break;
                        }
                        
                    }
                    else if (recebido.tab[i][j] == 'X'){
                        
                        al_draw_bitmap_region(tiles, 0, 288, 32, 32, 80+(32*j), 64+(32*i), 0);
                        al_draw_bitmap_region(tiles, 0, 260, 32, 32, 80+(32*j), 64+(32*i), 0);
                    }
                    else if (recebido.tab[i][j] == '1'){
        
                        al_draw_bitmap_region(tiles, 0, 288, 32, 32, 80+(32*j), 64+(32*i), 0);
                        al_draw_bitmap_region(tiles, 32, 320, 32, 32, 80+(32*j), 64+(32*i), 0);
                    }
                    else if (recebido.tab[i][j] == '2'){  
                        al_draw_bitmap_region(tiles, 0, 288, 32, 32, 80+(32*j), 64+(32*i), 0);
                        al_draw_bitmap_region(tiles, 0, 320, 32, 32, 80+(32*j), 64+(32*i), 0);
                    }
                    else if (recebido.tab[i][j] == '3'){
                        al_draw_bitmap_region(tiles, 0, 288, 32, 32, 80+(32*j), 64+(32*i), 0);
                        al_draw_bitmap_region(tiles, 64, 320, 32, 32, 80+(32*j), 64+(32*i), 0);
                    }
                    else if (recebido.tab[i][j] == '4'){
                        al_draw_bitmap_region(tiles, 0, 288, 32, 32, 80+(32*j), 64+(32*i), 0);
                        al_draw_bitmap_region(tiles, 96, 320, 32, 32, 80+(32*j), 64+(32*i), 0);
                    }
                }
            }
            al_flip_display();
            if(!keyboard_timer){
                sendMsgToServer(&dados, sizeof(dados));
                keyboard_timer = 5;
            }
            if(keyboard_timer) keyboard_timer--;
        }
        
        dados.isValid = validMove(move);
        if(dados.isValid){
            dados.move = move;
        }
        
        move = '.';
    }
    al_destroy_timer(timer);
    al_destroy_display(janela);
    return 0;
}

