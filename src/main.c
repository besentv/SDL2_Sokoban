#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "game.h"
#include "renderer.h"
#include "menu.h"

uint8_t initSDL(SDL_Window** window, const uint32_t w, const uint32_t h, SDL_Renderer** renderer);

int main(int argc, char* args[])
{
    mapStruct_t map;
    map.currMapNum = 0;
    map.mapStrLen = 25;
    //memcpy_s(map.mapStr, map.mapStrLen, "./level/map0000.txt", 20);
    memcpy(map.mapStr, "./level/map0000.txt", 20);
    map.isLoaded = 0;

    mapStruct_t lastMap;

    const uint32_t WINDOW_WIDTH = 640;
    const uint32_t WINDOW_HEIGHT = 480;
    const uint32_t tileSize = 32;

    SDL_Window* window = NULL;
    SDL_Renderer*  renderer = NULL;

    if (initSDL(&window, WINDOW_WIDTH, WINDOW_HEIGHT, &renderer) != 0) { //INIT SDL
        return 1;
    }

    int running = MENU_SHOWN; //1 Game run, 2 menu run

    while (game_loadMap(&map, &lastMap))
    {
        fprintf(stderr, "Error: could not load map %s.\nskipping to next map!\n", map.mapStr);
        map.currMapNum = (map.currMapNum + 1) % LEVEL_COUNT;
        game_getMapName(map.mapStr, map.mapStrLen, map.currMapNum);
    }

    game_mapCopy(&map, &lastMap);

    renderer_loadTexture(&gStoneTexture, renderer, "./textures/stone.png");
    renderer_loadTexture(&gCrateTexture, renderer, "./textures/crate.png");
    renderer_loadTexture(&gGoalTexture, renderer, "./textures/goal.png");
    renderer_loadTexture(&gCrateOnTargetTexture, renderer, "./textures/crate_on_target.png");
    renderer_loadTexture(&gPlayerTexture, renderer, "./textures/player.png");
    renderer_loadTexture(&gMainMenuBackground, renderer, "./textures/main_menu.png");
    renderer_loadTexture(&gMainMenuPlayButton, renderer, "./textures/main_menu_play.png");
    renderer_loadTexture(&gMainMenuExitButton, renderer, "./textures/main_menu_exit.png");
    renderer_loadFont("./textures/DejaVuSansMono.ttf", 150);

    /*MAIN MENU BUTTONS*/
    Menu_Rect_t mainMenuClikcables[MAIN_MENU_ITEM_COUNT];
    Menu_Rect_t buttonPlay = { 0.3f,0.4f, 0.4f, 0.2f, gMainMenuPlayButton};
    Menu_Rect_t buttonExit = { 0.0f,0.85f, 0.20f, 0.10f, gMainMenuExitButton };
    mainMenuClikcables[0] = buttonPlay;
    mainMenuClikcables[1] = buttonExit;

    const uint32_t FPS = 60;
    //uint32_t frameCount = 0;
    uint32_t ticksAtBeginningCurrentFrame = 0;

    SDL_Event e;
    while (running) {
        ticksAtBeginningCurrentFrame = SDL_GetTicks();
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                running = QUIT;
                break;
            }

            switch (running)
            {
            case GAME_SHOWN:
                running = game_handleEvent(&map, &lastMap, e);
                break;

            case MENU_SHOWN:
                if (e.type == SDL_MOUSEBUTTONDOWN)
                {
                    switch (menu_checkMouseColi(window, mainMenuClikcables, MAIN_MENU_ITEM_COUNT, e.button.x, e.button.y))
                    {
                    case 0:
                        running = GAME_SHOWN;
                        break;

                    case 1:
                        running = QUIT;
                        break;
                    }
                }
                break;

            default:
                break;
            }
        }
        switch (running)
        {
        case GAME_SHOWN:
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(renderer);
            renderer_calculateTileSize(window, &map);
            renderer_drawMap(renderer, window, &map);
            SDL_RenderPresent(renderer);
            break;

        case MENU_SHOWN:
            menu_draw(window, renderer, mainMenuClikcables, MAIN_MENU_ITEM_COUNT);
            SDL_RenderPresent(renderer);
            break;

        default:
            break;
        }

        //++frameCount;

        uint32_t deltaFrameTime = (SDL_GetTicks() - ticksAtBeginningCurrentFrame);
        //printf("Frame time %dms \n", deltaFrameTime);
        //printf("\x1b[A");
        if (deltaFrameTime < 1000.0f / (float_t)FPS)
        {
            SDL_Delay((uint32_t) ((1000.0f / (float_t)FPS) - deltaFrameTime));
        }
    }

    TTF_CloseFont(gFont);

    SDL_DestroyTexture(gStoneTexture);
    SDL_DestroyTexture(gCrateTexture);
    SDL_DestroyTexture(gGoalTexture);
    SDL_DestroyTexture(gCrateOnTargetTexture);
    SDL_DestroyTexture(gPlayerTexture);
    SDL_DestroyTexture(gMainMenuBackground);
    SDL_DestroyTexture(gMainMenuPlayButton);
    SDL_DestroyTexture(gFontTexture);

    game_unloadMap(&map, &lastMap);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}

uint8_t initSDL(SDL_Window** window, const uint32_t w, const uint32_t h, SDL_Renderer** renderer) {

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error in SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    else {
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            printf("Warning: Linear texture filtering not enabled!");
        }

        *window = SDL_CreateWindow("Sokoban", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
        if (*window == NULL) {
            fprintf(stderr, "Error in SDL_CreateWindow: %s\n", SDL_GetError());
            return 1;
        }
        else {

            *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);

            if (*renderer == NULL) {
                fprintf(stderr, "Error in SDL_CreateRenderer: %s\n", SDL_GetError());
                return 1;
            }
            else {
                SDL_SetRenderDrawColor(*renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }

            //Initialize PNG loading
            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) & imgFlags))
            {
                printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                return 1;
            }
            //Init SDL_ttf
            if (TTF_Init() == -1)
            {
                printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
                return 1;
            }

        }
    }
    return 0;
}

