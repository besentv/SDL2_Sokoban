
#include <SDL2/SDL.h>

#include "game.h"
#include "menu.h"
#include "renderer.h"


void menu_draw(SDL_Window* window, SDL_Renderer* renderer, Menu_Rect_t* menuItems, uint32_t itemAmount) {

    uint32_t w, h = 0;
    SDL_GetWindowSize(window, &w, &h);

    SDL_Rect r = { 0,0 , w ,h };
    SDL_RenderCopy(renderer, gMainMenuBackground, NULL, &r);

    for (uint32_t i = 0; i < itemAmount; i++)
    {
        SDL_Rect menuRectAbsCoords = { (int) (menuItems[i].x * w), (int) (menuItems[i].y * h), (int) (menuItems[i].w * w), (int) (menuItems[i].h * h) };

        SDL_RenderCopy(renderer, menuItems[i].texture, NULL, &menuRectAbsCoords);
    }
}

uint32_t menu_checkMouseColi(SDL_Window* window, Menu_Rect_t* menuItems, uint32_t itemAmount, uint32_t x, uint32_t y)
{

    uint32_t windW, windH = 0;

    SDL_GetWindowSize(window, &windW, &windH);

    SDL_Rect clickRect = { x,y,1,1 };
    SDL_Rect res;

    for (uint32_t i = 0; i < itemAmount; i++)
    {
        SDL_Rect menuRectAbsCoords = { (int) (menuItems[i].x * windW), (int) (menuItems[i].y * windH), (int) (menuItems[i].w * windW), (int) (menuItems[i].h * windH) };
        if (SDL_IntersectRect(&clickRect, &menuRectAbsCoords, &res))
        {
            return i;
        }

    }
    return -1;
}
