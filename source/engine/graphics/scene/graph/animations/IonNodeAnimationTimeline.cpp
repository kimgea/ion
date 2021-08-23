/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph/animations
File:	IonNodeAnimationTimeline.cpp
-------------------------------------------
*/

#include "IonNodeAnimationTimeline.h"

#include <algorithm>
#include <type_traits>

namespace ion::graphics::scene::graph::animations
{

using namespace node_animation_timeline;

namespace node_animation_timeline::detail
{

} //node_animation_timeline::detail


//Private

duration NodeAnimationTimeline::GetTotalDuration() const noexcept
{
	auto total_duration = 0.0_sec;

	for (auto &animation : AttachedAnimations())
		total_duration = std::max(total_duration_, animation.StartTime() +
			(animation.Get() ? animation.Get()->TotalDuration() : 0.0_sec));

	for (auto &animation_group : AttachedAnimationGroups())
		total_duration = std::max(total_duration_, animation_group.StartTime() +
			(animation_group.Get() ? animation_group.Get()->TotalDuration() : 0.0_sec));

	return total_duration;
}


//Public

NodeAnimationTimeline::NodeAnimationTimeline(real playback_rate, bool running) noexcept :
	
	playback_rate_{playback_rate},
	running_{running}
{
	//Empty
}

NodeAnimationTimeline::NodeAnimationTimeline(std::string name, real playback_rate, bool running) noexcept :

	managed::ManagedObject<NodeAnimationManager>{std::move(name)},
	playback_rate_{playback_rate},
	running_{running}
{
	//Empty
}


/*
	Modifiers
*/

void NodeAnimationTimeline::RepeatCount(std::optional<int> repeat_count) noexcept
{
	if (repeat_count)
	{
		auto current_cycle = repeat_count_ ? repeat_count_->first : 0;
		repeat_count_.emplace(current_cycle, *repeat_count < current_cycle ? current_cycle : *repeat_count);
	}
	else
		repeat_count_.reset();
}


/*
	Playback
*/

void NodeAnimationTimeline::Start() noexcept
{
	running_ = true;
}

void NodeAnimationTimeline::Stop() noexcept
{
	running_ = false;
}

void NodeAnimationTimeline::Reset() noexcept
{
	running_ = false;
	Revert();
}

void NodeAnimationTimeline::Restart() noexcept
{
	Reset();
	Start();
}


void NodeAnimationTimeline::Revert(duration total_duration)
{
	if (total_duration > 0.0_sec)
		reverse_playback_rate_ = current_time_ / total_duration;
	else
		current_time_ = 0.0_sec;

	reverse_ = true;
}


/*
	Node animations
	Attaching
*/

NonOwningPtr<AttachableNodeAnimation> NodeAnimationTimeline::Attach(NonOwningPtr<NodeAnimation> node_animation,
	duration start_time, bool enable)
{
	auto ptr = NodeAnimationBase::Create(node_animation, start_time, enable);
	total_duration_ = std::max(total_duration_, ptr->StartTime() +
		(node_animation ? node_animation->TotalDuration() : 0.0_sec));
	return ptr;
}


/*
	Node animations
	Detaching
*/

void NodeAnimationTimeline::DetachAllAnimations() noexcept
{
	NodeAnimationBase::Clear();

	if (std::empty(AttachedAnimationGroups()))
		total_duration_ = 0.0_sec;
	else
		total_duration_ = GetTotalDuration();
}

bool NodeAnimationTimeline::DetachAnimation(AttachableNodeAnimation &node_animation) noexcept
{
	auto removed = NodeAnimationBase::Remove(node_animation);
	total_duration_ = GetTotalDuration();
	return removed;
}


/*
	Animation groups
	Attaching
*/

NonOwningPtr<AttachableNodeAnimationGroup> NodeAnimationTimeline::Attach(NonOwningPtr<NodeAnimationGroup> node_animation_group,
	duration start_time, bool enable)
{
	auto ptr = NodeAnimationGroupBase::Create(node_animation_group, start_time, enable);
	total_duration_ = std::max(total_duration_, ptr->StartTime() +
		(node_animation_group ? node_animation_group->TotalDuration() : 0.0_sec));
	return ptr;
}


/*
	Node animation groups
	Detaching
*/

void NodeAnimationTimeline::DetachAllAnimationGroups() noexcept
{
	NodeAnimationGroupBase::Clear();

	if (std::empty(AttachedAnimations()))
		total_duration_ = 0.0_sec;
	else
		total_duration_ = GetTotalDuration();
}

bool NodeAnimationTimeline::DetachAnimationGroup(AttachableNodeAnimationGroup &node_animation_group) noexcept
{
	auto removed = NodeAnimationGroupBase::Remove(node_animation_group);
	total_duration_ = GetTotalDuration();
	return removed;
}


/*
	Detaching
*/

void NodeAnimationTimeline::DetachAll() noexcept
{
	NodeAnimationBase::Clear();
	NodeAnimationGroupBase::Clear();
	total_duration_ = 0.0_sec;
}


/*
	Elapse time
*/

void NodeAnimationTimeline::Elapse(duration time) noexcept
{
	if (running_)
	{
		if (reverse_)
			time = -time;

		current_time_ += time *
			(reverse_ ? reverse_playback_rate_ : playback_rate_);

		for (auto &animation : AttachedAnimations())
			animation.Elapse(time, current_time_);

		for (auto &animation_group : AttachedAnimationGroups())
			animation_group.Elapse(time, current_time_);


		//A timeline cycle has been completed
		if (current_time_ <= 0.0_sec || current_time_ >= total_duration_)
		{
			//Loop (next cycle)
			if (!reverse_ && //Forwarding
				(!repeat_count_ || //Indefinitely
				  repeat_count_->first < repeat_count_->second))
			{
				if (repeat_count_)
					++repeat_count_->first;

				current_time_ = 0.0_sec;
			}
			//Timeline is done
			else
			{
				Stop();
				current_time_ = std::clamp(current_time_, 0.0_sec, total_duration_);
					//Make sure animation stays at start or stop

				if (repeat_count_)
					repeat_count_->first = 0;

				reverse_ = false;
			}
		}
	}
}

} //ion::graphics::scene::graph::animations