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

controls::gui_control::ControlSkin make_control_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
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

			control_skin.Parts.ModelObject = model;

			//Center part
			if (center_part && *center_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, center_part->Enabled});
				sprite->FillColor(center_part->FillColor);
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);	

				control_skin.Parts.Center.SpriteObject = sprite;
				control_skin.Parts.Center.EnabledMaterial = center_part->Enabled;
				control_skin.Parts.Center.DisabledMaterial = center_part->Disabled;
				control_skin.Parts.Center.FocusedMaterial = center_part->Focused;
				control_skin.Parts.Center.PressedMaterial = center_part->Pressed;
				control_skin.Parts.Center.HoveredMaterial = center_part->Hovered;
			}

			//Sides
			//Top part
			if (top_part && *top_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, top_part->Enabled});
				sprite->FillColor(top_part->FillColor);
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);			

				control_skin.Parts.Top.SpriteObject = sprite;
				control_skin.Parts.Top.EnabledMaterial = top_part->Enabled;
				control_skin.Parts.Top.DisabledMaterial = top_part->Disabled;
				control_skin.Parts.Top.FocusedMaterial = top_part->Focused;
				control_skin.Parts.Top.PressedMaterial = top_part->Pressed;
				control_skin.Parts.Top.HoveredMaterial = top_part->Hovered;
			}

			//Bottom part
			if (bottom_part && *bottom_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, bottom_part->Enabled});
				sprite->FillColor(bottom_part->FillColor);
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.Bottom.SpriteObject = sprite;
				control_skin.Parts.Bottom.EnabledMaterial = bottom_part->Enabled;
				control_skin.Parts.Bottom.DisabledMaterial = bottom_part->Disabled;
				control_skin.Parts.Bottom.FocusedMaterial = bottom_part->Focused;
				control_skin.Parts.Bottom.PressedMaterial = bottom_part->Pressed;
				control_skin.Parts.Bottom.HoveredMaterial = bottom_part->Hovered;
			}

			//Left part
			if (left_part && *left_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, left_part->Enabled});
				sprite->FillColor(left_part->FillColor);
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.Left.SpriteObject = sprite;
				control_skin.Parts.Left.EnabledMaterial = left_part->Enabled;
				control_skin.Parts.Left.DisabledMaterial = left_part->Disabled;
				control_skin.Parts.Left.FocusedMaterial = left_part->Focused;
				control_skin.Parts.Left.PressedMaterial = left_part->Pressed;
				control_skin.Parts.Left.HoveredMaterial = left_part->Hovered;
			}

			//Right part
			if (right_part && *right_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, right_part->Enabled});
				sprite->FillColor(right_part->FillColor);
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.Right.SpriteObject = sprite;
				control_skin.Parts.Right.EnabledMaterial = right_part->Enabled;
				control_skin.Parts.Right.DisabledMaterial = right_part->Disabled;
				control_skin.Parts.Right.FocusedMaterial = right_part->Focused;
				control_skin.Parts.Right.PressedMaterial = right_part->Pressed;
				control_skin.Parts.Right.HoveredMaterial = right_part->Hovered;
			}

			//Corners
			//Top-left part
			if (top_left_part && *top_left_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, top_left_part->Enabled});
				sprite->FillColor(top_left_part->FillColor);
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.TopLeft.SpriteObject = sprite;
				control_skin.Parts.TopLeft.EnabledMaterial = top_left_part->Enabled;
				control_skin.Parts.TopLeft.DisabledMaterial = top_left_part->Disabled;
				control_skin.Parts.TopLeft.FocusedMaterial = top_left_part->Focused;
				control_skin.Parts.TopLeft.PressedMaterial = top_left_part->Pressed;
				control_skin.Parts.TopLeft.HoveredMaterial = top_left_part->Hovered;
			}

			//Top-right part
			if (top_right_part && *top_right_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, top_right_part->Enabled});
				sprite->FillColor(top_right_part->FillColor);
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.TopRight.SpriteObject = sprite;
				control_skin.Parts.TopRight.EnabledMaterial = top_right_part->Enabled;
				control_skin.Parts.TopRight.DisabledMaterial = top_right_part->Disabled;
				control_skin.Parts.TopRight.FocusedMaterial = top_right_part->Focused;
				control_skin.Parts.TopRight.PressedMaterial = top_right_part->Pressed;
				control_skin.Parts.TopRight.HoveredMaterial = top_right_part->Hovered;
			}

			//Bottom-left part
			if (bottom_left_part && *bottom_left_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, bottom_left_part->Enabled});
				sprite->FillColor(bottom_left_part->FillColor);
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.BottomLeft.SpriteObject = sprite;
				control_skin.Parts.BottomLeft.EnabledMaterial = bottom_left_part->Enabled;
				control_skin.Parts.BottomLeft.DisabledMaterial = bottom_left_part->Disabled;
				control_skin.Parts.BottomLeft.FocusedMaterial = bottom_left_part->Focused;
				control_skin.Parts.BottomLeft.PressedMaterial = bottom_left_part->Pressed;
				control_skin.Parts.BottomLeft.HoveredMaterial = bottom_left_part->Hovered;
			}

			//Bottom-right part
			if (bottom_right_part && *bottom_right_part)
			{
				auto sprite = model->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, bottom_right_part->Enabled});
				sprite->FillColor(bottom_right_part->FillColor);
				sprite->AutoSize(true);
				sprite->AutoRepeat(true);

				control_skin.Parts.BottomRight.SpriteObject = sprite;
				control_skin.Parts.BottomRight.EnabledMaterial = bottom_right_part->Enabled;
				control_skin.Parts.BottomRight.DisabledMaterial = bottom_right_part->Disabled;
				control_skin.Parts.BottomRight.FocusedMaterial = bottom_right_part->Focused;
				control_skin.Parts.BottomRight.PressedMaterial = bottom_right_part->Pressed;
				control_skin.Parts.BottomRight.HoveredMaterial = bottom_right_part->Hovered;
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
			auto text = scene_manager.CreateText(caption_part->Template);

			if (std::empty(skin.TextPasses()))
				text->AddPass(graphics::render::Pass{});
			else
				text->AddPasses(skin.GetTextPasses());

			control_skin.Caption.TextObject = text;
			control_skin.Caption.EnabledStyle = caption_part->Enabled;
			control_skin.Caption.DisabledStyle = caption_part->Disabled;
			control_skin.Caption.FocusedStyle = caption_part->Focused;
			control_skin.Caption.PressedStyle = caption_part->Pressed;
			control_skin.Caption.HoveredStyle = caption_part->Hovered;
		}
	}

	return control_skin;
}


