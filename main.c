#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

void draw_image(SDL_Window *window, SDL_Surface *image, float zoom)
{
    SDL_Surface *screen = SDL_GetWindowSurface(window);
    Uint32 white = SDL_MapRGB(screen->format, 255, 255, 255);
    SDL_FillRect(screen, NULL, white);

    float scale_x = (float)screen->w / image->w;
    float scale_y = (float)screen->h / image->h;
    float fit_scale = scale_x < scale_y ? scale_x : scale_y;
    float scale = fit_scale * zoom;

    int draw_w = (int)(image->w * scale);
    int draw_h = (int)(image->h * scale);

    SDL_Rect dst;
    dst.w = draw_w;
    dst.h = draw_h;
    dst.x = (screen->w - draw_w) / 2;
    dst.y = (screen->h - draw_h) / 2;

    SDL_BlitScaled(image, NULL, screen, &dst);
    SDL_UpdateWindowSurface(window);
}


int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    float zoom = 1.0f;
    SDL_Window *pwindow = SDL_CreateWindow(
        "Imgbuf",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        900,
        600,
        SDL_WINDOW_RESIZABLE
    );

    SDL_Surface *image = SDL_LoadBMP("test.bmp");
    if (!image)
    {
        printf("Unable to load image! SDL Error: %s\n", SDL_GetError());
        SDL_Delay(30000);
        return 1;
    }

    draw_image(pwindow, image,zoom);
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
                else if (
    event.key.keysym.sym == SDLK_KP_PLUS ||
    event.key.keysym.sym == SDLK_EQUALS  ||
    event.key.keysym.sym == SDLK_PLUS
)
                {
                    zoom *= 1.1f;
                    if (zoom > 10.0f) zoom = 10.0f;
                    draw_image(pwindow, image, zoom);
                }
                else if (
                    event.key.keysym.sym == SDLK_KP_MINUS ||
                    event.key.keysym.sym == SDLK_MINUS
                )
                {
                    zoom /= 1.1f;
                    if (zoom < 0.1f) zoom = 0.1f;
                    draw_image(pwindow, image, zoom);
                }

            }
            else if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    draw_image(pwindow, image,zoom);
                }
            }
            else if (event.type == SDL_MOUSEWHEEL)
            {
                if (event.wheel.y > 0) {        // scroll up
                    zoom *= 1.1f;
                }
                else if (event.wheel.y < 0) {   // scroll down
                    zoom /= 1.1f;
                }

                // clamp zoom
                if (zoom < 0.1f) zoom = 0.1f;
                if (zoom > 10.0f) zoom = 10.0f;

                draw_image(pwindow, image, zoom);
            }

        }

        SDL_Delay(16); // ~60 FPS, prevents CPU burn
    }
    SDL_FreeSurface(image);
    SDL_DestroyWindow(pwindow);
    SDL_Quit();
    return 0;


}
