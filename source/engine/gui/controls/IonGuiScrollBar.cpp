/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiScrollBar.cpp
-------------------------------------------
*/

#include "IonGuiScrollBar.h"

#include <algorithm>

#include "graphics/scene/IonModel.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "graphics/scene/shapes/IonSprite.h"

namespace ion::gui::controls
{

using namespace gui_scroll_bar;

namespace gui_scroll_bar::detail
{
} //gui_scroll_bar::detail


//Private

void GuiScrollBar::DefaultSetup() noexcept
{
	type_ = gui_slider::SliderType::Vertical;
	flipped_ = true;
}


//Protected

/*
	Skins
*/

void GuiScrollBar::UpdateHandle() noexcept
{
	if (auto [min, max] = Range(); min != max)
	{
		if (auto &skin = static_cast<ScrollBarSkin&>(*skin_); skin.Handle)
		{
			//Set handle size
			if (auto size = InnerSize(); size)
			{
				auto [width, height] = size->XY();
				auto [handle_width, handle_height] = skin.Handle->Size().XY();
				auto view_count = 16;	

				if (type_ == gui_slider::SliderType::Vertical)
				{
					handle_height = static_cast<real>(view_count) / (max - min) * height;
					handle_height = std::clamp(handle_height, height * handle_size_.first, height * handle_size_.second);
				}
				else
				{
					handle_width = static_cast<real>(view_count) / (max - min) * width;
					handle_width = std::clamp(handle_width, width * handle_size_.first, width * handle_size_.second);
				}

				if (auto handle_size = Vector2{handle_width, handle_height};
					handle_size != skin.Handle->Size())
					gui_slider::detail::resize_handle(skin, skin.Handle->Size(), handle_size);
			}
		}
	}

	GuiSlider::UpdateHandle(); //Use base functionality
}


//Public

GuiScrollBar::GuiScrollBar(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	OwningPtr<ScrollBarSkin> skin) :
	GuiSlider{std::move(name), std::move(caption), std::move(tooltip), std::move(skin)}
{
	DefaultSetup();
}

GuiScrollBar::GuiScrollBar(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	OwningPtr<ScrollBarSkin> skin, const Vector2 &size) :
	GuiSlider{std::move(name), std::move(caption), std::move(tooltip), std::move(skin), size}
{
	DefaultSetup();
}

GuiScrollBar::GuiScrollBar(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	OwningPtr<ScrollBarSkin> skin, gui_control::Areas areas) :
	GuiSlider{std::move(name), std::move(caption), std::move(tooltip), std::move(skin), std::move(areas)}
{
	DefaultSetup();
}


/*
	Modifiers
*/



bool GuiScrollBar::MouseReleased(MouseButton button, Vector2 position) noexcept
{
	if (button == MouseButton::Left)
	{
		if (dragged_)
			dragged_ = false;

		else if (auto [min, max] = Range(); min != max && skin_)
		{
			if (auto &skin = static_cast<ScrollBarSkin&>(*skin_); skin.Handle)
			{
				//Make position relative to handle
				if (auto node = skin.Parts->ParentNode(); node)
				{
					//Set handle position
					if (auto size = InnerSize(); size)
					{
						auto handle_size = skin.Handle->Size() * node->DerivedScaling();
						auto handle_position = skin.Handle->Position() * node->DerivedScaling();

						size = (*size - skin.Handle->Size()) * node->DerivedScaling();
						position = (position - node->DerivedPosition()).
							RotateCopy(-node->DerivedRotation(), vector2::Zero);
						handle_position +=
							(type_ == gui_slider::SliderType::Horizontal && position.X() < handle_position.X()) ||
							(type_ == gui_slider::SliderType::Vertical && position.Y() < handle_position.Y()) ?
							-handle_size : handle_size;

						auto percent =
							type_ == gui_slider::SliderType::Vertical ?
							(handle_position.Y() + size->Y() * 0.5_r) / size->Y() :
							(handle_position.X() + size->X() * 0.5_r) / size->X();

						auto current_pos = Position();

						if (flipped_)
							Percent(1.0_r - percent);
						else
							Percent(percent);

						if (current_pos != Position())
							Changed();
					}
				}
			}
		}
	}

	return false;
}

} //ion::gui::controls