/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph/animations
File:	IonNodeAnimation.h
-------------------------------------------
*/

#ifndef ION_NODE_ANIMATION_H
#define ION_NODE_ANIMATION_H

#include <any>
#include <cmath>
#include <optional>
#include <variant>
#include <vector>

#include "events/IonCallback.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "managed/IonManagedObject.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"
#include "utilities/IonMath.h"

namespace ion::graphics::scene::graph::animations
{
	class NodeAnimation; //Forward declaration
	class NodeAnimationManager; //Forward declaration
	class NodeAnimationTimeline; //Forward declaration

	using namespace graphics::utilities;
	using namespace types::type_literals;

	namespace node_animation
	{
		enum class NodeActionType
		{
			//Visibility
			FlipVisibility,
			FlipVisibilityCascading,
			Show,
			ShowCascading,
			Hide,
			HideCascading,	

			//Transformation
			InheritRotation,
			InheritScaling,
			DisinheritRotation,
			DisinheritScaling
		};

		enum class MotionTechniqueType
		{
			Cubic,
			Exponential,
			Linear,
			Logarithmic,
			Sigmoid,
			Sinh,
			Tanh
		};

		struct MotionTechnique
		{
			MotionTechniqueType type = MotionTechniqueType::Linear;
			std::optional<events::Callback<real, real, real>> method;

			MotionTechnique(MotionTechniqueType type) noexcept :
				type{type} {}
			MotionTechnique(events::Callback<real, real, real> method) noexcept :
				method{method} {}
		};
		

		namespace detail
		{
			struct moving_amount
			{		
				real current = 0.0_r;
				real target = 0.0_r;

				MotionTechniqueType technique = MotionTechniqueType::Linear;
				std::optional<events::Callback<real, real, real>> user_technique;
			};


			/**
				@name Curves
				@{
			*/

			inline auto cubic(real percent, real min, real max) noexcept
			{
				using namespace ion::utilities;
				auto x = math::Normalize(percent, 0.0_r, 1.0_r, min, max);
				return math::Normalize(std::pow(x, 3.0_r), std::pow(min, 3.0_r), std::pow(max, 3.0_r));
			}

			inline auto exp(real percent, real min, real max) noexcept
			{
				using namespace ion::utilities;
				auto x = math::Normalize(percent, 0.0_r, 1.0_r, min, max);
				return math::Normalize(std::exp(x), std::exp(min), std::exp(max));
			}

			inline auto log(real percent, real min, real max) noexcept
			{
				using namespace ion::utilities;
				auto x = math::Normalize(percent, 0.0_r, 1.0_r, min, max);
				return math::Normalize(std::log(x), std::log(min), std::log(max));
			}

			inline auto sigmoid(real x) noexcept
			{
				using namespace ion::utilities;
				return 1.0_r / (1.0_r + std::pow(math::E, -x));
			}

			inline auto sigmoid(real percent, real min, real max) noexcept
			{
				using namespace ion::utilities;
				auto x = math::Normalize(percent, 0.0_r, 1.0_r, min, max);
				return math::Normalize(sigmoid(x), sigmoid(min), sigmoid(max));
			}

			inline auto sinh(real percent, real min, real max) noexcept
			{
				using namespace ion::utilities;
				auto x = math::Normalize(percent, 0.0_r, 1.0_r, min, max);
				return math::Normalize(std::sinh(x), std::sinh(min), std::sinh(max));
			}

			inline auto tanh(real percent, real min, real max) noexcept
			{
				using namespace ion::utilities;
				auto x = math::Normalize(percent, 0.0_r, 1.0_r, min, max);
				return math::Normalize(std::tanh(x), std::tanh(min), std::tanh(max));
			}

			///@}

			/**
				@name Actions
				@{
			*/

			struct action
			{
				duration time = 0.0_sec;

				inline auto operator<(const action &rhs) const noexcept
				{
					return time < rhs.time;
				}
			};

			struct node_action : action
			{
				NodeActionType type = NodeActionType::FlipVisibilityCascading;
			};

			struct user_action : action
			{
				std::any user_data;
				events::Callback<void, NodeAnimation&, std::any&> on_execute;
				std::optional<events::Callback<void, NodeAnimation&, std::any&>> on_execute_opposite;
			};


			using action_types = std::variant<node_action, user_action>;
			using action_container = std::vector<action_types>;

