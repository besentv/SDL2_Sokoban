#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "menu.h"

//#define _DEBUG

int game_loadMap(mapStruct_t* map, mapStruct_t* lastMap)
{
    map->playerPositionX = -1;
    map->playerPositionY = -1;
    map->totalCrates = 0;
    map->totalTargetSquares = 0;
    map->cratesOnTargetSquares = 0;
    map->playerLookingDirection = UP;
    map->tileSize = 32;
    map->isLoaded = 0;
    map->offsetX = 0;
    map->offsetY = 0;
    map->canUndo = 0;
    map->currentMoveCount = 0;

    FILE *file = fopen(map->mapStr, "r");
    if (file == NULL)
    {
        printf("ERROR %d\n", file);
        return 1;
    }
    map->height = 1;
    int chars = 0;
    char c = fgetc(file);

    while (c != EOF)
    {
        if (c == '\n')
        {
            if (chars % (map->height) != 0)
            {
                fprintf(stderr, "error: Map not rectangular. File: %s at line %d.\n", map->mapStr, map->height);
                fclose(file);
                return 2;
            }
            (map->height)++;
        }
        else if(c == '\r'){
            //Ignore Windows carriage returns.
        }
        else
        {
            chars++;
        }
        c = fgetc(file);
    }

    map->width = chars / (map->height);

    map->mapArray = (uint8_t**)malloc(sizeof(uint8_t*) * (map->width));
    lastMap->mapArray = (uint8_t**)malloc(sizeof(uint8_t*) * (map->width));
    for (uint32_t i = 0; i < map->width; i++)
    {
        map->mapArray[i] = (uint8_t*)malloc(sizeof(uint8_t) * (map->height));
        lastMap->mapArray[i] = (uint8_t*)malloc(sizeof(uint8_t) * (map->height));
    }

    rewind(file);

    c = 0;
    for (uint32_t y = 0; y < map->height; y++)
    {
        uint32_t x = 0;
        for (x = 0; x < map->width; x++)
        {
checkChar:
            c = fgetc(file);

            switch (c)
            {
            case '\r':  //Ignore escape chars.
            case '\n':
                goto checkChar;
                break;
            case PLAYER_ON_TARGET:
                if (map->playerPositionX == -1 && map->playerPositionY == -1)
                {
                    map->playerPositionX = x;
                    map->playerPositionY = y;
                }
                else
                {
                    return 3; // found second player -> invalid map
                }
                (map->totalTargetSquares)++;
                break;
            case PLAYER:
                if (map->playerPositionX == -1 && map->playerPositionY == -1)
                {
                    map->playerPositionX = x;
                    map->playerPositionY = y;
                }
                else
                {
                    return 3; // found second player -> invalid map
                }
                break;
            case CRATE_ON_TARGET:
                map->cratesOnTargetSquares++;
                map->totalCrates++;
                map->totalTargetSquares++;
                break;
            case CRATE_TARGET:
                map->totalTargetSquares++;
                break;
            case CRATE:
                map->totalCrates++;
                break;
            }
            map->mapArray[x][y] = c; // MIRRORED INPUT -> FILE READ LINE BY LINE BUT WE INPUT COLUMN BY COLUMN
        }
        fgetc(file);
    }

    if (map->playerPositionX == -1)
    {
        fprintf(stderr, "error: No player found.\n");
        return 4;
    }

    if (map->totalCrates != map->totalTargetSquares)
    {
        fprintf(stderr, "error: Map has unequal crates and goal squares. %d Crates , %d Squares\n", map->totalCrates, map->totalTargetSquares);
        return 5;
    }

    fclose(file);

    map->isLoaded = 1;
    return 0;
}

void game_mapCopy(mapStruct_t* source, mapStruct_t* target)
{
    uint8_t** tempPtr = target->mapArray;
    *target = *source;
    target->mapArray = tempPtr;

    for (uint32_t i = 0; i < source->width; i++)
    {
        //memcpy_s(target->mapArray[i], sizeof(uint8_t) * (target->height), source->mapArray[i], sizeof(uint8_t) * (source->height));
        memcpy(target->mapArray[i], source->mapArray[i], sizeof(uint8_t) * (source->height));
    }
}

