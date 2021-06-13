/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonMovableSoundListener.h
-------------------------------------------
*/

#ifndef ION_MOVABLE_SOUND_LISTENER_H
#define ION_MOVABLE_SOUND_LISTENER_H

#include <optional>

#include "IonMovableObject.h"
#include "graphics/utilities/IonVector3.h"
#include "memory/IonNonOwningPtr.h"

//Forward declarations
namespace ion::sounds
{
	class SoundListener;
}

namespace ion::graphics::scene
{
	using utilities::Vector3;


	namespace movable_sound_listener
	{
		namespace detail
		{
		} //detail
	} //movable_sound_listener


	//A movable sound listener that can be attached to a scene node
	class MovableSoundListener final : public MovableObject
	{
		private:

			Vector3 position_;
			std::optional<Vector3> previous_world_position_;

			NonOwningPtr<sounds::SoundListener> sound_listener_;

		public:

			//Construct a new movable sound listener with the given sound listener
			explicit MovableSoundListener(NonOwningPtr<sounds::SoundListener> sound_listener);

			//Construct a new movable sound listener with the given position and sound listener
			MovableSoundListener(const Vector3 &position, NonOwningPtr<sounds::SoundListener> sound_listener);


			/*
				Modifiers
			*/

			//Sets the position of this sound listener to the given position
			inline void Position(const Vector3 &position) noexcept
			{
				position_ = position;
			}


			/*
				Observers
			*/

			//Returns the position of this sound listener
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}


			//Returns a mutable reference to the sound listener
			[[nodiscard]] auto& Get() noexcept
			{
				return sound_listener_;
			}

			//Returns an immutable reference to the sound listener
			[[nodiscard]] auto& Get() const noexcept
			{
				return sound_listener_;
			}


			/*
				Elapse time
			*/

			//Elapse the total time for this movable sound listener by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept override;
	};
} //ion::graphics::scene

#endif