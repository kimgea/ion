/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiSlider.cpp
-------------------------------------------
*/

#include "IonGuiSlider.h"

#include "graphics/scene/IonModel.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "graphics/scene/shapes/IonSprite.h"
#include "utilities/IonMath.h"

namespace ion::gui::controls
{

using namespace gui_slider;

namespace gui_slider::detail
{

void resize_skin(SliderSkin &skin, const Vector2 &from_size, const Vector2 &to_size) noexcept
{
	auto delta_size = to_size - from_size;
	auto delta_position = delta_size * 0.5_r;

	if (skin.Handle)
	{
		auto &center = skin.Handle->Position();
		gui_control::detail::resize_part(skin.Handle, delta_size, delta_position, center);
	}
}

} //gui_slider::detail


//Private

void GuiSlider::DefaultSetup() noexcept
{
	CaptionLayout(gui_control::ControlCaptionLayout::OutsideTopCenter);
}


//Protected

/*
	Events
*/

void GuiSlider::Resized(Vector2 from_size, Vector2 to_size) noexcept
{
	if (skin_)
	{
		//Handle should only resize to keep proportions
		switch (type_)
		{
			case SliderType::Horizontal:
			to_size.X(from_size.X() + (to_size.Y() - from_size.Y()));
			break;

			case SliderType::Vertical:
			to_size.Y(from_size.Y() + (to_size.X() - from_size.X()));
			break;
		}

		detail::resize_skin(static_cast<SliderSkin&>(*skin_), from_size, to_size);
	}

	GuiControl::Resized(from_size, to_size); //Use base functionality
	UpdateHandle();
}


/*
	States
*/

void GuiSlider::SetSkinState(gui_control::ControlState state, SliderSkin &skin) noexcept
{
	if (skin.Handle)
		SetPartState(state, skin.Handle);
}

void GuiSlider::SetState(gui_control::ControlState state) noexcept
{
	GuiControl::SetState(state); //Use base functionality

	if (visible_ && skin_)
		SetSkinState(state, static_cast<SliderSkin&>(*skin_));
}


/*
	Skins
*/

void GuiSlider::UpdateHandle() noexcept
{
	using namespace utilities;

	if (skin_)
	{
		if (auto &skin = static_cast<SliderSkin&>(*skin_); skin.Handle)
		{
			auto visual_area =
				gui_control::detail::get_visual_area(*skin_, true);
			auto center_area =
				gui_control::detail::get_center_area(*skin_, true);
			
			//Set handle position
			if (auto area = center_area.
				value_or(visual_area.
				value_or(aabb::Zero)); area != aabb::Zero)
			{
				auto percent = Percent();

				auto [min, max] =
					type_ == SliderType::Vertical ?
					std::pair{area.Min().Y(), area.Max().Y()} :
					std::pair{area.Min().X(), area.Max().X()};

				auto handle_half_size =
					(type_ == SliderType::Vertical ?
					skin.Handle->Size().Y() :
					skin.Handle->Size().X()) * 0.5_r;
				
				auto handle_pos =
					flipped_ ?
					math::Lerp(max - handle_half_size, min + handle_half_size, percent) :
					math::Lerp(min + handle_half_size, max - handle_half_size, percent);

				skin.Handle->Position(
					type_ == SliderType::Vertical ?
					Vector2{skin.Handle->Position().X(), handle_pos} :
					Vector2{handle_pos, skin.Handle->Position().Y()});
			}
		}
	}
}


//Public

GuiSlider::GuiSlider(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	OwningPtr<SliderSkin> skin) :
	GuiControl{std::move(name), std::move(caption), std::move(tooltip), std::move(skin)}
{
	DefaultSetup();
}

GuiSlider::GuiSlider(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	OwningPtr<SliderSkin> skin, const Vector2 &size) :
	GuiControl{std::move(name), std::move(caption), std::move(tooltip), std::move(skin), size}
{
	DefaultSetup();
}

GuiSlider::GuiSlider(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	OwningPtr<SliderSkin> skin, gui_control::Areas areas) :
	GuiControl{std::move(name), std::move(caption), std::move(tooltip), std::move(skin), std::move(areas)}
{
	DefaultSetup();
}


/*
	Modifiers
*/




/*
	Intersecting
*/

bool GuiSlider::IntersectsHandle(const Vector2 &point) const noexcept
{
	if (node_ && visible_ && skin_)
	{
		if (auto &skin = static_cast<SliderSkin&>(*skin_); skin.Handle)
		{
			if (auto node = skin.Parts->ParentNode(); node)
			{
				skin.Handle->Prepare();

				//Check for intersection
				if (skin.Handle->AxisAlignedBoundingBox().TransformCopy(node->FullTransformation()).Intersects(point))
					return node->AxisAligned() ||
						skin.Handle->OrientedBoundingBox().TransformCopy(node->FullTransformation()).Intersects(point);
			}
		}
	}

	return false;
}


/*
	Key events
*/

bool GuiSlider::KeyReleased([[maybe_unused]] KeyButton button) noexcept
{
	if (auto [min, max] = Range(); min != max)
	{
		if (flipped_)
			button = detail::flip_arrow_keys(button);

		switch (button)
		{
			//Increase
			case KeyButton::UpArrow:
			case KeyButton::RightArrow:
			{
				if (progress_.Position() < max)
				{
					Position(progress_.Position() + step_by_);
					Changed();
				}

				return true;
			}

			//Decrease
			case KeyButton::DownArrow:
			case KeyButton::LeftArrow:
			{
				if (progress_.Position() > min)
				{
					Position(progress_.Position() - step_by_);
					Changed();
				}

				return true;
			}
		}
	}

	return false;
}


/*
	Mouse events
*/

bool GuiSlider::MousePressed(MouseButton button, Vector2 position) noexcept
{
	if (button == MouseButton::Left)
		dragged_ = IntersectsHandle(position);

	return false;
}

bool GuiSlider::MouseReleased(MouseButton button, Vector2 position) noexcept
{
	if (button == MouseButton::Left)
	{
		if (dragged_)
		{
			dragged_ = false;
			return false;
		}

		//Todo
	}

	return false;
}

bool GuiSlider::MouseMoved(Vector2 position) noexcept
{
	if (auto [min, max] = Range(); min != max && dragged_)
	{
		//Todo
	}

	return false;
}

bool GuiSlider::MouseWheelRolled(int delta, Vector2 position) noexcept
{
	//Up or down
	if (delta != 0)
		return KeyReleased(delta > 0 ? KeyButton::UpArrow : KeyButton::DownArrow);
	else
		return false;
}

} //ion::gui::controls