/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiFrame.cpp
-------------------------------------------
*/

#include "IonGuiFrame.h"

#include "IonGuiController.h"
#include "controls/IonGuiTooltip.h"
#include "graphics/scene/graph/IonSceneNode.h"

namespace ion::gui
{

using namespace gui_frame;

namespace gui_frame::detail
{

void get_ordered_controls(GuiPanelContainer &owner, control_pointers &controls)
{
	for (auto &component : owner.OrderedComponents())
	{
		if (auto control = dynamic_cast<controls::GuiControl*>(&component); control)
			controls.push_back(control);
		else if (auto panel = dynamic_cast<GuiPanel*>(&component); panel)
			get_ordered_controls(*panel, controls); //Recursive
	}
}

control_pointers get_ordered_controls(GuiPanelContainer &owner)
{
	control_pointers controls;
	get_ordered_controls(owner, controls);
	return controls;
}


std::optional<control_pointers::iterator> get_current_control_iterator(control_pointers &controls,
	controls::GuiControl *focused_control) noexcept
{
	if (std::empty(controls))
		return {};
	else if (!focused_control)
		return std::end(controls);
	else
	{
		if (auto iter = std::find(std::begin(controls), std::end(controls), focused_control);
			iter != std::end(controls))
			return iter;
		else
			return {};
	}
}

} //gui_frame::detail


//Private

void GuiFrame::GatherControls()
{
	//Build the correct tab ordering for all controls in this frame
	if (std::empty(ordered_controls_))
		ordered_controls_ = detail::get_ordered_controls(*this);
}


bool GuiFrame::TabForward(GuiFrame &from_frame) noexcept
{
	GatherControls();

	if (auto current_iter = detail::get_current_control_iterator(
		ordered_controls_, this != &from_frame ? focused_control_ : last_focused_control_); current_iter)
	{
		auto iter =
			*current_iter != std::end(ordered_controls_) ? *current_iter :
			(*current_iter = detail::get_next_control_iterator(*current_iter, ordered_controls_));

		do
		{
			if (!(*iter)->IsFocused())
			{
				(*iter)->Focus();

				if ((*iter)->IsFocused())
					return true;
			}

			//Tab to next frame
			if (iter == std::end(ordered_controls_) - 1)
			{
				auto found = false;

				if (auto owner = Owner(); owner)
				{
					if (auto focusable_frame = owner->NextFocusableFrame(*this);
						focusable_frame && focusable_frame != &from_frame)

						found = focusable_frame->TabForward(from_frame); //Recursive
				}

				if (found || this != &from_frame)
					return found; //Unwind
			}
		} while ((iter = detail::get_next_control_iterator(iter, ordered_controls_)) != *current_iter);
	}

	return false;
}

bool GuiFrame::TabBackward(GuiFrame &from_frame) noexcept
{
	GatherControls();

	if (auto current_iter = detail::get_current_control_iterator(
		ordered_controls_, this != &from_frame ? focused_control_ : last_focused_control_); current_iter)
	{
		auto iter =
			*current_iter != std::end(ordered_controls_) ? *current_iter :
			(*current_iter = detail::get_previous_control_iterator(*current_iter, ordered_controls_));

		do
		{
			if (!(*iter)->IsFocused())
			{
				(*iter)->Focus();

				if ((*iter)->IsFocused())
					return true;
			}

			//Tab to previous frame
			if (iter == std::begin(ordered_controls_))
			{
				auto found = false;

				if (auto owner = Owner(); owner)
				{
					if (auto focusable_frame = owner->PreviousFocusableFrame(*this);
						focusable_frame && focusable_frame != &from_frame)

						found = focusable_frame->TabBackward(from_frame); //Recursive
				}

				if (found || this != &from_frame)
					return found; //Unwind
			}
		} while ((iter = detail::get_previous_control_iterator(iter, ordered_controls_)) != *current_iter);
	}

	return false;
}


//Protected

/*
	Events
*/

void GuiFrame::Created(GuiComponent &component) noexcept
{
	GuiPanelContainer::Created(component); //Use base functionality
	ordered_controls_.clear(); //Clear cache
}

void GuiFrame::Created(controls::GuiControl &control) noexcept
{
	GuiPanelContainer::Created(control); //Use base functionality
	control.Reset(); //Added control could be adopted
}


void GuiFrame::Removed(GuiComponent &component) noexcept
{
	ordered_controls_.clear(); //Clear cache
	GuiPanelContainer::Removed(component); //Use base functionality
}

void GuiFrame::Removed(controls::GuiControl &control) noexcept
{
	control.Reset(); //Execute opposite events based on its current state

	if (last_focused_control_ == &control)
		last_focused_control_ = nullptr;

	GuiPanelContainer::Removed(control); //Use base functionality
}


void GuiFrame::TabOrderChanged() noexcept
{
	GuiPanelContainer::TabOrderChanged(); //Use base functionality
	ordered_controls_.clear(); //Clear cache
}


bool GuiFrame::Unsubscribable(Listenable<events::listeners::GuiControlListener>&) noexcept
{
	//Cancel all unsubscribe attempts
	return false;
}


void GuiFrame::Enabled([[maybe_unused]] controls::GuiControl &control) noexcept
{
	//Empty
}

void GuiFrame::Disabled([[maybe_unused]] controls::GuiControl &control) noexcept
{
	//Empty
}


void GuiFrame::Shown([[maybe_unused]] controls::GuiControl &control) noexcept
{
	//Empty
}

void GuiFrame::Hidden([[maybe_unused]] controls::GuiControl &control) noexcept
{
	//Empty
}


void GuiFrame::Focused(controls::GuiControl &control) noexcept
{
	if (control.IsFocused() && focused_control_ != &control)
	{
		if (focused_control_)
			focused_control_->Defocus();

		Focus();
		last_focused_control_ = focused_control_ = &control;
	}
}

void GuiFrame::Defocused(controls::GuiControl &control) noexcept
{
	if (!control.IsFocused() && focused_control_ == &control)
		focused_control_ = nullptr;
}


void GuiFrame::Pressed(controls::GuiControl &control) noexcept
{
	if (control.IsPressed() && pressed_control_ != &control)
	{
		if (pressed_control_)
			pressed_control_->Release();

		pressed_control_ = &control;
	}
}

void GuiFrame::Released(controls::GuiControl &control) noexcept
{
	if (!control.IsPressed() && pressed_control_ == &control)
		pressed_control_ = nullptr;
}

void GuiFrame::Clicked([[maybe_unused]] controls::GuiControl &control) noexcept
{
	//Empty
}


void GuiFrame::Entered(controls::GuiControl &control) noexcept
{
	if (control.IsHovered() && hovered_control_ != &control)
	{
		if (hovered_control_)
			hovered_control_->Exit();

		hovered_control_ = &control;

		//Show tooltip (if any)
		if (control.Tooltip())
		{
			if (auto owner = Owner(); owner && owner->ActiveTooltip())
				owner->ActiveTooltip()->Show(*control.Tooltip());
		}
	}
}

void GuiFrame::Exited(controls::GuiControl &control) noexcept
{
	if (!control.IsHovered() && hovered_control_ == &control)
	{
		hovered_control_ = nullptr;

		//Hide tooltip (if any)
		if (auto owner = Owner(); owner && owner->ActiveTooltip())
			owner->ActiveTooltip()->Hide();
	}
}


void GuiFrame::Enabled() noexcept
{
	NotifyFrameEnabled();
	GuiPanelContainer::Enabled(); //Use base functionality
}

void GuiFrame::Disabled() noexcept
{
	Defocus();
	NotifyFrameDisabled();
	GuiPanelContainer::Disabled(); //Use base functionality
}


void GuiFrame::Shown() noexcept
{
	//Show all controls that should be visible
	for (auto &control : Controls())
	{
		if (control.IsVisible())
			control.Show();
	}

	//Show all panels that should be visible
	for (auto &panel : Panels())
	{
		if (panel.IsVisible())
			panel.Show();
	}

	GuiPanelContainer::Shown(); //Use base functionality
}

void GuiFrame::Hidden() noexcept
{
	GuiPanelContainer::Hidden(); //Use base functionality
}


void GuiFrame::Activated() noexcept
{
	NotifyFrameActivated();
}

void GuiFrame::Deactivated() noexcept
{
	Defocus();
	last_focused_control_ = nullptr;

	NotifyFrameDeactivated();
}


void GuiFrame::Focused() noexcept
{
	NotifyFrameFocused();
}

void GuiFrame::Defocused() noexcept
{
	if (pressed_control_)
		pressed_control_->Release();

	if (focused_control_)
		focused_control_->Defocus();

	if (hovered_control_)
		hovered_control_->Exit();

	NotifyFrameDefocused();
}


/*
	Notifying
*/

void GuiFrame::NotifyFrameEnabled() noexcept
{
	if (auto owner = Owner(); owner)
		FrameEventsGeneratorBase::NotifyAll(owner->FrameEvents().Listeners(),
			&events::listeners::GuiFrameListener::Enabled, std::ref(*this));
}

void GuiFrame::NotifyFrameDisabled() noexcept
{
	if (auto owner = Owner(); owner)
		FrameEventsGeneratorBase::NotifyAll(owner->FrameEvents().Listeners(),
			&events::listeners::GuiFrameListener::Disabled, std::ref(*this));
}


void GuiFrame::NotifyFrameActivated() noexcept
{
	if (auto owner = Owner(); owner)
		FrameEventsGeneratorBase::NotifyAll(owner->FrameEvents().Listeners(),
			&events::listeners::GuiFrameListener::Activated, std::ref(*this));

	//User callback
	if (on_activate_)
		(*on_activate_)(*this);
}

void GuiFrame::NotifyFrameDeactivated() noexcept
{
	if (auto owner = Owner(); owner)
		FrameEventsGeneratorBase::NotifyAll(owner->FrameEvents().Listeners(),
			&events::listeners::GuiFrameListener::Deactivated, std::ref(*this));

	//User callback
	if (on_deactivate_)
		(*on_deactivate_)(*this);
}


void GuiFrame::NotifyFrameFocused() noexcept
{
	if (auto owner = Owner(); owner)
		FrameEventsGeneratorBase::NotifyAll(owner->FrameEvents().Listeners(),
			&events::listeners::GuiFrameListener::Focused, std::ref(*this));

	//User callback
	if (on_focus_)
		(*on_focus_)(*this);
}

void GuiFrame::NotifyFrameDefocused() noexcept
{
	if (auto owner = Owner(); owner)
		FrameEventsGeneratorBase::NotifyAll(owner->FrameEvents().Listeners(),
			&events::listeners::GuiFrameListener::Defocused, std::ref(*this));

	//User callback
	if (on_defocus_)
		(*on_defocus_)(*this);
}


/*
	Intersection
*/

controls::GuiControl* GuiFrame::IntersectedControl(const Vector2 &position) noexcept
{
	detail::control_pointers::value_type candidate_control = nullptr;

	//Check all controls in ascending tab order
	for (GatherControls(); auto &control : ordered_controls_)
	{
		if ((!candidate_control ||
			control->GlobalZOrder() >= candidate_control->GlobalZOrder()) &&
			control->Intersects(position))
			//New candidate found that overlaps previous (greater or equal z-order)
			candidate_control = control;
	}

	return candidate_control;
}


//Public

GuiFrame::GuiFrame(std::string name) :
	GuiPanelContainer{std::move(name)}
{
	ControlEvents().Subscribe(*this);
}


/*
	Modifiers
*/

void GuiFrame::Activate(FrameMode mode) noexcept
{
	GuiPanelContainer::Show();

	if (!activated_)
	{
		activated_ = true;
		mode_ = mode;
		Activated();
	}
}

void GuiFrame::Deactivate() noexcept
{
	GuiPanelContainer::Hide();

	if (activated_)
	{
		activated_ = false;
		mode_ = {};
		Deactivated();
	}
}


void GuiFrame::Show(FrameMode mode) noexcept
{
	Activate(mode);
}

void GuiFrame::Hide() noexcept
{
	Deactivate();
}


void GuiFrame::Focus() noexcept
{
	if (!focused_ && IsFocusable())
	{
		focused_ = true;
		Focused();
	}
}

void GuiFrame::Defocus() noexcept
{
	if (focused_)
	{
		focused_ = false;
		Defocused();
	}
}


void GuiFrame::ActiveTheme(std::string_view name) noexcept
{
	if (auto owner = Owner(); owner)
		active_theme_ = owner->GetTheme(name);
}


/*
	Observers
*/

bool GuiFrame::IsFocusable() const noexcept
{
	if (enabled_ && visible_ && activated_)
	{
		if (auto owner = Owner(); owner)
			return owner->IsEnabled() && owner->IsVisible() && owner->IsOnTop(*this);
		else
			return true;
	}
	else
		return false;
}

bool GuiFrame::IsOnTop() const noexcept
{
	if (activated_)
	{
		if (auto owner = Owner(); owner)
			return owner->IsOnTop(*this);
		else
			return true;
	}
	else
		return false;
}


GuiController* GuiFrame::Owner() const noexcept
{
	return static_cast<GuiController*>(owner_);
}


/*
	Tabulating
*/

void GuiFrame::TabForward() noexcept
{
	TabForward(*this);
}

void GuiFrame::TabBackward() noexcept
{
	TabBackward(*this);
}


/*
	Frame events
*/

void GuiFrame::FrameStarted(duration time) noexcept
{
	for (auto &control : Controls())
		control.FrameStarted(time);

	for (auto &panel : Panels())
		panel.FrameStarted(time);
}

void GuiFrame::FrameEnded(duration time) noexcept
{
	for (auto &control : Controls())
		control.FrameEnded(time);

	for (auto &panel : Panels())
		panel.FrameEnded(time);
}


/*
	Key events
*/

bool GuiFrame::KeyPressed(KeyButton button) noexcept
{
	if (!enabled_)
		return false;

	if (focused_control_ &&
		focused_control_->KeyPressed(button))
		return true; //Consumed
	
	else
	{
		switch (button)
		{
			case KeyButton::Enter:
			case KeyButton::Space:
			{
				if (focused_control_ && !pressed_control_)
				{
					focused_control_->Press();
					return true; //Consumed
				}

				break;
			}
		}

		return false;
	}
}

bool GuiFrame::KeyReleased(KeyButton button) noexcept
{
	if (!enabled_)
		return false;

	if (focused_control_ &&
		focused_control_->KeyReleased(button))
		return true; //Consumed
	
	else
	{
		switch (button)
		{
			case KeyButton::Enter:
			case KeyButton::Space:
			{
				if (pressed_control_)
				{
					pressed_control_->Click();
					pressed_control_->Release();
					return true; //Consumed
				}

				break;
			}
		}

		return false;
	}
}

bool GuiFrame::CharacterPressed(char character) noexcept
{
	if (!enabled_)
		return false;

	return focused_control_ ?
		focused_control_->CharacterPressed(character) :
		false;
}


/*
	Mouse events
*/

bool GuiFrame::MousePressed(MouseButton button, Vector2 position) noexcept
{
	if (!enabled_)
		return false;

	if (hovered_control_ &&
		hovered_control_->MousePressed(button, position))
		return true; //Consumed

	else
	{
		switch (button)
		{
			case MouseButton::Left:
			{
				if (hovered_control_ && !pressed_control_)
				{
					hovered_control_->Press();
					return true; //Consumed
				}

				break;
			}
		}

		return false;
	}
}

bool GuiFrame::MouseReleased(MouseButton button, Vector2 position) noexcept
{
	if (!enabled_)
		return false;

	if (pressed_control_ &&
		pressed_control_->MouseReleased(button, position))
		return true; //Consumed

	else
	{
		switch (button)
		{
			case MouseButton::Left:
			{
				if (pressed_control_)
				{
					if (pressed_control_ == hovered_control_)
						pressed_control_->Click();

					pressed_control_->Release();
					return true; //Consumed
				}

				break;
			}
		}

		return false;
	}
}

bool GuiFrame::MouseMoved(Vector2 position) noexcept
{
	if (!enabled_)
		return false;

	if (pressed_control_ &&
		pressed_control_->MouseMoved(position))
		return true; //Consumed

	else if (hovered_control_ &&
		hovered_control_->MouseMoved(position))
		return true; //Consumed

	else
	{
		if (auto intersected_control = IntersectedControl(position);
			intersected_control != hovered_control_)
		{
			if (hovered_control_)
				hovered_control_->Exit();

			if (intersected_control)
				intersected_control->Enter();

			return true; //Consumed
		}

		return false;
	}
}

bool GuiFrame::MouseWheelRolled(int delta, Vector2 position) noexcept
{
	if (!enabled_)
		return false;

	return hovered_control_ ?
		hovered_control_->MouseWheelRolled(delta, position) :
		false;
}

} //ion::gui