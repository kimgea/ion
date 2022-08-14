/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonLight.cpp
-------------------------------------------
*/

#include "IonLight.h"

#include <array>
#include "graphics/IonGraphicsAPI.h"
#include "graphics/textures/IonTextureManager.h"
#include "query/IonSceneQuery.h"

namespace ion::graphics::scene
{

using namespace light;
using namespace utilities;

namespace light::detail
{

std::optional<textures::texture::TextureHandle> create_texture(int width, int depth) noexcept
{
	if (!textures::texture_manager::detail::has_support_for_array_texture())
		return {};

	auto texture_handle =
		textures::texture::TextureHandle{0, textures::texture::TextureType::ArrayTexture1D};

	glGenTextures(1, reinterpret_cast<unsigned int*>(&texture_handle));
	glBindTexture(GL_TEXTURE_1D_ARRAY, texture_handle.Id);

	if (auto max_lights = textures::texture_manager::detail::max_array_texture_layers(); depth > max_lights)
		depth = max_lights;

	//Create gl texture (POT)
	glTexImage2D(GL_TEXTURE_1D_ARRAY, 0,
		GL_RGBA, width, depth,
		0, GL_RGBA, GL_FLOAT, nullptr);

	glBindTexture(GL_TEXTURE_1D_ARRAY, 0);
	return texture_handle;
}

std::optional<textures::texture::TextureHandle> create_light_texture(light_pointers lights) noexcept
{
	constexpr auto width = static_cast<int>(textures::texture_manager::detail::upper_power_of_two(light_float_components));
	auto depth = static_cast<int>(textures::texture_manager::detail::upper_power_of_two(std::size(lights)));
	return create_texture(width, depth);
}

std::optional<textures::texture::TextureHandle> create_emissive_light_texture(light_pointers lights) noexcept
{
	constexpr auto width = static_cast<int>(textures::texture_manager::detail::upper_power_of_two(emissive_light_float_components));
	auto depth = static_cast<int>(textures::texture_manager::detail::upper_power_of_two(std::size(lights)));
	return create_texture(width, depth);
}


std::optional<textures::texture::TextureHandle> upload_light_data(
	std::optional<textures::texture::TextureHandle> texture_handle, light_pointers lights) noexcept
{
	auto depth = 0;

	if (texture_handle)
		glGetTexLevelParameteriv(
			textures::texture::detail::texture_type_to_gl_texture_type(texture_handle->Type), 0, GL_TEXTURE_HEIGHT, &depth);

	//Too many lights to fit inside texture, create new texture (next POT)
	if (depth < std::ssize(lights))
	{
		if (texture_handle)
			textures::texture_manager::detail::unload_texture(*texture_handle);
		
		texture_handle = create_light_texture(lights);
	}
	
	if (texture_handle)
	{
		std::array<float, light_float_components> light_data{};
		glBindTexture(GL_TEXTURE_1D_ARRAY, texture_handle->Id);

		for (auto i = 0; auto &light : lights)
		{
			light_data[0] = static_cast<float>(light->Type());

			{
				auto [x, y, z] = light->Position().XYZ();
				light_data[1] = static_cast<float>(x);
				light_data[2] = static_cast<float>(y);
				light_data[3] = static_cast<float>(z);
			}

			{
				auto [x, y, z] = light->Direction().XYZ();
				light_data[4] = static_cast<float>(x);
				light_data[5] = static_cast<float>(y);
				light_data[6] = static_cast<float>(z);
			}

			light_data[7] = static_cast<float>(light->Radius());

			{
				auto [r, g, b, a] = light->AmbientColor().RGBA();
				light_data[8] = static_cast<float>(r);
				light_data[9] = static_cast<float>(g);
				light_data[10] = static_cast<float>(b);
				light_data[11] = static_cast<float>(a);
			}

			{
				auto [r, g, b, a] = light->DiffuseColor().RGBA();
				light_data[12] = static_cast<float>(r);
				light_data[13] = static_cast<float>(g);
				light_data[14] = static_cast<float>(b);
				light_data[15] = static_cast<float>(a);
			}

			{
				auto [r, g, b, a] = light->SpecularColor().RGBA();
				light_data[16] = static_cast<float>(r);
				light_data[17] = static_cast<float>(g);
				light_data[18] = static_cast<float>(b);
				light_data[19] = static_cast<float>(a);
			}

			auto [constant, linear, quadratic] = light->Attenuation();
			light_data[20] = static_cast<float>(constant);
			light_data[21] = static_cast<float>(linear);
			light_data[22] = static_cast<float>(quadratic);

			auto [inner_cutoff, outer_cutoff] = light->Cutoff();
			light_data[23] = static_cast<float>(inner_cutoff);
			light_data[24] = static_cast<float>(outer_cutoff);

			//Upload light data to gl
			glTexSubImage2D(GL_TEXTURE_1D_ARRAY, 0,
				0, i++, light_float_components, 1,
				GL_RGBA, GL_FLOAT, std::data(light_data));
		}

		glBindTexture(GL_TEXTURE_1D_ARRAY, 0);
	}

	return texture_handle;
}

std::optional<textures::texture::TextureHandle> upload_emissive_light_data(
	std::optional<textures::texture::TextureHandle> texture_handle, light_pointers lights) noexcept
{
	auto depth = 0;

	if (texture_handle)
		glGetTexLevelParameteriv(
			textures::texture::detail::texture_type_to_gl_texture_type(texture_handle->Type), 0, GL_TEXTURE_HEIGHT, &depth);

	//Too many lights to fit inside texture, create new texture (next POT)
	if (depth < std::ssize(lights))
	{
		if (texture_handle)
			textures::texture_manager::detail::unload_texture(*texture_handle);

		texture_handle = create_emissive_light_texture(lights);
	}
	
	if (texture_handle)
	{
		std::array<float, emissive_light_float_components> light_data{};
		glBindTexture(GL_TEXTURE_1D_ARRAY, texture_handle->Id);

		for (auto i = 0; auto &light : lights)
		{
			auto [x, y, z] = light->Position().XYZ();
			light_data[0] = static_cast<float>(x);
			light_data[1] = static_cast<float>(y);
			light_data[2] = static_cast<float>(z);

			light_data[3] = static_cast<float>(light->Radius());

			auto [r, g, b, a] = light->DiffuseColor().RGBA();
			light_data[4] = static_cast<float>(r);
			light_data[5] = static_cast<float>(g);
			light_data[6] = static_cast<float>(b);
			light_data[7] = static_cast<float>(a);	

			//Upload light data to gl
			glTexSubImage2D(GL_TEXTURE_1D_ARRAY, 0,
				0, i++, emissive_light_float_components, 1,
				GL_RGBA, GL_FLOAT, std::data(light_data));
		}

		glBindTexture(GL_TEXTURE_1D_ARRAY, 0);
	}

	return texture_handle;
}

} //light::detail


//Private

void Light::PrepareBoundingVolumes() noexcept
{
	if (type_ == LightType::Point && radius_ > 0.0_r)
		aabb_ = Aabb::Size(radius_ * 2.0_r, position_);
	else
		aabb_ = {};

	obb_ = aabb_;
	sphere_ = {aabb_.ToHalfSize().Max(), aabb_.Center()};
}


//Public

Light::Light(std::optional<std::string> name, bool visible) noexcept :
	MovableObject{std::move(name), visible}
{
	query_type_flags_ |= query::scene_query::QueryType::Light;
}

Light::Light(std::optional<std::string> name, LightType type,
	const Vector3 &position, const Vector3 &direction, real radius,
	const Color &diffuse,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	real cutoff_angle, real outer_cutoff_angle, bool visible) noexcept :

