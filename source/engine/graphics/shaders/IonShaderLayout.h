/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders
File:	IonShaderLayout.h
-------------------------------------------
*/

#ifndef ION_SHADER_LAYOUT_H
#define ION_SHADER_LAYOUT_H

#include <algorithm>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "adaptors/IonFlatMap.h"
#include "managed/IonManagedObject.h"

namespace ion::graphics::shaders
{
	class ShaderProgramManager; //Forward declaration

	namespace shader_layout
	{
		enum class AttributeName
		{
			Vertex_Position,
			Vertex_Normal,
			Vertex_Color,
			Vertex_TexCoord,
			Vertex_PointSize
		};

		enum class UniformName
		{
			Matrix_ModelView,
			Matrix_Projection,
			Matrix_ModelViewProjection,

			Camera_Position,
			Camera_Gamma,
			
			Material_Ambient,
			Material_Diffuse,
			Material_Specular,
			Material_Shininess,
			Material_DiffuseMap,
			Material_SpecularMap,
			Material_NormalMap,
			Material_HasDiffuseMap,
			Material_HasSpecularMap,
			Material_HasNormalMap,
			Material_Enabled,

			Light_Type,
			Light_Position,
			Light_Direction,
			Light_Cutoff,
			Light_Ambient,
			Light_Diffuse,
			Light_Specular,
			Light_AttenuationConstant,
			Light_AttenuationLinear,
			Light_AttenuationQuadratic,
			Light_Count
		};


		class VariableDeclaration final
		{
			private:

				std::optional<std::string> name_;
				std::optional<int> location_;

			public:

				VariableDeclaration(std::string name);
				VariableDeclaration(int location) noexcept;
				VariableDeclaration(std::string name, int location);


				inline auto& Name() const noexcept
				{
					return name_;
				}

				inline auto Location() const noexcept
				{
					return location_;
				}
		};

		using AttributeBinding = std::pair<AttributeName, VariableDeclaration>;
		using UniformBinding = std::pair<UniformName, VariableDeclaration>;

		using AttributeBindings = std::vector<AttributeBinding>;
		using UniformBindings = std::vector<UniformBinding>;

		inline const auto DefaultAttributeBindings =
			AttributeBindings
			{
				{AttributeName::Vertex_Position, 	{"vertex_position", 	0}},
				{AttributeName::Vertex_Normal, 		{"vertex_normal", 		1}},
				{AttributeName::Vertex_Color, 		{"vertex_color", 		2}},
				{AttributeName::Vertex_TexCoord, 	{"vertex_tex_coord", 	3}},
				{AttributeName::Vertex_PointSize,	{"vertex_point_size", 	4}}
			};

		inline const auto DefaultUniformBindings =
			UniformBindings
			{
				{UniformName::Matrix_ModelView, 			{"matrix.model_view", 				0}},
				{UniformName::Matrix_Projection, 			{"matrix.projection", 				1}},
				{UniformName::Matrix_ModelViewProjection,	{"matrix.model_view_projection",	2}},	

				{UniformName::Camera_Position, 				{"camera.position", 				3}},
				{UniformName::Camera_Gamma,					{"camera.gamma",					4}},

				{UniformName::Material_Ambient, 			{"material.ambient", 				5}},
				{UniformName::Material_Diffuse, 			{"material.diffuse", 				6}},
				{UniformName::Material_Specular, 			{"material.specular", 				7}},
				{UniformName::Material_Shininess, 			{"material.shininess", 				8}},
				{UniformName::Material_DiffuseMap, 			{"material.diffuse_map", 			9}},
				{UniformName::Material_SpecularMap, 		{"material.specular_map", 			10}},
				{UniformName::Material_NormalMap, 			{"material.normal_map", 			11}},
				{UniformName::Material_HasDiffuseMap, 		{"material.has_diffuse_map", 		12}},
				{UniformName::Material_HasSpecularMap, 		{"material.has_specular_map", 		13}},
				{UniformName::Material_HasNormalMap, 		{"material.has_normal_map", 		14}},
				{UniformName::Material_Enabled, 			{"material.enabled", 				15}},

				{UniformName::Light_Type, 					{"light.type", 						16}},
				{UniformName::Light_Position, 				{"light.position", 					17}},
				{UniformName::Light_Direction, 				{"light.direction", 				18}},
				{UniformName::Light_Cutoff, 				{"light.cutoff", 					19}},
				{UniformName::Light_Ambient, 				{"light.ambient", 					20}},
				{UniformName::Light_Diffuse, 				{"light.diffuse", 					21}},
				{UniformName::Light_Specular, 				{"light.specular", 					22}},
				{UniformName::Light_AttenuationConstant, 	{"light.attenuation_constant", 		23}},
				{UniformName::Light_AttenuationLinear, 		{"light.attenuation_linear", 		24}},
				{UniformName::Light_AttenuationQuadratic, 	{"light.attenuation_quadratic", 	25}},
				{UniformName::Light_Count, 					{"light.count", 					26}}
			};

		namespace detail
		{
			constexpr auto attribute_name_count = static_cast<int>(AttributeName::Vertex_PointSize) + 1;
			constexpr auto uniform_name_count = static_cast<int>(UniformName::Light_Count) + 1;

