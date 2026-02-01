#include <SDL2/SDL.h>
#include <stdio.h>

/* =========================
   UI CONSTANTS
   ========================= */
#define TOOLBAR_HEIGHT 80
#define BTN_SIZE 48
#define BTN_PADDING 16

/* =========================
   ICON DRAWING
   ========================= */
void draw_plus(SDL_Renderer *r, SDL_Rect b) {
    SDL_RenderDrawLine(r, b.x + b.w/2, b.y + 8, b.x + b.w/2, b.y + b.h - 8);
    SDL_RenderDrawLine(r, b.x + 8, b.y + b.h/2, b.x + b.w - 8, b.y + b.h/2);
}

void draw_minus(SDL_Renderer *r, SDL_Rect b) {
    SDL_RenderDrawLine(r, b.x + 8, b.y + b.h/2, b.x + b.w - 8, b.y + b.h/2);
}

void draw_reset(SDL_Renderer *r, SDL_Rect b) {
    SDL_RenderDrawLine(r, b.x + 10, b.y + 10, b.x + b.w - 10, b.y + b.h - 10);
    SDL_RenderDrawLine(r, b.x + b.w - 10, b.y + 10, b.x + 10, b.y + b.h - 10);
}

/* =========================
   RENDER
   ========================= */
void render(
    SDL_Renderer *r,
    SDL_Texture *tex,
    int iw, int ih,
    float zoom, float px, float py,
    SDL_Rect zin, SDL_Rect zout, SDL_Rect reset,
    SDL_Point mouse
) {
    int ww, wh;
    SDL_GetRendererOutputSize(r, &ww, &wh);

    /* Background */
    SDL_SetRenderDrawColor(r, 245, 245, 245, 255);
    SDL_RenderClear(r);

    /* Image */
    float sx = (float)ww / iw;
    float sy = (float)(wh - TOOLBAR_HEIGHT) / ih;
    float s = (sx < sy ? sx : sy) * zoom;

    SDL_Rect img = {
        (ww - iw * s) / 2 + px,
        TOOLBAR_HEIGHT + (wh - TOOLBAR_HEIGHT - ih * s) / 2 + py,
        iw * s,
        ih * s
    };

    SDL_RenderCopy(r, tex, NULL, &img);

    /* Toolbar */
    SDL_Rect bar = {0, 0, ww, TOOLBAR_HEIGHT};
    SDL_SetRenderDrawColor(r, 60, 60, 60, 255);
    SDL_RenderFillRect(r, &bar);

    /* Buttons */
    SDL_Rect btns[3] = { zin, zout, reset };

    for (int i = 0; i < 3; i++) {
        int hover = SDL_PointInRect(&mouse, &btns[i]);

        SDL_SetRenderDrawColor(
            r,
            hover ? 120 : 90,
            hover ? 120 : 90,
            hover ? 120 : 90,
            255
        );
        SDL_RenderFillRect(r, &btns[i]);

        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        SDL_RenderDrawRect(r, &btns[i]);
    }

    /* Icons */
    draw_plus(r, zin);
    draw_minus(r, zout);
    draw_reset(r, reset);

    SDL_RenderPresent(r);
}

/* =========================
   MAIN
   ========================= */
int main(void)
{
    SDL_Init(SDL_INIT_VIDEO);

    float zoom = 1.0f, px = 0, py = 0;
    int dragging = 0, lx = 0, ly = 0;
    SDL_Point mouse = {0,0};

    SDL_Window *w = SDL_CreateWindow(
        "Imgbuf",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        900, 600,
        SDL_WINDOW_RESIZABLE
    );

    SDL_Renderer *r = SDL_CreateRenderer(
        w, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    SDL_Surface *s = SDL_LoadBMP("test.bmp");
    SDL_Surface *img = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(s);

    SDL_Texture *tex = SDL_CreateTextureFromSurface(r, img);
    SDL_FreeSurface(img);

    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_NONE);

    int iw, ih;
    SDL_QueryTexture(tex, NULL, NULL, &iw, &ih);

    SDL_Rect zin  = { BTN_PADDING, 16, BTN_SIZE, BTN_SIZE };
    SDL_Rect zout = { BTN_PADDING*2 + BTN_SIZE, 16, BTN_SIZE, BTN_SIZE };
    SDL_Rect rst  = { BTN_PADDING*3 + BTN_SIZE*2, 16, BTN_SIZE, BTN_SIZE };

    SDL_Event e;
    int run = 1;

    while (run) {
        while (SDL_PollEvent(&e)) {

            if (e.type == SDL_QUIT) run = 0;

            else if (e.type == SDL_MOUSEMOTION) {
                mouse = (SDL_Point){e.motion.x, e.motion.y};
                if (dragging) {
                    px += e.motion.x - lx;
                    py += e.motion.y - ly;
                    lx = e.motion.x;
                    ly = e.motion.y;
                }
            }

            else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                dragging = 1;
                lx = e.button.x;
                ly = e.button.y;
            }

            else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
                dragging = 0;
                SDL_Point p = {e.button.x, e.button.y};

                if (SDL_PointInRect(&p, &zin)) zoom *= 1.1f;
                else if (SDL_PointInRect(&p, &zout)) zoom /= 1.1f;
                else if (SDL_PointInRect(&p, &rst)) zoom = 1.0f, px = py = 0;
            }

            else if (e.type == SDL_MOUSEWHEEL) {
                zoom *= (e.wheel.y > 0) ? 1.1f : 0.9f;
            }
        }

        if (zoom < 0.1f) zoom = 0.1f;
        if (zoom > 10.0f) zoom = 10.0f;

        render(r, tex, iw, ih, zoom, px, py, zin, zout, rst, mouse);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(r);
    SDL_DestroyWindow(w);
    SDL_Quit();
    return 0;
}
