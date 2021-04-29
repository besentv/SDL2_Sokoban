#pragma once

/*
    Map Objects:
*/

#define EMPTY_FIELD ' '
#define PLAYER '@'
#define CRATE '*'
#define CRATE_TARGET '.'
#define PLAYER_ON_TARGET '+'
#define CRATE_ON_TARGET '&'
#define WALL 'X'

#define LEVEL_COUNT 30

typedef enum direction
{
    LEFT = -2,
    DOWN,
    UP = 1,
    RIGHT
} direction_t;


typedef struct mapStruct
{
    uint8_t isLoaded;

    uint32_t currMapNum;
    int mapStrLen;
    char mapStr[25];

    uint8_t** mapArray;

    uint32_t width;
    uint32_t height;
    uint32_t offsetX;
    uint32_t offsetY;
    uint32_t tileSize;

    int32_t playerPositionX;
    int32_t playerPositionY;
    direction_t playerLookingDirection;

    uint32_t cratesOnTargetSquares;
    uint32_t totalCrates;
    uint32_t totalTargetSquares;

    uint8_t canUndo;

    uint32_t currentMoveCount;

} mapStruct_t;

int game_loadMap(mapStruct_t* map, mapStruct_t* lastMap);
void game_unloadMap(mapStruct_t* map, mapStruct_t* lastMap);
void game_getMapName(char* mapName, uint16_t mapStrLen, uint32_t currMapNr);
uint32_t game_moveObject(mapStruct_t* map, uint32_t* objectX, uint32_t* objectY, SDL_Scancode button);
uint32_t game_handleEvent(mapStruct_t* map, mapStruct_t* lastMap, SDL_Event event);
void game_mapCopy(mapStruct_t* source, mapStruct_t* target);
void game_printMapToConsole(mapStruct_t* map);
