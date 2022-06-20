/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	engine
File:	IonEngine.h
-------------------------------------------
*/

#ifndef ION_ENGINE_H
#define ION_ENGINE_H

#include <optional>

#include "events/IonInputController.h"
#include "events/IonListenable.h"
#include "events/listeners/IonFrameListener.h"
#include "graphics/render/IonRenderWindow.h"
#include "graphics/scene/graph/IonSceneGraph.h"
#include "managed/IonObjectManager.h"
#include "managed/IonObjectRegister.h"
#include "memory/IonNonOwningPtr.h"
#include "timers/IonStopwatch.h"
#include "timers/IonTimerManager.h"
#include "types/IonTypes.h"

namespace ion
{
	using namespace types::type_literals;

	namespace engine
	{
		namespace detail
		{
			constexpr auto meters_to_feet_factor = 3.28084_r;
			constexpr auto feet_to_meters_factor = 0.3048_r;


			bool init_file_system() noexcept;
			bool init_graphics() noexcept;

			void set_swap_interval(bool vsync) noexcept;
			bool get_swap_interval() noexcept;
		} //detail
	} //engine


	class Engine final :
		protected events::Listenable<events::listeners::FrameListener>,	
		public managed::ObjectManager<graphics::scene::graph::SceneGraph, Engine>
	{
		private:

			using FrameEventsBase = events::Listenable<events::listeners::FrameListener>;
			using SceneGraphBase = managed::ObjectManager<graphics::scene::graph::SceneGraph, Engine>;


			bool initialized_ = false;
			bool syncronize_ = false;
			timers::Stopwatch frame_stopwatch_;
			timers::Stopwatch total_stopwatch_;

			std::optional<graphics::render::RenderWindow> render_window_;
			std::optional<events::InputController> input_controller_;
			timers::TimerManager timer_manager_;

			static inline auto pixels_per_unit_ = 1.0_r;
			static inline auto units_per_meter_ = 1.0_r;
			static managed::ObjectRegister<managed::ObjectManagerBase> manager_register_;


			/*
				Notifying
			*/

			bool NotifyFrameStarted(duration time) noexcept;
			bool NotifyFrameEnded(duration time) noexcept;

			bool UpdateFrame() noexcept;

		public:

			//Default constructor
			Engine() = default;

			//Deleted copy constructor
			Engine(const Engine&) = delete;

			//Default move constructor
			Engine(Engine &&rhs) = default;


			/*
				Operators
			*/

			//Deleted copy assignment
			Engine& operator=(const Engine&) = delete;

			//Default move assignment
			Engine& operator=(Engine&&) = default;


			/*
				Ranges
			*/

			//Returns a mutable range of all scene graphs in the engine
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto SceneGraphs() noexcept
			{
				return SceneGraphBase::Objects();
			}

			//Returns an immutable range of all scene graphs in the engine
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto SceneGraphs() const noexcept
			{
				return SceneGraphBase::Objects();
			}


			/*
				Events
			*/

			//Return a mutable reference to the frame listener
			[[nodiscard]] inline auto& FrameEvents() noexcept
			{
				return static_cast<FrameEventsBase&>(*this);
			}

			//Return an immutable reference to the frame listener
			[[nodiscard]] inline auto& FrameEvents() const noexcept
			{
				return static_cast<const FrameEventsBase&>(*this);
			}


			/*
				Managers
			*/

			//Return a reference to the static manager register
			[[nodiscard]] static inline auto& Managers() noexcept
			{
				return manager_register_;
			}


			/*
				Modifiers
			*/

			//Sets if the engine should use vertical sync or not by the given value
			void VerticalSync(bool vsync) noexcept;


			//Sets the pixels per unit (PPU) the engine should use (default is 1.0)
			static inline void PixelsPerUnit(real pixels) noexcept
			{
				if (pixels > 0.0_r)
					pixels_per_unit_ = pixels;
			}

			//Sets the units per meter the engine should use for distance measurements (default is 1.0)
			//This is mostly used when initializing the sound system (to set distance factor)
			static inline void UnitsPerMeter(real units) noexcept
			{
				if (units > 0.0_r)
					units_per_meter_ = units;
			}

			//Sets the units per foot the engine should use for distance measurements (default is 0.3048)
			//This is mostly used when initializing the sound system (to set distance factor)
			static inline void UnitsPerFoot(real units) noexcept
			{
				UnitsPerMeter(units * engine::detail::meters_to_feet_factor);
			}


			/*
				Observers
			*/

			//Returns true if the engine is using vertical sync
			[[nodiscard]] bool VerticalSync() const noexcept;


			//Returns a pointer to a mutable render window
			//Returns nullptr if the engine is not rendering to a target
			[[nodiscard]] inline auto Target() noexcept
			{
				return render_window_ ? &*render_window_ : nullptr;
			}

			//Returns a pointer to an immutable render window
			//Returns nullptr if the engine is not rendering to a target
			[[nodiscard]] inline auto Target() const noexcept
			{
				return render_window_ ? &*render_window_ : nullptr;
			}


			//Returns a pointer to a mutable input controller
			//Returns nullptr if the engine has no input controller (missing rendering target)
			[[nodiscard]] inline auto Input() noexcept
			{
				return input_controller_ ? &*input_controller_ : nullptr;
			}

			//Returns a pointer to an immutable input controller
			//Returns nullptr if the engine has no input controller (missing rendering target)
			[[nodiscard]] inline auto Input() const noexcept
			{
				return input_controller_ ? &*input_controller_ : nullptr;
			}


			//Returns a mutable reference to a timer manager containing syncronized timers
			[[nodiscard]] inline auto& SyncedTimers() noexcept
			{
				return timer_manager_;
			}

			//Returns an immutable reference to a timer manager containing syncronized timers
			[[nodiscard]] inline auto& SyncedTimers() const noexcept
			{
				return timer_manager_;
			}


			//Returns the pixels per unit (PPU) the engine should use (default is 1.0)
			[[nodiscard]] static inline auto PixelsPerUnit() noexcept
			{
				return pixels_per_unit_;
			}

			//Returns the units per meter the engine should use for distance measurements (default is 1.0)
			//This is mostly used when initializing the sound system (to set distance factor)
			[[nodiscard]] static inline auto UnitsPerMeter() noexcept
			{
				return units_per_meter_;
			}

			//Returns the units per foot the engine should use for distance measurements (default is 0.3048)
			//This is mostly used when initializing the sound system (to set distance factor)
			[[nodiscard]] static inline auto UnitsPerFoot() noexcept
			{
				return units_per_meter_ * engine::detail::feet_to_meters_factor;
			}


			/*
				Engine
			*/

			//Returns true if all extensions, internal and external requirements where initialized succesfully
			[[nodiscard]] bool Initialize() noexcept;

			//Start the rendering loop
			//Returns 0 if everything went fine
			[[nodiscard]] int Start() noexcept;


			/*
				Timing
			*/

			//Returns the last frame time
			[[nodiscard]] duration FrameTime() const noexcept;

			//Returns the total running time
			[[nodiscard]] duration TotalTime() const noexcept;

			//Returns the FPS
			[[nodiscard]] real FPS() const noexcept;


			//Returns true if the engine is running
			[[nodiscard]] bool Running() const noexcept;


			/*
				Rendering target
			*/

			//Render to the given render window, and create a default viewport
			graphics::render::RenderWindow& RenderTo(graphics::render::RenderWindow &&render_window) noexcept;


			/*
				Scene graphs
				Creating
			*/

			//Create a scene graph with the given name
			NonOwningPtr<graphics::scene::graph::SceneGraph> CreateSceneGraph(std::string name);


			/*
				Scene graphs
				Retrieving
			*/

			//Gets a pointer to a mutable scene graph with the given name
			//Returns nullptr if scene graph could not be found
			[[nodiscard]] NonOwningPtr<graphics::scene::graph::SceneGraph> GetSceneGraph(std::string_view name) noexcept;

			//Gets a pointer to an immutable scene graph with the given name
			//Returns nullptr if scene graph could not be found
			[[nodiscard]] NonOwningPtr<const graphics::scene::graph::SceneGraph> GetSceneGraph(std::string_view name) const noexcept;


			/*
				Scene graphs
				Removing
			*/

			//Clear all removable scene graphs from the engine
			void ClearSceneGraphs() noexcept;

			//Remove a removable scene graph from the engine
			bool RemoveSceneGraph(graphics::scene::graph::SceneGraph &scene_graph) noexcept;

			//Remove a removable scene graph with the given name from the engine
			bool RemoveSceneGraph(std::string_view name) noexcept;
	};
} //ion
#endif