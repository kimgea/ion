/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph
File:	IonSceneNode.h
-------------------------------------------
*/

#ifndef ION_SCENE_NODE
#define ION_SCENE_NODE

#include <algorithm>
#include <vector>

#include "adaptors/IonFlatSet.h"
#include "adaptors/ranges/IonDereferenceIterable.h"
#include "graphics/utilities/IonMatrix4.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "memory/IonNonOwningPtr.h"
#include "memory/IonOwningPtr.h"
#include "types/IonTypes.h"

//Forward declaration
namespace ion::graphics::scene
{
	class MovableObject;
	class Camera;
	class Light;
}

namespace ion::graphics::scene::graph
{
	using namespace graphics::utilities;
	using namespace types::type_literals;

	class SceneNode; //Forward declaration

	namespace scene_node
	{
		enum class NodeRotationOrigin : bool
		{
			Parent,
			Local
		};

		using SceneNodes = std::vector<OwningPtr<SceneNode>>;


		namespace detail
		{
			using scene_node_container = std::vector<SceneNode*>;
			using movable_object_container = std::vector<MovableObject*>;
			using camera_container = adaptors::FlatSet<Camera*>;
			using light_container = adaptors::FlatSet<Light*>;


			template <typename Compare>
			inline void add_node(scene_node_container &scene_nodes, SceneNode *scene_node, Compare compare)
			{
				auto iter = std::upper_bound(std::begin(scene_nodes), std::end(scene_nodes), scene_node, compare);
				scene_nodes.insert(iter, scene_node);
			}

			template <typename Compare>
			inline void remove_node(scene_node_container &scene_nodes, SceneNode *scene_node, Compare compare) noexcept
			{
				if (auto iter = std::lower_bound(std::begin(scene_nodes), std::end(scene_nodes), scene_node, compare);
					iter != std::end(scene_nodes) && !compare(scene_node, *iter)) //Todo

					scene_nodes.erase(iter);
			}

			template <typename Compare>
			inline void move_nodes(scene_node_container &dest, scene_node_container &source, Compare compare)
			{
				auto size = std::size(dest);

				dest.insert(
					std::end(dest),
					std::begin(source), std::end(source));

				//Something to move
				if (auto first = std::begin(dest) + size; first != std::begin(dest))
					//Move source nodes and merge with dest nodes
					std::inplace_merge(
						std::begin(dest), first,
						std::end(dest), compare);

				source.clear();
				source.shrink_to_fit();
			}


			Matrix4 make_transformation(const Vector3 &position, real rotation, const Vector2 &scaling) noexcept;
		} //detail
	} //scene_node


	class SceneNode final
	{
		private:

			Vector3 position_;
			Vector2 direction_ = vector2::UnitY;
			real rotation_ = 0.0_r;
			Vector2 scaling_ = vector2::UnitScale;

			Vector2 initial_direction_ = vector2::UnitY;
			scene_node::NodeRotationOrigin rotation_origin_ = scene_node::NodeRotationOrigin::Parent;
			bool inherit_rotation_ = true;
			bool inherit_scaling_ = true;
			bool visible_ = true;

			SceneNode *parent_node_ = nullptr;
			scene_node::SceneNodes child_nodes_;
			scene_node::detail::movable_object_container attached_objects_;

			scene_node::detail::scene_node_container ordered_nodes_; //Root node only
			scene_node::detail::camera_container attached_cameras_; //Root node only
			scene_node::detail::light_container attached_lights_; //Root node only


			mutable Vector3 derived_position_;
			mutable Vector2 derived_direction_ = vector2::UnitY;
			mutable real derived_rotation_ = 0.0_r;
			mutable Vector2 derived_scaling_ = vector2::UnitScale;
			mutable Matrix4 full_tranformation_;

			mutable bool need_update_ = true;
			mutable bool transformation_out_of_date_ = true;


			inline auto RootNode() noexcept -> decltype(this)
			{
				if (parent_node_)
					return parent_node_->RootNode(); //Recursive

				return this;
			}

