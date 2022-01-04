/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiControl.cpp
-------------------------------------------
*/

#include "IonGuiControl.h"

#include "graphics/render/IonViewport.h"
#include "graphics/scene/IonDrawableText.h"
#include "graphics/scene/IonModel.h"
#include "graphics/scene/IonSceneManager.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "graphics/scene/shapes/IonSprite.h"
#include "graphics/utilities/IonMatrix3.h"
#include "graphics/utilities/IonMatrix4.h"
#include "graphics/utilities/IonVector3.h"
#include "graphics/utilities/IonObb.h"
#include "gui/IonGuiFrame.h"
#include "gui/IonGuiPanelContainer.h"
#include "types/IonTypeTraits.h"

namespace ion::gui::controls
{

using namespace gui_control;

namespace gui_control::detail
{

/*
	Skins
*/

void resize_sprite(graphics::scene::shapes::Sprite &sprite, const Vector2 &delta_size, const Vector2 &delta_position, const Vector2 &center) noexcept
{
	auto position = Vector2{sprite.Position()};
	auto sign = (position - center).SignCopy();
	sprite.Position(sprite.Position() + sign * delta_position);
	sprite.Size(sprite.Size() + delta_size);
}

void resize_part(ControlVisualPart &part, const Vector2 &delta_size, const Vector2 &delta_position, const Vector2 &center) noexcept
{
	if (part)
		resize_sprite(*part.SpriteObject, delta_size, delta_position, center);
}

void resize_skin(ControlSkin &skin, const Vector2 &from_size, const Vector2 &to_size) noexcept
{
	auto delta_size = to_size - from_size;
	auto delta_position = delta_size * 0.5_r;
	auto [delta_width, delta_height] = delta_size.XY();

	if (skin.Parts)
	{
		auto &center = skin.Parts.Center ?
			skin.Parts.Center->Position() :
			vector3::Zero;

		//Center
		resize_part(skin.Parts.Center, delta_size, vector2::Zero, center);

		//Sides
		resize_part(skin.Parts.Top, {delta_width, 0.0_r}, delta_position, center);
		resize_part(skin.Parts.Left, {0.0_r, delta_height}, delta_position, center);
		resize_part(skin.Parts.Bottom, {delta_width, 0.0_r}, delta_position, center);
		resize_part(skin.Parts.Right, {0.0_r, delta_height}, delta_position, center);

		//Corners
		resize_part(skin.Parts.TopLeft, vector2::Zero, delta_position, center);
		resize_part(skin.Parts.TopRight, vector2::Zero, delta_position, center);
		resize_part(skin.Parts.BottomLeft, vector2::Zero, delta_position, center);
		resize_part(skin.Parts.BottomRight, vector2::Zero, delta_position, center);
	}
}


void resize_hit_box(Aabb &hit_box, const Vector2 &scaling) noexcept
{
	hit_box.Transform(Matrix3::Transformation(0.0_r, scaling, vector2::Zero));
}

void resize_hit_boxes(BoundingBoxes &hit_boxes, const Vector2 &from_size, const Vector2 &to_size) noexcept
{
	auto scaling = to_size / from_size;

	for (auto &hit_box : hit_boxes)
		resize_hit_box(hit_box, scaling);
}


std::optional<Aabb> get_area(const ControlSkin &skin, bool include_caption) noexcept
{
	if (skin.Parts)
	{
		skin.Parts->Prepare();
		return skin.Parts->AxisAlignedBoundingBox();
	}
	else if (skin.Caption && include_caption)
	{
		skin.Caption->Prepare();
		return skin.Caption->AxisAlignedBoundingBox();
	}
	else
		return {};
}

std::optional<Aabb> get_center_area(const ControlSkin &skin, bool include_caption) noexcept
{
	if (skin.Parts)
	{
		skin.Parts->Prepare();

		//Has center
		if (skin.Parts.Center)
			return skin.Parts.Center->AxisAlignedBoundingBox();

		//Use sides to find center
		else if (skin.Parts.Top && skin.Parts.Bottom)
		{
			auto [t_min, t_max] = skin.Parts.Top->AxisAlignedBoundingBox().MinMax();
			auto [b_min, b_max] = skin.Parts.Bottom->AxisAlignedBoundingBox().MinMax();
			return Aabb{{b_min.X(), b_max.Y()}, {t_max.X(), t_min.Y()}};
		}		
		else if (skin.Parts.Left && skin.Parts.Right)
		{
			auto [l_min, l_max] = skin.Parts.Left->AxisAlignedBoundingBox().MinMax();
			auto [r_min, r_max] = skin.Parts.Right->AxisAlignedBoundingBox().MinMax();
			return Aabb{{l_max.X(), l_min.Y()}, {r_min.X(), r_max.Y()}};
		}

		//Use corners to find center
		else if (skin.Parts.TopLeft && skin.Parts.BottomRight)
		{
			auto [tl_min, tl_max] = skin.Parts.TopLeft->AxisAlignedBoundingBox().MinMax();
			auto [br_min, br_max] = skin.Parts.BottomRight->AxisAlignedBoundingBox().MinMax();
			return Aabb{{tl_max.X(), br_max.Y()}, {br_min.X(), tl_min.Y()}};
		}
		else if (skin.Parts.BottomLeft && skin.Parts.TopRight)
			return Aabb{skin.Parts.BottomLeft->AxisAlignedBoundingBox().Max(),
						skin.Parts.TopRight->AxisAlignedBoundingBox().Min()};
	}
	//Use caption as center
	else if (skin.Caption && include_caption)
	{
		skin.Caption->Prepare();
		return skin.Caption->AxisAlignedBoundingBox();
	}
	
	return {};
}

std::optional<Aabb> get_inner_area(const ControlSkin &skin, bool include_caption) noexcept
{
	if (auto center_area = get_center_area(skin, include_caption); center_area)
		return center_area;
	else if (auto area = get_area(skin, include_caption); area)
		return area;
	else
		return {};
}


std::optional<Vector2> get_size(const ControlSkin &skin, bool include_caption) noexcept
{
	if (auto area = get_area(skin, include_caption); area)
		return area->ToSize();
	else
		return {};
}

std::optional<Vector2> get_center_size(const ControlSkin &skin, bool include_caption) noexcept
{
	if (auto area = get_center_area(skin, include_caption); area)
		return area->ToSize();
	else
		return {};
}

std::optional<Vector2> get_inner_size(const ControlSkin &skin, bool include_caption) noexcept
{
	if (auto area = get_inner_area(skin, include_caption); area)
		return area->ToSize();
	else
		return {};
}

std::optional<Vector2> get_border_size(const ControlSkin &skin, bool include_caption) noexcept
{
	if (auto size = get_size(skin, include_caption); size)
	{
		if (auto center_size = get_center_size(skin, include_caption); center_size)
			return *size - *center_size;
	}

	return {};
}


Vector2 caption_offset(ControlCaptionLayout caption_layout, const Vector2 &size, const Vector2 &border_size, const Vector2 &margin_size) noexcept
{
	auto half_size = size * 0.5_r;

	auto x =
		[&]() noexcept
		{
			switch (caption_layout)
			{
				case ControlCaptionLayout::OutsideLeftTop:
				case ControlCaptionLayout::OutsideLeftCenter:
				case ControlCaptionLayout::OutsideLeftBottom:
				return -half_size.X() - border_size.X() - margin_size.X();

				case ControlCaptionLayout::OutsideRightTop:
				case ControlCaptionLayout::OutsideRightCenter:
				case ControlCaptionLayout::OutsideRightBottom:
				return half_size.X() + border_size.X() + margin_size.X();

				case ControlCaptionLayout::OutsideTopLeft:
				case ControlCaptionLayout::OutsideBottomLeft:
				return -half_size.X();

				case ControlCaptionLayout::OutsideTopRight:
				case ControlCaptionLayout::OutsideBottomRight:
				return half_size.X();

				default:
				return 0.0_r;
			}
		}();

	auto y =
		[&]() noexcept
		{
			switch (caption_layout)
			{
				case ControlCaptionLayout::OutsideTopLeft:
				case ControlCaptionLayout::OutsideTopCenter:
				case ControlCaptionLayout::OutsideTopRight:
				return half_size.Y() + border_size.Y() + margin_size.Y();

				case ControlCaptionLayout::OutsideBottomLeft:
				case ControlCaptionLayout::OutsideBottomCenter:
				case ControlCaptionLayout::OutsideBottomRight:
				return -half_size.Y() - border_size.Y() - margin_size.Y();

				case ControlCaptionLayout::OutsideLeftTop:
				case ControlCaptionLayout::OutsideRightTop:				
				return half_size.Y();

				case ControlCaptionLayout::OutsideLeftBottom:
				case ControlCaptionLayout::OutsideRightBottom:
				return -half_size.Y();

				default:
				return 0.0_r;
			}
		}();

	return {x, y};
}

Vector2 caption_area_offset(ControlCaptionLayout caption_layout, const Vector2 &size, const Vector2 &border_size) noexcept
{
	switch (caption_layout)
	{
		case ControlCaptionLayout::OutsideLeftTop:
		case ControlCaptionLayout::OutsideLeftCenter:
		case ControlCaptionLayout::OutsideLeftBottom:
		return {-size.X() - border_size.X(), 0.0_r};

		case ControlCaptionLayout::OutsideRightTop:
		case ControlCaptionLayout::OutsideRightCenter:
		case ControlCaptionLayout::OutsideRightBottom:
		return {size.X() + border_size.X(), 0.0_r};

		case ControlCaptionLayout::OutsideTopLeft:
		case ControlCaptionLayout::OutsideTopCenter:
		case ControlCaptionLayout::OutsideTopRight:
		return {0.0_r, size.Y() + border_size.Y()};

		case ControlCaptionLayout::OutsideBottomLeft:
		case ControlCaptionLayout::OutsideBottomCenter:
		case ControlCaptionLayout::OutsideBottomRight:
		return {0.0_r, -size.Y() - border_size.Y()};

		default:
		return vector2::Zero;
	}
}

} //gui_control::detail

//Protected

/*
	Events
*/

void GuiControl::Created() noexcept
{
	AttachSkin();

	if (skin_)
	{
		if (auto to_size = size_; to_size)
		{
			size_ = detail::get_size(*skin_, true);
			Size(*to_size);
		}
	}
}

void GuiControl::Removed() noexcept
{
	DetachSkin();
}


void GuiControl::Enabled() noexcept
{
	SetState(ControlState::Enabled);
	NotifyControlEnabled();
	GuiComponent::Enabled(); //Use base functionality
}

void GuiControl::Disabled() noexcept
{
	Reset();
	SetState(ControlState::Disabled);
	NotifyControlDisabled();
	GuiComponent::Disabled(); //Use base functionality
}


void GuiControl::Shown() noexcept
{
	SetState(state_);
	NotifyControlShown();
	GuiComponent::Shown(); //Use base functionality
}

void GuiControl::Hidden() noexcept
{
	Reset();
	NotifyControlHidden();
	GuiComponent::Hidden(); //Use base functionality
}


void GuiControl::Focused() noexcept
{
	if (state_ == ControlState::Enabled)
		SetState(ControlState::Focused);

	NotifyControlFocused();
}

void GuiControl::Defocused() noexcept
{
	if (state_ == ControlState::Focused)
		SetState(ControlState::Enabled);

	NotifyControlDefocused();
}


void GuiControl::Pressed() noexcept
{
	Focus();
	SetState(ControlState::Pressed);
	NotifyControlPressed();
}

void GuiControl::Released() noexcept
{
	if (hovered_)
		SetState(ControlState::Hovered);
	else if (focused_)
		SetState(ControlState::Focused);
	else
		SetState(ControlState::Enabled);

	NotifyControlReleased();
}

void GuiControl::Clicked() noexcept
{
	NotifyControlClicked();
}


void GuiControl::Entered() noexcept
{
	if (state_ != ControlState::Pressed)
		SetState(ControlState::Hovered);

	NotifyControlEntered();
}

void GuiControl::Exited() noexcept
{
	if (state_ != ControlState::Pressed)
	{
		if (focused_)
			SetState(ControlState::Focused);
		else
			SetState(ControlState::Enabled);
	}

	NotifyControlExited();
}


void GuiControl::Changed() noexcept
{
	NotifyControlChanged();
}

void GuiControl::StateChanged() noexcept
{
	NotifyControlStateChanged();
}


void GuiControl::Resized([[maybe_unused]] Vector2 from_size, [[maybe_unused]] Vector2 to_size) noexcept
{
	NotifyControlResized();
}


/*
	Notifying
*/

void GuiControl::NotifyControlEnabled() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Enabled, std::ref(*this));
	}
}

