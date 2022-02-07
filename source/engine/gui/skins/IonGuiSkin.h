/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/skins
File:	IonGuiSkin.h
-------------------------------------------
*/

#ifndef ION_GUI_SKIN_H
#define ION_GUI_SKIN_H

#include <optional>
#include <string>
#include <string_view>
#include <typeindex>
#include <typeinfo>
#include <type_traits>

#include "adaptors/IonFlatMap.h"
#include "adaptors/ranges/IonIterable.h"
#include "events/IonCallback.h"
#include "graphics/fonts/IonText.h"
#include "graphics/render/IonPass.h"
#include "graphics/scene/IonDrawableObject.h"
#include "graphics/utilities/IonColor.h"
#include "gui/controls/IonGuiControl.h"
#include "managed/IonManagedObject.h"
#include "memory/IonNonOwningPtr.h"
#include "memory/IonOwningPtr.h"

//Forward declarations
namespace ion::graphics
{
	namespace materials
	{
		class Material;
	}

	namespace scene
	{
		class SceneManager;
	}
}

namespace ion::gui::skins
{
	//Forward declarations
	class GuiSkin;
	class GuiTheme;

	namespace gui_skin
	{
		struct SkinPart
		{
			NonOwningPtr<graphics::materials::Material> Enabled;
			NonOwningPtr<graphics::materials::Material> Disabled;
			NonOwningPtr<graphics::materials::Material> Focused;
			NonOwningPtr<graphics::materials::Material> Pressed;
			NonOwningPtr<graphics::materials::Material> Hovered;
			graphics::utilities::Color FillColor = graphics::utilities::color::White;

			[[nodiscard]] inline operator bool() const noexcept
			{
				return !!Enabled; //Required
			}
		};

		struct SkinTextPart
		{
			NonOwningPtr<graphics::fonts::Text> Template;
			std::optional<graphics::fonts::text::TextBlockStyle> Enabled;
			std::optional<graphics::fonts::text::TextBlockStyle> Disabled;
			std::optional<graphics::fonts::text::TextBlockStyle> Focused;
			std::optional<graphics::fonts::text::TextBlockStyle> Pressed;
			std::optional<graphics::fonts::text::TextBlockStyle> Hovered;

			[[nodiscard]] inline operator bool() const noexcept
			{
				return Template && Enabled; //Required
			}
		};

		struct SkinSideParts
		{
			SkinPart Top;
			SkinPart Bottom;
			SkinPart Left;
			SkinPart Right;

			[[nodiscard]] inline operator bool() const noexcept
			{
				return Top || Bottom || Left || Right;
			}
		};

		struct SkinCornerParts
		{
			SkinPart TopLeft;
			SkinPart TopRight;
			SkinPart BottomLeft;
			SkinPart BottomRight;

			[[nodiscard]] inline operator bool() const noexcept
			{
				return TopLeft || TopRight || BottomLeft || BottomRight;
			}
		};

		struct SkinBorderParts
		{
			SkinSideParts Sides;
			SkinCornerParts Corners;

			[[nodiscard]] inline operator bool() const noexcept
			{
				return Sides || Corners;
			}
		};

		struct SkinParts
		{
			SkinPart Center;
			SkinBorderParts Border;

			[[nodiscard]] inline operator bool() const noexcept
			{
				return Center || Border;
			}
		};


		using SkinPartMap = adaptors::FlatMap<std::string, SkinPart>;
		using SkinTextPartMap = adaptors::FlatMap<std::string, SkinTextPart>;
		using SkinBuilder = events::Callback<OwningPtr<controls::gui_control::ControlSkin>, const GuiSkin&, graphics::scene::SceneManager&>;


		namespace detail
		{
			controls::gui_control::ControlSkin make_control_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager);