			struct action_types_comparator
			{
				inline auto operator()(const action_types &x, const action_types &y) const noexcept
				{
					return std::visit([](auto &&a) noexcept -> const action& { return a; }, x) <
						   std::visit([](auto &&a) noexcept -> const action& { return a; }, y);
				}
			};

			///@}

			/**
				@name Motions
				@{
			*/

			struct motion
			{
				duration start_time = 0.0_sec;
				duration total_duration = 0.0_sec;

				inline auto operator<(const motion &rhs) const noexcept
				{
					return start_time + total_duration < rhs.start_time + rhs.total_duration;
				}
			};

			struct rotating_motion : motion
			{
				moving_amount angle;

				inline void Reset() noexcept
				{
					angle.current = 0.0_r;
				}
			};

			struct scaling_motion : motion
			{
				moving_amount x;
				moving_amount y;

				inline void Reset() noexcept
				{
					x.current = 0.0_r;
					y.current = 0.0_r;
				}
			};

			struct translating_motion : motion
			{
				moving_amount x;
				moving_amount y;
				moving_amount z;

				inline void Reset() noexcept
				{
					x.current = 0.0_r;
					y.current = 0.0_r;
					z.current = 0.0_r;
				}
			};

			struct user_motion : motion
			{
				moving_amount amount;
				events::Callback<void, NodeAnimation&, real> on_elapse;

				inline void Reset() noexcept
				{
					amount.current = 0.0_r;
				}
			};


			using motion_types = std::variant<rotating_motion, scaling_motion, translating_motion, user_motion>;
			using motion_container = std::vector<motion_types>;

			struct motion_types_comparator
			{
				inline auto operator()(const motion_types &x, const motion_types &y) const noexcept
				{
					return std::visit([](auto &&m) noexcept -> const motion& { return m; }, x) <
						   std::visit([](auto &&m) noexcept -> const motion& { return m; }, y);
				}
			};

			///@}

			/**
				@name Actions
				@{
			*/

			bool execute_action(action &a, duration time, duration current_time, duration start_time) noexcept;

			void elapse_action(NodeAnimation &animation, node_action &a, duration time, duration current_time, duration start_time) noexcept;
			void elapse_action(NodeAnimation &animation, user_action &a, duration time, duration current_time, duration start_time) noexcept;

			///@}

			/**
				@name Motions
				@{
			*/

			real move_amount(moving_amount &value, real percent) noexcept;

			real elapse_motion(motion &m, duration time, duration current_time, duration start_time) noexcept;
			void elapse_motion(NodeAnimation &animation, rotating_motion &m, duration time, duration current_time, duration start_time) noexcept;
			void elapse_motion(NodeAnimation &animation, scaling_motion &m, duration time, duration current_time, duration start_time) noexcept;
			void elapse_motion(NodeAnimation &animation, translating_motion &m, duration time, duration current_time, duration start_time) noexcept;
			void elapse_motion(NodeAnimation &animation, user_motion &m, duration time, duration current_time, duration start_time) noexcept;

			///@}
		} //detail
	} //node_animation


	///@brief A class representing a node animation that can contain both actions and motions
	///@details A node animation can be seen as a timeline where the total duration is calculated from all of the added action/motions
	class NodeAnimation final : public managed::ManagedObject<NodeAnimationManager>
	{
		private:

			duration total_duration_ = 0.0_sec;
			node_animation::detail::action_container actions_;
			node_animation::detail::motion_container motions_;

			std::optional<events::Callback<void, NodeAnimation&>> on_start_;
			std::optional<events::Callback<void, NodeAnimation&>> on_finish_;
			std::optional<events::Callback<void, NodeAnimation&>> on_finish_revert_;


			duration RetrieveTotalDuration() const noexcept;

		public:

			using managed::ManagedObject<NodeAnimationManager>::ManagedObject;


			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the on start callback
			inline void OnStart(events::Callback<void, NodeAnimation&> on_start) noexcept
			{
				on_start_ = on_start;
			}

			///@brief Sets the on start callback
			inline void OnStart(std::nullopt_t) noexcept
			{
				on_start_ = {};
			}


			///@brief Sets the on finish callback
			inline void OnFinish(events::Callback<void, NodeAnimation&> on_finish) noexcept
			{
				on_finish_ = on_finish;
			}

			///@brief Sets the on finish callback
			inline void OnFinish(std::nullopt_t) noexcept
			{
				on_finish_ = {};
			}


