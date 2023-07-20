#pragma once
#include "SDL.h"

struct Color
{
    Uint8 r, g, b, alpha;
    Color();
    Color(Uint8 r, Uint8 g, Uint8 b, Uint8 alpha = 255);
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

    void CreateWindow(int windoww, int windowh, int pixelscale, SDL_WindowFlags flag = SDL_WINDOW_SHOWN);
    void DrawPixel(int x, int y, Color col);
    void DrawLine(int x0, int y0, int x1, int y1, Color col);
    void DrawRect(int x0, int y0, int x1, int y1, Color col);
    void ApplicationQuit();

    static Screen* scr;
};