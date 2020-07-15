#include <stdio.h>
#include<math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#define GAME_TERMINATE -1

// ALLEGRO Variables
ALLEGRO_DISPLAY* display = NULL;
ALLEGRO_FONT *font = NULL;
ALLEGRO_FONT *font2 = NULL;
ALLEGRO_BITMAP*cover = NULL;
ALLEGRO_BITMAP*cover_remove = NULL;
ALLEGRO_BITMAP*menu = NULL;
ALLEGRO_BITMAP*option = NULL;
ALLEGRO_BITMAP*gamemap = NULL;
ALLEGRO_BITMAP*re_or_leave = NULL;
ALLEGRO_BITMAP*game_over = NULL;
ALLEGRO_BITMAP*win = NULL;
ALLEGRO_BITMAP*instruction = NULL;
ALLEGRO_BITMAP*back_to_menu = NULL;
ALLEGRO_BITMAP*checker = NULL;
ALLEGRO_BITMAP*heart = NULL;
ALLEGRO_SAMPLE*song =NULL;
ALLEGRO_TIMER*timer = NULL;  //tower
ALLEGRO_TIMER*timer2 = NULL; //enemy
ALLEGRO_TIMER*timer3 = NULL; //stage
ALLEGRO_TIMER*timer4 = NULL; //ice_bullet
ALLEGRO_TIMER*timer5 = NULL; //fire_bullet
ALLEGRO_TIMER*timer6 = NULL; //poison_bullet
ALLEGRO_TIMER*timer7 = NULL; //heal_bullet
ALLEGRO_TIMER*timer8 = NULL; //punch_bullet
ALLEGRO_TIMER*timer9 = NULL; //light_bullet
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_KEYBOARD_STATE keyState ;

//declare const
int width = 1300;
int height = 800;
int window = 1;
int stage_x = 0;
int mouse_x = 0, mouse_y=0;
int hp = 10, money=10, stage = 1;
bool judge_next_window = false;
bool click = false;
bool redraw = true;
bool judge = false;
bool deter[7] = {false, false, false, false, false, false};
const int FPS = 60;

bool ice_exp = false;
bool fire_exp = false;
bool poison_exp = false;
bool punch_exp = false;
bool heal_exp = false;
bool light_exp = false;

bool ture = true; //true: appear, false: disappear
bool next = false; //true: trigger
int  dir=2 ; //1: up, 2:down, 3: left, 4:right
int flag = 1;
void draw_game_map();

typedef struct character
{
    int x, y;
    int number=0;
    int cost=10;
    int atk;
    float speed;
    ALLEGRO_BITMAP *ip;
} Tower;
typedef struct
{
    int x = 195, y = -50, hp = 15000;
    int speed = 1;
    ALLEGRO_BITMAP *ip;
} Enemy;
typedef struct
{
    int x, y;
    int speed, dmg;
    ALLEGRO_BITMAP*explo_ip;
    ALLEGRO_BITMAP*bullet_ip;
} Bullet;

int shot(Tower, Enemy);
double dis(Bullet, Enemy);
Tower tower[7];// poison, heal, light, fire, punch, ice
Enemy enemy[10][3];
Bullet poison, heal, light, fire, punch, ice;

enum {UP, DOWN, LEFT, RIGHT};
bool keys[4] = {false, false, false, false};

void Move_tower_up(Tower* tower)
{
    tower->y +=5;
    if(tower->y > 600)
        tower->y = 600;
}
void Move_tower_down(Tower* tower)
{
    tower->y -=5;
    if(tower->y < 10)
        tower->y = 10;
}
void Move_tower_left(Tower* tower)
{
    tower->x -=5;
    if(tower->x < 135)
        tower->x = 135;
}
void Move_tower_right(Tower* tower)
{
    tower->x +=5;
    if(tower->x > 1065)
        tower->x = 1065;
}

//declare function
void show_err_msg(int msg);
void game_init();
void game_begin();
int process_event();
int game_run();
void game_destroy();

