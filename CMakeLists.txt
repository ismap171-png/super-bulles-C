cmake_minimum_required(VERSION 3.20)
project(SuperBulles C)

set(CMAKE_C_STANDARD 11)

# -----------------------------------------------
# Sources
# -----------------------------------------------
set(SOURCES
    src/main.c
    src/joueur.c
    src/bulle.c
    src/projectile.c
    src/eclair.c
    src/collision.c
    src/niveau.c
    src/boss.c
    src/sauvegarde.c
    src/affichage.c
    src/ihm.c
    src/ressources.c
)

add_executable(SuperBulles ${SOURCES})
target_include_directories(SuperBulles PRIVATE include)

# -----------------------------------------------
# Allegro 4 – adapter selon votre OS
# -----------------------------------------------

if(WIN32)
    # Windows avec MSYS2/MinGW :
    # Installez d'abord : pacman -S mingw-w64-x86_64-allegro
    target_link_libraries(SuperBulles alleg)
elseif(APPLE)
    # macOS : brew install allegro
    find_library(ALLEGRO_LIB alleg HINTS /usr/local/lib /opt/homebrew/lib)
    target_link_libraries(SuperBulles ${ALLEGRO_LIB} m)
else()
    # Linux : sudo apt-get install liballegro4-dev
    target_link_libraries(SuperBulles alleg m)
endif()

# Copier les assets dans le dossier de build
add_custom_command(TARGET SuperBulles POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:SuperBulles>/assets
    COMMENT "Copie des assets..."
)