			inline auto RootNode() const noexcept -> decltype(this)
			{
				if (parent_node_)
					return parent_node_->RootNode(); //Recursive

				return this;
			}


			/*
				Updating
			*/

			inline void NotifyUpdate() noexcept
			{
				need_update_ = true;

				for (auto &child_node : child_nodes_)
					child_node->NotifyUpdate(); //Recursive
			}

			void Update() const noexcept;


			/*
				Removing
			*/

			OwningPtr<SceneNode> Extract(SceneNode &child_node) noexcept;
			scene_node::SceneNodes ExtractAll() noexcept;

		public:

			//Default construct a scene node as the root
			SceneNode() = default;

			//Construct a scene node as the root with the given visibility
			explicit SceneNode(bool visible) noexcept;

			//Construct a scene node as the root with the given initial direction and visibility
			explicit SceneNode(const Vector2 &initial_direction, bool visible = true) noexcept;

			//Construct a scene node as the root with the given position, initial direction and visibility
			explicit SceneNode(const Vector3 &position, const Vector2 &initial_direction = vector2::UnitY, bool visible = true) noexcept;


			//Destructor
			~SceneNode() noexcept;


			/*
				Ranges
			*/

			//Returns a mutable range of all child nodes
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto ChildNodes() noexcept
			{
				return adaptors::ranges::DereferenceIterable<scene_node::SceneNodes&>{child_nodes_};
			}

			//Returns an immutable range of all child nodes
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto ChildNodes() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const scene_node::SceneNodes&>{child_nodes_};
			}


			//Returns a mutable range of all movable objects attached to this node
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedObjects() noexcept
			{
				return adaptors::ranges::DereferenceIterable<scene_node::detail::movable_object_container&>{attached_objects_};
			}

