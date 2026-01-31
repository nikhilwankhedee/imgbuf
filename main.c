#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *pwindow = SDL_CreateWindow(
        "Imgbuf",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        900,
        600,
        0
    );

    SDL_Surface *screen = SDL_GetWindowSurface(pwindow);
    Uint32 white = SDL_MapRGB(screen->format, 255, 255, 255);
    SDL_FillRect(screen, NULL, white);
    SDL_UpdateWindowSurface(pwindow);

    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }
        SDL_Delay(16); // ~60 FPS, prevents CPU burn
    }

    SDL_DestroyWindow(pwindow);
    SDL_Quit();
    return 0;
}