	Light{std::move(name), type,
		  position, direction, radius,
		  diffuse, diffuse, color::DarkGray,
		  attenuation_constant, attenuation_linear, attenuation_quadratic,
		  cutoff_angle, outer_cutoff_angle, visible}
{
	//Empty
}

Light::Light(std::optional<std::string> name, LightType type,
	const Vector3 &position, const Vector3 &direction, real radius,
	const Color &ambient, const Color &diffuse, const Color &specular,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	real cutoff_angle, real outer_cutoff_angle, bool visible) noexcept :

	MovableObject{std::move(name), visible},

	type_{type},
	position_{position},
	direction_{direction},
	radius_{radius},

	ambient_color_{ambient},
	diffuse_color_{diffuse},
	specular_color_{specular},

	attenuation_constant_{attenuation_constant},
	attenuation_linear_{attenuation_linear},
	attenuation_quadratic_{attenuation_quadratic},
	
	cutoff_{detail::angle_to_cutoff(cutoff_angle)},
	outer_cutoff_{detail::angle_to_cutoff(outer_cutoff_angle)}
{
	query_type_flags_ |= query::scene_query::QueryType::Light;
}


/*
	Static light conversions
*/

Light Light::Point(std::optional<std::string> name,
	const Vector3 &position, real radius, const Color &diffuse,
	bool visible) noexcept
{
	return {std::move(name), LightType::Point,
			position, vector3::Zero, radius,
			diffuse,
			1.0_r, 0.0_r, 0.0_r,
			0.0_r, 0.0_r, visible};
}

Light Light::Point(std::optional<std::string> name,
	const Vector3 &position, real radius, const Color &diffuse,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	bool visible) noexcept
{
	return {std::move(name), LightType::Point,
			position, vector3::Zero, radius,
			diffuse,
			attenuation_constant, attenuation_linear, attenuation_quadratic,
			0.0_r, 0.0_r, visible};
}

Light Light::Point(std::optional<std::string> name,
	const Vector3 &position, real radius,
	const Color &ambient, const Color &diffuse, const Color &specular,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	bool visible) noexcept
{
	return {std::move(name), LightType::Point,
			position, vector3::Zero, radius,
			ambient, diffuse, specular,
			attenuation_constant, attenuation_linear, attenuation_quadratic,
			0.0_r, 0.0_r, visible};
}


Light Light::Directional(std::optional<std::string> name,
	const Vector3 &direction, const Color &diffuse, bool visible) noexcept
{
	return {std::move(name), LightType::Directional,
			vector3::Zero, direction, 0.0,
			diffuse,
			1.0_r, 0.0_r, 0.0_r,
			0.0_r, 0.0_r, visible};
}

Light Light::Directional(std::optional<std::string> name,
	const Vector3 &direction,
	const Color &ambient, const Color &diffuse, const Color &specular,
	bool visible) noexcept
{
	return {std::move(name), LightType::Directional,
			vector3::Zero, direction, 0.0,
			ambient, diffuse, specular,
			1.0_r, 0.0_r, 0.0_r,
			0.0_r, 0.0_r, visible};
}


Light Light::Spot(std::optional<std::string> name,
	const Vector3 &position, const Vector3 &direction, const Color &diffuse,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	real cutoff_angle, real outer_cutoff_angle, bool visible) noexcept
{
	return {std::move(name), LightType::Spot,
			position, direction, 0.0,
			diffuse,
			attenuation_constant, attenuation_linear, attenuation_quadratic,
			cutoff_angle, outer_cutoff_angle, visible};
}

Light Light::Spot(std::optional<std::string> name,
	const Vector3 &position, const Vector3 &direction,
	const Color &ambient, const Color &diffuse, const Color &specular,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	real cutoff_angle, real outer_cutoff_angle, bool visible) noexcept
{
	return {std::move(name), LightType::Spot,
			position, direction, 0.0,
			ambient, diffuse, specular,
			attenuation_constant, attenuation_linear, attenuation_quadratic,
			cutoff_angle, outer_cutoff_angle, visible};
}


/*
	Rendering
*/

void Light::Render() noexcept
{
	Prepare();
}


/*
	Preparing
*/

void Light::Prepare() noexcept
{
	if (update_bounding_volumes_)
	{
		PrepareBoundingVolumes();
		update_bounding_volumes_ = false;
	}
}

} //ion::graphics::scene