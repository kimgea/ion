/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/skins
File:	IonGuiSkin.cpp
-------------------------------------------
*/

#include "IonGuiSkin.h"

#include "IonGuiTheme.h"
#include "graphics/render/IonPass.h"
#include "graphics/scene/IonDrawableText.h"
#include "graphics/scene/IonModel.h"
#include "graphics/scene/IonSceneManager.h"
#include "graphics/scene/shapes/IonSprite.h"
#include "graphics/utilities/IonVector2.h"
#include "gui/controls/IonGuiButton.h"
#include "gui/controls/IonGuiCheckBox.h"
#include "gui/controls/IonGuiGroupBox.h"
#include "gui/controls/IonGuiLabel.h"
#include "gui/controls/IonGuiListBox.h"
#include "gui/controls/IonGuiMouseCursor.h"
#include "gui/controls/IonGuiProgressBar.h"
#include "gui/controls/IonGuiRadioButton.h"
#include "gui/controls/IonGuiScrollBar.h"
#include "gui/controls/IonGuiSlider.h"
#include "gui/controls/IonGuiTextBox.h"
#include "gui/controls/IonGuiTooltip.h"
#include "types/IonTypes.h"

namespace ion::gui::skins
{

using namespace gui_skin;
using namespace graphics::utilities;
using namespace types::type_literals;

namespace gui_skin::detail
{

controls::gui_control::ControlSkin make_skin_base(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	controls::gui_control::ControlSkin control_skin;

	if (!std::empty(skin.Parts()))
	{
		auto center_part = skin.GetPart("center");

		//Sides
		auto top_part = skin.GetPart("top");
		auto bottom_part = skin.GetPart("bottom");
		auto left_part = skin.GetPart("left");
		auto right_part = skin.GetPart("right");

		//Corners
		auto top_left_part = skin.GetPart("top-left");
		auto top_right_part = skin.GetPart("top-right");
		auto bottom_left_part = skin.GetPart("bottom-left");
		auto bottom_right_part = skin.GetPart("bottom-right");

		//Minimum required parts
		if (center_part ||
			(top_part && bottom_part) || (left_part && right_part) ||
			(top_left_part && bottom_right_part) || (bottom_left_part && top_right_part))
		{
			auto model = scene_manager.CreateModel();

			if (std::empty(skin.Passes()))
				model->AddPass(graphics::render::Pass{});
			else
				model->AddPasses(skin.GetPasses());

			control_skin.Parts.Object = model;

			//Center part
			if (center_part && *center_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, center_part->Enabled});
				sprite->FillColor(center_part->FillColor);
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);	

				control_skin.Parts.Center.Object = sprite;
				control_skin.Parts.Center.Enabled = center_part->Enabled;
				control_skin.Parts.Center.Disabled = center_part->Disabled;
				control_skin.Parts.Center.Focused = center_part->Focused;
				control_skin.Parts.Center.Pressed = center_part->Pressed;
				control_skin.Parts.Center.Hovered = center_part->Hovered;
			}

