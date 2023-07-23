#include "Screen.h"

Screen* Screen::scr = new Screen();

Color::Color()
{
    r = 0;
    g = 0;
    b = 0;
    alpha = 225;
}

Color::Color(Uint8 r, Uint8 g, Uint8 b, Uint8 alpha)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->alpha = alpha;
}

void Screen::CreateWindow(int windoww, int windowh, int pixelscale, SDL_WindowFlags flag)
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

    screen_tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, screenw, screenh);
}

void Screen::DrawPixel(int x, int y, Color col)
{
    SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, col.alpha);
    SDL_RenderDrawPoint(ren, x, y);
}

void Screen::DrawLine(int x0, int y0, int x1, int y1, Color col)
{
    SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, col.alpha);
    SDL_RenderDrawLine(ren, x0, y0, x1, y1);
}

void Screen::DrawRect(int x, int y, int w, int h, Color col)
{
    SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, col.alpha);
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_RenderFillRect(ren, &rect);
}

void Screen::ApplicationQuit()
{
    SDL_DestroyTexture(screen_tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(window);
    SDL_Quit();
}