OwningPtr<controls::gui_control::ControlSkin> make_button_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto button_skin = make_owning<controls::gui_button::ButtonSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*button_skin) = make_control_skin(skin, scene_manager);
	return button_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_check_box_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto check_box_skin = make_owning<controls::gui_check_box::CheckBoxSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*check_box_skin) = make_control_skin(skin, scene_manager);

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

			check_box_skin->CheckMark.SpriteObject = sprite;
			check_box_skin->CheckMark.EnabledMaterial = check_mark_part->Enabled;
			check_box_skin->CheckMark.DisabledMaterial = check_mark_part->Disabled;
			check_box_skin->CheckMark.FocusedMaterial = check_mark_part->Focused;
			check_box_skin->CheckMark.PressedMaterial = check_mark_part->Pressed;
			check_box_skin->CheckMark.HoveredMaterial = check_mark_part->Hovered;
		}
	}

	return check_box_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_group_box_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto group_box_skin = make_owning<controls::gui_group_box::GroupBoxSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*group_box_skin) = make_control_skin(skin, scene_manager);
	return group_box_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_label_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto label_skin = make_owning<controls::gui_label::LabelSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*label_skin) = make_control_skin(skin, scene_manager);
	return label_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_list_box_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto list_box_skin = make_owning<controls::gui_list_box::ListBoxSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*list_box_skin) = make_control_skin(skin, scene_manager);

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

			list_box_skin->Selection.SpriteObject = sprite;
			list_box_skin->Selection.EnabledMaterial = selection_part->Enabled;
			list_box_skin->Selection.DisabledMaterial = selection_part->Disabled;
			list_box_skin->Selection.FocusedMaterial = selection_part->Focused;
			list_box_skin->Selection.PressedMaterial = selection_part->Pressed;
			list_box_skin->Selection.HoveredMaterial = selection_part->Hovered;
		}
	}

	if (!std::empty(skin.TextParts()))
	{
		auto lines_part = skin.GetTextPart("lines");

		//Lines part
		if (lines_part && *lines_part)
		{
			auto text = scene_manager.CreateText(lines_part->Template);

			if (std::empty(skin.TextPasses()))
				text->AddPass(graphics::render::Pass{});
			else
				text->AddPasses(skin.GetTextPasses());

			list_box_skin->Lines.TextObject = text;
			list_box_skin->Lines.EnabledStyle = lines_part->Enabled;
			list_box_skin->Lines.DisabledStyle = lines_part->Disabled;
			list_box_skin->Lines.FocusedStyle = lines_part->Focused;
			list_box_skin->Lines.PressedStyle = lines_part->Pressed;
			list_box_skin->Lines.HoveredStyle = lines_part->Hovered;
		}
	}

	return list_box_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_progress_bar_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto progress_bar_skin = make_owning<controls::gui_progress_bar::ProgressBarSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*progress_bar_skin) = make_control_skin(skin, scene_manager);

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

			progress_bar_skin->Bar.SpriteObject = sprite;
			progress_bar_skin->Bar.EnabledMaterial = bar_part->Enabled;
			progress_bar_skin->Bar.DisabledMaterial = bar_part->Disabled;
			progress_bar_skin->Bar.FocusedMaterial = bar_part->Focused;
			progress_bar_skin->Bar.PressedMaterial = bar_part->Pressed;
			progress_bar_skin->Bar.HoveredMaterial = bar_part->Hovered;
		}

		//Bar interpolated part
		if (bar_interpolated_part && *bar_interpolated_part)
		{
			auto sprite = progress_bar_skin->Parts->CreateMesh(graphics::scene::shapes::Sprite{vector2::Zero, bar_interpolated_part->Enabled});
			sprite->FillColor(bar_interpolated_part->FillColor);
			sprite->AutoSize(true);
			sprite->AutoRepeat(true);
			sprite->IncludeBoundingVolumes(false);

			progress_bar_skin->BarInterpolated.SpriteObject = sprite;
			progress_bar_skin->BarInterpolated.EnabledMaterial = bar_interpolated_part->Enabled;
			progress_bar_skin->BarInterpolated.DisabledMaterial = bar_interpolated_part->Disabled;
			progress_bar_skin->BarInterpolated.FocusedMaterial = bar_interpolated_part->Focused;
			progress_bar_skin->BarInterpolated.PressedMaterial = bar_interpolated_part->Pressed;
			progress_bar_skin->BarInterpolated.HoveredMaterial = bar_interpolated_part->Hovered;
		}
	}

	return progress_bar_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_radio_button_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto radio_button_skin = make_owning<controls::gui_radio_button::RadioButtonSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*radio_button_skin) = make_control_skin(skin, scene_manager);

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

			radio_button_skin->CheckMark.SpriteObject = sprite;
			radio_button_skin->CheckMark.EnabledMaterial = check_mark_part->Enabled;
			radio_button_skin->CheckMark.DisabledMaterial = check_mark_part->Disabled;
			radio_button_skin->CheckMark.FocusedMaterial = check_mark_part->Focused;
			radio_button_skin->CheckMark.PressedMaterial = check_mark_part->Pressed;
			radio_button_skin->CheckMark.HoveredMaterial = check_mark_part->Hovered;
		}
	}

	return radio_button_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_scroll_bar_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto scroll_bar_skin = make_owning<controls::gui_scroll_bar::ScrollBarSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*scroll_bar_skin) = make_control_skin(skin, scene_manager);

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

			scroll_bar_skin->Handle.SpriteObject = sprite;
			scroll_bar_skin->Handle.EnabledMaterial = handle_part->Enabled;
			scroll_bar_skin->Handle.DisabledMaterial = handle_part->Disabled;
			scroll_bar_skin->Handle.FocusedMaterial = handle_part->Focused;
			scroll_bar_skin->Handle.PressedMaterial = handle_part->Pressed;
			scroll_bar_skin->Handle.HoveredMaterial = handle_part->Hovered;
		}
	}

	return scroll_bar_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_slider_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto slider_skin = make_owning<controls::gui_slider::SliderSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*slider_skin) = make_control_skin(skin, scene_manager);

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

			slider_skin->Handle.SpriteObject = sprite;
			slider_skin->Handle.EnabledMaterial = handle_part->Enabled;
			slider_skin->Handle.DisabledMaterial = handle_part->Disabled;
			slider_skin->Handle.FocusedMaterial = handle_part->Focused;
			slider_skin->Handle.PressedMaterial = handle_part->Pressed;
			slider_skin->Handle.HoveredMaterial = handle_part->Hovered;
		}
	}

	return slider_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_text_box_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto text_box_skin = make_owning<controls::gui_text_box::TextBoxSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*text_box_skin) = make_control_skin(skin, scene_manager);

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

			text_box_skin->Cursor.SpriteObject = sprite;
			text_box_skin->Cursor.EnabledMaterial = cursor_part->Enabled;
			text_box_skin->Cursor.DisabledMaterial = cursor_part->Disabled;
			text_box_skin->Cursor.FocusedMaterial = cursor_part->Focused;
			text_box_skin->Cursor.PressedMaterial = cursor_part->Pressed;
			text_box_skin->Cursor.HoveredMaterial = cursor_part->Hovered;
		}
	}

	if (!std::empty(skin.TextParts()))
	{
		auto text_part = skin.GetTextPart("text");
		auto placeholder_text_part = skin.GetTextPart("placeholder-text");

		//Text part
		if (text_part && *text_part)
		{
			auto text = scene_manager.CreateText(text_part->Template);

			if (std::empty(skin.TextPasses()))
				text->AddPass(graphics::render::Pass{});
			else
				text->AddPasses(skin.GetTextPasses());

			text_box_skin->Text.TextObject = text;
			text_box_skin->Text.EnabledStyle = text_part->Enabled;
			text_box_skin->Text.DisabledStyle = text_part->Disabled;
			text_box_skin->Text.FocusedStyle = text_part->Focused;
			text_box_skin->Text.PressedStyle = text_part->Pressed;
			text_box_skin->Text.HoveredStyle = text_part->Hovered;
		}

		//Placeholder text part
		if (placeholder_text_part && *placeholder_text_part)
		{
			auto text = scene_manager.CreateText(placeholder_text_part->Template);

			if (std::empty(skin.TextPasses()))
				text->AddPass(graphics::render::Pass{});
			else
				text->AddPasses(skin.GetTextPasses());

			text_box_skin->PlaceholderText.TextObject = text;
			text_box_skin->PlaceholderText.EnabledStyle = placeholder_text_part->Enabled;
			text_box_skin->PlaceholderText.DisabledStyle = placeholder_text_part->Disabled;
			text_box_skin->PlaceholderText.FocusedStyle = placeholder_text_part->Focused;
			text_box_skin->PlaceholderText.PressedStyle = placeholder_text_part->Pressed;
			text_box_skin->PlaceholderText.HoveredStyle = placeholder_text_part->Hovered;
		}
	}

	return text_box_skin;
}

