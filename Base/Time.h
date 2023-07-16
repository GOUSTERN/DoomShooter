#pragma once
#include <iostream>
#include <vector>
#include "SDL.h"

class Time
{
public:
	class Timer
	{
	public:
		/// <summary>
		/// creates new timer and automaticly adds it to Time::timers_to_update
		/// </summary>
		Timer()
		{
			Time::Add_timer(this);
		}
		/// <summary>
		/// delates timer and automaticly delets it from Time::timers_to_update
		/// </summary>
		~Timer()
		{
			Time::Delete_timer(this);
		}
		/// <summary>
		/// resets timer and stops it;
		/// </summary>
		void Reset()
		{
			this->Stop();
			this->frame_duration = 0;
		}
		/// <summary>
		/// starts timer
		/// </summary>
		void Start()
		{
			this->active = true;
		}
		/// <summary>
		/// updates time data
		/// </summary>
		void Tick()
		{
			if (active)
				frame_duration += Time::dt;
		}
		/// <summary>
		/// stops timer
		/// </summary>
		void Stop()
		{
			this->active = false;
		}
		/// <summary>
		/// returns timer duration
		/// </summary>
		double Get_time()
		{
			return this->frame_duration;
		}
	private:
		bool active = false;
		double frame_duration = 0;
	};
	/// <summary>
	/// Updates time data
	/// </summary>
	static void Tick();
	/// <summary>
	/// sets framerate to value
	/// </summary>
	/// <returns>0 if ok and 1 if error</returns>
	static bool Set_target_framerate(int target_framerate);
	/// <summary>
	/// adds timer to states to update
	/// </summary>
	/// <returns>0 if ok and 1 if error</returns>
	static bool Add_timer(Timer* timer);
	/// <summary>
	/// delets timer
	/// </summary>
	/// <returns>0 if ok and 1 if error</returns>
	static bool Delete_timer(Timer* timer);
	/// <summary>
	/// returns delta time
	/// </summary>
	static double Delta_time() { return dt; }
	/// <summary>
	/// returns target framerate
	/// </summary>
	static double Get_target_framerate() { return target_fps; }

	static int f()
	{
		return Time::timers_to_update.size();
	}
private:
	static double dt;
	static int target_fps;
	static std::vector<Timer*> timers_to_update;
	static Uint32 last_tick;
	static double target_delay;
};

