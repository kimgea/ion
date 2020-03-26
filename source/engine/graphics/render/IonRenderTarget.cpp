/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render
File:	IonRenderTarget.cpp
-------------------------------------------
*/

#include "IonRenderTarget.h"

#include <memory>

namespace ion::graphics::render
{

namespace render_target::detail
{

} //render_target::detail


//Private

/*
	Notifying
*/

void RenderTarget::NotifyRenderTargetResized(const Vector2 &size) noexcept
{
	for (auto &listener : Listeners())
		Notify(&events::listeners::RenderTargetListener::RenderTargetResized, listener, size);
}


//Public

/*
	Buffers
*/

void RenderTarget::SwapBuffers() noexcept
{
	DoSwapBuffers();
}


/*
	Viewports
*/

Viewport& RenderTarget::AddViewport(Viewport &&viewport)
{
	return *viewports_.emplace_back(std::make_unique<Viewport>(std::move(viewport)));
}

void RenderTarget::ClearViewports() noexcept
{
	viewports_.clear();
	viewports_.shrink_to_fit();
}

bool RenderTarget::RemoveViewport(const Viewport &viewport) noexcept
{
	auto iter =
		std::find_if(std::begin(viewports_), std::end(viewports_),
			[&](auto &x) noexcept
			{
				return x.get() == &viewport;
			});

	//Viewport found
	if (iter != std::end(viewports_))
	{
		viewports_.erase(iter);
		return true;
	}
	else
		return false;
}

} //ion::graphics::render