/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	timers
File:	IonTimerManager.h
-------------------------------------------
*/

#ifndef _ION_TIMER_MANAGER_
#define _ION_TIMER_MANAGER_

#include <chrono>

#include "resources/IonResourceManager.h"
#include "timers/IonTimer.h"

namespace ion::timers
{
	namespace timer_manager::detail
	{
	} //timer_manager::detail


	struct TimerManager : resources::ResourceManager<TimerManager, Timer>
	{
		//Default constructor
		TimerManager() = default;

		//Deleted copy constructor
		TimerManager(const TimerManager&) = delete;

		//Default move constructor
		TimerManager(TimerManager&&) = default;


		/*
			Operators
		*/

		//Deleted copy assignment
		TimerManager& operator=(const TimerManager&) = delete;

		//Move assignment
		TimerManager& operator=(TimerManager&&) = default;


		/*
			Creating
		*/

		//
		[[nodiscard]] Timer& CreateTimer(std::chrono::duration<real> interval);

		//
		[[nodiscard]] Timer& CreateTimer(std::chrono::duration<real> interval, events::Callback<void, Timer&> on_tick);

		//
		[[nodiscard]] Timer& CreateTimer(const Timer &timer);


		/*
			Elapse time
		*/

		//Elapse all timers by the given time in seconds
		//This function is typically called each frame, with the time in seconds since last frame
		void Elapse(std::chrono::duration<real> time) noexcept;
	};
} //ion::timers

#endif