			//Sides
			//Top part
			if (top_part && *top_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, top_part->Enabled});
				sprite->FillColor(top_part->FillColor);
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);			

				control_skin.Parts.Top.Object = sprite;
				control_skin.Parts.Top.Enabled = top_part->Enabled;
				control_skin.Parts.Top.Disabled = top_part->Disabled;
				control_skin.Parts.Top.Focused = top_part->Focused;
				control_skin.Parts.Top.Pressed = top_part->Pressed;
				control_skin.Parts.Top.Hovered = top_part->Hovered;
			}

			//Bottom part
			if (bottom_part && *bottom_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, bottom_part->Enabled});
				sprite->FillColor(bottom_part->FillColor);
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.Bottom.Object = sprite;
				control_skin.Parts.Bottom.Enabled = bottom_part->Enabled;
				control_skin.Parts.Bottom.Disabled = bottom_part->Disabled;
				control_skin.Parts.Bottom.Focused = bottom_part->Focused;
				control_skin.Parts.Bottom.Pressed = bottom_part->Pressed;
				control_skin.Parts.Bottom.Hovered = bottom_part->Hovered;
			}

			//Left part
			if (left_part && *left_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, left_part->Enabled});
				sprite->FillColor(left_part->FillColor);
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.Left.Object = sprite;
				control_skin.Parts.Left.Enabled = left_part->Enabled;
				control_skin.Parts.Left.Disabled = left_part->Disabled;
				control_skin.Parts.Left.Focused = left_part->Focused;
				control_skin.Parts.Left.Pressed = left_part->Pressed;
				control_skin.Parts.Left.Hovered = left_part->Hovered;
			}

			//Right part
			if (right_part && *right_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, right_part->Enabled});
				sprite->FillColor(right_part->FillColor);
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.Right.Object = sprite;
				control_skin.Parts.Right.Enabled = right_part->Enabled;
				control_skin.Parts.Right.Disabled = right_part->Disabled;
				control_skin.Parts.Right.Focused = right_part->Focused;
				control_skin.Parts.Right.Pressed = right_part->Pressed;
				control_skin.Parts.Right.Hovered = right_part->Hovered;
			}

			//Corners
			//Top-left part
			if (top_left_part && *top_left_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, top_left_part->Enabled});
				sprite->FillColor(top_left_part->FillColor);
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.TopLeft.Object = sprite;
				control_skin.Parts.TopLeft.Enabled = top_left_part->Enabled;
				control_skin.Parts.TopLeft.Disabled = top_left_part->Disabled;
				control_skin.Parts.TopLeft.Focused = top_left_part->Focused;
				control_skin.Parts.TopLeft.Pressed = top_left_part->Pressed;
				control_skin.Parts.TopLeft.Hovered = top_left_part->Hovered;
			}

			//Top-right part
			if (top_right_part && *top_right_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, top_right_part->Enabled});
				sprite->FillColor(top_right_part->FillColor);
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.TopRight.Object = sprite;
				control_skin.Parts.TopRight.Enabled = top_right_part->Enabled;
				control_skin.Parts.TopRight.Disabled = top_right_part->Disabled;
				control_skin.Parts.TopRight.Focused = top_right_part->Focused;
				control_skin.Parts.TopRight.Pressed = top_right_part->Pressed;
				control_skin.Parts.TopRight.Hovered = top_right_part->Hovered;
			}

			//Bottom-left part
			if (bottom_left_part && *bottom_left_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, bottom_left_part->Enabled});
				sprite->FillColor(bottom_left_part->FillColor);
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.BottomLeft.Object = sprite;
				control_skin.Parts.BottomLeft.Enabled = bottom_left_part->Enabled;
				control_skin.Parts.BottomLeft.Disabled = bottom_left_part->Disabled;
				control_skin.Parts.BottomLeft.Focused = bottom_left_part->Focused;
				control_skin.Parts.BottomLeft.Pressed = bottom_left_part->Pressed;
				control_skin.Parts.BottomLeft.Hovered = bottom_left_part->Hovered;
			}

			//Bottom-right part
			if (bottom_right_part && *bottom_right_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, bottom_right_part->Enabled});
				sprite->FillColor(bottom_right_part->FillColor);
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.BottomRight.Object = sprite;
				control_skin.Parts.BottomRight.Enabled = bottom_right_part->Enabled;
				control_skin.Parts.BottomRight.Disabled = bottom_right_part->Disabled;
				control_skin.Parts.BottomRight.Focused = bottom_right_part->Focused;
				control_skin.Parts.BottomRight.Pressed = bottom_right_part->Pressed;
				control_skin.Parts.BottomRight.Hovered = bottom_right_part->Hovered;
			}


			//Sides
			//Top part (position)
			if (control_skin.Parts.Top)
			{
				if (control_skin.Parts.Center)
					control_skin.Parts.Top->Position(
						{0.0_r, control_skin.Parts.Center->Size().Y() * 0.5_r + control_skin.Parts.Top->Size().Y() * 0.5_r, 0.0_r});
				else if (control_skin.Parts.Left)
					control_skin.Parts.Top->Position(
						{0.0_r, control_skin.Parts.Left->Size().Y() * 0.5_r + control_skin.Parts.Top->Size().Y() * 0.5_r, 0.0_r});
				else if (control_skin.Parts.Right)
					control_skin.Parts.Top->Position(
						{0.0_r, control_skin.Parts.Right->Size().Y() * 0.5_r + control_skin.Parts.Top->Size().Y() * 0.5_r, 0.0_r});
			}

			//Bottom part (position)
			if (control_skin.Parts.Bottom)
			{
				if (control_skin.Parts.Center)
					control_skin.Parts.Bottom->Position(
						{0.0_r, -control_skin.Parts.Center->Size().Y() * 0.5_r - control_skin.Parts.Bottom->Size().Y() * 0.5_r, 0.0_r});
				else if (control_skin.Parts.Left)
					control_skin.Parts.Bottom->Position(
						{0.0_r, -control_skin.Parts.Left->Size().Y() * 0.5_r - control_skin.Parts.Bottom->Size().Y() * 0.5_r, 0.0_r});
				else if (control_skin.Parts.Right)
					control_skin.Parts.Bottom->Position(
						{0.0_r, -control_skin.Parts.Right->Size().Y() * 0.5_r - control_skin.Parts.Bottom->Size().Y() * 0.5_r, 0.0_r});
			}

			//Left part (position)
			if (control_skin.Parts.Left)
			{
				if (control_skin.Parts.Center)
					control_skin.Parts.Left->Position(
						{-control_skin.Parts.Center->Size().X() * 0.5_r - control_skin.Parts.Left->Size().X() * 0.5_r, 0.0_r, 0.0_r});
				else if (control_skin.Parts.Top)
					control_skin.Parts.Left->Position(
						{-control_skin.Parts.Top->Size().X() * 0.5_r - control_skin.Parts.Left->Size().X() * 0.5_r, 0.0_r, 0.0_r});
				else if (control_skin.Parts.Bottom)
					control_skin.Parts.Left->Position(
						{-control_skin.Parts.Bottom->Size().X() * 0.5_r - control_skin.Parts.Left->Size().X() * 0.5_r, 0.0_r, 0.0_r});
			}

			//Right part (position)
			if (control_skin.Parts.Right)
			{
				if (control_skin.Parts.Center)
					control_skin.Parts.Right->Position(
						{control_skin.Parts.Center->Size().X() * 0.5_r + control_skin.Parts.Right->Size().X() * 0.5_r, 0.0_r, 0.0_r});
				else if (control_skin.Parts.Top)
					control_skin.Parts.Right->Position(
						{control_skin.Parts.Top->Size().X() * 0.5_r + control_skin.Parts.Right->Size().X() * 0.5_r, 0.0_r, 0.0_r});
				else if (control_skin.Parts.Bottom)
					control_skin.Parts.Right->Position(
						{control_skin.Parts.Bottom->Size().X() * 0.5_r + control_skin.Parts.Right->Size().X() * 0.5_r, 0.0_r, 0.0_r});
			}

			//Corners
			//Top-left part (position)
			if (control_skin.Parts.TopLeft)
			{
				if (control_skin.Parts.Center)
					control_skin.Parts.TopLeft->Position(
						{-control_skin.Parts.Center->Size().X() * 0.5_r - control_skin.Parts.TopLeft->Size().X() * 0.5_r,
						control_skin.Parts.Center->Size().Y() * 0.5_r + control_skin.Parts.TopLeft->Size().Y() * 0.5_r, 0.0_r});
				else if (control_skin.Parts.Top && control_skin.Parts.Left)
					control_skin.Parts.TopLeft->Position(
						{-control_skin.Parts.Top->Size().X() * 0.5_r - control_skin.Parts.TopLeft->Size().X() * 0.5_r,
						control_skin.Parts.Left->Size().Y() * 0.5_r + control_skin.Parts.TopLeft->Size().Y() * 0.5_r, 0.0_r});
			}

			//Top-right part (position)
			if (control_skin.Parts.TopRight)
			{
				if (control_skin.Parts.Center)
					control_skin.Parts.TopRight->Position(
						{control_skin.Parts.Center->Size().X() * 0.5_r + control_skin.Parts.TopRight->Size().X() * 0.5_r,
						control_skin.Parts.Center->Size().Y() * 0.5_r + control_skin.Parts.TopRight->Size().Y() * 0.5_r, 0.0_r});
				else if (control_skin.Parts.Top && control_skin.Parts.Right)
					control_skin.Parts.TopRight->Position(
						{control_skin.Parts.Top->Size().X() * 0.5_r - control_skin.Parts.TopRight->Size().X() * 0.5_r,
						control_skin.Parts.Right->Size().Y() * 0.5_r + control_skin.Parts.TopRight->Size().Y() * 0.5_r, 0.0_r});
			}

			//Bottom-left part (position)
			if (control_skin.Parts.BottomLeft)
			{
				if (control_skin.Parts.Center)
					control_skin.Parts.BottomLeft->Position(
						{-control_skin.Parts.Center->Size().X() * 0.5_r - control_skin.Parts.BottomLeft->Size().X() * 0.5_r,
						-control_skin.Parts.Center->Size().Y() * 0.5_r - control_skin.Parts.BottomLeft->Size().Y() * 0.5_r, 0.0_r});
				else if (control_skin.Parts.Bottom && control_skin.Parts.Left)
					control_skin.Parts.BottomLeft->Position(
						{-control_skin.Parts.Bottom->Size().X() * 0.5_r - control_skin.Parts.BottomLeft->Size().X() * 0.5_r,
						-control_skin.Parts.Left->Size().Y() * 0.5_r - control_skin.Parts.BottomLeft->Size().Y() * 0.5_r, 0.0_r});
			}

			//Bottom-right part (position)
			if (control_skin.Parts.BottomRight)
			{
				if (control_skin.Parts.Center)
					control_skin.Parts.BottomRight->Position(
						{control_skin.Parts.Center->Size().X() * 0.5_r + control_skin.Parts.BottomRight->Size().X() * 0.5_r,
						-control_skin.Parts.Center->Size().Y() * 0.5_r - control_skin.Parts.BottomRight->Size().Y() * 0.5_r, 0.0_r});
				else if (control_skin.Parts.Bottom && control_skin.Parts.Right)
					control_skin.Parts.BottomRight->Position(
						{control_skin.Parts.Bottom->Size().X() * 0.5_r + control_skin.Parts.BottomRight->Size().X() * 0.5_r,
						-control_skin.Parts.Right->Size().Y() * 0.5_r - control_skin.Parts.BottomRight->Size().Y() * 0.5_r, 0.0_r});
			}
		}
	}

	if (!std::empty(skin.TextParts()))
	{
		auto caption_part = skin.GetTextPart("caption");

		//Caption part
		if (caption_part && *caption_part)
		{
			auto text = scene_manager.CreateText(caption_part->Base);

			if (std::empty(skin.TextPasses()))
				text->AddPass(graphics::render::Pass{});
			else
				text->AddPasses(skin.GetTextPasses());

			control_skin.Caption.Object = text;
			control_skin.Caption.Enabled = caption_part->Enabled;
			control_skin.Caption.Disabled = caption_part->Disabled;
			control_skin.Caption.Focused = caption_part->Focused;
			control_skin.Caption.Pressed = caption_part->Pressed;
			control_skin.Caption.Hovered = caption_part->Hovered;
		}
	}

	return control_skin;
}


