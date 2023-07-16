#include <iostream>
#include "SDL.h"
#include "Base/Time.h"
#include "Base/Input.h"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define PIXEL_SCALE 2

struct Color
{
    Uint8 r, g, b, alpha;

    Color()
    {
        r = 0;
        g = 0;
        b = 0;
        alpha = 225;
    }

    Color(Uint8 r, Uint8 g, Uint8 b, Uint8 alpha = 255)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->alpha = alpha;
    }
};

class Screen
{
private:
    int screenw, screenh;
    int windoww, windowh;
    int pixelscale;
public:
    SDL_Surface* screen_surface = NULL;
    SDL_Texture* screen_tex = NULL;
    SDL_Window* window = NULL;
    SDL_Renderer* ren = NULL;

    int GetScreenWidth() { return screenw; }
    int GetScreenHight() { return screenh; }
    int GetWindowWidth() { return windoww; }
    int GetWindowHight() { return windowh; }

    void CreateWindow(int windoww, int windowh, int pixelscale, SDL_WindowFlags flag = SDL_WINDOW_SHOWN)
    { 
        this->pixelscale = pixelscale;
        this->windoww = windoww;
        this->windowh = windowh;     
        this->screenw = windoww / pixelscale;
        this->screenh = windowh / pixelscale;

        if (window != NULL)
            SDL_DestroyWindow(window);
        if (ren != NULL)
            SDL_DestroyRenderer(ren);
        if (screen_tex != NULL)
            SDL_DestroyTexture(screen_tex);
        window = SDL_CreateWindow("Hello, SDL 2!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windoww, windowh, flag);
        screen_surface = SDL_GetWindowSurface(window);
        ren = SDL_CreateRenderer(window, -1, 0);

        screen_tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH / PIXEL_SCALE, WINDOW_HEIGHT / PIXEL_SCALE);
    }

    void DrawPixel(int x, int y, Color col)
    {
        SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, col.alpha);
        SDL_RenderDrawPoint(ren, x, y);
    }

    void DrawLine(int x0, int y0, int x1, int y1, Color col)
    {
        SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, col.alpha);
        SDL_RenderDrawLine(ren, x0, y0, x1, y1);
    }
    void ApplicationQuit()
    {
        SDL_DestroyTexture(screen_tex);
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

Screen* scr = new Screen();

bool InitSDL()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        return 1;
    std::cout << "SDL init successfully\n";
}

int main(int argc, char** argv)
{
    srand(time(NULL));
    InitSDL();
    scr->CreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, PIXEL_SCALE);

    Time::Set_target_framerate(120);

    while (!Input::Qiut())
    {
        Input::Handle_events();
        SDL_SetRenderTarget(scr->ren, scr->screen_tex);

        for (int i = 0; i < scr->GetScreenWidth(); i++)
            scr->DrawLine(i, 0, i, scr->GetWindowHight(), Color(rand() % 255, rand() % 255, rand() % 255));

        SDL_SetRenderTarget(scr->ren, NULL);

        SDL_RenderCopy(scr->ren, scr->screen_tex, NULL, NULL);
        SDL_RenderPresent(scr->ren);

        Time::Tick();
    }
    scr->ApplicationQuit();
    delete(scr);
    return 0;
}