#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "renderer.h"

SDL_Texture* gStoneTexture = NULL;
SDL_Texture* gCrateTexture = NULL;
SDL_Texture* gGoalTexture = NULL;
SDL_Texture* gCrateOnTargetTexture = NULL;
SDL_Texture* gPlayerTexture = NULL;
SDL_Texture* gMainMenuBackground = NULL;
SDL_Texture* gMainMenuPlayButton = NULL;
SDL_Texture* gMainMenuExitButton = NULL;
SDL_Texture* gFontTexture = NULL;
TTF_Font* gFont = NULL;



uint32_t renderer_drawMap(SDL_Renderer* renderer, SDL_Window* window, mapStruct_t* map)
{
    if (map == NULL)
    {
        return 1;
    }

    uint32_t windowHeight = 0;
    uint32_t windowWidth = 0;

    SDL_Rect rect;
    rect.h = map->tileSize;
    rect.w = map->tileSize;
    rect.x = 0;
    rect.y = 0;

    double playerAngle = 0;

    switch (map->playerLookingDirection)
    {
    case UP:
        playerAngle = 0.0;
        break;
    case DOWN:
        playerAngle = 180.0;
        break;
    case LEFT:
        playerAngle = 270.0;
        break;
    case RIGHT:
        playerAngle = 90.0;
        break;
    }

    for (uint32_t y = 0; y < map->height; y++) {
        rect.y = map->tileSize * (y);

        for (uint32_t x = 0; x < map->width; x++) {
            rect.x = map->tileSize * (x);
            switch (map->mapArray[x][y])
            {
            case WALL:
                if (SDL_RenderCopy(renderer, gStoneTexture, NULL, &rect) != 0) {
                    fprintf(stderr, "%s\n", SDL_GetError());
                }
                break;
            case PLAYER_ON_TARGET:
            case PLAYER:
                if (SDL_RenderCopyEx(renderer, gPlayerTexture, NULL, &rect, playerAngle, NULL, SDL_FLIP_NONE) != 0) {
                    fprintf(stderr, "%s\n", SDL_GetError());
                }
                break;
            case CRATE_TARGET:
                if (SDL_RenderCopy(renderer, gGoalTexture, NULL, &rect) != 0) {
                    fprintf(stderr, "%s\n", SDL_GetError());
                }
                break;
            case CRATE:
                if (SDL_RenderCopy(renderer, gCrateTexture, NULL, &rect) != 0) {
                    fprintf(stderr, "%s\n", SDL_GetError());
                }
                break;
            case CRATE_ON_TARGET:
                if (SDL_RenderCopy(renderer, gCrateOnTargetTexture, NULL, &rect) != 0) {
                    fprintf(stderr, "%s\n", SDL_GetError());
                }
                break;
            default:
                SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer, &rect);
                break;
            }
        }

    }

    char buf[10];
    snprintf(buf, 10, "%5d", map->currentMoveCount);

    SDL_Color textColor = { 0x00, 0x00 ,0xff ,0xff };
    renderer_loadTextTexture(window, renderer, buf, textColor);
    renderer_paintTexturePercentage(window, renderer, gFontTexture, 0.85f, 0.013f, 0.12f, 0.06f);

    return 0;
}

void renderer_calculateTileSize(SDL_Window* window, mapStruct_t* map)
{
    uint32_t winSizeH = 0;
    uint32_t winSizeW = 0;
    SDL_GetWindowSize(window, &winSizeW, &winSizeH);

    uint32_t maxTileSizeX = map->tileSize;
    uint32_t maxTileSizeY = map->tileSize;


    while (map->width * maxTileSizeX > winSizeW) {
        maxTileSizeX -= 4;
    }
    while (map->height * maxTileSizeY > winSizeH) {
        maxTileSizeY -= 4;
    }

    while (map->width * (maxTileSizeX + 4) < winSizeW) {
        maxTileSizeX += 4;
    }
    while (map->height * (maxTileSizeY + 4) < winSizeH) {
        maxTileSizeY += 4;
    }

    map->tileSize = maxTileSizeX > maxTileSizeY ? maxTileSizeY : maxTileSizeX;
}



uint32_t renderer_loadTexture(SDL_Texture** texture, SDL_Renderer* renderer, const char* path)
{

    SDL_Texture* retTexture = NULL;

    SDL_Surface* tempSurface = IMG_Load(path);
    if (tempSurface == NULL) {
        fprintf(stderr, "Could not open image file!!!!!!! %s\n", path);
        return 1;
    }
    else
    {
        *texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
        if (*texture == NULL) {
            fprintf(stderr, "Could not create texture from image surface!!!!!!!\n");
            SDL_FreeSurface(tempSurface);
            return 1;
        }

        SDL_FreeSurface(tempSurface);
    }

    return 0;
}

uint32_t renderer_loadFont(const char* path, uint32_t size)
{
    gFont = TTF_OpenFont(path, size);
    if (gFont == NULL)
    {
        printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
    }
    return 0;
}

void renderer_loadTextTexture(SDL_Window* window, SDL_Renderer* renderer, const char* text, SDL_Color color)
{
    SDL_Surface* surface = TTF_RenderText_Solid(gFont, text, color);
    if (surface == NULL)
    {
        fprintf(stderr, "Could not LOAD TEXT!!!!!!!\n");
        gFontTexture = NULL;
    }
    else
    {
        if (gFontTexture != NULL) {
            SDL_DestroyTexture(gFontTexture);
        }

        gFontTexture = SDL_CreateTextureFromSurface(renderer, surface);
        if (gFontTexture == NULL)
        {
            fprintf(stderr, "Could not LOAD TEXT texture!!!!!!!\n");

        }


        SDL_FreeSurface(surface);
    }
}

uint32_t renderer_paintTexturePercentage(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, float_t px, float_t py, float_t pw, float_t ph)
{
    if (gFontTexture != NULL)
    {
        uint32_t w, h = 0;
        SDL_GetWindowSize(window, &w, &h);

        SDL_Rect menuRectAbsCoords = { (uint32_t) (px * w),(uint32_t) (py * h),(uint32_t) (pw * w),  (uint32_t)(ph * h) };

        SDL_RenderCopy(renderer, texture, NULL, &menuRectAbsCoords);
    }

    return 0;
}
