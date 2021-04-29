#pragma once

#define QUIT 0
#define GAME_SHOWN 1
#define MENU_SHOWN 2

#define MAIN_MENU_ITEM_COUNT 2

typedef struct Menu_Rect
{
    float x;
    float y;
    float w;
    float h;
    SDL_Texture* texture;
}Menu_Rect_t;

void menu_draw(SDL_Window* window, SDL_Renderer* renderer, Menu_Rect_t* menuItems, uint32_t items);
uint32_t menu_checkMouseColi(SDL_Window* window, Menu_Rect_t* menuItems, uint32_t itemAmount, uint32_t x, uint32_t y);
