#include <iostream>
#include "SDL.h"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define PIXEL_SCALE 10

class Screen
{
private:
    int screenw, screenh;
    int windoww, windowh;
    int pixelscale;
public:
    SDL_Surface* screen_surface = NULL;
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
        window = SDL_CreateWindow("Hello, SDL 2!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windoww, windowh, flag);
        screen_surface = SDL_GetWindowSurface(window);
        //ren = SDL_CreateRenderer(window, -1, 0);
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
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
    scr->CreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, PIXEL_SCALE);

    Uint32 a;
    a = SDL_GetTicks();

    SDL_Renderer* ren = NULL;
    ren = SDL_CreateRenderer(scr->window, -1, 0);

    SDL_Texture* tex = NULL;
    tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH / PIXEL_SCALE, WINDOW_HEIGHT / PIXEL_SCALE);

    SDL_SetRenderTarget(ren, tex);

    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    
    while (true)
    {
        std::cout << SDL_GetTicks() - a << '\n';
        a = SDL_GetTicks();

        SDL_SetRenderTarget(ren, tex);

        for (int i = 0; i < scr->GetScreenWidth(); i++)
        {
            SDL_SetRenderDrawColor(ren, rand() % 255, rand() % 255, rand() % 255, 255);
            SDL_RenderDrawLine(ren, i, 0, i, scr->GetWindowHight());
        }

        SDL_SetRenderTarget(ren, NULL);

        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);
    }
    

    SDL_Delay(20000);

    SDL_DestroyWindow(scr->window);
    SDL_Quit();

    return 0;
}