OwningPtr<controls::gui_control::ControlSkin> make_control_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto control_skin = make_owning<controls::gui_control::ControlSkin>();
	control_skin->Assign(make_skin_base(skin, scene_manager));
	return control_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_button_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto button_skin = make_owning<controls::gui_button::ButtonSkin>();
	button_skin->controls::gui_control::ControlSkin::Assign(make_skin_base(skin, scene_manager));
	return button_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_check_box_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto check_box_skin = make_owning<controls::gui_check_box::CheckBoxSkin>();
	check_box_skin->controls::gui_control::ControlSkin::Assign(make_skin_base(skin, scene_manager));

	if (!std::empty(skin.Parts()))
	{
		auto check_mark_part = skin.GetPart("check-mark");

		//Check mark part
		if (check_mark_part && *check_mark_part)
		{
			auto sprite = check_box_skin->Parts->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, check_mark_part->Enabled});
			sprite->FillColor(check_mark_part->FillColor);
			sprite->AutoSize(true);
			sprite->IncludeBoundingVolumes(false);

			check_box_skin->CheckMark.Object = sprite;
			check_box_skin->CheckMark.Enabled = check_mark_part->Enabled;
			check_box_skin->CheckMark.Disabled = check_mark_part->Disabled;
			check_box_skin->CheckMark.Focused = check_mark_part->Focused;
			check_box_skin->CheckMark.Pressed = check_mark_part->Pressed;
			check_box_skin->CheckMark.Hovered = check_mark_part->Hovered;
		}
	}

	return check_box_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_group_box_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto group_box_skin = make_owning<controls::gui_group_box::GroupBoxSkin>();
	group_box_skin->controls::gui_control::ControlSkin::Assign(make_skin_base(skin, scene_manager));
	return group_box_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_label_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto label_skin = make_owning<controls::gui_label::LabelSkin>();
	label_skin->controls::gui_control::ControlSkin::Assign(make_skin_base(skin, scene_manager));
	return label_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_list_box_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto list_box_skin = make_owning<controls::gui_list_box::ListBoxSkin>();
	list_box_skin->controls::gui_control::ControlSkin::Assign(make_skin_base(skin, scene_manager));

	if (!std::empty(skin.Parts()))
	{
		auto selection_part = skin.GetPart("selection");

		//Selection part
		if (selection_part && *selection_part)
		{
			auto sprite = list_box_skin->Parts->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, selection_part->Enabled});
			sprite->FillColor(selection_part->FillColor);
			sprite->AutoSize(true);
			sprite->AutoRepeat(true);
			sprite->IncludeBoundingVolumes(false);

			list_box_skin->Selection.Object = sprite;
			list_box_skin->Selection.Enabled = selection_part->Enabled;
			list_box_skin->Selection.Disabled = selection_part->Disabled;
			list_box_skin->Selection.Focused = selection_part->Focused;
			list_box_skin->Selection.Pressed = selection_part->Pressed;
			list_box_skin->Selection.Hovered = selection_part->Hovered;
		}
	}

	if (!std::empty(skin.TextParts()))
	{
		auto lines_part = skin.GetTextPart("lines");

		//Lines part
		if (lines_part && *lines_part)
		{
			auto text = scene_manager.CreateText(lines_part->Base);

			if (std::empty(skin.TextPasses()))
				text->AddPass(graphics::render::Pass{});
			else
				text->AddPasses(skin.GetTextPasses());

			list_box_skin->Lines.Object = text;
			list_box_skin->Lines.Enabled = lines_part->Enabled;
			list_box_skin->Lines.Disabled = lines_part->Disabled;
			list_box_skin->Lines.Focused = lines_part->Focused;
			list_box_skin->Lines.Pressed = lines_part->Pressed;
			list_box_skin->Lines.Hovered = lines_part->Hovered;
		}
	}

	return list_box_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_mouse_cursor_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto mouse_cursor_skin = make_owning<controls::gui_mouse_cursor::MouseCursorSkin>();
	mouse_cursor_skin->controls::gui_control::ControlSkin::Assign(make_skin_base(skin, scene_manager));
	return mouse_cursor_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_progress_bar_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto progress_bar_skin = make_owning<controls::gui_progress_bar::ProgressBarSkin>();
	progress_bar_skin->controls::gui_control::ControlSkin::Assign(make_skin_base(skin, scene_manager));

	if (!std::empty(skin.Parts()))
	{
		auto bar_part = skin.GetPart("bar");
		auto bar_interpolated_part = skin.GetPart("bar-interpolated");

		//Bar part
		if (bar_part && *bar_part)
		{
			auto sprite = progress_bar_skin->Parts->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, bar_part->Enabled});
			sprite->FillColor(bar_part->FillColor);
			sprite->AutoSize(true);
			sprite->AutoRepeat(true);
			sprite->IncludeBoundingVolumes(false);

			progress_bar_skin->Bar.Object = sprite;
			progress_bar_skin->Bar.Enabled = bar_part->Enabled;
			progress_bar_skin->Bar.Disabled = bar_part->Disabled;
			progress_bar_skin->Bar.Focused = bar_part->Focused;
			progress_bar_skin->Bar.Pressed = bar_part->Pressed;
			progress_bar_skin->Bar.Hovered = bar_part->Hovered;
		}

		//Bar interpolated part
		if (bar_interpolated_part && *bar_interpolated_part)
		{
			auto sprite = progress_bar_skin->Parts->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, bar_interpolated_part->Enabled});
			sprite->FillColor(bar_interpolated_part->FillColor);
			sprite->AutoSize(true);
			sprite->AutoRepeat(true);
			sprite->IncludeBoundingVolumes(false);

			progress_bar_skin->BarInterpolated.Object = sprite;
			progress_bar_skin->BarInterpolated.Enabled = bar_interpolated_part->Enabled;
			progress_bar_skin->BarInterpolated.Disabled = bar_interpolated_part->Disabled;
			progress_bar_skin->BarInterpolated.Focused = bar_interpolated_part->Focused;
			progress_bar_skin->BarInterpolated.Pressed = bar_interpolated_part->Pressed;
			progress_bar_skin->BarInterpolated.Hovered = bar_interpolated_part->Hovered;
		}
	}

	return progress_bar_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_radio_button_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto radio_button_skin = make_owning<controls::gui_radio_button::RadioButtonSkin>();
	radio_button_skin->controls::gui_control::ControlSkin::Assign(make_skin_base(skin, scene_manager));

	if (!std::empty(skin.Parts()))
	{
		auto check_mark_part = skin.GetPart("check-mark");

		//Check mark part
		if (check_mark_part && *check_mark_part)
		{
			auto sprite = radio_button_skin->Parts->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, check_mark_part->Enabled});
			sprite->FillColor(check_mark_part->FillColor);
			sprite->AutoSize(true);
			sprite->IncludeBoundingVolumes(false);

			radio_button_skin->CheckMark.Object = sprite;
			radio_button_skin->CheckMark.Enabled = check_mark_part->Enabled;
			radio_button_skin->CheckMark.Disabled = check_mark_part->Disabled;
			radio_button_skin->CheckMark.Focused = check_mark_part->Focused;
			radio_button_skin->CheckMark.Pressed = check_mark_part->Pressed;
			radio_button_skin->CheckMark.Hovered = check_mark_part->Hovered;
		}
	}

	return radio_button_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_scroll_bar_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto scroll_bar_skin = make_owning<controls::gui_scroll_bar::ScrollBarSkin>();
	scroll_bar_skin->controls::gui_control::ControlSkin::Assign(make_skin_base(skin, scene_manager));

	if (!std::empty(skin.Parts()))
	{
		auto handle_part = skin.GetPart("handle");

		//Handle part
		if (handle_part && *handle_part)
		{
			auto sprite = scroll_bar_skin->Parts->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, handle_part->Enabled});
			sprite->FillColor(handle_part->FillColor);
			sprite->AutoSize(true);
			sprite->IncludeBoundingVolumes(false);

			scroll_bar_skin->Handle.Object = sprite;
			scroll_bar_skin->Handle.Enabled = handle_part->Enabled;
			scroll_bar_skin->Handle.Disabled = handle_part->Disabled;
			scroll_bar_skin->Handle.Focused = handle_part->Focused;
			scroll_bar_skin->Handle.Pressed = handle_part->Pressed;
			scroll_bar_skin->Handle.Hovered = handle_part->Hovered;
		}
	}

	return scroll_bar_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_slider_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto slider_skin = make_owning<controls::gui_slider::SliderSkin>();
	slider_skin->controls::gui_control::ControlSkin::Assign(make_skin_base(skin, scene_manager));

	if (!std::empty(skin.Parts()))
	{
		auto handle_part = skin.GetPart("handle");

		//Handle part
		if (handle_part && *handle_part)
		{
			auto sprite = slider_skin->Parts->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, handle_part->Enabled});
			sprite->FillColor(handle_part->FillColor);
			sprite->AutoSize(true);
			sprite->IncludeBoundingVolumes(false);

			slider_skin->Handle.Object = sprite;
			slider_skin->Handle.Enabled = handle_part->Enabled;
			slider_skin->Handle.Disabled = handle_part->Disabled;
			slider_skin->Handle.Focused = handle_part->Focused;
			slider_skin->Handle.Pressed = handle_part->Pressed;
			slider_skin->Handle.Hovered = handle_part->Hovered;
		}
	}

	return slider_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_text_box_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto text_box_skin = make_owning<controls::gui_text_box::TextBoxSkin>();
	text_box_skin->controls::gui_control::ControlSkin::Assign(make_skin_base(skin, scene_manager));

	if (!std::empty(skin.Parts()))
	{
		auto cursor_part = skin.GetPart("cursor");

		//Cursor part
		if (cursor_part && *cursor_part)
		{
			auto sprite = text_box_skin->Parts->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, cursor_part->Enabled});
			sprite->FillColor(cursor_part->FillColor);
			sprite->AutoSize(true);
			sprite->IncludeBoundingVolumes(false);

			text_box_skin->Cursor.Object = sprite;
			text_box_skin->Cursor.Enabled = cursor_part->Enabled;
			text_box_skin->Cursor.Disabled = cursor_part->Disabled;
			text_box_skin->Cursor.Focused = cursor_part->Focused;
			text_box_skin->Cursor.Pressed = cursor_part->Pressed;
			text_box_skin->Cursor.Hovered = cursor_part->Hovered;
		}
	}

	if (!std::empty(skin.TextParts()))
	{
		auto text_part = skin.GetTextPart("text");
		auto placeholder_text_part = skin.GetTextPart("placeholder-text");

		//Text part
		if (text_part && *text_part)
		{
			auto text = scene_manager.CreateText(text_part->Base);

			if (std::empty(skin.TextPasses()))
				text->AddPass(graphics::render::Pass{});
			else
				text->AddPasses(skin.GetTextPasses());

			text_box_skin->Text.Object = text;
			text_box_skin->Text.Enabled = text_part->Enabled;
			text_box_skin->Text.Disabled = text_part->Disabled;
			text_box_skin->Text.Focused = text_part->Focused;
			text_box_skin->Text.Pressed = text_part->Pressed;
			text_box_skin->Text.Hovered = text_part->Hovered;
		}

		//Placeholder text part
		if (placeholder_text_part && *placeholder_text_part)
		{
			auto text = scene_manager.CreateText(placeholder_text_part->Base);

			if (std::empty(skin.TextPasses()))
				text->AddPass(graphics::render::Pass{});
			else
				text->AddPasses(skin.GetTextPasses());

			text_box_skin->PlaceholderText.Object = text;
			text_box_skin->PlaceholderText.Enabled = placeholder_text_part->Enabled;
			text_box_skin->PlaceholderText.Disabled = placeholder_text_part->Disabled;
			text_box_skin->PlaceholderText.Focused = placeholder_text_part->Focused;
			text_box_skin->PlaceholderText.Pressed = placeholder_text_part->Pressed;
			text_box_skin->PlaceholderText.Hovered = placeholder_text_part->Hovered;
		}
	}

	return text_box_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_tooltip_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto tooltip_skin = make_owning<controls::gui_tooltip::TooltipSkin>();
	tooltip_skin->controls::gui_control::ControlSkin::Assign(make_skin_base(skin, scene_manager));
	return tooltip_skin;
}

} //gui_skin::detail

