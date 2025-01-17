/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiControl.h
-------------------------------------------
*/

#ifndef ION_GUI_CONTROL_H
#define ION_GUI_CONTROL_H

#include <optional>
#include <string>
#include <vector>

#include "events/IonCallback.h"
#include "events/IonEventGenerator.h"
#include "events/listeners/IonGuiControlListener.h"
#include "events/listeners/IonKeyListener.h"
#include "events/listeners/IonMouseListener.h"
#include "graphics/fonts/IonText.h"
#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonVector2.h"
#include "gui/IonGuiComponent.h"
#include "memory/IonNonOwningPtr.h"
#include "memory/IonOwningPtr.h"
#include "types/IonTypes.h"

//Forward declarations
namespace ion
{
	namespace gui
	{
		class GuiPanelContainer;

		namespace skins
		{
			class GuiSkin;
		}
	}

	namespace graphics
	{
		namespace materials
		{
			class Material;
		}

		namespace scene
		{
			class DrawableText;
			class Model;

			namespace shapes
			{
				class Sprite;
			}
		}
	}

	namespace sounds
	{
		class Sound;
		class SoundChannel;
	}
}

namespace ion::gui::controls
{
	using namespace events::listeners;
	using namespace graphics::utilities;

	namespace gui_control
	{
		enum class ControlState
		{
			Enabled,
			Disabled,
			Focused,
			Pressed,
			Hovered
		};

		enum class ControlCaptionLayout
		{
								OutsideTopLeft,		OutsideTopCenter,		OutsideTopRight,
			OutsideLeftTop,		TopLeft,			TopCenter,				TopRight,			OutsideRightTop,
			OutsideLeftCenter,	Left,				Center,					Right,				OutsideRightCenter,
			OutsideLeftBottom,	BottomLeft,			BottomCenter,			BottomRight,		OutsideRightBottom,
								OutsideBottomLeft,	OutsideBottomCenter,	OutsideBottomRight
		};

		using BoundingBoxes = std::vector<Aabb>;


		struct ControlSkinPart final
		{
			NonOwningPtr<graphics::scene::shapes::Sprite> Object;
			NonOwningPtr<graphics::materials::Material> Enabled;
			NonOwningPtr<graphics::materials::Material> Disabled;
			NonOwningPtr<graphics::materials::Material> Focused;
			NonOwningPtr<graphics::materials::Material> Pressed;
			NonOwningPtr<graphics::materials::Material> Hovered;

			[[nodiscard]] inline operator bool() const noexcept
			{
				return !!Object;
			}

			[[nodiscard]] inline auto operator->() const noexcept
			{
				return Object.get();
			}
		};

		struct ControlSkinTextPart final
		{
			NonOwningPtr<graphics::scene::DrawableText> Object;
			std::optional<graphics::fonts::text::TextBlockStyle> Enabled;
			std::optional<graphics::fonts::text::TextBlockStyle> Disabled;
			std::optional<graphics::fonts::text::TextBlockStyle> Focused;
			std::optional<graphics::fonts::text::TextBlockStyle> Pressed;
			std::optional<graphics::fonts::text::TextBlockStyle> Hovered;

			[[nodiscard]] inline operator bool() const noexcept
			{
				return !!Object;
			}

			[[nodiscard]] inline auto operator->() const noexcept
			{
				return Object.get();
			}
		};

		struct ControlSkinSoundPart final
		{
			NonOwningPtr<sounds::Sound> Object;

			[[nodiscard]] inline operator bool() const noexcept
			{
				return !!Object;
			}

			[[nodiscard]] inline auto operator->() const noexcept
			{
				return Object.get();
			}
		};


		struct ControlSkinParts final
		{
			NonOwningPtr<graphics::scene::Model> Object;

			//Center
			ControlSkinPart Center;

			//Sides
			ControlSkinPart Top;
			ControlSkinPart Left;
			ControlSkinPart Bottom;
			ControlSkinPart Right;

