/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events/listeners
File:	IonFrameListener.h
-------------------------------------------
*/

#ifndef _ION_FRAME_LISTENER_
#define _ION_FRAME_LISTENER_

#include <chrono>

#include "IonListener.h"
#include "types/IonTypes.h"

namespace ion::events::listeners
{
	struct FrameListener : Listener<FrameListener>
	{
		/*
			Events
		*/

		//Called just after a frame has been rendered, with elapsed time in seconds since the last frame
		//Return true to continue rendering, or false to drop out of the rendering loop
		virtual bool FrameStarted(std::chrono::duration<real> time) noexcept = 0;

		//Called when a frame is about to begin rendering, with elapsed time in seconds since the last frame
		//Return true to continue rendering, or false to drop out of the rendering loop
		virtual bool FrameEnded(std::chrono::duration<real> time) noexcept = 0;
	};
} //ion::events::listeners

#endif