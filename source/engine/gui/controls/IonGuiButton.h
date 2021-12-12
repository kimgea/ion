/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiButton.h
-------------------------------------------
*/

#ifndef ION_GUI_BUTTON_H
#define ION_GUI_BUTTON_H

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "IonGuiControl.h"
#include "adaptors/ranges/IonIterable.h"

namespace ion::gui::controls
{
	namespace gui_button
	{
		enum class ButtonActionType
		{
			//Controller actions
			ShowGui,
			HideGui,
			EnableGui,
			DisableGui,

			//Frame actions
			ShowFrame,
			ShowFrameModal,
			HideFrame,
			EnableFrame,
			DisableFrame,
			FocusFrame,
			DefocusFrame,

			//Panel actions
			ShowPanel,
			HidePanel,
			EnablePanel,
			DisablePanel,

			//Control actions
			ShowControl,
			HideControl,
			EnableControl,
			DisableControl,
			FocusControl,
			DefocusControl
		};

		using ButtonAction = std::pair<ButtonActionType, std::string>;
		using ButtonActions = std::vector<ButtonAction>;


		namespace detail
		{
		} //detail
	} //gui_button


	class GuiButton : public GuiControl
	{
		private:

			void ExecuteActions() noexcept;

		protected:

			gui_button::ButtonActions actions_;


			/*
				Events
			*/

			//See GuiControl::Clicked for more details
			virtual void Clicked() noexcept override;

		public:

			//Construct a button with the given name, caption, tooltip and skin
			GuiButton(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
				gui_control::ControlSkin skin);

			//Construct a button with the given name, caption, tooltip, skin and size
			GuiButton(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
				gui_control::ControlSkin skin, const Vector2 &size);

			//Construct a button with the given name, caption, tooltip, skin and hit areas
			GuiButton(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
				gui_control::ControlSkin skin, gui_control::Areas areas);


			/*
				Ranges
			*/

			//Returns a mutable range of all actions that are executed when clicking on this button
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Actions() noexcept
			{
				return adaptors::ranges::Iterable<gui_button::ButtonActions&>{actions_};
			}

			//Returns an immutable range of all actions that are executed when clicking on this button
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Actions() const noexcept
			{
				return adaptors::ranges::Iterable<const gui_button::ButtonActions&>{actions_};
			}


			/*
				Modifiers
			*/

			//Adds the given action to this button
			void AddAction(gui_button::ButtonAction action);

			//Adds the given actions to this button
			void AddActions(gui_button::ButtonActions actions);


			//Clears all of the added actions on this button
			void ClearActions() noexcept;


			/*
				Observers
			*/


	};

} //ion::gui::controls

#endif