void GuiControl::NotifyControlDisabled() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Disabled, std::ref(*this));
	}
}


void GuiControl::NotifyControlShown() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Shown, std::ref(*this));
	}
}

void GuiControl::NotifyControlHidden() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Hidden, std::ref(*this));
	}
}


void GuiControl::NotifyControlFocused() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Focused, std::ref(*this));
	}

	//User callback
	if (on_focus_)
		(*on_focus_)(*this);
}

void GuiControl::NotifyControlDefocused() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Defocused, std::ref(*this));
	}

	//User callback
	if (on_defocus_)
		(*on_defocus_)(*this);
}


void GuiControl::NotifyControlPressed() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Pressed, std::ref(*this));
	}

	//User callback
	if (on_press_)
		(*on_press_)(*this);
}

void GuiControl::NotifyControlReleased() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Released, std::ref(*this));
	}

	//User callback
	if (on_release_)
		(*on_release_)(*this);
}

void GuiControl::NotifyControlClicked() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Clicked, std::ref(*this));
	}

	//User callback
	if (on_click_)
		(*on_click_)(*this);
}


void GuiControl::NotifyControlEntered() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Entered, std::ref(*this));
	}

	//User callback
	if (on_enter_)
		(*on_enter_)(*this);
}

void GuiControl::NotifyControlExited() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Exited, std::ref(*this));
	}

	//User callback
	if (on_exit_)
		(*on_exit_)(*this);
}


