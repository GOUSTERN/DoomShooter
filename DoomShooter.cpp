#include <iostream>
#include "SDL.h"
#include "Base/Time.h"
#include "Base/Input.h"
#include "Base/Screen.h"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define PIXEL_SCALE 10

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
    Screen::scr->CreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, PIXEL_SCALE);

    Time::Set_target_framerate(120);

    while (!Input::Qiut())
    {
        Input::Handle_events();
        SDL_SetRenderTarget(Screen::scr->ren, Screen::scr->screen_tex);

        for (int i = 0; i < Screen::scr->GetScreenWidth(); i++)
            Screen::scr->DrawLine(i, 0, i, Screen::scr->GetWindowHight(), Color(rand() % 255, rand() % 255, rand() % 255));

        SDL_SetRenderTarget(Screen::scr->ren, NULL);

        SDL_RenderCopy(Screen::scr->ren, Screen::scr->screen_tex, NULL, NULL);
        SDL_RenderPresent(Screen::scr->ren);

        Time::Tick();
    }
    Screen::scr->ApplicationQuit();
    delete(Screen::scr);
    return 0;
}