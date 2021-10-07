/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiPanel.h
-------------------------------------------
*/

#ifndef ION_GUI_PANEL_H
#define ION_GUI_PANEL_H

#include <optional>
#include <string>
#include <string_view>

#include "IonGuiPanelContainer.h"
#include "controls/IonGuiControl.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::gui
{
	namespace gui_panel::detail
	{
	} //gui_panel::detail


	class GuiPanel : public GuiPanelContainer
	{
		private:

			

		public:

			//Construct a panel with the given name
			GuiPanel(std::string name);


			/*
				Modifiers
			*/




			/*
				Observers
			*/

			//Returns a pointer to the owner of this panel
			[[nodiscard]] inline auto Owner() const noexcept
			{
				return static_cast<GuiPanelContainer*>(owner_);
			}


			/*
				Tabulation
			*/

			//Sets the tab order of this panel to the given order
			void TabOrder(int order) noexcept;

			//Returns the tab order of this panel
			//Returns nullopt if this panel has no owner
			[[nodiscard]] std::optional<int> TabOrder() const noexcept;
	};
} //ion::gui

#endif