			//Corners
			ControlSkinPart TopLeft;
			ControlSkinPart BottomLeft;
			ControlSkinPart TopRight;
			ControlSkinPart BottomRight;

			[[nodiscard]] inline operator bool() const noexcept
			{
				return !!Object;
			}

			[[nodiscard]] inline auto operator->() const noexcept
			{
				return Object.get();
			}
		};

		struct ControlSkinSoundParts final
		{
			NonOwningPtr<sounds::SoundChannel> Object;

			//Events
			ControlSkinSoundPart Focused;
			ControlSkinSoundPart Defocused;
			ControlSkinSoundPart Pressed;
			ControlSkinSoundPart Released;
			ControlSkinSoundPart Clicked;
			ControlSkinSoundPart Entered;
			ControlSkinSoundPart Exited;
			ControlSkinSoundPart Changed;

			[[nodiscard]] inline operator bool() const noexcept
			{
				return !!Object;
			}

			[[nodiscard]] inline auto operator->() const noexcept
			{
				return Object.get();
			}
		};


		struct ControlSkin
		{
			ControlSkinParts Parts;
			ControlSkinTextPart Caption;
			ControlSkinSoundParts Sounds;

			//Default virtual destructor
			virtual ~ControlSkin() = default;

			//Copy from most derived
			virtual void Assign(const ControlSkin &control_skin) noexcept
			{
				*this = control_skin;
			}
		};


		namespace detail
		{
			constexpr auto default_caption_margin_size = 4.0_r;
			constexpr auto default_caption_padding_size = 2.0_r;


			/**
				@name States
				@{
			*/

			inline auto control_state_to_material(ControlState state, ControlSkinPart &part) noexcept
			{
				switch (state)
				{
					case ControlState::Disabled:
					return part.Disabled;

					case ControlState::Focused:
					return part.Focused;

					case ControlState::Pressed:
					return part.Pressed;

					case ControlState::Hovered:
					return part.Hovered;

					case ControlState::Enabled:
					default:
					return part.Enabled;
				}
			}

			inline auto& control_state_to_style(ControlState state, ControlSkinTextPart &part) noexcept
			{
				switch (state)
				{
					case ControlState::Disabled:
					return part.Disabled;

					case ControlState::Focused:
					return part.Focused;

					case ControlState::Pressed:
					return part.Pressed;

					case ControlState::Hovered:
					return part.Hovered;

					case ControlState::Enabled:
					default:
					return part.Enabled;
				}
			}

			///@}

			/**
				@name Skins
				@{
			*/

			void resize_part(graphics::scene::shapes::Sprite &sprite, const Vector2 &delta_size, const Vector2 &delta_position, const Vector2 &center) noexcept;
			void resize_part(ControlSkinPart &part, const Vector2 &delta_size, const Vector2 &delta_position, const Vector2 &center) noexcept;		
			void resize_skin(ControlSkin &skin, const Vector2 &from_size, const Vector2 &to_size) noexcept;

			void resize_hit_box(Aabb &hit_box, const Vector2 &scaling) noexcept;
			void resize_hit_boxes(BoundingBoxes &hit_boxes, const Vector2 &from_size, const Vector2 &to_size) noexcept;


			std::optional<Aabb> get_area(const ControlSkin &skin, bool include_caption) noexcept;
			std::optional<Aabb> get_center_area(const ControlSkin &skin, bool include_caption) noexcept;
			std::optional<Aabb> get_inner_area(const ControlSkin &skin, bool include_caption) noexcept;

			std::optional<Vector2> get_size(const ControlSkin &skin, bool include_caption) noexcept;
			std::optional<Vector2> get_center_size(const ControlSkin &skin, bool include_caption) noexcept;
			std::optional<Vector2> get_inner_size(const ControlSkin &skin, bool include_caption) noexcept;
			std::optional<Vector2> get_border_size(const ControlSkin &skin, bool include_caption) noexcept;

			///@}

