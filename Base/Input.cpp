#include "Input.h"

SDL_Event Input::sdl_event = SDL_Event();
const Uint8* Input::key_states = NULL;
bool Input::is_quit = false;
int8_t Input::scroll_whell_state = 0;


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

double Input::Get_scroll_whell()
{
	return Input::scroll_whell_state;
}

void Input::Scroll_wheel(int8_t status)
{
	Input::scroll_whell_state = status;
}
