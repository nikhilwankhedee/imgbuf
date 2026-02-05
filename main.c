#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#define TOOLBAR_HEIGHT 80
#define BTN_SIZE 48
#define BTN_PAD 16

SDL_Texture *tex = NULL;
int img_w = 0, img_h = 0;

float zoom = 1.0f;
float pan_x = 0;
float pan_y = 0;

int dragging = 0;
int last_x, last_y;

/* Toolbar Buttons */
SDL_Rect btn_open  = {BTN_PAD, 16, BTN_SIZE, BTN_SIZE};
SDL_Rect btn_zoom_in  = {BTN_PAD*2 + BTN_SIZE, 16, BTN_SIZE, BTN_SIZE};
SDL_Rect btn_zoom_out = {BTN_PAD*3 + BTN_SIZE*2, 16, BTN_SIZE, BTN_SIZE};
SDL_Rect btn_reset    = {BTN_PAD*4 + BTN_SIZE*3, 16, BTN_SIZE, BTN_SIZE};

/* ================= FILE PICKER ================= */
char* open_file_dialog()
{
    FILE *fp = popen("zenity --file-selection --title='Open Image'", "r");
    if (!fp) return NULL;

    static char path[512];

    if (fgets(path, sizeof(path), fp))
    {
        path[strcspn(path, "\n")] = 0;
        pclose(fp);
        return path;
    }

    pclose(fp);
    return NULL;
}

/* ================= FIT IMAGE ================= */
void fit_image(SDL_Renderer *ren)
{
    if (!tex) return;

    int ww, wh;
    SDL_GetRendererOutputSize(ren, &ww, &wh);

    float sx = (float)ww / img_w;
    float sy = (float)(wh - TOOLBAR_HEIGHT) / img_h;

    zoom = (sx < sy) ? sx : sy;
    pan_x = (ww - img_w * zoom) / 2;
    pan_y = 0;
}

/* ================= LOAD IMAGE ================= */
int load_image(SDL_Renderer *ren, const char *path)
{
    SDL_Surface *s = IMG_Load(path);
    if (!s)
    {
        printf("Failed load: %s\n", IMG_GetError());
        return 0;
    }

    if (tex) SDL_DestroyTexture(tex);

    tex = SDL_CreateTextureFromSurface(ren, s);
    img_w = s->w;
    img_h = s->h;

    SDL_FreeSurface(s);

    fit_image(ren);
    return 1;
}

/* ================= ZOOM ================= */
void zoom_at(float factor, int mx, int my)
{
    float old_zoom = zoom;
    zoom *= factor;

    float scale = zoom / old_zoom;

    pan_x = mx - (mx - pan_x) * scale;
    pan_y = my - TOOLBAR_HEIGHT - (my - TOOLBAR_HEIGHT - pan_y) * scale;
}

/* ================= ICON DRAW ================= */
void draw_plus(SDL_Renderer *r, SDL_Rect b)
{
    SDL_RenderDrawLine(r,b.x+b.w/2,b.y+8,b.x+b.w/2,b.y+b.h-8);
    SDL_RenderDrawLine(r,b.x+8,b.y+b.h/2,b.x+b.w-8,b.y+b.h/2);
}

void draw_minus(SDL_Renderer *r, SDL_Rect b)
{
    SDL_RenderDrawLine(r,b.x+8,b.y+b.h/2,b.x+b.w-8,b.y+b.h/2);
}

void draw_reset(SDL_Renderer *r, SDL_Rect b)
{
    SDL_RenderDrawLine(r,b.x+10,b.y+10,b.x+b.w-10,b.y+b.h-10);
    SDL_RenderDrawLine(r,b.x+b.w-10,b.y+10,b.x+10,b.y+b.h-10);
}

void draw_open(SDL_Renderer *r, SDL_Rect b)
{
    SDL_Rect file = {b.x+12, b.y+12, b.w-24, b.h-24};
    SDL_RenderDrawRect(r, &file);
}

/* ================= RENDER ================= */
void render(SDL_Renderer *ren)
{
    int ww, wh;
    SDL_GetRendererOutputSize(ren,&ww,&wh);

    SDL_SetRenderDrawColor(ren,240,240,240,255);
    SDL_RenderClear(ren);

    /* Clip below toolbar */
    SDL_Rect clip = {0, TOOLBAR_HEIGHT, ww, wh - TOOLBAR_HEIGHT};
    SDL_RenderSetClipRect(ren, &clip);

    if(tex)
    {
        SDL_Rect dst =
        {
            pan_x,
            TOOLBAR_HEIGHT + pan_y,
            img_w * zoom,
            img_h * zoom
        };

        SDL_RenderCopy(ren, tex, NULL, &dst);
    }

    SDL_RenderSetClipRect(ren, NULL);

    /* Toolbar */
    SDL_Rect bar = {0,0,ww,TOOLBAR_HEIGHT};
    SDL_SetRenderDrawColor(ren,60,60,60,255);
    SDL_RenderFillRect(ren,&bar);

    SDL_Rect btns[4] = {btn_open, btn_zoom_in, btn_zoom_out, btn_reset};

    for(int i=0;i<4;i++)
    {
        SDL_SetRenderDrawColor(ren,90,90,90,255);
        SDL_RenderFillRect(ren,&btns[i]);

        SDL_SetRenderDrawColor(ren,255,255,255,255);
        SDL_RenderDrawRect(ren,&btns[i]);
    }

    draw_open(ren, btn_open);
    draw_plus(ren, btn_zoom_in);
    draw_minus(ren, btn_zoom_out);
    draw_reset(ren, btn_reset);

    SDL_RenderPresent(ren);
}

/* ================= MAIN ================= */
int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    SDL_Window *win = SDL_CreateWindow(
        "Viewer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1000,700,
        SDL_WINDOW_RESIZABLE
    );

    SDL_Renderer *ren = SDL_CreateRenderer(
        win,-1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    load_image(ren,"../test.bmp");

    SDL_Event e;
    int run = 1;

    while(run)
    {
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT) run = 0;

            if(e.type == SDL_MOUSEBUTTONDOWN &&
               e.button.button == SDL_BUTTON_LEFT)
            {
                SDL_Point p = {e.button.x,e.button.y};

                if(SDL_PointInRect(&p,&btn_open))
                {
                    char *file = open_file_dialog();
                    if (file) load_image(ren,file);
                }
                else if(SDL_PointInRect(&p,&btn_zoom_in))
                    zoom_at(1.1f,p.x,p.y);

                else if(SDL_PointInRect(&p,&btn_zoom_out))
                    zoom_at(0.9f,p.x,p.y);

                else if(SDL_PointInRect(&p,&btn_reset))
                    fit_image(ren);

                else if(p.y >= TOOLBAR_HEIGHT)
                {
                    dragging = 1;
                    last_x = p.x;
                    last_y = p.y;
                }
            }

            if(e.type == SDL_MOUSEBUTTONUP)
                dragging = 0;

            if(e.type == SDL_MOUSEMOTION && dragging)
            {
                pan_x += e.motion.x - last_x;
                pan_y += e.motion.y - last_y;

                last_x = e.motion.x;
                last_y = e.motion.y;
            }

            if(e.type == SDL_MOUSEWHEEL)
            {
                int mx,my;
                SDL_GetMouseState(&mx,&my);
                zoom_at(e.wheel.y>0?1.1f:0.9f,mx,my);
            }
        }

        render(ren);
    }

    SDL_Quit();
    return 0;
}
