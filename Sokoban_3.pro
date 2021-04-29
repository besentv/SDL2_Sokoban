TEMPLATE = app
CONFIG += desktop
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    src/game.c \
    src/main.c \
    src/menu.c \
    src/renderer.c

HEADERS += \
    include/game.h \
    include/menu.h \
    include/renderer.h

INCLUDEPATH += include/

DISTFILES += \
    level/map0000.txt \
    level/map0001.txt \
    level/map0002.txt \
    level/map0003.txt \
    level/map0004.txt \
    level/map0005.txt \
    level/map0006.txt \
    level/map0007.txt \
    level/map0008.txt \
    level/map0009.txt \
    level/map0010.txt \
    level/map0011.txt \
    level/map0012.txt \
    level/map0013.txt \
    level/map0014.txt \
    level/map0015.txt \
    level/map0016.txt \
    level/map0017.txt \
    level/map0018.txt \
    level/map0019.txt \
    level/map0020.txt \
    level/map0021.txt \
    level/map0022.txt \
    level/map0023.txt \
    level/map0024.txt \
    level/map0025.txt \
    level/map0026.txt \
    level/map0027.txt \
    level/map0028.txt \
    level/map0029.txt \
    textures/DejaVuSansMono.ttf \
    textures/_stone.png \
    textures/crate.png \
    textures/crate_on_target.png \
    textures/goal.png \
    textures/main_menu.png \
    textures/main_menu_exit.png \
    textures/main_menu_play.png \
    textures/main_menu_settings.png \
    textures/player.png \
    textures/stone.png \
    textures/stone_7.png \
    textures/test.png \
    textures/test_.png \
    textures/unused/_stone.png \
    textures/unused/main_menu_settings.png \
    textures/unused/stone_7.png \
    textures/unused/test.png \
    textures/unused/test_.png

unix:!macx: LIBS += -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
