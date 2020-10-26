/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles
File:	IonParticleSystem.cpp
-------------------------------------------
*/

#include "IonParticleSystem.h"

namespace ion::graphics::particles
{

using namespace particle_system;

namespace particle_system::detail
{

} //particle_system::detail


ParticleSystem::ParticleSystem(std::string name) :
	managed::ManagedObject<scene::SceneManager>{std::move(name)}
{
	//Empty
}

ParticleSystem::ParticleSystem(std::string name, ParticlePrimitive particle_primitive) :

	managed::ManagedObject<scene::SceneManager>{std::move(name)},
	particle_primitive_{particle_primitive}
{
	//Empty
}


/*
	Common functions for controlling the particle system
*/

void ParticleSystem::StartAll() noexcept
{
	for (auto &emitter : Emitters())
		emitter.Start();
}

void ParticleSystem::StopAll() noexcept
{
	for (auto &emitter : Emitters())
		emitter.Stop();
}

void ParticleSystem::ResetAll() noexcept
{
	for (auto &emitter : Emitters())
		emitter.Reset();
}

void ParticleSystem::RestartAll() noexcept
{
	for (auto &emitter : Emitters())
		emitter.Restart();
}


/*
	Elapse time
*/

void ParticleSystem::Elapse(duration time) noexcept
{
	//Elapse emitters
	for (auto &emitter : Emitters())
	{
		emitter.Elapse(time);

		//Affect particles (in emitter)
		for (auto &affector : Affectors())
			affector.Affect(emitter.Particles(), time);
	}
}


/*
	Particles
*/

bool ParticleSystem::HasActiveParticles() const noexcept
{
	for (auto &emitter : Emitters())
	{
		if (emitter.HasActiveParticles())
			return true;
	}

	return false;
}

} //ion::graphics::particles