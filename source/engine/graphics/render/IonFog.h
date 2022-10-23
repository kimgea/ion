/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render
File:	IonFog.h
-------------------------------------------
*/

#ifndef ION_FOG_H
#define ION_FOG_H

#include <cassert>

#include "graphics/utilities/IonColor.h"
#include "types/IonTypes.h"

namespace ion::graphics::render
{
	using namespace graphics::utilities;
	using namespace types::type_literals;	

	namespace fog
	{
		enum class FogMode
		{
			Exp,
			Exp2,
			Linear
		};

		namespace detail
		{
			constexpr auto log2e = 1.44269504089_r;
			inline const auto default_color = color::LightGray;
		} //detail
	} //fog


	//A class representing fog which can be linear, exponential or exponential2
	//Fog is rendered with a fragment shader if enabled in the scene
	class Fog final
	{
		private:

			fog::FogMode mode_ = fog::FogMode::Exp;
			real density_ = 1.0_r;
			real near_distance_ = 0.0_r; //Start
			real far_distance_ = 1.0_r; //End
			Color color_ = fog::detail::default_color;

		public:

			//Default constructor
			Fog() = default;

			//Constructs a new fog with the given values
			Fog(fog::FogMode mode, real density, real near_distance, real far_distance,
				const Color &color = fog::detail::default_color) noexcept;


			/*
				Static fog conversions
			*/

			//Returns a new exponential fog from the given values
			[[nodiscard]] static Fog Exp(real density, real near_distance, real far_distance,
				const Color &color = fog::detail::default_color) noexcept;

			//Returns a new exponential2 (squared) fog from the given values
			[[nodiscard]] static Fog Exp2(real density, real near_distance, real far_distance,
				const Color &color = fog::detail::default_color) noexcept;

			//Returns a new linear fog from the given values
			[[nodiscard]] static Fog Linear(real near_distance, real far_distance,
				const Color &color = fog::detail::default_color) noexcept;


			/*
				Modifiers
			*/

			//Sets the mode this fog uses to the given mode
			inline void Mode(fog::FogMode mode) noexcept
			{
				mode_ = mode;
			}

			//Sets the density of this fog to the given value
			//Density only applies if the fog mode is either exp or exp2
			inline void Density(real density) noexcept
			{
				density_ = density;
			}

			//Sets the near distance (start) of this fog to the given value
			inline void NearDistance(real near_distance) noexcept
			{
				near_distance_ = near_distance;
			}

			//Sets the far distance (end) of this fog to the given value
			inline void FarDistance(real far_distance) noexcept
			{
				far_distance_ = far_distance;
			}

			//Sets the tint of this fog to the given color
			inline void Tint(const Color &color) noexcept
			{
				color_ = color;
			}


			/*
				Observers
			*/

			//Returns the mode this fog uses
			[[nodiscard]] inline auto Mode() const noexcept
			{
				return mode_;
			}

			//Returns the density of this fog
			[[nodiscard]] inline auto Density() const noexcept
			{
				return density_;
			}

			//Returns the near distance (start) of this fog
			[[nodiscard]] inline auto NearDistance() const noexcept
			{
				return near_distance_;
			}

			//Returns the far distance (end) of this fog
			[[nodiscard]] inline auto FarDistance() const noexcept
			{
				return far_distance_;
			}

			//Returns the tint of this fog
			[[nodiscard]] inline auto& Tint() const noexcept
			{
				return color_;
			}


			//Returns the scale factor of this fog
			//Scale only applies if the fog mode is linear
			[[nodiscard]] inline auto Scale() const noexcept
			{
				assert(far_distance_ - near_distance_ != 0.0_r);
				return 1.0_r / (far_distance_ - near_distance_);
			}
	};
} //ion::graphics::render

#endif