OwningPtr<controls::gui_control::ControlSkin> make_tooltip_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager)
{
	auto tooltip_skin = make_owning<controls::gui_tooltip::TooltipSkin>();
	static_cast<controls::gui_control::ControlSkin&>(*tooltip_skin) = make_control_skin(skin, scene_manager);
	return tooltip_skin;
}

} //gui_skin::detail

//Private

void GuiSkin::RegisterDefaultSkins()
{
	static auto registered = false;

	if (!registered)
	{
		RegisterSkin<controls::GuiButton>("GuiButton", detail::make_button_skin);
		RegisterSkin<controls::GuiCheckBox>("GuiCheckBox", detail::make_check_box_skin);
		RegisterSkin<controls::GuiGroupBox>("GuiGroupBox", detail::make_group_box_skin);
		RegisterSkin<controls::GuiLabel>("GuiLabel", detail::make_label_skin);
		RegisterSkin<controls::GuiListBox>("GuiListBox", detail::make_list_box_skin);
		RegisterSkin<controls::GuiProgressBar>("GuiProgressBar", detail::make_progress_bar_skin);
		RegisterSkin<controls::GuiRadioButton>("GuiRadioButton", detail::make_radio_button_skin);
		RegisterSkin<controls::GuiScrollBar>("GuiScrollBar", detail::make_scroll_bar_skin);
		RegisterSkin<controls::GuiSlider>("GuiSlider", detail::make_slider_skin);
		RegisterSkin<controls::GuiTextBox>("GuiTextBox", detail::make_text_box_skin);
		RegisterSkin<controls::GuiTooltip>("GuiTooltip", detail::make_tooltip_skin);
		registered = true;
	}
}


