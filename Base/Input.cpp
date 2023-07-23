#include "Input.h"

SDL_Event Input::sdl_event = SDL_Event();
const Uint8* Input::key_states = NULL;
bool Input::is_quit = false;
int8_t Input::scroll_whell_state = 0;
int Input::mousex = 0;
int Input::mousey = 0;
int Input::lmousex = 0;
int Input::lmousey = 0;
bool Input::mouselocked = false;
SDL_Window* Input::windowtolock = NULL;
int Input::windowtolockw = 0;
int Input::windowtolockh = 0;

Input::Input() {

}

void Input::Handle_events()
{
	Input::scroll_whell_state = 0;
	Input::is_quit = false;
	while (SDL_PollEvent(&Input::sdl_event) != NULL)
		switch (Input::sdl_event.type) {
		case SDL_QUIT:
			Input::is_quit = true;
			break;
		case SDL_KEYDOWN:
			Input::Key_down();
			break;
		case SDLK_UP:
			Input::Key_up();
			break;
		case SDL_MOUSEWHEEL:
			Input::Scroll_wheel(Input::sdl_event.wheel.y);
			break;
		}
	lmousex = mousex;
	lmousey = mousey;
	SDL_GetMouseState(&mousex, &mousey);
	if (mouselocked)
		SDL_WarpMouseInWindow(windowtolock, windowtolockw / 2, windowtolockh / 2);
}

void Input::Key_up()
{
	Input::key_states = SDL_GetKeyboardState(nullptr);
}

void Input::Key_down()
{
	Input::key_states = SDL_GetKeyboardState(nullptr);
}

bool Input::Get_key_down(SDL_Scancode key)
{
	if (Input::key_states == NULL)
		return false;
	return Input::key_states[key] == 1;
}

float Input::Get_scroll_whell()
{
	return Input::scroll_whell_state;
}

void Input::Lock_cursor(SDL_Window* win)
{
	windowtolock = win;
	SDL_GetWindowSize(windowtolock, &windowtolockw, &windowtolockh);
	mouselocked = true;
}

void Input::Unlock_cursor()
{
	mouselocked = false;
}

float Input::Get_mouse_x()
{
	return mousex;
}

float Input::Get_mouse_y()
{
	return mousey;
}

int Input::Get_delta_mouse_x()
{
	if (mouselocked)
		return mousex - windowtolockw / 2;
	return mousex - lmousex;
}

int Input::Get_delta_mouse_y()
{
	if (mouselocked)
		return mousey - windowtolockh / 2;
	return lmousey - mousey;
}

void Input::Scroll_wheel(int8_t status)
{
	Input::scroll_whell_state = status;
}
