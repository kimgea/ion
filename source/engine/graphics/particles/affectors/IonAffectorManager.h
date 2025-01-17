/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles/affectors
File:	IonAffectorManager.h
-------------------------------------------
*/

#ifndef ION_AFFECTOR_MANAGER_H
#define ION_AFFECTOR_MANAGER_H

#include "IonAffector.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::graphics::particles::affectors
{
	namespace affector_manager::detail
	{
	} //affector_manager::detail


	///@brief A class that manages and stores affectors
	class AffectorManager : public managed::ObjectManager<Affector, AffectorManager>
	{
		protected:

			///@brief Can only be instantiated by derived
			AffectorManager() = default;

			///@brief Deleted copy constructor
			AffectorManager(const AffectorManager&) = delete;

			///@brief Default move constructor
			AffectorManager(AffectorManager&&) = default;

		public:

			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			AffectorManager& operator=(const AffectorManager&) = delete;

			///@brief Default move assignment
			AffectorManager& operator=(AffectorManager&&) = default;

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all affectors in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Affectors() noexcept
			{
				return Objects();
			}

			///@brief Returns an immutable range of all affectors in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Affectors() const noexcept
			{
				return Objects();
			}

			///@}

			/**
				@name Affectors - Creating
				@{
			*/

			///@brief Creates an affector of type T with the given name and arguments
			template <typename T, typename... Args>
			auto CreateAffector(std::string name, Args &&...args)
			{
				static_assert(std::is_base_of_v<Affector, T>);

				auto ptr = Create<T>(std::move(name), std::forward<Args>(args)...);
				return static_pointer_cast<T>(ptr);
			}


			///@brief Creates an affector of type T as a copy of the given affector
			template <typename T>
			auto CreateAffector(const T &affector_t)
			{
				static_assert(std::is_base_of_v<Affector, T>);

				auto ptr = Create(affector_t);
				return static_pointer_cast<T>(ptr);
			}

			///@brief Creates an affector of type T by moving the given affector
			template <typename T>
			auto CreateAffector(T &&affector_t)
			{
				static_assert(std::is_base_of_v<Affector, T>);

				auto ptr = Create(std::move(affector_t));
				return static_pointer_cast<T>(ptr);
			}

			///@}

			/**
				@name Affectors - Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable affector with the given name
			///@details Returns nullptr if affector could not be found
			[[nodiscard]] auto GetAffector(std::string_view name) noexcept
			{
				return Get(name);
			}

			///@brief Gets a pointer to an immutable affector with the given name
			///@details Returns nullptr if affector could not be found
			[[nodiscard]] auto GetAffector(std::string_view name) const noexcept
			{
				return Get(name);
			}

			///@}

			/**
				@name Affectors - Removing
				@{
			*/

			///@brief Clears all affectors from this manager
			void ClearAffectors() noexcept
			{
				Clear();
			}

			///@brief Removes an affector from this manager
			auto RemoveAffector(Affector &affector) noexcept
			{
				return Remove(affector);
			}

			///@}
	};
} //ion::graphics::particles::affectors

#endif