int main()
{
    tower[0].x = 250, tower[0].y = 683;
    tower[1].x = 380, tower[1].y = 683;
    tower[2].x = 510, tower[2].y = 683;
    tower[3].x = 640, tower[3].y = 683;
    tower[4].x = 770, tower[4].y = 683;
    tower[5].x = 900, tower[5].y = 683;
    poison.dmg = 50, light.dmg = 70, heal.dmg = 70, ice.dmg = 70, punch.dmg = 100, fire.dmg = 70;
    int msg = 0;

    game_init();
    game_begin();
    while (msg != GAME_TERMINATE)
    {
        msg = game_run();
        if (msg == GAME_TERMINATE)
            printf("Game Over\n");
    }

    game_destroy();
    return 0;
}

void show_err_msg(int msg)
{
    fprintf(stderr, "unexpected msg: %d\n", msg);
    game_destroy();
    exit(9);
}

void game_init()
{
    if (!al_init())
    {
        show_err_msg(-1);
    }
    // Create display
    display = al_create_display(width, height);
    event_queue = al_create_event_queue();
    if (display == NULL || event_queue == NULL)
    {
        show_err_msg(-5);
    }
    // Initialize Allegro settings
    al_set_window_position(display, 0, 0);
    al_set_window_title(display, "Final_project_107071006");
    al_init_primitives_addon();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_keyboard();
    al_init_acodec_addon();
    al_install_mouse();
    al_install_audio();

    // Register event
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_keyboard_event_source());
}

void game_begin()
{
    song = al_load_sample( "hello.wav" );
    if (!song){
        printf( "Audio clip sample not loaded!\n" );
        show_err_msg(-6);
    }
    // Loop the song until the display closes
    al_play_sample(song, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);

    //Load cover
    al_clear_to_color(al_map_rgb(255, 255, 255));
    cover = al_load_bitmap("cover.png");
    al_draw_bitmap(cover, -160, 0, 0);
    al_flip_display();
}