//Private

void GuiSkin::RegisterBuiltInControls()
{
	static auto registered = false;

	if (!registered)
	{
		RegisterControl<controls::GuiButton>("GuiButton", detail::make_button_skin);
		RegisterControl<controls::GuiCheckBox>("GuiCheckBox", detail::make_check_box_skin);
		RegisterControl<controls::GuiGroupBox>("GuiGroupBox", detail::make_group_box_skin);
		RegisterControl<controls::GuiLabel>("GuiLabel", detail::make_label_skin);
		RegisterControl<controls::GuiListBox>("GuiListBox", detail::make_list_box_skin);
		RegisterControl<controls::GuiMouseCursor>("GuiMouseCursor", detail::make_mouse_cursor_skin);
		RegisterControl<controls::GuiProgressBar>("GuiProgressBar", detail::make_progress_bar_skin);
		RegisterControl<controls::GuiRadioButton>("GuiRadioButton", detail::make_radio_button_skin);
		RegisterControl<controls::GuiScrollBar>("GuiScrollBar", detail::make_scroll_bar_skin);
		RegisterControl<controls::GuiSlider>("GuiSlider", detail::make_slider_skin);
		RegisterControl<controls::GuiTextBox>("GuiTextBox", detail::make_text_box_skin);
		RegisterControl<controls::GuiTooltip>("GuiTooltip", detail::make_tooltip_skin);
		registered = true;
	}
}