			OwningPtr<controls::gui_control::ControlSkin> make_button_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager);
			OwningPtr<controls::gui_control::ControlSkin> make_check_box_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager);
			OwningPtr<controls::gui_control::ControlSkin> make_group_box_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager);
			OwningPtr<controls::gui_control::ControlSkin> make_label_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager);
			OwningPtr<controls::gui_control::ControlSkin> make_list_box_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager);
			OwningPtr<controls::gui_control::ControlSkin> make_progress_bar_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager);
			OwningPtr<controls::gui_control::ControlSkin> make_radio_button_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager);
			OwningPtr<controls::gui_control::ControlSkin> make_scroll_bar_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager);
			OwningPtr<controls::gui_control::ControlSkin> make_slider_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager);
			OwningPtr<controls::gui_control::ControlSkin> make_text_box_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager);
			OwningPtr<controls::gui_control::ControlSkin> make_tooltip_skin(const GuiSkin &skin, graphics::scene::SceneManager &scene_manager);
		} //detail
	} //gui_skin


	class GuiSkin final : public managed::ManagedObject<GuiTheme>
	{
		private:

			gui_skin::SkinPartMap parts_;
			gui_skin::SkinTextPartMap text_parts_;

			graphics::scene::drawable_object::Passes passes_;
			graphics::scene::drawable_object::Passes text_passes_;


			static inline adaptors::FlatMap<std::type_index, std::string> registered_skins_;
			static inline adaptors::FlatMap<std::string, gui_skin::SkinBuilder> registered_skin_builders_;

			static void RegisterDefaultSkins();

		public:

			//Construct a skin with the given name
			explicit GuiSkin(std::string name);
			
			//Construct a skin with the given name, parts and caption
			GuiSkin(std::string name, const gui_skin::SkinParts &parts, const gui_skin::SkinTextPart &caption = {});

			//Construct a skin with the given name, border parts and caption
			GuiSkin(std::string name, const gui_skin::SkinBorderParts &border_parts, const gui_skin::SkinTextPart &caption = {});

			//Construct a skin with the given name, side parts and caption
			GuiSkin(std::string name, const gui_skin::SkinSideParts &side_parts, const gui_skin::SkinTextPart &caption = {});

			//Construct a skin with the given name, center part and caption
			GuiSkin(std::string name, const gui_skin::SkinPart &center, const gui_skin::SkinTextPart &caption = {});

			//Construct a skin with the given name and caption
			GuiSkin(std::string name, const gui_skin::SkinTextPart &caption);


			/*
				Ranges
			*/

			//Returns a mutable range of all parts in this skin
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Parts() noexcept
			{
				return parts_.Elements();
			}

			//Returns an immutable range of all parts in this skin
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Parts() const noexcept
			{
				return parts_.Elements();
			}


			//Returns a mutable range of all text parts in this skin
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto TextParts() noexcept
			{
				return text_parts_.Elements();
			}

			//Returns an immutable range of all text parts in this skin
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto TextParts() const noexcept
			{
				return text_parts_.Elements();
			}


			//Returns a mutable range of all passes in this skin
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Passes() noexcept
			{
				return adaptors::ranges::Iterable<graphics::scene::drawable_object::Passes&>{passes_};
			}

			//Returns an immutable range of all passes in this skin
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Passes() const noexcept
			{
				return adaptors::ranges::Iterable<const graphics::scene::drawable_object::Passes&>{passes_};
			}


			//Returns a mutable range of all text passes in this skin
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto TextPasses() noexcept
			{
				return adaptors::ranges::Iterable<graphics::scene::drawable_object::Passes&>{text_passes_};
			}

			//Returns an immutable range of all text passes in this skin
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto TextPasses() const noexcept
			{
				return adaptors::ranges::Iterable<const graphics::scene::drawable_object::Passes&>{text_passes_};
			}
			

			/*
				Modifiers
			*/




			/*
				Observers
			*/




			/*
				Instantiating
			*/

			//Returns an instantiation of this skin
			[[nodiscard]] OwningPtr<controls::gui_control::ControlSkin> Instantiate() const;


			/*
				Parts
				Adding
			*/

			//Adds a part to this skin with the given name
			void AddPart(std::string name, const gui_skin::SkinPart &part);

			//Adds a text part to this skin with the given name
			void AddTextPart(std::string name, const gui_skin::SkinTextPart &text_part);


			/*
				Parts
				Retrieving
			*/

			//Gets a pointer to the part with the given name
			//Returns nullptr if no part is found with the given name
			const gui_skin::SkinPart* GetPart(std::string_view name) const noexcept;

			//Gets a pointer to the text part with the given name
			//Returns nullptr if no text part is found with the given name
			const gui_skin::SkinTextPart* GetTextPart(std::string_view name) const noexcept;


			/*
				Parts
				Removing
			*/

			//Clear all parts from this skin
			void ClearParts() noexcept;

			//Remove a part with the given name from this skin
			bool RemovePart(std::string_view name) noexcept;


			//Clear all text parts from this skin
			void ClearTextParts() noexcept;

			//Remove a text part with the given name from this skin
			bool RemoveTextPart(std::string_view name) noexcept;


			/*
				Passes
				Adding
			*/

			//Adds a pass to this skin
			void AddPass(graphics::render::Pass pass);

			//Adds passes to this skin
			void AddPasses(graphics::scene::drawable_object::Passes passes);


			//Adds a text pass to this skin
			void AddTextPass(graphics::render::Pass pass);

			//Adds text passes to this skin
			void AddTextPasses(graphics::scene::drawable_object::Passes passes);


			/*
				Passes
				Retrieving
			*/

			//Gets an immutable reference to all passes in this skin
			const graphics::scene::drawable_object::Passes& GetPasses() const noexcept;

			//Gets an immutable reference to all text passes in this skin
			const graphics::scene::drawable_object::Passes& GetTextPasses() const noexcept;


			/*
				Passes
				Removing
			*/

			//Clear all passes from this skin
			void ClearPasses() noexcept;

			//Clear all text passes from this skin
			void ClearTextPasses() noexcept;


			/*
				Skins
				Registering/retrieving
			*/

			template <typename T>
			static inline void RegisterSkin(std::string name, gui_skin::SkinBuilder builder)
			{
				static_assert(std::is_base_of_v<controls::GuiControl, T>);
				registered_skins_[std::type_index{typeid(T)}] = name;
				registered_skin_builders_[std::move(name)] = builder;
			}

			template <typename T>
			static inline std::optional<std::string_view> SkinName() noexcept
			{
				RegisterDefaultSkins();

				if (auto iter = registered_skins_.find(std::type_index{typeid(T)});
					iter != std::end(registered_skins_))
					return iter->second;
				else
					return {};
			}
	};
} //ion::gui::skins

#endif