			//Returns an immutable range of all movable objects attached to this node
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedObjects() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const scene_node::detail::movable_object_container&>{attached_objects_};
			}


			/*
				Ranges
				Root node only
			*/

			//Returns a mutable range of this and all descendant nodes ordered for rendering
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto OrderedSceneNodes() noexcept
			{
				return adaptors::ranges::DereferenceIterable<scene_node::detail::scene_node_container&>{ordered_nodes_};
			}

			//Returns an immutable range of this and all descendant nodes ordered for rendering
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto OrderedSceneNodes() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const scene_node::detail::scene_node_container&>{ordered_nodes_};
			}


			//Returns a mutable range of all cameras attached to this and all descendant nodes
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedCameras() noexcept
			{
				return adaptors::ranges::DereferenceIterable<scene_node::detail::camera_container&>{attached_cameras_};
			}

			//Returns an immutable range of all cameras attached to this and all descendant nodes
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedCameras() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const scene_node::detail::camera_container&>{attached_cameras_};
			}


			//Returns a mutable range of all lights attached to this and all descendant nodes
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedLights() noexcept
			{
				return adaptors::ranges::DereferenceIterable<scene_node::detail::light_container&>{attached_lights_};
			}

			//Returns an immutable range of all lights attached to this and all descendant nodes
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedLights() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const scene_node::detail::light_container&>{attached_lights_};
			}


			/*
				Modifiers
			*/

			//Sets the local position of this node to the given position
			inline void Position(const Vector3 &position) noexcept
			{
				if (position_ != position)
				{
					if (position_.Z() != position.Z())
						; //Todo

					position_ = position;
					NotifyUpdate();
				}
			}

			//Sets the local direction of this node to the given direction
			inline void Direction(const Vector2 &direction) noexcept
			{
				if (direction_ != direction)
				{
					direction_ = direction;
					rotation_ = direction.SignedAngleBetween(initial_direction_); //Update rotation
					NotifyUpdate();
				}
			}

			//Sets the local rotation of this node to the given angle in radians
			inline void Rotation(real angle) noexcept
			{
				if (rotation_ != angle)
				{
					rotation_ = angle;
					direction_ = initial_direction_.Deviant(angle); //Update direction
					NotifyUpdate();
				}
			}

			//Sets the local scaling of this node to the given scaling
			inline void Scaling(const Vector2 &scaling) noexcept
			{
				if (scaling_ != scaling)
				{
					scaling_ = scaling;
					NotifyUpdate();
				}
			}


			//Sets the rotation origin of this node to the given origin
			inline void RotationOrigin(scene_node::NodeRotationOrigin origin) noexcept
			{
				if (rotation_origin_ != origin)
				{
					rotation_origin_ = origin;
					NotifyUpdate();
				}
			}

			//Sets whether or not this node should inherit rotation
			inline void InheritRotation(bool inherit) noexcept
			{
				if (inherit_rotation_ != inherit)
				{
					inherit_rotation_ = inherit;
					NotifyUpdate();
				}
			}

			//Sets whether or not this node should inherit scaling
			inline void InheritScaling(bool inherit) noexcept
			{
				if (inherit_scaling_ != inherit)
				{
					inherit_scaling_ = inherit;
					NotifyUpdate();
				}
			}

			//Sets whether or not this and all descendant nodes should be visible
			//If cascade is set to false, only this node is set
			inline void Visible(bool visible, bool cascade = true) noexcept
			{
				visible_ = visible;

				if (cascade)
				{
					for (auto &child_node : child_nodes_)
						child_node->Visible(visible); //Recursive
				}
			}

			//Flips the visibility of this and all descendant nodes
			//If cascade is set to false, only this node is flipped
			inline void FlipVisibility(bool cascade = true) noexcept
			{
				visible_ = !visible_;

				if (cascade)
				{
					for (auto &child_node : child_nodes_)
						child_node->FlipVisibility(); //Recursive
				}
			}


			/*
				Observers
			*/

			//Returns the local position of this node
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}

			//Returns the local direction of this node
			[[nodiscard]] inline auto& Direction() const noexcept
			{
				return direction_;
			}

			//Returns the local rotation of this node in radians
			[[nodiscard]] inline auto Rotation() const noexcept
			{
				return rotation_;
			}

			//Returns the local scaling of this node
			[[nodiscard]] inline auto& Scaling() const noexcept
			{
				return scaling_;
			}


			//Returns the initial direction of this node
			[[nodiscard]] inline auto& InitialDirection() const noexcept
			{
				return initial_direction_;
			}

			//Returns the rotation origin of this node
			[[nodiscard]] inline auto RotationOrigin() const noexcept
			{
				return rotation_origin_;
			}

			//Returns whether or not this node inherit rotation
			[[nodiscard]] inline auto InheritRotation() const noexcept
			{
				return inherit_rotation_;
			}

			//Returns whether or not this node inherit scaling
			[[nodiscard]] inline auto InheritScaling() const noexcept
			{
				return inherit_scaling_;
			}

			//Returns whether or not this node is visible
			[[nodiscard]] inline auto Visible() const noexcept
			{
				return visible_;
			}


			//Returns the parent node of this node
			//Returns nullptr if this node is the root
			[[nodiscard]] inline auto ParentNode() const noexcept
			{
				return parent_node_;
			}



			//Returns the derived position of this node
			[[nodiscard]] inline auto& DerivedPosition() const noexcept
			{
				if (need_update_)
					Update();

				return derived_position_;
			}

			//Returns the derived direction of this node
			[[nodiscard]] inline auto& DerivedDirection() const noexcept
			{
				if (need_update_)
					Update();

				return derived_direction_;
			}

			//Returns the derived rotation of this node in radians
			[[nodiscard]] inline auto DerivedRotation() const noexcept
			{
				if (need_update_)
					Update();

				return derived_rotation_;
			}

			//Returns the derived scaling of this node
			[[nodiscard]] inline auto& DerivedScaling() const noexcept
			{
				if (need_update_)
					Update();

				return derived_scaling_;
			}

			//Returns the full transformation matrix for this node
			[[nodiscard]] inline auto& FullTransformation() const noexcept
			{
				if (need_update_)
					Update();

				if (transformation_out_of_date_)
				{
					full_tranformation_ =
						scene_node::detail::make_transformation(derived_position_, derived_rotation_, derived_scaling_);
					transformation_out_of_date_ = false;
				}

				return full_tranformation_;
			}


			//Returns true if this node is axis aligned
			[[nodiscard]] bool AxisAligned() const noexcept;


			/*
				Transformations
				Relative
			*/

			//Translate this node by the given unit
			void Translate(const Vector3 &unit) noexcept;

			//Translate this node by the given unit
			void Translate(real unit) noexcept;

			//Rotate this node by the given angle in radians
			void Rotate(real angle) noexcept;

			//Scale this node by the given unit
			void Scale(const Vector2 &unit) noexcept;


			/*
				Transformations
				Absolute
			*/

			//Turn this node such that it faces the given position
			void LookAt(const Vector3 &position) noexcept;


			/*
				Child nodes
				Creating
			*/

			//Create a new scene node as a child of this node with the given visibility
			[[nodiscard]] NonOwningPtr<SceneNode> CreateChildNode(bool visible = true);

			//Create a new scene node as a child of this node with the given initial direction and visibility
			[[nodiscard]] NonOwningPtr<SceneNode> CreateChildNode(const Vector2 &initial_direction, bool visible = true);

			//Create a new scene node as a child of this node with the given position, initial direction and visibility
			[[nodiscard]] NonOwningPtr<SceneNode> CreateChildNode(const Vector3 &position, const Vector2 &initial_direction = vector2::UnitY, bool visible = true);


			/*
				Child nodes
				Take / release ownership
			*/

			//Adopt (take ownership of) the given scene node and returns a pointer to the adopted node
			//Returns nullptr if the scene node could not be adopted and scene node will remain untouched
			NonOwningPtr<SceneNode> Adopt(OwningPtr<SceneNode> &scene_node);

			//Adopt (take ownership of) the given scene node and returns a pointer to the adopted node
			//Returns nullptr if the scene node could not be adopted and scene node will be released
			NonOwningPtr<SceneNode> Adopt(OwningPtr<SceneNode> &&scene_node);


			//Adopt (take ownership of) all the given scene nodes
			//If one or more scene nodes could not be adopted, they will remain untouched in the given container
			void Adopt(scene_node::SceneNodes &scene_nodes);

			//Adopt (take ownership of) all the given scene nodes
			//If one or more scene nodes could not be adopted, they will be released
			void Adopt(scene_node::SceneNodes &&scene_nodes);


			//Orphan (release ownership of) the given child node
			//Returns a pointer to the scene node released
			[[nodiscard]] OwningPtr<SceneNode> Orphan(SceneNode &child_node) noexcept;

			//Orphan (release ownership of) all child nodes in this scene node
			//Returns pointers to the scene nodes released
			[[nodiscard]] scene_node::SceneNodes OrphanAll() noexcept;


			/*
				Child nodes
				Removing
			*/

			//Clear all child nodes from this scene node
			void ClearChildNodes() noexcept;

			//Remove the given child node from this scene node
			//Returns true if the given child node was removed
			bool RemoveChildNode(SceneNode &child_node) noexcept;


			/*
				Attached objects
			*/

			//Attach the given movable object to this node if not already attached
			//Return true if the given movable object was attached
			bool AttachObject(MovableObject &movable_object);

			//Detach the given movable objects if attached to this node
			//Returns true if the given movable object was detached
			bool DetachObject(MovableObject &movable_object) noexcept;

			//Detach all movable objects attached to this node
			void DetachAllObjects() noexcept;


			/*
				Attached objects
				Camera
			*/

			//Attach the given camera to this node if not already attached
			//Return true if the given camera was attached
			bool AttachObject(Camera &camera);

			//Detach the given camera if attached to this node
			//Returns true if the given camera was detached
			bool DetachObject(Camera &camera) noexcept;


			/*
				Attached objects
				Light
			*/

			//Attach the given light to this node if not already attached
			//Return true if the given light was attached
			bool AttachObject(Light &light);

			//Detach the given light if attached to this node
			//Returns true if the given light was detached
			bool DetachObject(Light &light) noexcept;
	};
} //ion::graphics::scene::graph

#endif