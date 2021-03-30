/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/shapes
File:	IonLine.h
-------------------------------------------
*/

#ifndef ION_LINE_H
#define ION_LINE_H

#include <utility>

#include "IonShape.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector3.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::shapes
{
	using namespace types::type_literals;
	using namespace utilities;

	namespace line::detail
	{
		mesh::Vertices line_vertices(const Vector3 &a, const Vector3 &b, const Color &color);
	} //line::detail


	class Line : public Shape
	{
		protected:

			Vector3 a_;
			Vector3 b_;
			real thickness_ = 1.0_r;

		public:
		
			//Construct a new line with the given points a b, color and visibility
			Line(const Vector3 &a, const Vector3 &b, const Color &color, bool visible = true);

			//Construct a new line with the given points a b, color, thickness and visibility
			Line(const Vector3 &a, const Vector3 &b, const Color &color, real thickness, bool visible = true);


			/*
				Modifiers
			*/

			//Sets the point a of this line to the given point
			inline void A(const Vector3 &a) noexcept
			{
				if (a_ != a)
				{
					a_ = a;
					Mesh::VertexData(line::detail::line_vertices(a_, b_, color_));
				}
			}

			//Sets the point b of this line to the given point
			inline void B(const Vector3 &b) noexcept
			{
				if (b_ != b)
				{
					b_ = b;
					Mesh::VertexData(line::detail::line_vertices(a_, b_, color_));
				}
			}

			//Sets the points a b of this line to the given points
			inline void AB(const Vector3 &a, const Vector3 &b) noexcept
			{
				if (a_ != a || b_ != b)
				{
					a_ = a;
					b_ = b;
					Mesh::VertexData(line::detail::line_vertices(a_, b_, color_));
				}
			}

			//Sets the thickness of this line
			inline void Thickness(real thickness) noexcept
			{
				thickness_ = thickness;
			}


			/*
				Observers
			*/

			//Returns the point a of this line
			[[nodiscard]] inline auto& A() const noexcept
			{
				return a_;
			}

			//Returns the point b of this line
			[[nodiscard]] inline auto& B() const noexcept
			{
				return b_;
			}

			//Returns the points a b of this line
			[[nodiscard]] inline auto AB() const noexcept
			{
				return std::pair{a_, b_};
			}

			//Returns the thickness of this line
			[[nodiscard]] inline auto Thickness() const noexcept
			{
				return thickness_;
			}


			/*
				Drawing
			*/

			//Draw this line with the given shader program (optional)
			//This can be called multiple times if more than one pass
			void Draw(shaders::ShaderProgram *shader_program = nullptr) noexcept override;
	};
} //ion::graphics::scene::shapes

#endif