			inline auto is_caption_outside(ControlCaptionLayout caption_layout) noexcept
			{
				switch (caption_layout)
				{
					case ControlCaptionLayout::TopLeft:
					case ControlCaptionLayout::TopCenter:
					case ControlCaptionLayout::TopRight:
					case ControlCaptionLayout::Left:
					case ControlCaptionLayout::Center:
					case ControlCaptionLayout::Right:
					case ControlCaptionLayout::BottomLeft:
					case ControlCaptionLayout::BottomCenter:
					case ControlCaptionLayout::BottomRight:
					return false;

					default:
					return true;
				}
			}

			inline auto caption_layout_to_text_alignment(ControlCaptionLayout caption_layout) noexcept
			{
				switch (caption_layout)
				{
					case ControlCaptionLayout::Left:
					case ControlCaptionLayout::TopLeft:
					case ControlCaptionLayout::BottomLeft:
					case ControlCaptionLayout::OutsideRightTop:
					case ControlCaptionLayout::OutsideRightCenter:
					case ControlCaptionLayout::OutsideRightBottom:
					case ControlCaptionLayout::OutsideTopLeft:
					case ControlCaptionLayout::OutsideBottomLeft:
					return graphics::fonts::text::TextAlignment::Left;

					case ControlCaptionLayout::Right:
					case ControlCaptionLayout::TopRight:
					case ControlCaptionLayout::BottomRight:
					case ControlCaptionLayout::OutsideLeftTop:
					case ControlCaptionLayout::OutsideLeftCenter:
					case ControlCaptionLayout::OutsideLeftBottom:
					case ControlCaptionLayout::OutsideTopRight:
					case ControlCaptionLayout::OutsideBottomRight:
					return graphics::fonts::text::TextAlignment::Right;

					default:
					return graphics::fonts::text::TextAlignment::Center;
				}
			}

			inline auto caption_layout_to_text_vertical_alignment(ControlCaptionLayout caption_layout) noexcept
			{
				switch (caption_layout)
				{
					case ControlCaptionLayout::TopLeft:
					case ControlCaptionLayout::TopCenter:
					case ControlCaptionLayout::TopRight:
					case ControlCaptionLayout::OutsideLeftTop:
					case ControlCaptionLayout::OutsideRightTop:
					case ControlCaptionLayout::OutsideBottomLeft:
					case ControlCaptionLayout::OutsideBottomCenter:
					case ControlCaptionLayout::OutsideBottomRight:
					return graphics::fonts::text::TextVerticalAlignment::Top;

					case ControlCaptionLayout::BottomLeft:
					case ControlCaptionLayout::BottomCenter:
					case ControlCaptionLayout::BottomRight:
					case ControlCaptionLayout::OutsideLeftBottom:
					case ControlCaptionLayout::OutsideRightBottom:
					case ControlCaptionLayout::OutsideTopLeft:
					case ControlCaptionLayout::OutsideTopCenter:
					case ControlCaptionLayout::OutsideTopRight:
					return graphics::fonts::text::TextVerticalAlignment::Bottom;

					default:
					return graphics::fonts::text::TextVerticalAlignment::Middle;
				}
			}

			Vector2 caption_offset(ControlCaptionLayout caption_layout, const Vector2 &size, const Vector2 &border_size, const Vector2 &margin_size) noexcept;
			Vector2 caption_area_offset(ControlCaptionLayout caption_layout, const Vector2 &size, const Vector2 &border_size) noexcept;
		} //detail
	} //gui_control


