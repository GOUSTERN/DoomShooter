#pragma once
#include "SDL.h"
#include <iostream>

class Input
{
public:
	Input();
	/// <summary>
	/// updates all events and clers old
	/// </summary>
	static void Handle_events();
	/// <summary>
	/// return true if this key was pressed
	/// </summary>
	static bool Get_key_down(SDL_Scancode key);
	/// <summary>
	/// scroll whell y value 
	/// </summary>
	/// <returns>-1 if it was scrolled down and 1 if was scrolled up</returns>
	static float Get_scroll_whell();
	static void Lock_cursor(SDL_Window* win);
	static void Unlock_cursor();
	/// <summary>
	/// returns delta x of mouse
	/// </summary>
	static float Get_mouse_x();
	/// <summary>
	/// returns delta x of mouse
	/// </summary>
	static float Get_mouse_y();
	/// <summary>
	/// returns delta x of mouse
	/// </summary>
	static int Get_delta_mouse_x();
	/// <summary>
	/// returns delta x of mouse
	/// </summary>
	static int Get_delta_mouse_y();
	/// <summary>
	/// returns true if there were an attempt to close the application
	/// </summary>
	static bool Qiut() { return Input::is_quit; }
private:
	static SDL_Window* windowtolock;
	static int windowtolockw, windowtolockh;
	static bool mouselocked;
	static int lmousex, lmousey;
	static int mousex, mousey;
	static bool is_quit;
	static int8_t scroll_whell_state;
	static SDL_Event sdl_event;
	static const Uint8* key_states;
	static void Key_up();
	static void Key_down();
	static void Scroll_wheel(int8_t status);
};