void GuiControl::NotifyControlChanged() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Changed, std::ref(*this));
	}

	//User callback
	if (on_change_)
		(*on_change_)(*this);
}

void GuiControl::NotifyControlStateChanged() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::StateChanged, std::ref(*this));
	}

	//User callback
	if (on_state_change_)
		(*on_state_change_)(*this);
}


void GuiControl::NotifyControlResized() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Resized, std::ref(*this));
	}

	//User callback
	if (on_resize_)
		(*on_resize_)(*this);
}


/*
	States
*/

NonOwningPtr<graphics::materials::Material> GuiControl::GetStateMaterial(ControlState state, ControlVisualPart &part) noexcept
{
	auto material = detail::control_state_to_material(state, part);

	//Fallback
	if (!material)
	{
		//Check hovered
		if (hovered_ && state != ControlState::Hovered)
		{
			if (part.HoveredMaterial)
				return part.HoveredMaterial; //Display hovered material instead
		}

		//Check focused
		if (focused_ && state != ControlState::Focused)
		{
			if (part.FocusedMaterial)
				return part.FocusedMaterial; //Display focused material instead
		}

		//Check enabled
		if (state != ControlState::Enabled)
		{
			if (part.EnabledMaterial)
				return part.EnabledMaterial; //Display enabled material instead
		}
	}

	return material;
}

