/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/query
File:	IonSceneQuery.h
-------------------------------------------
*/

#ifndef ION_SCENE_QUERY
#define ION_SCENE_QUERY

#include <optional>
#include <variant>
#include <vector>

#include "graphics/scene/IonCamera.h"
#include "graphics/scene/IonLight.h"
#include "graphics/scene/IonMovableObject.h"
#include "graphics/scene/graph/IonSceneGraph.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::query
{
	using namespace graphics::scene::graph;

	namespace scene_query
	{
		struct QueryType
		{
			static constexpr uint32 Movable = 1 << 0;
			static constexpr uint32 Drawable = 1 << 1;

			static constexpr uint32 Animation = 1 << 2;
			static constexpr uint32 Camera = 1 << 3;
			static constexpr uint32 Light = 1 << 4;
			static constexpr uint32 Model = 1 << 5;
			static constexpr uint32 ParticleSystem = 1 << 6;
			static constexpr uint32 Sound = 1 << 7;
			static constexpr uint32 SoundListener = 1 << 8;
			static constexpr uint32 Text = 1 << 9;

			static constexpr uint32 UserType0 = 1 << 10;
				//First (pot) value not in use by the engine
		};


		namespace detail
		{
			using object_type = MovableObject*;
			using object_container_type = std::vector<object_type>;


			inline void get_eligible_objects(SceneNode &node, uint32 type_mask, bool only_visible,
				object_container_type &objects) noexcept
			{
				//Check if node is eligible
				if (!only_visible || node.Visible())
				{
					for (auto &attached_object : node.AttachedObjects())
					{
						auto object =
							std::visit([](auto &&x) noexcept -> MovableObject* { return x; }, attached_object);

						//Check if object is eligible
						if ((!only_visible || object->Visible()) &&
							object->QueryTypeFlags() & type_mask)

							objects.push_back(object); //Eligible for querying
					}
				}

				for (auto &child_node : node.ChildNodes())
					get_eligible_objects(child_node, type_mask, only_visible, objects); //Recursive
			}

			inline auto get_eligible_objects(SceneNode &node, uint32 type_mask, bool only_visible) noexcept
			{
				object_container_type objects;
				get_eligible_objects(node, type_mask, only_visible, objects);
				return objects;
			}
		} //detail
	} //scene_query


	template <typename ResultT>
	class SceneQuery
	{
		protected:
		
			std::optional<uint32> query_mask_;
			std::optional<uint32> query_type_mask_ = scene_query::QueryType::Model;
			bool only_visible_objects_ = true;
			NonOwningPtr<SceneGraph> scene_graph_;

		public:

			//Default constructor
			SceneQuery() = default;

			//Construct a new scene query with the given scene graph
			SceneQuery(NonOwningPtr<SceneGraph> scene_graph) noexcept :
				scene_graph_{scene_graph}
			{
				//Empty
			}


			/*
				Modifiers
			*/

			//Sets the query mask for this scene query to the given mask
			//This scene query will only query objects if a bitwise AND operation between the query mask and the object query flags is non-zero
			//The meaning of the bits is user-specific
			inline void QueryMask(std::optional<uint32> mask) noexcept
			{
				query_mask_ = mask;
			}

			//Adds the given mask to the already existing query mask for this scene query
			//This scene query will only query objects if a bitwise AND operation between the query mask and the object query flags is non-zero
			//The meaning of the bits is user-specific
			inline void AddQueryMask(uint32 mask) noexcept
			{
				if (query_mask_)
					*query_mask_ |= mask;
				else
					query_mask_ = mask;
			}

			//Removes the given mask to the already existing query mask for this scene query
			//This scene query will only query objects if a bitwise AND operation between the query mask and the object query flags is non-zero
			//The meaning of the bits is user-specific
			inline void RemoveQueryMask(uint32 mask) noexcept
			{
				if (query_mask_)
					*query_mask_ &= ~mask;
			}


			//Sets the query type mask for this scene query to the given mask
			//This scene query will only query objects if a bitwise AND operation between the query type mask and the object type is non-zero
			inline void QueryTypeMask(std::optional<uint32> mask) noexcept
			{
				query_type_mask_ = mask;
			}

			//Adds the given mask to the already existing query type mask for this scene query
			//This scene query will only query objects if a bitwise AND operation between the query type mask and the object type is non-zero
			inline void AddQueryTypeMask(uint32 mask) noexcept
			{
				if (query_type_mask_)
					*query_type_mask_ |= mask;
				else
					query_type_mask_ = mask;
			}

			//Removes the given mask to the already existing query type mask for this scene query
			//This scene query will only query objects if a bitwise AND operation between the query type mask and the object type is non-zero
			inline void RemoveQueryTypeMask(uint32 mask) noexcept
			{
				if (query_type_mask_)
					*query_type_mask_ &= ~mask;
			}


			//Sets whether or not this scene query is only querying visible objects
			inline void OnlyVisibleObjects(bool only_visible) noexcept
			{
				only_visible_objects_ = only_visible;
			}

			//Sets the scene graph this scene query is querying
			inline void Scene(NonOwningPtr<SceneGraph> scene_graph) noexcept
			{
				scene_graph_ = scene_graph;
			}


			/*
				Observers
			*/

			//Returns the query mask for this scene query
			//This scene query will only query objects if a bitwise AND operation between the query mask and the query flags is non-zero
			//The meaning of the bits is user-specific
			[[nodiscard]] inline auto QueryMask() const noexcept
			{
				return query_mask_;
			}

			//Returns the query type mask for this scene query
			//This scene query will only query objects if a bitwise AND operation between the query type mask and the object type is non-zero
			[[nodiscard]] inline auto QueryTypeMask() const noexcept
			{
				return query_type_mask_;
			}


			//Returns whether or not this scene query is only querying visible objects
			[[nodiscard]] inline auto OnlyVisibleObjects() const noexcept
			{
				return only_visible_objects_;
			}

			//Returns the scene graph this scene query is querying
			[[nodiscard]] inline auto Scene() const noexcept
			{
				return scene_graph_;
			}


			/*
				Querying
			*/

			//Returns the result of the scene query
			[[nodiscard]] virtual ResultT Execute() const noexcept = 0;
	};
} //ion::graphics::scene::query

#endif