			///@brief Sets the on finish revert callback
			inline void OnFinishRevert(events::Callback<void, NodeAnimation&> on_finish_revert) noexcept
			{
				on_finish_revert_ = on_finish_revert;
			}

			///@brief Sets the on finish revert callback
			inline void OnFinishRevert(std::nullopt_t) noexcept
			{
				on_finish_revert_ = {};
			}


			///@brief Resets this node animation
			void Reset() noexcept;

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the total duration of this node animation
			[[nodiscard]] inline auto TotalDuration() const noexcept
			{
				return total_duration_;
			}


			///@brief Returns the on start callback
			[[nodiscard]] inline auto OnStart() const noexcept
			{
				return on_start_;
			}

			///@brief Returns the on finish callback
			[[nodiscard]] inline auto OnFinish() const noexcept
			{
				return on_finish_;
			}

			///@brief Returns the on finish revert callback
			[[nodiscard]] inline auto OnFinishRevert() const noexcept
			{
				return on_finish_revert_;
			}

			///@}

			/**
				@name Elapse time
				@{
			*/

			///@brief Elapses the total time for this node animation group by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time, duration current_time, duration start_time) noexcept;

			///@brief Elapses the total time for this node animation group by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			void Elapse(NodeAnimation &animation, duration time, duration current_time, duration start_time) noexcept;

			///@}

			/**
				@name Playback
				@{
			*/

			///@brief Returns a newly created timeline with this animation attached to it
			NonOwningPtr<NodeAnimationTimeline> Start(real playback_rate = 1.0_r, bool running = true);

			///@}

			/**
				@name Actions
				@{
			*/

			///@brief Adds an action to this node animation with the given type and execution time
			void AddAction(node_animation::NodeActionType type, duration time);


			///@brief Adds a user defined action to this node animation with the given callback and execution time
			void AddAction(events::Callback<void, NodeAnimation&, std::any&> on_execute,
				duration time, std::any user_data = {});

			///@brief Adds a user defined action to this node animation with the given callback, opposite callback and execution time
			///@details The opposite callback is called instead of the regular callback when an animation is in reverse
			void AddAction(events::Callback<void, NodeAnimation&, std::any&> on_execute,
				events::Callback<void, NodeAnimation&, std::any&> on_execute_opposite,
				duration time, std::any user_data = {});


			///@brief Clears all actions from this node animation
			void ClearActions() noexcept;

			///@}

			/**
				@name Motions
				@{
			*/

			///@brief Adds a user defined motion to this node animation with the given target amount, total duration and callback
			void AddMotion(real target_amount, duration total_duration,
				events::Callback<void, NodeAnimation&, real> on_elapse, duration start_time = 0.0_sec,
				node_animation::MotionTechnique technique = node_animation::MotionTechniqueType::Linear);


			///@brief Adds a rotation motion to this node animation with the given angle (in radians) and total duration
			void AddRotation(real angle, duration total_duration, duration start_time = 0.0_sec,
				node_animation::MotionTechnique technique = node_animation::MotionTechniqueType::Linear);


			///@brief Adds a scaling motion to this node animation with the given unit and total duration
			void AddScaling(const Vector2 &unit, duration total_duration, duration start_time = 0.0_sec,
				node_animation::MotionTechnique technique = node_animation::MotionTechniqueType::Linear);

			///@brief Adds a scaling motion to this node animation with the given unit and total duration
			void AddScaling(const Vector2 &unit, duration total_duration, duration start_time,
				node_animation::MotionTechnique technique_x,
				node_animation::MotionTechnique technique_y);


			///@brief Adds a translation motion to this node animation with the given unit and total duration
			void AddTranslation(const Vector3 &unit, duration total_duration, duration start_time = 0.0_sec,
				node_animation::MotionTechnique technique = node_animation::MotionTechniqueType::Linear);

			///@brief Adds a translation motion to this node animation with the given unit and total duration
			void AddTranslation(const Vector3 &unit, duration total_duration, duration start_time,
				node_animation::MotionTechnique technique_x,
				node_animation::MotionTechnique technique_y,
				node_animation::MotionTechnique technique_z);


			///@brief Clears all motions from this node animation
			void ClearMotions() noexcept;

			///@}

			/**
				@name Actions/motions
				@{
			*/

			///@brief Clears all actions and motions from this node animation
			void Clear() noexcept;

			///@}
	};
} //ion::graphics::scene::graph::animations

#endif