std::optional<graphics::fonts::text::TextBlockStyle>& GuiControl::GetStateStyle(ControlState state, ControlCaptionPart &part) noexcept
{
	auto &style = detail::control_state_to_style(state, part);

	//Fallback
	if (!style)
	{
		//Check hovered
		if (hovered_ && state != ControlState::Hovered)
		{
			if (part.HoveredStyle)
				return part.HoveredStyle; //Display hovered style instead
		}

		//Check focused
		if (focused_ && state != ControlState::Focused)
		{
			if (part.FocusedStyle)
				return part.FocusedStyle; //Display focused style instead
		}

		//Check enabled
		if (state != ControlState::Enabled)
		{
			if (part.EnabledStyle)
				return part.EnabledStyle; //Display enabled style instead
		}
	}

	return style;
}


void GuiControl::SetPartState(ControlState state, ControlVisualPart &part) noexcept
{
	if (part)
	{
		if (auto material = GetStateMaterial(state, part); material)
			part->SurfaceMaterial(material);
	}
}

void GuiControl::SetCaptionState(ControlState state, ControlCaptionPart &part) noexcept
{
	if (part)
	{
		//Get() will not reload vertex streams when called from an immutable reference
		if (const auto &c_part = *part.TextObject; c_part.Get())
		{
			if (auto &style = GetStateStyle(state, part); style)
			{
				if (style->ForegroundColor)
				{
					if (c_part.Get()->DefaultForegroundColor() != *style->ForegroundColor)
						part->Get()->DefaultForegroundColor(*style->ForegroundColor);
				}

				if (style->BackgroundColor)
				{
					if (c_part.Get()->DefaultBackgroundColor() != *style->BackgroundColor)
						part->Get()->DefaultBackgroundColor(*style->BackgroundColor);
				}

				if (style->FontStyle)
				{
					if (c_part.Get()->DefaultFontStyle() != *style->FontStyle)
						part->Get()->DefaultFontStyle(*style->FontStyle);
				}
				else if (c_part.Get()->DefaultFontStyle())
					part->Get()->DefaultFontStyle({});

				if (style->Decoration)
				{
					if (c_part.Get()->DefaultDecoration() != *style->Decoration)
						part->Get()->DefaultDecoration(*style->Decoration);
				}
				else if (c_part.Get()->DefaultDecoration())
					part->Get()->DefaultDecoration({});

				if (style->DecorationColor)
				{
					if (c_part.Get()->DefaultDecorationColor() != *style->DecorationColor)
						part->Get()->DefaultDecorationColor(*style->DecorationColor);
				}
				else if (c_part.Get()->DefaultDecorationColor())
					part->Get()->DefaultDecorationColor({});
			}
		}
	}
}

