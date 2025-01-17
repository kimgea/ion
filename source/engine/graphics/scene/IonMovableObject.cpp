/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonMovableObject.cpp
-------------------------------------------
*/

#include "IonMovableObject.h"

#include "IonSceneManager.h"
#include "graph/IonSceneGraph.h"
#include "graph/IonSceneNode.h"
#include "graphics/shaders/IonShaderProgramManager.h"
#include "graphics/utilities/IonMatrix3.h"
#include "query/IonSceneQuery.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene
{

using namespace movable_object;
using namespace types::type_literals;

namespace movable_object::detail
{
} //movable_object::detail


//Protected

render::Renderer* MovableObject::ParentRenderer() const noexcept
{
	if (auto scene_manager = Owner(); scene_manager)
	{
		if (auto scene_graph = scene_manager->Owner(); scene_graph)
			return &scene_graph->SceneRenderer();
	}

	return nullptr;
}


/*
	Bounding volumes
*/

Aabb MovableObject::DeriveWorldAxisAlignedBoundingBox(Aabb aabb, bool apply_extent) const noexcept
{
	if (parent_node_ && !aabb.Empty())
	{
		if (apply_extent)
		{
			if (auto [min, max] = bounding_volume_extent_.MinMax();
				min != vector2::Zero || max != vector2::UnitScale)
			{
				auto size = aabb.ToSize();
				aabb = Aabb{aabb.Min() + min * size,
							aabb.Max() + (max - vector2::UnitScale) * size};
			}
		}

		aabb.Transform(Matrix3::Transformation(parent_node_->FullTransformation()));
	}

	return aabb;
}

Obb MovableObject::DeriveWorldOrientedBoundingBox(Obb obb, Aabb aabb, bool apply_extent) const noexcept
{
	if (parent_node_ && !obb.Empty())
	{
		if (apply_extent)
		{
			if (auto [min, max] = bounding_volume_extent_.MinMax();
				min != vector2::Zero || max != vector2::UnitScale)
			{
				auto size = aabb.ToSize();
				aabb = Aabb{aabb.Min() + min * size,
							aabb.Max() + (max - vector2::UnitScale) * size};
				obb = aabb;
			}
		}

		obb.Transform(Matrix3::Transformation(parent_node_->FullTransformation()));
	}

	return obb;
}

Sphere MovableObject::DeriveWorldBoundingSphere(Sphere sphere, Aabb aabb, bool apply_extent) const noexcept
{
	if (parent_node_ && !sphere.Empty())
	{
		if (apply_extent)
		{
			if (auto [min, max] = bounding_volume_extent_.MinMax();
				min != vector2::Zero || max != vector2::UnitScale)
			{
				auto size = aabb.ToSize();
				aabb = Aabb{aabb.Min() + min * size,
							aabb.Max() + (max - vector2::UnitScale) * size};
				sphere = {aabb.ToHalfSize().Max(), aabb.Center()};
			}
		}

		sphere.Transform(Matrix3::Transformation(parent_node_->FullTransformation()));
	}

	return sphere;
}


void MovableObject::DrawBoundingVolumes(const Aabb &aabb, const Obb &obb, const Sphere &sphere,
	const Color &aabb_color, const Color &obb_color, const Color &sphere_color, real z) const noexcept
{
	if (parent_node_)
	{
		graph::scene_graph::detail::push_gl_matrix(); //New view matrix
		graph::scene_graph::detail::mult_gl_model_view_matrix(
			Matrix4::Translation({0.0_r, 0.0_r, z})); //view * z translation matrix
	}


	//Enable fixed-function pipeline
	auto active_program = shaders::shader_program_manager::detail::get_active_shader_program();
	if (active_program > 0)
		shaders::shader_program_manager::detail::use_shader_program(0);

	//Draw bounding sphere
	if (!sphere.Empty() && sphere_color != color::Transparent)
		sphere.Draw(sphere_color);

	//Draw oriented bounding box
	if (!obb.Empty() && obb_color != color::Transparent)
		obb.Draw(obb_color);

	//Draw axis-aligned bounding box
	if (!aabb.Empty() && aabb_color != color::Transparent)
		aabb.Draw(aabb_color);

	//Re-enable active shader program (if any)
	if (active_program > 0)
		shaders::shader_program_manager::detail::use_shader_program(active_program);


	if (parent_node_)
		graph::scene_graph::detail::pop_gl_matrix(); //Restore
}


//Private

/*
	Helper functions
*/

void MovableObject::Detach() noexcept
{
	if (parent_node_)
	{
		parent_node_->DetachObject(*this);
		parent_node_ = nullptr;
	}
}


//Public

MovableObject::MovableObject(std::optional<std::string> name, bool visible) noexcept :

	managed::ManagedObject<SceneManager>{std::move(name)},
	query_type_flags_{query::scene_query::QueryType::Movable},
	visible_{visible}
{
	//Empty
}

MovableObject::~MovableObject() noexcept
{
	Detach();
}


/*
	Observers
*/

RenderPrimitiveRange MovableObject::AllRenderPrimitives() noexcept
{
	//Optional to override
	return {};
}

ShaderProgramRange MovableObject::AllShaderPrograms() noexcept
{
	//Optional to override
	return {};
}

LightRange MovableObject::AllEmissiveLights() noexcept
{
	//Optional to override
	return {};
}


/*
	Rendering
*/

void MovableObject::Prepare()
{
	//Optional to override
}


/*
	Drawing
*/

void MovableObject::DrawBounds(real z) noexcept
{
	if (show_bounding_volumes_)
		DrawBoundingVolumes(
			WorldAxisAlignedBoundingBox(), WorldOrientedBoundingBox(), WorldBoundingSphere(),
			aabb_color_, obb_color_, sphere_color_, z);
}


/*
	Elapse time
*/

void MovableObject::Elapse([[maybe_unused]] duration time) noexcept
{
	//Optional to override
}

} //ion::graphics::scene