void GuiSkin::AddDefaultParts(const SkinParts &parts, const SkinTextPart &caption_part)
{
	if (parts.Center)
		parts_["center"] = parts.Center;

	//Sides
	if (parts.Border.Sides.Top)
		parts_["top"] = parts.Border.Sides.Top;

	if (parts.Border.Sides.Bottom)
		parts_["bottom"] = parts.Border.Sides.Bottom;

	if (parts.Border.Sides.Left)
		parts_["left"] = parts.Border.Sides.Left;

	if (parts.Border.Sides.Right)
		parts_["right"] = parts.Border.Sides.Right;

	//Corners
	if (parts.Border.Corners.TopLeft)
		parts_["top-left"] = parts.Border.Corners.TopLeft;

	if (parts.Border.Corners.TopRight)
		parts_["top-right"] = parts.Border.Corners.TopRight;

	if (parts.Border.Corners.BottomLeft)
		parts_["bottom-left"] = parts.Border.Corners.BottomLeft;

	if (parts.Border.Corners.BottomRight)
		parts_["bottom-right"] = parts.Border.Corners.BottomRight;

	//Caption
	if (caption_part)
		text_parts_["caption"] = caption_part;
}


//Public

GuiSkin::GuiSkin(std::string name, std::type_index type) :

	managed::ManagedObject<GuiTheme>{std::move(name)},
	skin_builder_{GetSkinBuilder(type).value_or(detail::make_control_skin)}
{
	//Empty
}

