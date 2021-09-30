/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiComponent.h
-------------------------------------------
*/

#ifndef ION_GUI_COMPONENT_H
#define ION_GUI_COMPONENT_H

#include <string>

#include "managed/IonManagedObject.h"
#include "memory/IonNonOwningPtr.h"

//Forward declarations
namespace ion::graphics::scene::graph
{
	class SceneNode;
}

namespace ion::gui
{
	using namespace graphics::scene::graph;
	class GuiContainer; //Forward declaration

	namespace gui_component::detail
	{
	} //gui_component::detail


	class GuiComponent : public managed::ManagedObject<GuiContainer>
	{
		protected:

			NonOwningPtr<SceneNode> node_;

		public:

			//Construct a component with the given name
			explicit GuiComponent(std::string name);

			//Construct a component with the given owner and name
			GuiComponent(GuiContainer &owner, std::string name);

			//Default copy constructor
			GuiComponent(const GuiComponent&) = default;

			//Default move constructor
			GuiComponent(GuiComponent&&) = default;

			//Virtual destructor
			virtual ~GuiComponent() noexcept;


			/*
				Operators
			*/

			//Default copy assignment
			GuiComponent& operator=(const GuiComponent&) = default;

			//Default move assignment
			GuiComponent& operator=(GuiComponent&&) = default;


			/*
				Modifiers
			*/




			/*
				Observers
			*/

			//Returns a pointer to the node for this component
			[[nodiscard]] inline auto Node() const noexcept
			{
				return node_;
			}
	};
} //ion::gui

#endif