void game_unloadMap(mapStruct_t* map, mapStruct_t* lastMap)
{
    if (map == NULL)
        return;

    for (uint32_t i = 0; i < map->width; i++)
    {
        free(map->mapArray[i]);
    }
    free(map->mapArray);
    map->mapArray = NULL;
}

void game_getMapName(char* mapName, uint16_t mapStrLen, uint32_t currMapNr)
{
    uint16_t offset = 0;
    //strncpy_s(mapName + offset, mapStrLen-1, "./level/map", sizeof("./level/map"));
    strncpy(mapName + offset, "./level/map", sizeof("./level/map"));
    offset += (sizeof("./level/map") - 1);

    char temp[5];
    snprintf(temp,5, "%04d", currMapNr);
    //strncpy_s(mapName + offset, 5, temp, 5);
    strncpy(mapName + offset, temp, 5);
    offset += 4;

    //strncpy_s(mapName + offset, 5, ".txt", 5);
    strncpy(mapName + offset, ".txt", 5);

    for(int i = 0; i<20; i++)
        printf("%x,", mapName[i]);
    printf("\n%s\n", mapName);

}

uint32_t game_handleEvent(mapStruct_t* map, mapStruct_t* lastMap, SDL_Event event)
{
    switch (event.type)
    {
    case SDL_KEYDOWN:
        switch (event.key.keysym.scancode)
        {
        case SDL_SCANCODE_UP:
        case SDL_SCANCODE_W:
        case SDL_SCANCODE_LEFT:
        case SDL_SCANCODE_A:
        case SDL_SCANCODE_DOWN:
        case SDL_SCANCODE_S:
        case SDL_SCANCODE_RIGHT:
        case SDL_SCANCODE_D:
            game_mapCopy(map, lastMap);
            map->canUndo = 1;
            game_moveObject(map, &map->playerPositionX, &map->playerPositionY, event.key.keysym.scancode);
            break;
        case SDL_SCANCODE_R:
            game_unloadMap(map, lastMap);
            game_loadMap(map, lastMap);
#ifdef _DEBUG
            printf("Moves: %d\n\n", map->currentMoveCount);
            game_printMapToConsole(map);
#endif _DEBUG
            break;
        case SDL_SCANCODE_U:
            if (map->canUndo)
            {
                game_mapCopy(lastMap, map);
                map->canUndo = 0;
#ifdef _DEBUG
                printf("Moves: %d\n\n", map->currentMoveCount);
                game_printMapToConsole(map);
#endif _DEBUG
            }
            break;
        case SDL_SCANCODE_1: //Go to next map.
            printf("\nIT'S REWIND TIME\n\n");
            game_unloadMap(map, lastMap);
            map->currMapNum = (map->currMapNum > 0) ? (map->currMapNum - 1) : (LEVEL_COUNT - 1);
            game_getMapName(map->mapStr, map->mapStrLen, map->currMapNum);
            while (game_loadMap(map, lastMap))
            {
                fprintf(stderr, "Error: could not load map %s.\nskipping to previous map!\n", map->mapStr);
                map->currMapNum = (map->currMapNum - 1) % LEVEL_COUNT;
                game_getMapName(map->mapStr, map->mapStrLen, map->currMapNum);
            }
            break;
            break;
        case SDL_SCANCODE_ESCAPE:
            return MENU_SHOWN;
            break;
        case SDL_SCANCODE_2: //Go to prev map.
            printf("\nSKIP\n\n");
            game_unloadMap(map, lastMap);
            map->currMapNum = (map->currMapNum + 1) % LEVEL_COUNT;
            game_getMapName(map->mapStr, map->mapStrLen, map->currMapNum);
            while (game_loadMap(map, lastMap))
            {
                fprintf(stderr, "Error: could not load map %s.\nskipping to next map!\n", map->mapStr);
                map->currMapNum = (map->currMapNum + 1) % LEVEL_COUNT;
                game_getMapName(map->mapStr, map->mapStrLen, map->currMapNum);
            }
            break;
        default:
            break;
        }
        if (map->totalCrates == map->cratesOnTargetSquares && map->totalCrates != 0)
        {
            printf("WIN\n");
            game_unloadMap(map, lastMap);
            map->currMapNum = (map->currMapNum + 1) % LEVEL_COUNT;
            game_getMapName(map->mapStr, map->mapStrLen, map->currMapNum);
            while (game_loadMap(map, lastMap))
            {
                fprintf(stderr, "Error: could not load map %s.\nskipping to next map.", map->mapStr);
                map->currMapNum = (map->currMapNum + 1) % LEVEL_COUNT;
                game_getMapName(map->mapStr, map->mapStrLen, map->currMapNum);
            }
        }
        if (!map->isLoaded)
        {
            fprintf(stderr, "Error: invalid map. Exiting...\n");
            return 1;
        }
        break;
    default:
        break;
    }
    return GAME_SHOWN;
}


