/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiRadioButton.cpp
-------------------------------------------
*/

#include "IonGuiRadioButton.h"

#include "gui/IonGuiPanelContainer.h"

namespace ion::gui::controls
{

using namespace gui_radio_button;

namespace gui_radio_button::detail
{
} //gui_radio_button::detail


//Protected

/*
	Events
*/

void GuiRadioButton::Checked() noexcept
{
	UnselectEqualTag(tag_);
	GuiCheckBox::Checked(); //Use base functionality
}

void GuiRadioButton::Unchecked() noexcept
{
	checked_ = true; //Not allowed to unselect a radio button
	//Do not use base functionality
}


/*
	Skins
*/

OwningPtr<gui_control::ControlSkin> GuiRadioButton::AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const
{
	//Not fully compatible
	if (skin && !dynamic_cast<RadioButtonSkin*>(skin.get()))
	{
		auto radio_button_skin = make_owning<RadioButtonSkin>();
		radio_button_skin->Assign(*skin);
		return radio_button_skin;
	}
	else
		return skin;
}


/*
	Tags
*/

void GuiRadioButton::Unselect() noexcept
{
	checked_ = false;
	GuiCheckBox::Unchecked(); //Use base functionality
}

void GuiRadioButton::UnselectEqualTag(int tag) noexcept
{
	if (auto owner = Owner(); owner)
	{
		//Unselect radio button with equal tag
		for (auto &control : owner->Controls())
		{
			if (auto radio_button = dynamic_cast<GuiRadioButton*>(&control);
				radio_button && this != radio_button &&
				radio_button->Tag() == tag && radio_button->IsSelected())
			{
				radio_button->Unselect();
				break;
			}
		}
	}
}

bool GuiRadioButton::UniqueTag(int tag) const noexcept
{
	if (auto owner = Owner(); owner)
	{
		for (auto &control : owner->Controls())
		{
			if (auto radio_button = dynamic_cast<GuiRadioButton*>(&control);
				radio_button && this != radio_button &&
				radio_button->Tag() == tag)

				return false; //Not unique
		}
	}

	return true;
}


//Public

GuiRadioButton::GuiRadioButton(std::string name, const std::optional<Vector2> &size,
	std::optional<std::string> caption, std::optional<std::string> tooltip, gui_control::BoundingBoxes hit_boxes) noexcept :
	GuiCheckBox{std::move(name), size, std::move(caption), std::move(tooltip), std::move(hit_boxes)}
{
	//Empty
}

GuiRadioButton::GuiRadioButton(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
	std::optional<std::string> caption, std::optional<std::string> tooltip, gui_control::BoundingBoxes hit_boxes) :
	GuiCheckBox{std::move(name), skin, size, std::move(caption), std::move(tooltip), std::move(hit_boxes)}
{
	//Empty
}


/*
	Modifiers
*/

void GuiRadioButton::Select() noexcept
{
	Check();
}

void GuiRadioButton::Tag(int tag) noexcept
{
	if (tag_ != tag)
	{
		if (IsSelected() && !UniqueTag(tag))
			Unselect();

		tag_ = tag;
	}
}

} //ion::gui::controls