GuiSkin::GuiSkin(std::string name, std::type_index type, const SkinParts &parts, const SkinTextPart &caption_part) :

	managed::ManagedObject<GuiTheme>{std::move(name)},
	skin_builder_{GetSkinBuilder(type).value_or(detail::make_control_skin)}
{
	AddDefaultParts(parts, caption_part);
}

GuiSkin::GuiSkin(std::string name, std::type_index type, const SkinBorderParts &border_parts, const SkinTextPart &caption_part) :
	GuiSkin{std::move(name), type, SkinParts{{}, border_parts}, caption_part}
{
	//Empty
}

GuiSkin::GuiSkin(std::string name, std::type_index type, const SkinSideParts &side_parts, const SkinTextPart &caption_part) :
	GuiSkin{std::move(name), type, SkinParts{{}, {side_parts, {}}}, caption_part}
{
	//Empty
}

GuiSkin::GuiSkin(std::string name, std::type_index type, const SkinPart &center_part, const SkinTextPart &caption_part) :
	GuiSkin{std::move(name), type, SkinParts{center_part}, caption_part}
{
	//Empty
}

GuiSkin::GuiSkin(std::string name, std::type_index type, const SkinTextPart &caption_part) :
	GuiSkin{std::move(name), type, SkinParts{}, caption_part}
{
	//Empty
}