int process_event()
{

    //Request the event
    ALLEGRO_EVENT event;
    al_wait_for_event(event_queue, &event);

    //Keyboard
    if(event.type == ALLEGRO_EVENT_KEY_DOWN)
    {
        switch(event.keyboard.keycode)
        {
        case ALLEGRO_KEY_UP:
            keys[DOWN] = true;
            break;
        case ALLEGRO_KEY_DOWN:
            keys[UP] = true;
            break;
        case ALLEGRO_KEY_LEFT:
            keys[LEFT] = true;
            break;
        case ALLEGRO_KEY_RIGHT:
            keys[RIGHT] = true;
            break;
        case ALLEGRO_KEY_ESCAPE:
            return GAME_TERMINATE;
            break;
        case ALLEGRO_KEY_TAB:
            judge_next_window = true;
        case ALLEGRO_KEY_ENTER:
            ice.x = tower[5].x;
            ice.y = tower[5].y;
            fire.x = tower[3].x;
            fire.y = tower[3].y;
            poison.x = tower[0].x;
            poison.y = tower[0].y;
            heal.x = tower[1].x;
            heal.y = tower[1].y;
            light.x = tower[2].x;
            light.y = tower[2].y;
            punch.x = tower[4].x;
            punch.y = tower[4].y;

            judge = true;
        }
    }
    else if(event.type == ALLEGRO_EVENT_KEY_UP)
    {
        switch(event.keyboard.keycode)
        {
        case ALLEGRO_KEY_UP:
            keys[DOWN] = false;
            break;
        case ALLEGRO_KEY_DOWN:
            keys[UP] = false;
            break;
        case ALLEGRO_KEY_LEFT:
            keys[LEFT] = false;
            break;
        case ALLEGRO_KEY_RIGHT:
            keys[RIGHT] = false;
            break;
        }
    }

    //Mouse
    if(event.type == ALLEGRO_EVENT_MOUSE_AXES)
    {
        mouse_x = event.mouse.x;
        mouse_y = event.mouse.y;
    }
    else if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
    {
        if(event.mouse.button&1)
        {
            click = true;
        }
    }
    else if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
    {
        if(event.mouse.button&1)
        {
            click = false;
        }
    }
    //Shutdown program
    if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        return GAME_TERMINATE;

    //Timer
    if(event.timer.source == timer)
    {
        redraw = true;
        if((keys[UP])&&(mouse_x>250)&&(mouse_x<350))
            Move_tower_up(&tower[0]);
        else if((keys[UP])&&(mouse_x>380)&&(mouse_x<480))
            Move_tower_up(&tower[1]);
        else if((keys[UP])&&(mouse_x>510)&&(mouse_x<610))
            Move_tower_up(&tower[2]);
        else if((keys[UP])&&(mouse_x>640)&&(mouse_x<740))
            Move_tower_up(&tower[3]);
        else if((keys[UP])&&(mouse_x>770)&&(mouse_x<870))
            Move_tower_up(&tower[4]);
        else if((keys[UP])&&(mouse_x>900)&&(mouse_x<1000))
            Move_tower_up(&tower[5]);

        if((keys[DOWN])&&(mouse_x>250)&&(mouse_x<350))
            Move_tower_down(&tower[0]);
        else if((keys[DOWN])&&(mouse_x>380)&&(mouse_x<480))
            Move_tower_down(&tower[1]);
        else if((keys[DOWN])&&(mouse_x>510)&&(mouse_x<610))
            Move_tower_down(&tower[2]);
        else if((keys[DOWN])&&(mouse_x>640)&&(mouse_x<740))
            Move_tower_down(&tower[3]);
        else if((keys[DOWN])&&(mouse_x>770)&&(mouse_x<870))
            Move_tower_down(&tower[4]);
        else if((keys[DOWN])&&(mouse_x>900)&&(mouse_x<1000))
            Move_tower_down(&tower[5]);

        if((keys[LEFT])&&(mouse_x>250)&&(mouse_x<350))
            Move_tower_left(&tower[0]);
        else if((keys[LEFT])&&(mouse_x>380)&&(mouse_x<480))
            Move_tower_left(&tower[1]);
        else if((keys[LEFT])&&(mouse_x>510)&&(mouse_x<610))
            Move_tower_left(&tower[2]);
        else if((keys[LEFT])&&(mouse_x>640)&&(mouse_x<740))
            Move_tower_left(&tower[3]);
        else if((keys[LEFT])&&(mouse_x>770)&&(mouse_x<870))
            Move_tower_left(&tower[4]);
        else if((keys[LEFT])&&(mouse_x>900)&&(mouse_x<1000))
            Move_tower_left(&tower[5]);

        if((keys[RIGHT])&&(mouse_x>250)&&(mouse_x<350))
            Move_tower_right(&tower[0]);
        else if((keys[RIGHT])&&(mouse_x>380)&&(mouse_x<480))
            Move_tower_right(&tower[1]);
        else if((keys[RIGHT])&&(mouse_x>510)&&(mouse_x<610))
            Move_tower_right(&tower[2]);
        else if((keys[RIGHT])&&(mouse_x>640)&&(mouse_x<740))
            Move_tower_right(&tower[3]);
        else if((keys[RIGHT])&&(mouse_x>770)&&(mouse_x<870))
            Move_tower_right(&tower[4]);
        else if((keys[RIGHT])&&(mouse_x>900)&&(mouse_x<1000))
            Move_tower_right(&tower[5]);
    }
    // Our setting for controlling animation

    if(event.timer.source == timer2)
    {
        if(enemy[0][0].hp>0)
        {
            if((dis(ice, enemy[0][0])<10)&&(dis(ice, enemy[0][0])>0)) //減速
            {
                if((enemy[0][0].y>457)&&(enemy[0][0].y < 467)&& (flag==1))
                {
                    dir = 4;
                    flag++;
                }
                if((enemy[0][0].x>440)&&(enemy[0][0].x<450)&&(flag==2))
                {
                    dir = 1;
                    flag++;
                }
                else if (enemy[0][0].y < 37 &&(flag==3))
                {
                    dir = 4;
                    flag++;
                }
                else if ((enemy[0][0].x>930)&&(flag==4))
                {
                    dir = 2;
                    flag++;
                }
                else if ((enemy[0][0].y>457)&&(flag ==5))
                {
                    dir = 3;
                    flag++;
                }
                else if ((enemy[0][0].x<700)&&(flag==6))
                {
                    dir = 1;
                    flag++;
                }
                else if ((enemy[0][0].y<150)&&(flag==7))
                {
                    flag++; //Gameover
                }

                if(dir ==1)
                    enemy[0][0].y -= 1;
                else if(dir == 2)
                    enemy[0][0].y +=1;
                else if(dir == 3)
                    enemy[0][0].x -=1;
                else if(dir == 4)
                    enemy[0][0].x +=1;
            }
            else//沒被冰塊砸到
            {
                if((enemy[0][0].y>457)&&(enemy[0][0].y < 467)&& (flag==1))
                {
                    dir = 4;
                    flag++;
                }
                if((enemy[0][0].x>440)&&(enemy[0][0].x<450)&&(flag==2))
                {
                    dir = 1;
                    flag++;
                }
                else if (enemy[0][0].y < 37 &&(flag==3))
                {
                    dir = 4;
                    flag++;
                }
                else if ((enemy[0][0].x>930)&&(flag==4))
                {
                    dir = 2;
                    flag++;
                }
                else if ((enemy[0][0].y>457)&&(flag ==5))
                {
                    dir = 3;
                    flag++;
                }
                else if ((enemy[0][0].x<700)&&(flag==6))
                {
                    dir = 1;
                    flag++;
                }
                else if ((enemy[0][0].y<150)&&(flag==7))
                {
                    flag++; //Gameover
                }
                if(dir ==1)
                    enemy[0][0].y -= 6;
                else if(dir == 2)
                    enemy[0][0].y +=6;
                else if(dir == 3)
                    enemy[0][0].x -=6;
                else if(dir == 4)
                    enemy[0][0].x +=6;
            }
        }
        else
            flag = 100;

    }
    if(event.timer.source == timer3)
    {
        stage_x +=30;
    }
    if(event.timer.source == timer4)
    {
        ice.x += (enemy[0][0].x-ice.x)/2;
        ice.y += (enemy[0][0].y-ice.y)/2;
        if(dis(ice, enemy[0][0] )<5)
        {
            ice_exp = true;
            ice.x = tower[5].x, ice.y = tower[5].y;
            enemy[0][0].hp -=ice.dmg;
        }
    }
    if(event.timer.source == timer5)
    {
        fire.x += ((enemy[0][0].x-fire.x)/2);
        fire.y += ((enemy[0][0].y-fire.y)/2);
        if(dis(fire, enemy[0][0] )<5)
        {
            fire_exp = true;
            fire.x = tower[3].x, fire.y = tower[3].y;
            enemy[0][0].hp -=fire.dmg;
        }
    }

    if(event.timer.source == timer6)
    {
        poison.x += ((enemy[0][0].x-poison.x)/2);
        poison.y += ((enemy[0][0].y-poison.y)/2);
        if(dis(poison, enemy[0][0] )<5)
        {
            poison_exp = true;
            poison.x = tower[0].x, poison.y = tower[0].y;
            enemy[0][0].hp -=poison.dmg;
        }
    }

    if(event.timer.source == timer7)
    {
        heal.x += ((enemy[0][0].x-heal.x)/2);
        heal.y += ((enemy[0][0].y-heal.y)/2);
        if(dis(heal, enemy[0][0] )<5)
        {
            heal_exp = true;
            heal.x = tower[1].x, heal.y = tower[1].y;
            enemy[0][0].hp -=heal.dmg;
        }
    }

    if(event.timer.source == timer8)
    {
        punch.x += ((enemy[0][0].x-punch.x)/2);
        punch.y += ((enemy[0][0].y-punch.y)/2);
        if(dis(punch, enemy[0][0] )<5)
        {
            punch_exp = true;
            punch.x = tower[4].x, punch.y = tower[4].y;
            enemy[0][0].hp -=punch.dmg;
        }
    }

    if(event.timer.source == timer9)
    {
        light.x += ((enemy[0][0].x-light.x)/2);
        light.y += ((enemy[0][0].y-light.y)/2);
        if(dis(light, enemy[0][0] )<5)
        {
            light_exp = true;
            light.x = tower[2].x, light.y = tower[2].y;
            enemy[0][0].hp -=light.dmg;
        }
    }

    return 0;
}

