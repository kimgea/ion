/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	engine
File:	IonEngine.cpp
-------------------------------------------
*/

#include "IonEngine.h"

#include "graphics/IonGraphicsAPI.h"
#include "system/IonSystemUtility.h"
#include "utilities/IonFileUtility.h"

namespace ion
{

using namespace engine;

namespace engine::detail
{

bool init_file_system() noexcept
{
	if (auto application_path = system::utilities::ApplicationPath(); application_path)
	{
		application_path->remove_filename();

		if (utilities::file::CurrentPath(*application_path))
			return true;
	}
	
	return false;
}

bool init_graphics() noexcept
{
	#ifdef ION_GLEW
	if (glewInit() != GLEW_OK)
		return false;
	#endif

	glShadeModel(GL_SMOOTH);

	//Depth buffer setup
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	
	glAlphaFunc(GL_GREATER, 0.0f); //Only draw pixels greater than 0% alpha
	glEnable(GL_ALPHA_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	return true;
}


void set_swap_interval(bool vsync) noexcept
{
	#ifdef ION_WIN_GLEW
	if (wglSwapIntervalEXT)
		wglSwapIntervalEXT((int)vsync);
	#else

	#endif
}

bool get_swap_interval() noexcept
{
	#ifdef ION_WIN_GLEW
	return wglGetSwapIntervalEXT && wglGetSwapIntervalEXT() != 0;
	#else
	return false;
	#endif
}

} //engine::detail


//Protected

/*
	Notifying
*/

bool Engine::NotifyFrameStarted(duration time) noexcept
{
	for (auto &listener : FrameEventsBase::Listeners())
	{
		if (!FrameEventsBase::Notify(&events::listeners::FrameListener::FrameStarted, listener, time).value_or(true))
			return false;
	}

	return true;
}

bool Engine::NotifyFrameEnded(duration time) noexcept
{
	for (auto &listener : FrameEventsBase::Listeners())
	{
		if (!FrameEventsBase::Notify(&events::listeners::FrameListener::FrameEnded, listener, time).value_or(true))
			return false;
	}

	return true;
}


bool Engine::UpdateFrame() noexcept
{
	auto time = frame_stopwatch_.Restart();
		//Time should always be 0.0 sec on first frame

	timer_manager_.Elapse(time);

	if (!NotifyFrameStarted(time))
		return false;

	for (auto &viewport : render_window_->Viewports())
	{
		viewport.RenderTo();

		for (auto &scene_graph : SceneGraphs())
			scene_graph.Render(viewport, time);
	}

	if (syncronize_)
		glFinish();
	else
		glFlush();

	return NotifyFrameEnded(time);
}


//Public

/*
	Modifiers
*/

void Engine::VerticalSync(bool vsync) noexcept
{
	detail::set_swap_interval(vsync);
}


/*
	Observers
*/

bool Engine::VerticalSync() const noexcept
{
	return detail::get_swap_interval();
}


/*
	Engine
*/

bool Engine::Initialize() noexcept
{
	//Already initialized
	if (initialized_)
		return true;

	//No window to render to
	if (!render_window_)
		return false;

	//Create render window (render target)
	if (!render_window_->Created() && !render_window_->Create())
		return false;

	//Initialize file system and graphics
	initialized_ = detail::init_file_system() && detail::init_graphics();
	return initialized_;
}

int Engine::Start() noexcept
{
	//Not successfully initialized
	if (!Initialize())
		return 1;

	//Already running
	if (Running())
		return 1;


	//Show window
	if (render_window_)
		render_window_->Show();

	syncronize_ = VerticalSync();
	total_stopwatch_.Restart();

	//Main loop
	while (render_window_ && render_window_->ProcessMessages() && UpdateFrame())
		render_window_->SwapBuffers();

	frame_stopwatch_.Reset();
	total_stopwatch_.Stop(); //Total time could be retrieved later

	//Hide window
	if (render_window_)
		render_window_->Hide();

	return 0;
}


/*
	Timing
*/

duration Engine::FrameTime() const noexcept
{
	return frame_stopwatch_.Elapsed();
}

duration Engine::TotalTime() const noexcept
{
	return total_stopwatch_.Elapsed();
}

real Engine::FPS() const noexcept
{
	auto time = FrameTime();
	return time > 0.0_sec ?
		1.0_r / time.count() :
		0.0_r;
}


bool Engine::Running() const noexcept
{
	return total_stopwatch_.IsRunning();
}


/*
	Window
*/

graphics::render::RenderWindow& Engine::RenderTo(graphics::render::RenderWindow &&render_window) noexcept
{
	if (Running())
		return *render_window_;


	if (input_controller_)
		input_controller_.reset();

	//Create render window, viewport and input controller
	render_window_.emplace(std::move(render_window));
	render_window_->CreateViewport("main", *render_window_);
	input_controller_.emplace(*render_window_);

	return *render_window_;
}


/*
	Scene graphs
	Creating
*/

NonOwningPtr<graphics::scene::graph::SceneGraph> Engine::CreateSceneGraph(std::string name)
{
	return SceneGraphBase::Create(std::move(name));
}


/*
	Scene graphs
	Retrieving
*/

NonOwningPtr<graphics::scene::graph::SceneGraph> Engine::GetSceneGraph(std::string_view name) noexcept
{
	return SceneGraphBase::Get(name);
}

NonOwningPtr<const graphics::scene::graph::SceneGraph> Engine::GetSceneGraph(std::string_view name) const noexcept
{
	return SceneGraphBase::Get(name);
}


/*
	Scene graphs
	Removing
*/

void Engine::ClearSceneGraphs() noexcept
{
	SceneGraphBase::Clear();
}

bool Engine::RemoveSceneGraph(graphics::scene::graph::SceneGraph &scene_graph) noexcept
{
	return SceneGraphBase::Remove(scene_graph);
}

bool Engine::RemoveSceneGraph(std::string_view name) noexcept
{
	return SceneGraphBase::Remove(name);
}

} //ion