void GuiControl::SetSkinState(ControlState state, ControlSkin &skin) noexcept
{
	if (skin.Parts)
	{
		//Center
		SetPartState(state, skin.Parts.Center);

		//Sides
		SetPartState(state, skin.Parts.Top);
		SetPartState(state, skin.Parts.Left);
		SetPartState(state, skin.Parts.Bottom);
		SetPartState(state, skin.Parts.Right);

		//Corners
		SetPartState(state, skin.Parts.TopLeft);
		SetPartState(state, skin.Parts.BottomLeft);
		SetPartState(state, skin.Parts.TopRight);
		SetPartState(state, skin.Parts.BottomRight);
	}

	if (skin.Caption)
		SetCaptionState(state, skin.Caption);
}


void GuiControl::SetState(ControlState state) noexcept
{
	if (visible_ && skin_)
		SetSkinState(state, *skin_);

	if (state_ != state)
	{
		state_ = state;
		StateChanged();
	}
}


/*
	Skins
*/

void GuiControl::AttachSkin()
{
	if (skin_)
	{
		if (skin_->Parts)
		{
			if (auto node = skin_->Parts->ParentNode(); node)
				node->DetachObject(*skin_->Parts.ModelObject);
		
			if (node_) //Create node for all parts
				node_->CreateChildNode(node_->Visible())->AttachObject(*skin_->Parts.ModelObject);
		}

		if (skin_->Caption)
		{
			if (auto node = skin_->Caption->ParentNode(); node)
				node->DetachObject(*skin_->Caption.TextObject);
		
			if (node_) //Create node for caption
				node_->CreateChildNode(node_->Visible())->AttachObject(*skin_->Caption.TextObject);
		}
	}
	
	SetState(state_);
	UpdateCaption();
}