int game_run()
{
    int error = 0;
    //First window(Cover)
    if(window == 1)
    {
        if (!al_is_event_queue_empty(event_queue))
        {
            error = process_event();
            if(judge_next_window)
            {
                window = 2;
                font = al_load_ttf_font("pirulen.ttf", 35,0);
                font2 = al_load_ttf_font("pirulen.ttf", 20,0);
                menu = al_load_bitmap("menu2.jpg");
                option = al_load_bitmap("option.png");
                cover_remove = al_load_bitmap("cover-remove.png");
                //Initialize Timer
                timer = al_create_timer(1.0/FPS);
                timer2 = al_create_timer(1.0/15.0);
                timer3 = al_create_timer(1.0/15.0);
                timer4 = al_create_timer(1.0/8.0); //控制ICE攻擊速度
                timer5 = al_create_timer(1.0/12.0);  //控制FIRE攻擊速度
                timer6 = al_create_timer(1.0/11.0);  //控制poison攻擊速度
                timer7 = al_create_timer(1.0/10.0);  //控制heal攻擊速度
                timer8 = al_create_timer(1.0/20.0);  //控制punch攻擊速度
                timer9 = al_create_timer(1.0/15.0);  //控制lightning攻擊速度

                al_register_event_source(event_queue, al_get_timer_event_source(timer));
                al_register_event_source(event_queue, al_get_timer_event_source(timer2));
                al_register_event_source(event_queue, al_get_timer_event_source(timer3));
                al_register_event_source(event_queue, al_get_timer_event_source(timer4));
                al_register_event_source(event_queue, al_get_timer_event_source(timer5));
                al_register_event_source(event_queue, al_get_timer_event_source(timer6));
                al_register_event_source(event_queue, al_get_timer_event_source(timer7));
                al_register_event_source(event_queue, al_get_timer_event_source(timer8));
                al_register_event_source(event_queue, al_get_timer_event_source(timer9));

            }
        }
    }
    //Second window(Menu)
    if(window == 2)
    {
        al_draw_bitmap(menu, -100, 0, 0);
        al_draw_bitmap(cover_remove, width/2-190, 100, 0);
        al_draw_bitmap(option, width/2-250, 270, 0);
        al_draw_text(font, al_map_rgb(0, 0, 0), width/2, 350, ALLEGRO_ALIGN_CENTRE, "MAIN MENU");
        al_draw_bitmap(option, width/2-250, 340, 0);
        al_draw_text(font, al_map_rgb(0, 0, 0), width/2, 420, ALLEGRO_ALIGN_CENTRE, "GAME START");
        al_draw_bitmap(option, width/2-250, 410, 0);
        al_draw_text(font, al_map_rgb(0, 0, 0), width/2, 490, ALLEGRO_ALIGN_CENTRE, "Instructions");
        al_draw_bitmap(option, width/2-250, 480, 0);
        al_draw_text(font, al_map_rgb(0, 0, 0), width/2, 560, ALLEGRO_ALIGN_CENTRE, "LEAVE");
        al_flip_display();
        // Listening for new event
        if (!al_is_event_queue_empty(event_queue))
        {
            error = process_event();
        }
        //game start
        if((click == true)&&(mouse_x>width/2 - 250)&&(mouse_x<width/2 + 250)&& (mouse_y>370)&&(mouse_y<470))
        {
            gamemap = al_load_bitmap("map.jpg");
            tower[0].ip = al_load_bitmap("poison.png");
            tower[1].ip = al_load_bitmap("heal.png");
            tower[2].ip = al_load_bitmap("light.png");
            tower[3].ip = al_load_bitmap("fire.png");
            tower[4].ip = al_load_bitmap("dark.png");
            tower[5].ip = al_load_bitmap("ice.png");
            enemy[0][0].ip = al_load_bitmap("enemy1.png");
            checker = al_load_bitmap("base.png");
            heart = al_load_bitmap("heart_bar.png");
            ice.bullet_ip = al_load_bitmap("ice_bullet.png");
            fire.bullet_ip = al_load_bitmap("fire_bullet.png");
            ice.explo_ip = al_load_bitmap("ice_explosion.gif");
            fire.explo_ip = al_load_bitmap("fire_explosion.gif");
            heal.bullet_ip = al_load_bitmap("heal_bullet.png");
            punch.bullet_ip = al_load_bitmap("dark_bullet.png");
            heal.explo_ip = al_load_bitmap("heal_explosion.png");
            punch.explo_ip = al_load_bitmap("dark_explosion.gif");
            poison.bullet_ip = al_load_bitmap("poison_bullet.png");
            light.bullet_ip = al_load_bitmap("light_bullet.png");
            poison.explo_ip = al_load_bitmap("poison_explosion.gif");
            light.explo_ip = al_load_bitmap("light_explosion.gif");

            window = 7; //換場
        }
        //Instruction
        else if((click == true)&&(mouse_x>width/2 - 250)&&(mouse_x<width/2 + 250)&& (mouse_y>470)&&(mouse_y<570))
        {
            instruction = al_load_bitmap("instruction.png");
            back_to_menu = al_load_bitmap("back.png");
            window = 4;
        }
    }
    //Game Start
    if(window == 3)
    {
        if(redraw && al_is_event_queue_empty(event_queue))
        {
            al_start_timer(timer);
            al_start_timer(timer2);
            al_start_timer(timer4);
            al_start_timer(timer5);
            al_start_timer(timer6);
            al_start_timer(timer7);
            al_start_timer(timer8);
            al_start_timer(timer9);

            redraw = false;
            draw_game_map();
            al_draw_bitmap(heart, 10, 205, 0);
            al_draw_text(font2, al_map_rgb(0, 0, 0), 65, 200, ALLEGRO_ALIGN_CENTER, "STAGE 1");
            al_draw_bitmap(checker, 696, 225,0);
            al_draw_bitmap(enemy[0][0].ip, enemy[0][0].x, enemy[0][0].y, 0);
            for(int i =0 ; i<6 ; i++)
            {
                al_draw_bitmap(tower[i].ip, tower[i].x, tower[i].y, 0);
            }

            if((click == true)&&(mouse_x>250)&&(mouse_x<350))
            {
                al_draw_bitmap(tower[0].ip, tower[0].x, tower[0].y, 0);
                if(judge)
                    deter[0] = true;
                judge = false;
            }

            else if((click == true)&&(mouse_x>380)&&(mouse_x<480))
            {
                al_draw_bitmap(tower[1].ip, tower[1].x, tower[1].y, 0);
                if(judge)
                    deter[0] = true;
                judge = false;
            }

            else if((click == true)&&(mouse_x>510)&&(mouse_x<610))
            {
                al_draw_bitmap(tower[2].ip, tower[2].x, tower[2].y, 0);
                if(judge)
                    deter[0] = true;
                judge = false;
            }

            else if((click == true)&&(mouse_x>640)&&(mouse_x<740))
            {
                al_draw_bitmap(tower[3].ip, tower[3].x, tower[3].y, 0);
                if(judge)
                    deter[0] = true;
                judge = false;
            }

            else if((click == true)&&(mouse_x>770)&&(mouse_x<870))
            {
                al_draw_bitmap(tower[4].ip, tower[4].x, tower[4].y, 0);
                if(judge)
                    deter[0] = true;
                judge = false;
            }

            else if((click == true)&&(mouse_x>900)&&(mouse_x<1000))
            {
                al_draw_bitmap(tower[5].ip, tower[5].x, tower[5].y, 0);
                if(judge)
                    deter[0] = true;
                judge = false;
            }
            //Shot ice_bullet
            if(shot(tower[5], enemy[0][0])&& tower[5].y<680)
            {
                al_draw_bitmap(ice.bullet_ip, ice.x, ice.y, 0);
            }

            if(ice_exp)
            {
                al_draw_bitmap(ice.explo_ip, (enemy[0][0].x/8)*7, (enemy[0][0].y/8)*7, 0);
                ice_exp = false;
            }

            //Shot fire_bullet
            if(shot(tower[3], enemy[0][0])&& tower[3].y<680 )
            {
                al_draw_bitmap(fire.bullet_ip, fire.x, fire.y, 0);
            }
            if(fire_exp)
            {
                al_draw_bitmap(fire.explo_ip, (enemy[0][0].x/8)*7, (enemy[0][0].y/8)*7, 0);
                fire_exp = false;
            }

            //Shot poison_bullet
            if(shot(tower[0], enemy[0][0])&& tower[0].y<680)
            {
                al_draw_bitmap(poison.bullet_ip, poison.x, poison.y, 0);
            }
            if(poison_exp)
            {
                al_draw_bitmap(poison.explo_ip, (enemy[0][0].x/8)*7, (enemy[0][0].y/8)*7, 0);
                poison_exp = false;
            }

            //Shot heal_bullet
            if(shot(tower[1], enemy[0][0])&& tower[1].y<680)
            {
                al_draw_bitmap(heal.bullet_ip, heal.x, heal.y, 0);
            }
            if(heal_exp)
            {
                al_draw_bitmap(heal.explo_ip, (enemy[0][0].x/8)*7, (enemy[0][0].y/8)*7, 0);
                heal_exp = false;
            }

            //Shot punch_bullet
            if(shot(tower[4], enemy[0][0])&& tower[4].y<680)
            {
                al_draw_bitmap(punch.bullet_ip, punch.x, punch.y, 0);
            }
            if(punch_exp)
            {
                al_draw_bitmap(punch.explo_ip, (enemy[0][0].x/8)*7, (enemy[0][0].y/8)*7, 0);
                punch_exp = false;
            }

            //Shot light_bullet
            if(shot(tower[2], enemy[0][0])&& tower[2].y<680)
            {
                al_draw_bitmap(light.bullet_ip, light.x, light.y, 0);
            }
            if(light_exp)
            {
                al_draw_bitmap(light.explo_ip, (enemy[0][0].x/8)*7, (enemy[0][0].y/8)*7, 0);
                light_exp = false;
            }

            //enemy
            al_draw_bitmap(enemy[0][0].ip, enemy[0][0].x, enemy[0][0].y, 0);

            //Gameover
            if(flag == 8)
            {
                game_over = al_load_bitmap("gameover-remove.png");
                re_or_leave = al_load_bitmap("re_or_lea.png");
                window = 5;
            }
            //Win
            if(flag == 100)
            {
                re_or_leave = al_load_bitmap("re_or_lea.png");
                win = al_load_bitmap("victory.png");
                window = 6;
            }
            al_flip_display();
            al_clear_to_color(al_map_rgb(255, 255, 255));
        }

        // Listening for new event
        if (!al_is_event_queue_empty(event_queue))
        {
            error = process_event();
        }

    }
    //Instruction
    if(window == 4)
    {
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_bitmap(instruction, 0, -175, 0);
        al_draw_bitmap(back_to_menu, 100, 450, 0);
        al_flip_display();
        // Listening for new event
        if (!al_is_event_queue_empty(event_queue))
        {
            error = process_event();
        }
        //Menu
        if((click == true )&&(mouse_y>650)&&(mouse_y<750))
        {
            window = 2;
        }
    }
    //Game Over
    if(window ==5)
    {
        al_clear_to_color(al_map_rgb(255, 255, 255));
        al_draw_bitmap(menu, 60, 0, 0);
        al_draw_bitmap(game_over, width/2-260, 50, 0);
        al_draw_bitmap(re_or_leave, 100, 400, 0);
        al_flip_display();
        // Listening for new event
        if (!al_is_event_queue_empty(event_queue))
        {
            error = process_event();
        }
        //Restart
        if((click == true)&&(mouse_y>550)&&(mouse_y<650)&&(mouse_x>800)&&(mouse_x<1000))
        {
            flag =1;
            enemy[0][0].x =195, enemy[0][0].y = -50;
            tower[0].x = 250, tower[0].y = 683;
            tower[1].x = 380, tower[1].y = 683;
            tower[2].x = 510, tower[2].y = 683;
            tower[3].x = 640, tower[3].y = 683;
            tower[4].x = 770, tower[4].y = 683;
            tower[5].x = 900, tower[5].y = 683;
            window = 2;
        }
        if((click == true)&&(mouse_y>550)&&(mouse_y<650)&&(mouse_x>200)&&(mouse_x<400))
        {
            return GAME_TERMINATE;
        }
    }
    //Win
    if(window == 6)
    {
        al_clear_to_color(al_map_rgb(255, 255, 255));
        al_draw_bitmap(menu, -100, 0, 0);
        al_draw_bitmap(win, 425, 100, 0);
        al_draw_bitmap(re_or_leave, 100, 400, 0);
        al_flip_display();
        // Listening for new event
        if (!al_is_event_queue_empty(event_queue))
        {
            error = process_event();
        }
        //Restart
        if((click == true)&&(mouse_y>550)&&(mouse_y<650)&&(mouse_x>800)&&(mouse_x<1000))
        {
            flag =1;
            enemy[0][0].x =195, enemy[0][0].y = -50;
            tower[0].x = 250, tower[0].y = 683;
            tower[1].x = 380, tower[1].y = 683;
            tower[2].x = 510, tower[2].y = 683;
            tower[3].x = 640, tower[3].y = 683;
            tower[4].x = 770, tower[4].y = 683;
            tower[5].x = 900, tower[5].y = 683;
            window = 2;
        }
        //Leave
        if((click == true)&&(mouse_y>550)&&(mouse_y<650)&&(mouse_x>200)&&(mouse_x<400))
        {
            return GAME_TERMINATE;
        }
    }
    //換場
    if(window == 7)
    {
        al_start_timer(timer3);
        draw_game_map();

        al_draw_bitmap(heart, 10, 205, 0);
        al_draw_bitmap(checker, 696, 225,0);
        al_draw_bitmap(enemy[0][0].ip, enemy[0][0].x, enemy[0][0].y, 0);
        for(int i =0 ; i<6 ; i++)
        {
            al_draw_bitmap(tower[i].ip, tower[i].x, tower[i].y, 0);
        }
        al_draw_text(font, al_map_rgb(0, 0, 0), stage_x, 300, ALLEGRO_ALIGN_CENTER, "STAGE 1");
        al_flip_display();
        al_clear_to_color(al_map_rgb(255, 255, 255));
        if(stage_x>width)
            window = 3;
        // Listening for new event
        if (!al_is_event_queue_empty(event_queue))
        {
            error = process_event();
        }
    }
    return error;
}

