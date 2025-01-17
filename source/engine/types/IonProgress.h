/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	types
File:	IonProgress.h
-------------------------------------------
*/

#ifndef ION_PROGRESS_H
#define ION_PROGRESS_H

#include <algorithm>
#include <type_traits>

#include "IonTypes.h"
#include "IonTypeTraits.h"

namespace ion::types
{
	namespace progress::detail
	{
		template <typename T>
		constexpr auto underlying_value(T value) noexcept
		{
			return value;
		}

		///@brief Enables support for duration
		constexpr auto underlying_value(duration value) noexcept
		{
			return value.count();
		}
	} //progress::detail
	

	///@brief A class representing a progress in range [min, max]
	template <typename T>
	class Progress
	{
		static_assert(std::is_arithmetic_v<representation_t<T>>);

		private:

			T position_{};
			T min_{};		
			T max_{};
		
		public:

			///@brief Default constructor
			constexpr Progress() = default;

			///@brief Constructs a new progress with the given min and max
			constexpr Progress(T min, T max) noexcept :
				position_{min},
				min_{min},
				max_{min > max ? min : max}
			{
				//Empty
			}


			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the current position to the given value
			constexpr void Position(T position) noexcept
			{
				position_ = std::clamp(position, min_, max_);
			}

			///@brief Sets the min progress to the given value
			constexpr void Min(T min) noexcept
			{
				min_ = min > max_ ? max_ : min;
				position_ = std::clamp(position_, min_, max_);
			}

			///@brief Sets the max progress to the given value
			constexpr void Max(T max) noexcept
			{
				max_ = max < min_ ? min_ : max;
				position_ = std::clamp(position_, min_, max_);
			}

			///@brief Sets the extents to the given min and max values
			///@details This is faster than modify via min and then max
			constexpr void Extents(T min, T max) noexcept
			{
				min_ = min;
				max_ = min > max ? min : max;
				position_ = std::clamp(position_, min_, max_);
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the current position in range [min, max]
			[[nodiscard]] constexpr auto Position() const noexcept
			{
				return position_;
			}

			///@brief Returns the min progress
			[[nodiscard]] constexpr auto Min() const noexcept
			{
				return min_;
			}

			///@brief Returns the max progress
			[[nodiscard]] constexpr auto Max() const noexcept
			{
				return max_;
			}

			///@brief Returns the min and max progress
			[[nodiscard]] constexpr auto MinMax() const noexcept
			{
				return std::pair{min_, max_};
			}

			///@}

			/**
				@name Percentage
				@{
			*/

			///@brief Sets the current progress to the given percentage in range [0.0, 1.0]
			constexpr void Percent(real percent) noexcept
			{
				position_ = std::clamp(min_ + T(progress::detail::underlying_value(max_ - min_) * percent), min_, max_);
			}

			///@brief Returns the current progress as a percentage in range [0.0, 1.0]
			[[nodiscard]] constexpr auto Percent() const noexcept
			{
				return max_ - min_ > T{} ?
					progress::detail::underlying_value(position_ - min_) /
					static_cast<real>(progress::detail::underlying_value(max_ - min_)) :
					1.0_r;
			}

			///@}

			/**
				@name Remaining
				@{
			*/

			///@brief Sets the current remaining progress to the given value
			constexpr void Remaining(T amount) noexcept
			{
				position_ = std::clamp(max_ - amount, min_, max_);
			}

			///@brief Returns the current remaining progress
			[[nodiscard]] constexpr T Remaining() const noexcept
			{
				return max_ - position_;
			}

			///@}

			/**
				@name Resetting
				@{
			*/

			///@brief Resets the current position to the min progress
			constexpr void Reset() noexcept
			{
				position_ = min_;
			}

			///@}

			/**
				@name Stepping
				@{
			*/

			///@brief Returns true if the current position has reached max progress
			[[nodiscard]] constexpr auto IsComplete() const noexcept
			{
				return position_ == max_;
			}


			///@brief Increase the current position by the given amount
			constexpr auto& StepBy(T amount) noexcept
			{
				if (!IsComplete())
					position_ = std::clamp(position_ + amount, min_, max_);

				return *this;
			}

			///@brief Increase the current position by the given percentage (of max)
			constexpr auto& StepByPercent(real percent) noexcept
			{
				return StepBy(T(progress::detail::underlying_value(max_ - min_) * percent));
			}

			///@}
	};
} //ion::types

#endif