/*
	Instantiating
*/

OwningPtr<controls::gui_control::ControlSkin> GuiSkin::Instantiate() const
{
	if (owner_)
	{
		if (auto scene_manager = owner_->ConnectedSceneManager(); scene_manager)
			return skin_builder_(*this, *scene_manager);
	}

	return nullptr;
}


/*
	Parts
	Adding
*/

void GuiSkin::AddPart(std::string name, const SkinPart &part)
{
	parts_[std::move(name)] = part;
}

void GuiSkin::AddTextPart(std::string name, const SkinTextPart &text_part)
{
	text_parts_[std::move(name)] = text_part;
}


/*
	Parts
	Retrieving
*/

const SkinPart* GuiSkin::GetPart(std::string_view name) const noexcept
{
	if (auto iter = parts_.find(name); iter != std::end(parts_))
		return &iter->second;
	else
		return nullptr;
}

const SkinTextPart* GuiSkin::GetTextPart(std::string_view name) const noexcept
{
	if (auto iter = text_parts_.find(name); iter != std::end(text_parts_))
		return &iter->second;
	else
		return nullptr;
}


/*
	Parts
	Removing
*/

void GuiSkin::ClearParts() noexcept
{
	parts_.clear();
	parts_.shrink_to_fit();
}

bool GuiSkin::RemovePart(std::string_view name) noexcept
{
	return parts_.erase(name);
}