			using attribute_binding_map = adaptors::FlatMap<AttributeName, shader_layout::VariableDeclaration>;
			using uniform_binding_map = adaptors::FlatMap<UniformName, shader_layout::VariableDeclaration>;


			template <typename T>
			auto make_binding_map(T variable_bindings) noexcept
			{
				//Sort on location (asc)
				std::stable_sort(std::begin(variable_bindings), std::end(variable_bindings),
					[](auto &x, auto &y) noexcept
					{
						return x.second.Location() < y.second.Location();
					});

				//Remove duplicate locations
				variable_bindings.erase(
					std::unique(std::begin(variable_bindings), std::end(variable_bindings),
						[](auto &x, auto &y) noexcept
						{
							return x.second.Location() && y.second.Location() &&
								   x.second.Location() == y.second.Location();
						}), std::end(variable_bindings));


				//Sort on name (asc)
				std::stable_sort(std::begin(variable_bindings), std::end(variable_bindings),
					[](auto &x, auto &y) noexcept
					{
						return x.second.Name() < y.second.Name();
					});

				//Remove duplicate names
				variable_bindings.erase(
					std::unique(std::begin(variable_bindings), std::end(variable_bindings),
						[](auto &x, auto &y) noexcept
						{
							return x.second.Name() && y.second.Name() &&
								   x.second.Name() == y.second.Name();
						}), std::end(variable_bindings));

				return adaptors::FlatMap<T::value_type::first_type, T::value_type::second_type>{std::move(variable_bindings)};
			}

			template <typename T>
			auto is_declaration_unique(VariableDeclaration declaration, const T &variable_bindings)
			{
				//Declaration has location
				if (declaration.Location())
				{
					//No duplicate locations allowed
					for (auto &binding : variable_bindings)
					{
						if (binding.second.Location() == declaration.Location())
							return false;
					}
				}

				//Declaration has name
				if (declaration.Name())
				{
					//No duplicate names allowed
					for (auto &binding : variable_bindings)
					{
						if (binding.second.Name() == declaration.Name())
							return false;
					}
				}

				return true;
			}
		} //detail
	} //shader_layout


	class ShaderLayout final : public managed::ManagedObject<ShaderProgramManager>
	{
		private:

			shader_layout::detail::attribute_binding_map attribute_bindings_;
			shader_layout::detail::uniform_binding_map uniform_bindings_;

		public:

			//Construct a new empty shader layout with the given name
			ShaderLayout(std::string name);

			//Construct a new shader layout with the given name, attribute and uniform bindings
			ShaderLayout(std::string name,
				shader_layout::AttributeBindings attribute_bindings, shader_layout::UniformBindings uniform_bindings);


			/*
				Ranges
			*/

			//Returns a mutable range of all attributes in this shader layout
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Attributes() noexcept
			{
				return attribute_bindings_.Elements();
			}

			//Returns an immutable range of all attributes in this shader layout
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Attributes() const noexcept
			{
				return attribute_bindings_.Elements();
			}


			//Returns a mutable range of all uniforms in this shader layout
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Uniforms() noexcept
			{
				return uniform_bindings_.Elements();
			}

			//Returns an immutable range of all uniforms in this shader layout
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Uniforms() const noexcept
			{
				return uniform_bindings_.Elements();
			}


			/*
				Modifiers
			*/

			//Bind the given name to the given attribute declaration
			//Returns true if the attribute binding suceeded
			bool BindAttribute(shader_layout::AttributeName name, shader_layout::VariableDeclaration declaration);

			//Bind the given name to the given uniform declaration
			//Returns true if the uniform binding suceeded
			bool BindUniform(shader_layout::UniformName name, shader_layout::VariableDeclaration declaration);


			/*
				Observers
			*/

			//Returns the attribute declaration bound to the given standardized attribute name
			//Returns nullopt if no attribute binding could be found
			[[nodiscard]] std::optional<shader_layout::VariableDeclaration> BoundAttribute(shader_layout::AttributeName name) const noexcept;

			//Returns the uniform declaration bound to the given standardized uniform name
			//Returns nullopt if no uniform binding could be found
			[[nodiscard]] std::optional<shader_layout::VariableDeclaration> BoundUniform(shader_layout::UniformName name) const noexcept;


			//Returns the standardized attribute name that is bound to the given attribute name
			//Returns nullopt if no attribute binding could be found
			[[nodiscard]] std::optional<shader_layout::AttributeName> GetAttributeName(std::string_view name) const noexcept;

			//Returns the standardized uniform name that is bound to the given uniform name
			//Returns nullopt if no uniform binding could be found
			[[nodiscard]] std::optional<shader_layout::UniformName> GetUniformName(std::string_view name) const noexcept;


			//Returns the standardized attribute name that is bound to the given attribute location
			//Returns nullopt if no attribute binding could be found
			[[nodiscard]] std::optional<shader_layout::AttributeName> GetAttributeName(int location) const noexcept;

			//Returns the standardized uniform name that is bound to the given uniform location
			//Returns nullopt if no uniform binding could be found
			[[nodiscard]] std::optional<shader_layout::UniformName> GetUniformName(int location) const noexcept;
	};


	namespace shader_layout
	{
		/*
			Predefined shader layouts
		*/

		inline const auto DefaultShaderLayout =
			ShaderLayout{"", DefaultAttributeBindings, DefaultUniformBindings};
	} //shader_layout
} //ion::graphics::shaders

#endif