	///@brief A class representing a general GUI control, the base class of all GUI controls
	///@details A control can be focused/defocused, clicked/pressed/released, entered/exited and resized
	class GuiControl :
		public GuiComponent,
		protected events::EventGenerator<events::listeners::GuiControlListener>
	{
		protected:
		
			bool focused_ = false;
			bool pressed_ = false;
			bool hovered_ = false;
			bool focusable_ = true;	
			
			OwningPtr<gui_control::ControlSkin> skin_;
			std::optional<Vector2> size_;

			std::optional<std::string> caption_;
			std::optional<std::string> tooltip_;		
			std::optional<Vector2> caption_size_;
			std::optional<Vector2> caption_margin_;
			std::optional<Vector2> caption_padding_;
			gui_control::ControlCaptionLayout caption_layout_ = gui_control::ControlCaptionLayout::Center;

			gui_control::ControlState state_ = gui_control::ControlState::Enabled;
			gui_control::BoundingBoxes hit_boxes_;		
			NonOwningPtr<SceneNode> skin_node_;
			
			std::optional<events::Callback<void, GuiControl&>> on_focus_;
			std::optional<events::Callback<void, GuiControl&>> on_defocus_;
			std::optional<events::Callback<void, GuiControl&>> on_press_;
			std::optional<events::Callback<void, GuiControl&>> on_release_;
			std::optional<events::Callback<void, GuiControl&>> on_click_;
			std::optional<events::Callback<void, GuiControl&>> on_enter_;
			std::optional<events::Callback<void, GuiControl&>> on_exit_;
			std::optional<events::Callback<void, GuiControl&>> on_change_;		
			std::optional<events::Callback<void, GuiControl&>> on_state_change_;
			std::optional<events::Callback<void, GuiControl&>> on_resize_;


			/**
				@name Events
				@{
			*/

			///@brief See GuiComponent::Created for more details
			virtual void Created() noexcept override;

			///@brief See GuiComponent::Removed for more details
			virtual void Removed() noexcept override;


			///@brief See GuiComponent::Enabled for more details
			virtual void Enabled() noexcept override;

			///@brief See GuiComponent::Disabled for more details
			virtual void Disabled() noexcept override;


			///@brief See GuiComponent::Shown for more details
			virtual void Shown() noexcept override;

			///@brief See GuiComponent::Hidden for more details
			virtual void Hidden() noexcept override;


			///@brief Called right after a control has been focused
			virtual void Focused() noexcept;

			///@brief Called right after a control has been defocused
			virtual void Defocused() noexcept;


			///@brief Called right after a control has been pressed
			virtual void Pressed() noexcept;

			///@brief Called right after a control has been released
			virtual void Released() noexcept;

			///@brief Called right after a control has been clicked
			///@details Namely after a complete press and release
			virtual void Clicked() noexcept;


			///@brief Called right after a control has been entered
			///@details Namely when the mouse cursor has entered the control
			virtual void Entered() noexcept;

			///@brief Called right after a control has been exited
			///@details Namely when the mouse cursor has exited the control
			virtual void Exited() noexcept;


			///@brief Called right after a control has been changed
			virtual void Changed() noexcept;

			///@brief Called right after a control has changed state
			virtual void StateChanged() noexcept;


			///@brief Called right after a control has been resized with the from and to size
			virtual void Resized(Vector2 from_size, Vector2 to_size) noexcept;

			///@}

			/**
				@name Notifying
				@{
			*/

			void NotifyControlEnabled() noexcept;
			void NotifyControlDisabled() noexcept;

			void NotifyControlShown() noexcept;
			void NotifyControlHidden() noexcept;

			void NotifyControlFocused() noexcept;
			void NotifyControlDefocused() noexcept;

			void NotifyControlPressed() noexcept;
			void NotifyControlReleased() noexcept;
			void NotifyControlClicked() noexcept;

			void NotifyControlEntered() noexcept;
			void NotifyControlExited() noexcept;

			void NotifyControlChanged() noexcept;
			void NotifyControlStateChanged() noexcept;

			void NotifyControlResized() noexcept;

			///@}

			/**
				@name States
				@{
			*/
			
			NonOwningPtr<graphics::materials::Material> GetStateMaterial(gui_control::ControlState state, gui_control::ControlSkinPart &part) noexcept;
			std::optional<graphics::fonts::text::TextBlockStyle>& GetStateStyle(gui_control::ControlState state, gui_control::ControlSkinTextPart &part) noexcept;

			void SetPartState(gui_control::ControlState state, gui_control::ControlSkinPart &part) noexcept;
			void SetCaptionState(gui_control::ControlState state, gui_control::ControlSkinTextPart &part) noexcept;
			void SetSkinState(gui_control::ControlState state, gui_control::ControlSkin &skin) noexcept;

			virtual void SetState(gui_control::ControlState state) noexcept;

			///@}

			/**
				@name Skins
				@{
			*/

			virtual OwningPtr<gui_control::ControlSkin> AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const;

			virtual void AttachSkin();
			virtual void DetachSkin() noexcept;
			virtual void RemoveSkin() noexcept;

			virtual void UpdateCaption() noexcept;

			void PlaySound(gui_control::ControlSkinSoundPart &part);

			///@}

		public:

			///@brief Constructs a control with the given name
			explicit GuiControl(std::string name) noexcept;

			///@brief Constructs a control with the given name and size
			GuiControl(std::string name, const std::optional<Vector2> &size) noexcept;

			///@brief Constructs a control with the given name, skin and size
			GuiControl(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size);


			///@brief Constructs a control with the given name, size, caption, tooltip and hit boxes
			GuiControl(std::string name, const std::optional<Vector2> &size,
				std::optional<std::string> caption, std::optional<std::string> tooltip, gui_control::BoundingBoxes hit_boxes = {}) noexcept;

			///@brief Constructs a control with the given name, skin, size, caption, tooltip and hit boxes
			GuiControl(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
				std::optional<std::string> caption, std::optional<std::string> tooltip, gui_control::BoundingBoxes hit_boxes = {});

			
			///@brief Default move constructor
			GuiControl(GuiControl&&) = default;


			///@brief Virtual destructor
			virtual ~GuiControl() noexcept;


			/**
				@name Operators
				@{
			*/

			///@brief Default move assignment
			GuiControl& operator=(GuiControl&&) = default;

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Shows this control
			void Show() noexcept;


			///@brief Focuses this control
			void Focus() noexcept;

			///@brief Defocuses this control
			void Defocus() noexcept;


			///@brief Presses this control
			void Press() noexcept;

			///@brief Releases this control
			void Release() noexcept;

			///@brief Clicks this control
			void Click() noexcept;


			///@brief Enters this control (start hovering)
			void Enter() noexcept;

			///@brief Exits this control (stop hovering)
			void Exit() noexcept;


			///@brief Resets this control to its enabled/disabled state
			///@details Namely by executing release, defocus and exit
			void Reset() noexcept;


			///@brief Sets whether or not this control is enabled
			inline void Enabled(bool enabled) noexcept
			{
				return GuiComponent::Enabled(enabled);
			}

			///@brief Sets whether or not this control is focused
			inline void Focused(bool focused) noexcept
			{
				if (focused)
					Focus();
				else
					Defocus();
			}

			///@brief Sets whether or not this control is focusable
			inline void Focusable(bool focusable) noexcept
			{
				if (focusable_ != focusable)
				{
					focusable_ = focusable;

					if (!focusable && focused_)
						Defocus();
				}
			}


			///@brief Sets the skin for this control to the given skin
			void Skin(const skins::GuiSkin &skin) noexcept;

			///@brief Sets the size of this control to the given size
			void Size(const Vector2 &size) noexcept;


			///@brief Sets the caption text for this control to the given text
			inline void Caption(std::optional<std::string> text) noexcept
			{
				if (caption_ != text)
				{
					caption_ = std::move(text);
					UpdateCaption();
				}
			}

			///@brief Sets the tooltip text (hint) for this control to the given text
			inline void Tooltip(std::optional<std::string> text) noexcept
			{
				tooltip_ = std::move(text);
			}


			///@brief Sets the caption size for this control to the given size
			inline void CaptionSize(const std::optional<Vector2> &size) noexcept
			{
				if (caption_size_ != size)
				{
					caption_size_ = size;
					UpdateCaption();
				}
			}

			///@brief Sets the caption margin for this control to the given margin
			inline void CaptionMargin(const std::optional<Vector2> &margin) noexcept
			{
				if (caption_margin_ != margin)
				{
					caption_margin_ = margin;
					UpdateCaption();
				}
			}

			///@brief Sets the caption padding for this control to the given padding
			inline void CaptionPadding(const std::optional<Vector2> &padding) noexcept
			{
				if (caption_padding_ != padding)
				{
					caption_padding_ = padding;
					UpdateCaption();
				}
			}

			///@brief Sets the caption layout for this control to the given layout
			inline void CaptionLayout(gui_control::ControlCaptionLayout layout) noexcept
			{
				if (caption_layout_ != layout)
				{
					caption_layout_ = layout;
					UpdateCaption();
				}
			}


			///@brief Sets the hit boxes for this control to the given hit boxes
			inline void HitBoxes(gui_control::BoundingBoxes hit_boxes) noexcept
			{
				hit_boxes_ = std::move(hit_boxes);
			}

			///@brief Sets the hit box for this control to the given hit box
			inline void HitBox(const Aabb &hit_box) noexcept
			{
				hit_boxes_.clear();
				hit_boxes_.push_back(hit_box);
			}


			///@brief Sets the on focus callback
			inline void OnFocus(events::Callback<void, GuiControl&> on_focus) noexcept
			{
				on_focus_ = on_focus;
			}

			///@brief Sets the on focus callback
			inline void OnFocus(std::nullopt_t) noexcept
			{
				on_focus_ = {};
			}


			///@brief Sets the on defocus callback
			inline void OnDefocus(events::Callback<void, GuiControl&> on_defocus) noexcept
			{
				on_defocus_ = on_defocus;
			}

			///@brief Sets the on defocus callback
			inline void OnDefocus(std::nullopt_t) noexcept
			{
				on_defocus_ = {};
			}


			///@brief Sets the on press callback
			inline void OnPress(events::Callback<void, GuiControl&> on_press) noexcept
			{
				on_press_ = on_press;
			}

			///@brief Sets the on press callback
			inline void OnPress(std::nullopt_t) noexcept
			{
				on_press_ = {};
			}


			///@brief Sets the on release callback
			inline void OnRelease(events::Callback<void, GuiControl&> on_release) noexcept
			{
				on_release_ = on_release;
			}

			///@brief Sets the on release callback
			inline void OnRelease(std::nullopt_t) noexcept
			{
				on_release_ = {};
			}


			///@brief Sets the on click callback
			inline void OnClick(events::Callback<void, GuiControl&> on_click) noexcept
			{
				on_click_ = on_click;
			}

			///@brief Sets the on click callback
			inline void OnClick(std::nullopt_t) noexcept
			{
				on_click_ = {};
			}


			///@brief Sets the on enter callback
			inline void OnEnter(events::Callback<void, GuiControl&> on_enter) noexcept
			{
				on_enter_ = on_enter;
			}

			///@brief Sets the on enter callback
			inline void OnEnter(std::nullopt_t) noexcept
			{
				on_enter_ = {};
			}


			///@brief Sets the on exit callback
			inline void OnExit(events::Callback<void, GuiControl&> on_exit) noexcept
			{
				on_exit_ = on_exit;
			}

			///@brief Sets the on exit callback
			inline void OnExit(std::nullopt_t) noexcept
			{
				on_exit_ = {};
			}


			///@brief Sets the on change callback
			inline void OnChange(events::Callback<void, GuiControl&> on_change) noexcept
			{
				on_change_ = on_change;
			}

			///@brief Sets the on change callback
			inline void OnChange(std::nullopt_t) noexcept
			{
				on_change_ = {};
			}


			///@brief Sets the on state change callback
			inline void OnStateChange(events::Callback<void, GuiControl&> on_state_change) noexcept
			{
				on_state_change_ = on_state_change;
			}

			///@brief Sets the on state change callback
			inline void OnStateChange(std::nullopt_t) noexcept
			{
				on_state_change_ = {};
			}


			///@brief Sets the on resize callback
			inline void OnResize(events::Callback<void, GuiControl&> on_resize) noexcept
			{
				on_resize_ = on_resize;
			}

			///@brief Sets the on resize callback
			inline void OnResize(std::nullopt_t) noexcept
			{
				on_resize_ = {};
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns true if this control is focused
			[[nodiscard]] inline auto IsFocused() const noexcept
			{
				return focused_;
			}

			///@brief Returns true if this control is focusable
			[[nodiscard]] inline auto IsFocusable() const noexcept
			{
				return focusable_;
			}

			///@brief Returns true if this control is pressed
			[[nodiscard]] inline auto IsPressed() const noexcept
			{
				return pressed_;
			}

			///@brief Returns true if this control is hovered
			[[nodiscard]] inline auto IsHovered() const noexcept
			{
				return hovered_;
			}


			///@brief Returns the skin attached to this control
			///@details Returns nullptr if no skin is attached
			[[nodiscard]] inline auto Skin() const noexcept
			{
				return NonOwningPtr<gui_control::ControlSkin>{skin_};
			}

			///@brief Returns the size of this control
			///@details The returned size includes all parts or caption (if no parts).
			///Returns nullopt if this control has no size
			[[nodiscard]] inline auto& Size() const noexcept
			{
				return size_;
			}

			///@brief Returns the center size of this control
			///@details The returned size includes center part or caption (if no center).
			///Returns nullopt if this control has no center size
			[[nodiscard]] std::optional<Vector2> CenterSize() const noexcept;

			///@brief Returns the inner size of this control
			///@details The returned size includes center area or area (if no center).
			///Returns nullopt if this control has no inner size
			[[nodiscard]] std::optional<Vector2> InnerSize() const noexcept;

			///@brief Returns the border size of this control
			///@details Returns nullopt if this control has no border size
			[[nodiscard]] std::optional<Vector2> BorderSize() const noexcept;


			///@brief Returns the caption text for this control
			///@details Returns nullopt if this control has no caption
			[[nodiscard]] inline auto& Caption() const noexcept
			{
				return caption_;
			}

			///@brief Returns the tooltip text (hint) for this control
			///@details Returns nullopt if this control has no tooltip
			[[nodiscard]] inline auto& Tooltip() const noexcept
			{
				return tooltip_;
			}


			///@brief Returns the caption size for this control
			///@details Returns nullopt if no custom caption size has been set
			[[nodiscard]] inline auto& CaptionSize() const noexcept
			{
				return caption_size_;
			}

			///@brief Returns the caption margin for this control
			///@details Returns nullopt if no custom caption margin has been set
			[[nodiscard]] inline auto& CaptionMargin() const noexcept
			{
				return caption_margin_;
			}

			///@brief Returns the caption padding for this control
			///@details Returns nullopt if no custom caption padding has been set
			[[nodiscard]] inline auto& CaptionPadding() const noexcept
			{
				return caption_padding_;
			}

			///@brief Returns the caption layout for this control
			[[nodiscard]] inline auto& CaptionLayout() const noexcept
			{
				return caption_layout_;
			}


			///@brief Returns the current state of this control
			[[nodiscard]] inline auto State() const noexcept
			{
				return state_;
			}

			///@brief Returns the hit boxes for this control
			[[nodiscard]] inline auto& HitBoxes() const noexcept
			{
				return hit_boxes_;
			}


			///@brief Returns the area of this control
			///@details The returned area includes all parts or caption (if no parts).
			///Returns nullopt of this control has no area
			[[nodiscard]] std::optional<Aabb> Area() const noexcept;

			///@brief Returns the inne area of this control
			///@details The returned area includes center part or caption (if no center).
			///Returns nullopt of this control has no center area
			[[nodiscard]] std::optional<Aabb> CenterArea() const noexcept;

			///@brief Returns the inne area of this control
			///@details The returned area includes center area or area (if no center).
			///Returns nullopt of this control has no inner area
			[[nodiscard]] std::optional<Aabb> InnerArea() const noexcept;

			///@brief Returns the hit area of this control
			///@details Returns nullopt of this control has no hit area
			[[nodiscard]] std::optional<Aabb> HitArea() const noexcept;


			///@brief Returns the on focus callback
			[[nodiscard]] inline auto OnFocus() const noexcept
			{
				return on_focus_;
			}

			///@brief Returns the on defocus callback
			[[nodiscard]] inline auto OnDefocus() const noexcept
			{
				return on_defocus_;
			}

			///@brief Returns the on press callback
			[[nodiscard]] inline auto OnPress() const noexcept
			{
				return on_press_;
			}

			///@brief Returns the on release callback
			[[nodiscard]] inline auto OnRelease() const noexcept
			{
				return on_release_;
			}

			///@brief Returns the on enter callback
			[[nodiscard]] inline auto OnEnter() const noexcept
			{
				return on_enter_;
			}

			///@brief Returns the on exit callback
			[[nodiscard]] inline auto OnExit() const noexcept
			{
				return on_exit_;
			}

			///@brief Returns the on change callback
			[[nodiscard]] inline auto OnChange() const noexcept
			{
				return on_change_;
			}

			///@brief Returns the on state change callback
			[[nodiscard]] inline auto OnStateChange() const noexcept
			{
				return on_state_change_;
			}

			///@brief Returns the on resize callback
			[[nodiscard]] inline auto OnResize() const noexcept
			{
				return on_resize_;
			}


			///@brief Returns a pointer to the owner of this control
			[[nodiscard]] GuiPanelContainer* Owner() const noexcept;

			///@}

			/**
				@name Intersecting
				@{
			*/

			///@brief Returns true if the given point intersects with this control
			[[nodiscard]] bool Intersects(const Vector2 &point) const noexcept;

			///@}

			/**
				@name Tabulating
				@{
			*/

			///@brief Sets the tab order of this control to the given order
			void TabOrder(int order) noexcept;

			///@brief Returns the tab order of this control
			///@details Returns nullopt if this control has no owner
			[[nodiscard]] std::optional<int> TabOrder() const noexcept;

			///@}

			/**
				@name Frame events
				@{
			*/

			///@brief Called from gui frame when a frame has started
			virtual void FrameStarted(duration time) noexcept;

			///@brief Called from gui frame when a frame has ended
			virtual void FrameEnded(duration time) noexcept;

			///@}

			/**
				@name Key events
				@{
			*/

			///@brief Called from gui frame when a key button has been pressed
			///@details Returns true if the key press event has been consumed by the control
			virtual bool KeyPressed(KeyButton button) noexcept;

			///@brief Called from gui frame when a key button has been released
			///@details Returns true if the key release event has been consumed by the control
			virtual bool KeyReleased(KeyButton button) noexcept;

			///@brief Called from gui frame when a character has been pressed
			///@details Returns true if the character press event has been consumed by the control
			virtual bool CharacterPressed(char character) noexcept;

			///@}

			/**
				@name Mouse events
				@{
			*/

			///@brief Called from gui frame when the mouse button has been pressed
			///@details Returns true if the mouse press event has been consumed by the control
			virtual bool MousePressed(MouseButton button, Vector2 position) noexcept;

			///@brief Called from gui frame when the mouse button has been released
			///@details Returns true if the mouse release event has been consumed by the control
			virtual bool MouseReleased(MouseButton button, Vector2 position) noexcept;

			///@brief Called from gui frame when the mouse has been moved
			///@details Returns true if the mouse move event has been consumed by the control
			virtual bool MouseMoved(Vector2 position) noexcept;

			///@brief Called from gui frame when the mouse wheel has been rolled
			///@details Returns true if the mouse wheel roll event has been consumed by the control
			virtual bool MouseWheelRolled(int delta, Vector2 position) noexcept;

			///@}
	};
} //ion::gui::controls

#endif