void GuiControl::DetachSkin() noexcept
{
	if (skin_)
	{
		if (skin_->Parts)
		{
			if (auto node = skin_->Parts->ParentNode(); node_ && node)
				node_->RemoveChildNode(*node); //Remove parts node
		}

		if (skin_->Caption)
		{
			if (auto node = skin_->Caption->ParentNode(); node_ && node)
				node_->RemoveChildNode(*node); //Remove caption node
		}
	}
}

void GuiControl::RemoveSkin() noexcept
{
	if (skin_)
	{
		DetachSkin();

		if (skin_->Parts)
			skin_->Parts->Owner()->RemoveModel(*skin_->Parts.ModelObject); //Remove all parts

		if (skin_->Caption)
			skin_->Caption->Owner()->RemoveText(*skin_->Caption.TextObject); //Remove caption

		skin_.reset();
	}
}


void GuiControl::UpdateCaption() noexcept
{
	if (skin_ && skin_->Caption)
	{
		//Caption text
		if (auto &text = skin_->Caption->Get(); text)
		{
			auto area_size = detail::get_inner_size(*skin_, false).value_or(vector2::Zero);
			auto border_size = detail::get_border_size(*skin_, false).value_or(vector2::Zero);
			auto center = detail::get_center_area(*skin_, false).value_or(aabb::Zero).Center();

			//Area size
			if (auto size = caption_size_.
				value_or(detail::is_caption_outside(caption_layout_) ? vector2::Zero : area_size);
				size != vector2::Zero)
			{
				auto ortho_viewport_ratio =
					[&]() noexcept
					{
						//Adjust area size from ortho to viewport space
						if (auto scene_manager = skin_->Caption->Owner(); scene_manager)
						{
							if (auto viewport = scene_manager->ConnectedViewport(); viewport)
								return viewport->OrthoToViewportRatio();
						}

						return vector2::UnitScale;
					}();

				text->AreaSize(size * ortho_viewport_ratio);

				if (auto node = skin_->Caption->ParentNode(); node)
					node->Position(center + detail::caption_area_offset(caption_layout_, size, border_size));
			}
			else
			{
				auto viewport_ortho_ratio =
					[&]() noexcept
					{
						//Adjust area size from ortho to viewport space
						if (auto scene_manager = skin_->Caption->Owner(); scene_manager)
						{
							if (auto viewport = scene_manager->ConnectedViewport(); viewport)
								return viewport->ViewportToOrthoRatio();
						}

						return vector2::UnitScale;
					}();

				text->AreaSize({});

				if (auto node = skin_->Caption->ParentNode(); node)
					node->Position(center + detail::caption_offset(caption_layout_, area_size, border_size,
						caption_margin_.value_or(detail::default_caption_margin_size) * viewport_ortho_ratio));
			}

			//Padding
			text->Padding(caption_padding_.value_or(detail::default_caption_padding_size));

			//Alignment
			text->Alignment(detail::caption_layout_to_text_alignment(caption_layout_));

			//Vertical alignment
			text->VerticalAlignment(detail::caption_layout_to_text_vertical_alignment(caption_layout_));

			//Content
			if (caption_)
				text->Content(*caption_);
			else
				text->Content({});
		}
	}
}


//Public

GuiControl::GuiControl(std::string name) :
	GuiComponent{std::move(name)}
{
	//Empty
}

GuiControl::GuiControl(std::string name, const Vector2 &size) :

	GuiComponent{std::move(name)},
	size_{size}
{
	//Empty
}


GuiControl::GuiControl(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	OwningPtr<ControlSkin> skin, BoundingBoxes hit_boxes) :

	GuiComponent{std::move(name)},
	size_{skin ? detail::get_size(*skin, true) : std::nullopt},
	caption_{std::move(caption)},
	tooltip_{std::move(tooltip)},
	skin_{std::move(skin)},
	hit_boxes_{std::move(hit_boxes)}
{
	//Empty
}

