/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiController.cpp
-------------------------------------------
*/

#include "IonGuiController.h"

#include "graphics/scene/graph/IonSceneNode.h"

namespace ion::gui
{

namespace gui_controller::detail
{

} //gui_controller::detail


GuiController::GuiController(SceneNode &parent_node) :
	node_{parent_node.CreateChildNode()}
{
	//Empty
}


/*
	Frame events
*/

bool GuiController::FrameStarted(duration time) noexcept
{
	return true;
}

bool GuiController::FrameEnded(duration time) noexcept
{
	return true;
}


/*
	Window events
*/

void GuiController::WindowActionReceived(WindowAction action) noexcept
{

}


/*
	Key events
*/

void GuiController::KeyPressed(KeyButton button) noexcept
{

}

void GuiController::KeyReleased(KeyButton button) noexcept
{

}

void GuiController::CharacterPressed(char character) noexcept
{

}


/*
	Mouse events
*/

void GuiController::MousePressed(MouseButton button, Vector2 position) noexcept
{

}

void GuiController::MouseReleased(MouseButton button, Vector2 position) noexcept
{

}

void GuiController::MouseMoved(Vector2 position) noexcept
{

}

void GuiController::MouseWheelRolled(int delta, Vector2 position) noexcept
{

}


/*
	Frames
	Creating
*/

NonOwningPtr<GuiFrame> GuiController::CreateFrame(std::string name)
{
	return Create(std::move(name));
}

NonOwningPtr<GuiFrame> GuiController::CreateFrame(GuiFrame &&frame)
{
	return Create(std::move(frame));
}


/*
	Frames
	Retrieving
*/

NonOwningPtr<GuiFrame> GuiController::GetFrame(std::string_view name) noexcept
{
	return Get(name);
}

NonOwningPtr<const GuiFrame> GuiController::GetFrame(std::string_view name) const noexcept
{
	return Get(name);
}


/*
	Frames
	Removing
*/

void GuiController::ClearFrames() noexcept
{
	return Clear();
}

bool GuiController::RemoveFrame(GuiFrame &frame) noexcept
{
	return Remove(frame);
}

bool GuiController::RemoveFrame(std::string_view name) noexcept
{
	return Remove(name);
}


} //ion::gui