void draw_game_map()
{
    al_draw_bitmap(gamemap, 135, 10, 0);
    al_draw_bitmap(tower[0].ip, 250, 683, 0);
    al_draw_bitmap(tower[1].ip, 380, 683, 0);
    al_draw_bitmap(tower[2].ip, 510, 683, 0);
    al_draw_bitmap(tower[3].ip, 640, 683, 0);
    al_draw_bitmap(tower[4].ip, 770, 683, 0);
    al_draw_bitmap(tower[5].ip, 900, 683, 0);
}
void game_destroy()
{
    al_destroy_display(display);
    al_destroy_bitmap(cover);
    al_destroy_bitmap(menu);
    al_destroy_bitmap(option);
    al_destroy_bitmap(cover_remove);
    al_destroy_bitmap(tower[0].ip);
    al_destroy_bitmap(tower[1].ip);
    al_destroy_bitmap(tower[2].ip);
    al_destroy_bitmap(tower[3].ip);
    al_destroy_bitmap(tower[4].ip);
    al_destroy_bitmap(tower[5].ip);
    al_destroy_bitmap(back_to_menu);
    al_destroy_bitmap(instruction);
    al_destroy_bitmap(win);
    al_destroy_bitmap(menu);
    al_destroy_bitmap(gamemap);
    al_destroy_bitmap(re_or_leave);
    al_destroy_bitmap(enemy[0][0].ip);
    al_destroy_bitmap(checker);
    al_destroy_bitmap(heart);
    al_destroy_bitmap(ice.bullet_ip);
    al_destroy_bitmap(ice.explo_ip);
    al_destroy_bitmap(fire.bullet_ip);
    al_destroy_bitmap(fire.explo_ip);
    al_destroy_bitmap(heal.bullet_ip);
    al_destroy_bitmap(heal.explo_ip);
    al_destroy_bitmap(poison.bullet_ip);
    al_destroy_bitmap(poison.explo_ip);
    al_destroy_bitmap(punch.bullet_ip);
    al_destroy_bitmap(punch.explo_ip);
    al_destroy_bitmap(light.bullet_ip);
    al_destroy_bitmap(light.explo_ip);

    al_destroy_timer(timer);
    al_destroy_timer(timer2);
    al_destroy_timer(timer3);
    al_destroy_timer(timer4);
    al_destroy_timer(timer5);
    al_destroy_timer(timer6);
    al_destroy_timer(timer7);
    al_destroy_timer(timer8);
    al_destroy_timer(timer9);
    al_destroy_font(font);
    al_destroy_font(font2);
    al_destroy_event_queue(event_queue);
    al_destroy_sample(song);
}
int shot(Tower t, Enemy e)
{
    int tx = t.x, ty = t.y, ex = e.x, ey = e.y;
    double distance = sqrt( (pow(tx-ex,2)) + pow(ty-ey, 2));
    if(distance < 300)
        return 1;
    else
        return 0;
}

double dis(Bullet b, Enemy e)
{
    int bx = b.x, by = b.y, ex = e.x, ey = e.y;
    double distance = sqrt( pow(bx-ex, 2)+pow(by-ey, 2) );
    return distance;
}