GuiControl::GuiControl(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	OwningPtr<ControlSkin> skin, const Vector2 &size, BoundingBoxes hit_boxes) :

	GuiComponent{std::move(name)},
	size_{size},
	caption_{std::move(caption)},
	tooltip_{std::move(tooltip)},
	skin_{std::move(skin)},
	hit_boxes_{std::move(hit_boxes)}
{
	//Empty
}


GuiControl::~GuiControl() noexcept
{
	RemoveSkin();
}


/*
	Modifiers
*/

void GuiControl::Show() noexcept
{
	GuiComponent::Show();

	if (node_ && node_->ParentNode())
		node_->Visible(node_->ParentNode()->Visible());
}


void GuiControl::Focus() noexcept
{
	if (!focused_ &&
		enabled_ && visible_ && focusable_)
	{
		if (auto owner = Owner(); !owner || owner->IsFocusable())
		{
			focused_ = true;
			Focused();
		}
	}
}

void GuiControl::Defocus() noexcept
{
	if (focused_)
	{
		focused_ = false;
		Defocused();
	}
}


void GuiControl::Press() noexcept
{
	if (!pressed_ && enabled_)
	{
		if (auto owner = Owner(); !owner || owner->IsFocusable())
		{
			pressed_ = true;
			Pressed();
		}
	}
}

void GuiControl::Release() noexcept
{
	if (pressed_)
	{
		pressed_ = false;
		Released();
	}
}

void GuiControl::Click() noexcept
{
	if (enabled_)
		Clicked();
}


void GuiControl::Enter() noexcept
{
	if (!hovered_ && enabled_)
	{
		if (auto owner = Owner(); !owner || owner->IsFocusable())
		{
			hovered_ = true;
			Entered();
		}
	}
}

void GuiControl::Exit() noexcept
{
	if (hovered_)
	{
		hovered_ = false;
		Exited();
	}
}


void GuiControl::Reset() noexcept
{
	Release();
	Defocus();
	Exit();
}


void GuiControl::Size(const Vector2 &size) noexcept
{
	if (!size_ || *size_ != size)
	{
		if (size_)
		{
			//Resize skin
			if (skin_)
				detail::resize_skin(*skin_, *size_, size);

			//Resize hit boxes
			if (!std::empty(hit_boxes_) && !size_->ZeroLength())
				detail::resize_hit_boxes(hit_boxes_, *size_, size);
		}

		auto from_size = size_;
		size_ = size;

		UpdateCaption();
		Resized(from_size.value_or(size), size);
	}
}

void GuiControl::Skin(OwningPtr<ControlSkin> skin) noexcept
{
	if (skin_ != skin)
	{
		RemoveSkin();

		//Re-skin
		if (skin)
		{
			//Resize new skin
			if (auto from_size = detail::get_size(*skin, true); from_size && size_)
				detail::resize_skin(*skin, *from_size, *size_);
			else
				size_ = from_size;

			skin_ = std::move(skin);	
		}

		AttachSkin();
	}
}


/*
	Observers
*/

std::optional<Vector2> GuiControl::CenterSize() const noexcept
{
	if (size_ && skin_)
		return detail::get_center_size(*skin_, true);
	else
		return {};
}

std::optional<Vector2> GuiControl::InnerSize() const noexcept
{
	if (size_ && skin_)
	{
		//Faster than calling get_inner_size (because we know the size)
		if (auto center_size = CenterSize(); center_size)
			return center_size;
	}
	
	return size_;
}

std::optional<Vector2> GuiControl::BorderSize() const noexcept
{
	if (size_)
	{
		//Faster than calling get_border_size (because we know the size)
		if (auto inner_size = InnerSize(); inner_size)
			return (*size_ - *inner_size) * 0.5_r;
	}

	return {};
}


