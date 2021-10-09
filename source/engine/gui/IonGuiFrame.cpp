/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiFrame.cpp
-------------------------------------------
*/

#include "IonGuiFrame.h"

#include "IonGuiController.h"

namespace ion::gui
{

namespace gui_frame::detail
{

} //gui_frame::detail


//Protected

/*
	Events
*/

void GuiFrame::Enabled(GuiComponent &component) noexcept
{

}

void GuiFrame::Disabled(GuiComponent &component) noexcept
{

}


void GuiFrame::Focused(controls::GuiControl &control) noexcept
{

}

void GuiFrame::Defocused(controls::GuiControl &control) noexcept
{

}


//Public

GuiFrame::GuiFrame(std::string name) :
	GuiPanelContainer{std::move(name)}
{
	//Empty
}


/*
	Observers
*/

GuiController* GuiFrame::Owner() const noexcept
{
	return static_cast<GuiController*>(owner_);
}

} //ion::gui