uint32_t game_moveObject(mapStruct_t* map, uint32_t* objectX, uint32_t* objectY, SDL_Scancode button)
{
    if (map == NULL)
        return 0;

    int8_t xOffset = 0;
    int8_t yOffset = 0;
    switch (button)
    {
    case SDL_SCANCODE_UP:
    case SDL_SCANCODE_W:
        yOffset = -1;
        map->playerLookingDirection = UP;
        break;
    case SDL_SCANCODE_DOWN:
    case SDL_SCANCODE_S:
        yOffset = 1;
        map->playerLookingDirection = DOWN;
        break;
    case SDL_SCANCODE_LEFT:
    case SDL_SCANCODE_A:
        xOffset = -1;
        map->playerLookingDirection = LEFT;
        break;
    case SDL_SCANCODE_RIGHT:
    case SDL_SCANCODE_D:
        xOffset = 1;
        map->playerLookingDirection = RIGHT;
        break;
    }

    if (*objectX + xOffset < map->width && *objectX + xOffset >= 0 && *objectY + yOffset < map->height && *objectY + yOffset >= 0)
    {
        uint8_t isPlayer;
        if (map->mapArray[*objectX][*objectY] == PLAYER || map->mapArray[*objectX][*objectY] == PLAYER_ON_TARGET)
        {
            isPlayer = 1;
        }
        else
        {
            isPlayer = 0;
        }
        switch (map->mapArray[*objectX + xOffset][*objectY + yOffset])
        {
        case WALL:
            return -1;
        case CRATE_ON_TARGET:
        case CRATE:
            if (isPlayer)
            {
                uint32_t crateX = (*objectX) + xOffset;
                uint32_t crateY = (*objectY) + yOffset;
                if (game_moveObject(map, &crateX, &crateY, button))
                {
                    return 1;
                }
            }
            else
            {
                return 1;
            }
            break;
        case CRATE_TARGET:
            if (isPlayer)
            {
                map->mapArray[*objectX + xOffset][*objectY + yOffset] = PLAYER_ON_TARGET;
            }
            else
            {
                map->mapArray[*objectX + xOffset][*objectY + yOffset] = CRATE_ON_TARGET;
                map->cratesOnTargetSquares++;
            }
            break;
        case EMPTY_FIELD:
            map->mapArray[*objectX + xOffset][*objectY + yOffset] = isPlayer ? PLAYER : CRATE;
            break;
        }
        if (isPlayer)
        {
            if (map->mapArray[*objectX][*objectY] == PLAYER_ON_TARGET)
            {
                map->mapArray[*objectX][*objectY] = CRATE_TARGET;
            }
            else
            {
                map->mapArray[*objectX][*objectY] = EMPTY_FIELD;
            }
        }
        else
        {
            if (map->mapArray[*objectX][*objectY] == CRATE_ON_TARGET)
            {
                map->mapArray[*objectX][*objectY] = PLAYER_ON_TARGET;
                map->cratesOnTargetSquares--;
            }
            else
            {
                map->mapArray[*objectX][*objectY] = PLAYER;
            }
        }
        (*objectX) += xOffset;
        (*objectY) += yOffset;
        if (isPlayer)
        {
            map->currentMoveCount++;
#ifdef _DEBUG
            printf("Moves: %d\n\n", map->currentMoveCount);
            game_printMapToConsole(map);
#endif _DEBUG
        }
        return 0;
    }
    return 1;
}

void game_printMapToConsole(mapStruct_t* map){
    for (uint32_t i = 0; i < map->height; i++)
    {
        uint32_t j = 0;
        for (j = 0; j < map->width; j++)
        {
            printf("%c", map->mapArray[j][i]);
        }
        printf("\n");
    }
    printf("\n");
}