void GuiSkin::ClearTextParts() noexcept
{
	text_parts_.clear();
	text_parts_.shrink_to_fit();
}

bool GuiSkin::RemoveTextPart(std::string_view name) noexcept
{
	return text_parts_.erase(name);
}


/*
	Passes
	Adding
*/

void GuiSkin::AddPass(graphics::render::Pass pass)
{
	passes_.push_back(std::move(pass));
}

void GuiSkin::AddPasses(graphics::scene::drawable_object::Passes passes)
{
	if (std::empty(passes_))
		passes_ = std::move(passes);
	else
		std::move(std::begin(passes), std::end(passes), std::back_inserter(passes_));
}


void GuiSkin::AddTextPass(graphics::render::Pass pass)
{
	text_passes_.push_back(std::move(pass));
}

void GuiSkin::AddTextPasses(graphics::scene::drawable_object::Passes passes)
{
	if (std::empty(text_passes_))
		text_passes_ = std::move(passes);
	else
		std::move(std::begin(passes), std::end(passes), std::back_inserter(text_passes_));
}


/*
	Passes
	Removing
*/

void GuiSkin::ClearPasses() noexcept
{
	passes_.clear();
	passes_.shrink_to_fit();
}

void GuiSkin::ClearTextPasses() noexcept
{
	text_passes_.clear();
	text_passes_.shrink_to_fit();
}


/*
	Static
	Retrieving
*/

std::optional<std::string_view> GuiSkin::GetDefaultSkinName(std::type_index type) noexcept
{
	RegisterBuiltInControls();

	if (auto iter = registered_controls_.find(type);
		iter != std::end(registered_controls_))
		return iter->second.first;
	else
		return {};
}

std::optional<SkinBuilder> GuiSkin::GetSkinBuilder(std::type_index type) noexcept
{
	RegisterBuiltInControls();

	if (auto iter = registered_controls_.find(type);
		iter != std::end(registered_controls_))
		return iter->second.second;
	else
		return {};
}

} //ion::gui::skins