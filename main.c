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
    SDL_Surface *image = SDL_LoadBMP("test.bmp");
    if (!image)
    {
        printf("Unable to load image %s! SDL Error: %s\n", image, SDL_GetError());
        SDL_Delay(30000);
        return 1;
    }
    SDL_Surface *image2 = SDL_LoadBMP("test.bmp");

    //Clear Background
    Uint32 white = SDL_MapRGB(image->format, 255, 255, 255);
    SDL_FillRect(screen, NULL, white);

    //Compute scale to fit window.
    float scale_x = (float)screen->w / image->w;
    float scale_y = (float)screen->h / image->h;
    float scale = scale_x < scale_y ? scale_x : scale_y;

    //Scaled size.
    int draw_w = (int)(image->w * scale);
    int draw_h = (int)(image->h * scale);

    //Centering the image.
    SDL_Rect dst;
    dst.w = image->w;
    dst.h = image->h;
    dst.x = (screen->w - image->w) / 2;
    dst.y = (screen->h - image->h) / 2;

    //Drawing image
    SDL_BlitSurface(image, NULL, screen, &dst);
    SDL_UpdateWindowSurface(pwindow);
    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = 0;
                }
            }
        }
        SDL_Delay(16); // ~60 FPS, prevents CPU burn
    }
    SDL_FreeSurface(image);
    SDL_DestroyWindow(pwindow);
    SDL_Quit();
    return 0;


}
