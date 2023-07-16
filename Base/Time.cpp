#include "Time.h"

Uint32 Time::last_tick = 0;
double Time::dt = 0;
int Time::target_fps = 60;
double Time::target_delay = 1.0 / target_fps;
std::vector<Time::Timer*> Time::timers_to_update;

void Time::Tick()
{
	dt = (SDL_GetTicks() - last_tick) / 1000.0;

	if (dt < 1.0 / target_fps)
	{
		int delay = (int)((target_delay - dt) * 1000);
		SDL_Delay(delay);
		dt = target_delay;
	}

	for (auto i : timers_to_update)
	{
		i->Tick();
	}

	last_tick = SDL_GetTicks();
}

bool Time::Set_target_framerate(int target_framerate)
{
	if (target_framerate <= 0)
		return 1;
	if (target_framerate >= 1000)
		return 1;
	target_fps = target_framerate;
	target_delay = 1.0 / target_fps;
	return 0;
}

bool Time::Add_timer(Timer* timer)
{
	if (timer == NULL)
		return 1;
	timers_to_update.push_back(timer);
	return 0;
}

bool Time::Delete_timer(Timer* timer)
{
	int pos = 0;
	for (auto i : timers_to_update)
	{
		if (i == timer)
		{
			timers_to_update.erase(timers_to_update.begin() + pos, timers_to_update.begin() + pos + 1);
			return 0;
		}
		pos++;
	}
	return 1;
}