//Public

GuiSkin::GuiSkin(std::string name) :
	managed::ManagedObject<GuiTheme>{std::move(name)}
{
	//Empty
}

GuiSkin::GuiSkin(std::string name, const SkinParts &parts, const SkinTextPart &caption) :
	managed::ManagedObject<GuiTheme>{std::move(name)}
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
	if (caption)
		text_parts_["caption"] = caption;
}

GuiSkin::GuiSkin(std::string name, const SkinBorderParts &border_parts, const SkinTextPart &caption) :
	GuiSkin{std::move(name), SkinParts{{}, border_parts}, caption}
{
	//Empty
}

GuiSkin::GuiSkin(std::string name, const SkinSideParts &side_parts, const SkinTextPart &caption) :
	GuiSkin{std::move(name), SkinParts{{}, {side_parts, {}}}, caption}
{
	//Empty
}

GuiSkin::GuiSkin(std::string name, const SkinPart &center, const SkinTextPart &caption) :
	GuiSkin{std::move(name), SkinParts{center}, caption}
{
	//Empty
}

GuiSkin::GuiSkin(std::string name, const SkinTextPart &caption) :
	GuiSkin{std::move(name), SkinParts{}, caption}
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
		{
			RegisterDefaultSkins();

			if (auto iter = registered_skin_builders_.find(*name_);
				iter != std::end(registered_skin_builders_))
				return iter->second(*this, *scene_manager);
		}
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

const gui_skin::SkinPart* GuiSkin::GetPart(std::string_view name) const noexcept
{
	if (auto iter = parts_.find(name); iter != std::end(parts_))
		return &iter->second;
	else
		return nullptr;
}

const gui_skin::SkinTextPart* GuiSkin::GetTextPart(std::string_view name) const noexcept
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
	Retrieving
*/

const graphics::scene::drawable_object::Passes& GuiSkin::GetPasses() const noexcept
{
	return passes_;
}

const graphics::scene::drawable_object::Passes& GuiSkin::GetTextPasses() const noexcept
{
	return text_passes_;
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

} //ion::gui::skins