std::optional<Aabb> GuiControl::Area() const noexcept
{
	return skin_ ?
		detail::get_area(*skin_, true) :
		std::nullopt;
}

std::optional<Aabb> GuiControl::CenterArea() const noexcept
{
	return skin_ ?
		detail::get_center_area(*skin_, true) :
		std::nullopt;
}

std::optional<Aabb> GuiControl::InnerArea() const noexcept
{
	return skin_ ?
		detail::get_inner_area(*skin_, true) :
		std::nullopt;
}

std::optional<Aabb> GuiControl::HitArea() const noexcept
{
	//No custom defined hit boxes
	//Use visuals as hit area
	if (std::empty(hit_boxes_))
		return Area();

	//Single hit box
	else if (std::size(hit_boxes_) == 1)
		return hit_boxes_.back();
	else //Multiple hit boxes
		return Aabb::Enclose(hit_boxes_);
}


GuiPanelContainer* GuiControl::Owner() const noexcept
{
	return dynamic_cast<GuiPanelContainer*>(owner_);
}


/*
	Intersecting
*/

bool GuiControl::Intersects(const Vector2 &point) const noexcept
{
	if (node_ && visible_)
	{
		//No custom defined hit boxes
		if (std::empty(hit_boxes_))
		{
			if (auto object =
				[&]() noexcept -> graphics::scene::DrawableObject*
				{
					if (skin_)
					{
						if (skin_->Parts)
							return skin_->Parts.ModelObject.get();
						else if (skin_->Caption)
							return skin_->Caption.TextObject.get();
					}

					return nullptr;
				}(); object)
			{
				object->Prepare();

				//Check for intersection
				if (object->WorldAxisAlignedBoundingBox().Intersects(point))
				{
					auto node = object->ParentNode();
					return !node || node->AxisAligned() ||
							object->WorldOrientedBoundingBox().Intersects(point);
				}
			}
		}
		else //Custom defined hit boxes
		{
			for (auto &hit_box : hit_boxes_)
			{
				//Check for intersection
				if (hit_box.TransformCopy(node_->FullTransformation()).Intersects(point))
				{
					if (!node_ || node_->AxisAligned() ||
						Obb{hit_box}.Transform(node_->FullTransformation()).Intersects(point))
						return true;
				}
			}
		}
	}

	return false;
}


/*
	Tabulating
*/

void GuiControl::TabOrder(int order) noexcept
{
	if (auto owner = Owner(); owner)
		owner->TabOrder(*this, order);
}

std::optional<int> GuiControl::TabOrder() const noexcept
{
	if (auto owner = Owner(); owner)
		return owner->TabOrder(*this);
	else
		return {};
}


/*
	Frame events
*/

void GuiControl::FrameStarted([[maybe_unused]] duration time) noexcept
{
	//Optional to override
}

void GuiControl::FrameEnded([[maybe_unused]] duration time) noexcept
{
	//Optional to override
}


/*
	Key events
*/

bool GuiControl::KeyPressed([[maybe_unused]] KeyButton button) noexcept
{
	//Optional to override
	return false;
}

bool GuiControl::KeyReleased([[maybe_unused]] KeyButton button) noexcept
{
	//Optional to override
	return false;
}

bool GuiControl::CharacterPressed([[maybe_unused]] char character) noexcept
{
	//Optional to override
	return false;
}


/*
	Mouse events
*/

bool GuiControl::MousePressed([[maybe_unused]] MouseButton button, [[maybe_unused]] Vector2 position) noexcept
{
	//Optional to override
	return false;
}

bool GuiControl::MouseReleased([[maybe_unused]] MouseButton button, [[maybe_unused]] Vector2 position) noexcept
{
	//Optional to override
	return false;
}

bool GuiControl::MouseMoved([[maybe_unused]] Vector2 position) noexcept
{
	//Optional to override
	return false;
}

bool GuiControl::MouseWheelRolled([[maybe_unused]] int delta, [[maybe_unused]] Vector2 position) noexcept
{
	//Optional to override
	return false;
}

} //ion::gui::controls