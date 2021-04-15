/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/shapes
File:	IonTriangle.h
-------------------------------------------
*/

#ifndef ION_TRIANGLE_H
#define ION_TRIANGLE_H

#include <tuple>

#include "IonShape.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector3.h"

namespace ion::graphics::scene::shapes
{
	using namespace utilities;

	namespace triangle::detail
	{
		mesh::Vertices triangle_vertices(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Color &color);
	} //triangle::detail


	class Triangle : public Shape
	{
		protected:

			Vector3 a_;
			Vector3 b_;
			Vector3 c_;


			virtual mesh::Vertices GetVertices() const noexcept override;

		public:
		
			//Construct a new triangle with the given points a b c, color and visibility
			Triangle(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Color &color, bool visible = true);


			/*
				Modifiers
			*/

			//Sets the point a of this triangle to the given point
			inline void A(const Vector3 &a) noexcept
			{
				if (a_ != a)
				{
					a_ = a;
					Mesh::VertexData(GetVertices());
				}
			}

			//Sets the point b of this triangle to the given point
			inline void B(const Vector3 &b) noexcept
			{
				if (b_ != b)
				{
					b_ = b;
					Mesh::VertexData(GetVertices());
				}
			}

			//Sets the point c of this triangle to the given point
			inline void C(const Vector3 &c) noexcept
			{
				if (c_ != c)
				{
					c_ = c;
					Mesh::VertexData(GetVertices());
				}
			}

			//Sets the points a b c of this triangle to the given points
			inline void ABC(const Vector3 &a, const Vector3 &b, const Vector3 &c) noexcept
			{
				if (a_ != a || b_ != b || c_ != c)
				{
					a_ = a;
					b_ = b;
					c_ = c;
					Mesh::VertexData(GetVertices());
				}
			}


			/*
				Observers
			*/

			//Returns the point a of this triangle
			[[nodiscard]] inline auto& A() const noexcept
			{
				return a_;
			}

			//Returns the point b of this triangle
			[[nodiscard]] inline auto& B() const noexcept
			{
				return b_;
			}

			//Returns the point c of this triangle
			[[nodiscard]] inline auto& C() const noexcept
			{
				return c_;
			}

			//Returns the points a b c of this triangle
			[[nodiscard]] inline auto ABC() const noexcept
			{
				return std::tuple{a_, b_, c_};
			}
	};
} //ion::graphics::scene::shapes

#endif