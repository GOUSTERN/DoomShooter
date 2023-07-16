#pragma once
#include "SDL.h"

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
	static double Get_scroll_whell();
	/// <summary>
	/// returns true if there were an attempt to close the application
	/// </summary>
	static bool Qiut() { return Input::is_quit; }
private:
	static bool is_quit;
	static int8_t scroll_whell_state;
	static SDL_Event sdl_event;
	static const Uint8* key_states;
	static void Key_up();
	static void Key_down();
	static void Scroll_wheel(int8_t status);
};