#pragma once

#include <SDL2/SDL_ttf.h>

extern SDL_Texture* gStoneTexture;
extern SDL_Texture* gCrateTexture;
extern SDL_Texture* gGoalTexture;
extern SDL_Texture* gCrateOnTargetTexture;
extern SDL_Texture* gPlayerTexture;
extern SDL_Texture* gMainMenuBackground;
extern SDL_Texture* gMainMenuPlayButton;
extern SDL_Texture* gMainMenuExitButton;
extern SDL_Texture* gFontTexture;
extern TTF_Font* gFont;


uint32_t renderer_drawMap(SDL_Renderer* renderer, SDL_Window* window, mapStruct_t* map);
void renderer_calculateTileSize(SDL_Window* window, mapStruct_t* map);
uint32_t renderer_loadTexture(SDL_Texture** texture, SDL_Renderer* renderer, const char* path);
uint32_t renderer_loadFont(const char* path, uint32_t size);
uint32_t renderer_paintTexturePercentage(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, float_t px, float_t py, float_t pw, float_t ph);
void renderer_loadTextTexture(SDL_Window* window, SDL_Renderer* renderer, const char* text, SDL_Color color);
