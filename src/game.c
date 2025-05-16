#include <SDL.h>
#include "simple_logger.h"

#include "gfc_audio.h"
#include "gfc_input.h"
#include "gfc_config_def.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"

#include "camera.h"
#include "entity.h"
#include "font.h"
#include "particle.h"
#include "player.h"
#include "spawn.h"
#include "windows.h"
#include "world.h"

#include "bug.h"
#include "bullet_bill.h"
#include "coin.h"
#include "brick.h"
#include "ice.h"
#include "question_block.h"
#include "platform.h"
#include "conveyor.h"
#include "trampoline.h"
#include "p_switch.h"
#include "one_way_wall.h"

Uint8 _DRAWBOUNDS = 0;
Uint32 ms;
Uint32 last_ms = 0;
int mx, my;
int game_done;
int in_main_menu;
void parse_args(int argc, char* argv[]);

int main(int argc, char * argv[])
{
    /*variable declarations*/
    game_done = 0;
    in_main_menu = 1;
    const Uint8 * keys;

    float mf = 0;
    Sprite *mouse;
    GFC_Color mouseGFC_Color = gfc_color8(216,2,0,255);
    Mix_Music* background_music;
    Window* pause_menu;
    Window* main_menu;
    /*
    Entity* player;

    Entity* bug;
    Entity* bug2;
    Entity* bullet_bill;

    Entity* coin;
    Entity* brick;
    Entity* ice1;
    Entity* ice2;

    Entity* question_block;
    Entity* invisible_question_block;
    Entity* platform;
    Entity* conveyor;

    Entity* trampoline;
    Entity* p_switch;
    Entity* one_way_wall;
    */
    char formatted_string[100];

    PlayerEntityData* data = NULL;

    /*program initializtion*/
    init_logger("gf2d.log",0);
    parse_args(argc, argv); //start the game in some other mode
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        1280,
        720,
        1280,
        720,
        gfc_vector4d(0,0,0,255),
        0);
    gfc_input_init("config/input.cfg");
    gfc_audio_init(128, 2, 2, 2, 1, 1);
    gf2d_graphics_set_frame_delay(16);
    gf2d_sprite_init(1024);
    font_init();
    gfc_config_def_init();
    particle_system_init(10000);
    entity_system_init(256); 
    windows_init(10);

    SDL_ShowCursor(SDL_DISABLE);
    camera_set_size(gfc_vector2d(1280, 720));
    
    /*load resource files*/
    gfc_config_def_load("def/particles.def");
    gfc_config_def_load("def/entities.def");

    /*demo setup*/
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16,0);
    /*
    player = player_new(gfc_vector2d(20, 20));
    bug = bug_new(gfc_vector2d(200, 200));
    bug2 = bug2_new(gfc_vector2d(300, 200));
    bullet_bill = bullet_bill_new(gfc_vector2d(0, 550));
    platform = platform_new(gfc_vector2d(850, 900));

    coin = coin_new(gfc_vector2d(900, 900));
    brick = brick_new(gfc_vector2d(950, 900));
    ice1 = ice_new(gfc_vector2d(1000, 900));
    ice2 = ice_new(gfc_vector2d(1050, 900));

    question_block = question_block_new(gfc_vector2d(1100, 900));
    invisible_question_block = invisible_question_block_new(gfc_vector2d(1150, 900));
    conveyor = conveyor_new(gfc_vector2d(1200, 900));
    trampoline = trampoline_new(gfc_vector2d(1250, 900));
    p_switch = p_switch_new(gfc_vector2d(1350, 900));
    one_way_wall = one_way_wall_new(gfc_vector2d(1400, 900));
    */

    world_load("worlds/world_save.world");

    background_music = gfc_sound_load_music("audio/00. The Tale of a Cruel World.wav");
    gfc_sound_load("audio/Wilhelm.wav", 1, 2);

    if (entity_get_player()) {
        data = (PlayerEntityData*)entity_get_player()->data;
    }
    main_menu = window_new_from_file("windows/main_menu.window");
    pause_menu = window_new_from_file("windows/pause_menu.window");
    slog("press [escape] to quit");
    if (background_music) {
        Mix_PlayMusic(background_music, -1);
    }
    /*main game loop*/
    while(!game_done)
    {
        gfc_input_update(); // update SDL's internal event structures and input data
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        font_cleanup();
        /*update things here*/
        ms = SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0) {
            mf = 0;
        }
        
        //check window stuff
        if (gfc_input_command_pressed("toggle_window")) {
            if (pause_menu->visible) {
                pause_menu->visible = 0;
            }
            else {
                pause_menu->visible = 1;
            }
        }
         
        
        /**
        * Test Stuff for Level Editor and Particles
        if (ms & SDL_BUTTON_X2MASK && !(last_ms & SDL_BUTTON_X2MASK)) { //this code just checks if the mouse has been pressed this frame (wasn't pressed last frame)
            particles_from_def("eruption", 100, gfc_vector2d(mx, my), gfc_vector2d(0, -1), gfc_vector2d(0, 0.1));
            spawn_entity_to_world_gridlock(gfc_vector2d_added(gfc_vector2d(mx, my),camera_get_position()), "brick", NULL);
        }
        */
        if (ms & SDL_BUTTON_LMASK && !(last_ms & SDL_BUTTON_LMASK)) { //this code just checks if the mouse has been pressed this frame (wasn't pressed last frame)
            world_spawn_tile(gfc_vector2d_added(gfc_vector2d(mx, my), camera_get_position()), 3);
        }
        
        if (!pause_menu->visible) {
            entity_system_think_all();
            entity_system_update_all();
        }
        else {
            windows_system_update_all();
        }

        if (data) {
            sprintf(formatted_string, "Coins: %d\nLives: %d", data->coin_count, data->lives_count);
        }

        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first

            world_draw_background(world_get_active());

            particle_system_draw();

            world_draw_tile_layer(world_get_active());

            //draw entities next
            entity_system_draw_all();

            font_draw_test(formatted_string, FS_medium, GFC_COLOR_WHITE, gfc_vector2d(10, 10));

            window_system_draw_all();

            //UI elements last
            gf2d_sprite_draw(
                mouse,
                gfc_vector2d(mx,my),
                NULL,
                NULL,
                NULL,
                NULL,
                &mouseGFC_Color,
                (int)mf);

        gf2d_graphics_next_frame();// render current draw frame and skip to the next frame
        
        if (keys[SDL_SCANCODE_ESCAPE])game_done = 1; // exit condition
        //slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
        last_ms = ms;
        if (gfc_input_command_pressed("next_world")) {
            world_switch_to_next();
        }
        if (gfc_input_command_pressed("give_coins")) {
            data->coin_count += 50;
        }
    }
    Mix_FreeMusic(background_music);
    world_save(world_get_active(), "worlds/world_save1.world");
    entity_system_free_all();
    world_free(world_get_active());
    slog("---==== END ====---");
    return 0;
}

void parse_args(int argc, char* argv[]) {
    int i;
    if (argc < 2) {
        return;
    }
    for (i = 1; i < argc; i++) {
        if (gfc_strlcmp("--drawbounds", argv[i]) == 0) { //draw bounding boxes
            _DRAWBOUNDS = 1;
            slog("Bounding Boxes will be drawn in this instance");
        }
    }
}

/*eol@eof*/
