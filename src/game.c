#include <SDL.h>
#include "simple_logger.h"

#include "gfc_input.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"

#include "bug.h"
#include "camera.h"
#include "entity.h"
#include "player.h"
#include "world.h"

Uint8 _DRAWBOUNDS = 0;

void parse_args(int argc, char* argv[]);

int main(int argc, char * argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;
    World* world;

    int mx,my;
    float mf = 0;
    Sprite *mouse;
    GFC_Color mouseGFC_Color = gfc_color8(216,2,0,255);
    
    Entity* player;
    Entity* bug;

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
    gf2d_graphics_set_frame_delay(16);
    gf2d_sprite_init(1024);
    entity_system_init(128); 
    SDL_ShowCursor(SDL_DISABLE);
    camera_set_size(gfc_vector2d(1280, 720));
    
    /*demo setup*/
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16,0);
    player = player_new(gfc_vector2d(20, 20));
    bug = bug_new(gfc_vector2d(200, 200));
    world = world_load("worlds/testworld.world");

    slog("press [escape] to quit");

    /*main game loop*/
    while(!done)
    {
        gfc_input_update(); // update SDL's internal event structures and input data
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)mf = 0;
        
        entity_system_think_all();
        entity_system_update_all();

        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            world_draw(world);

            //draw entities next
            entity_system_draw_all();

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
        
        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
        //slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    entity_system_free_all();
    world_free(world);
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
