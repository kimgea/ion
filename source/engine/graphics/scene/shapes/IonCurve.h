/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/shapes
File:	IonCurve.h
-------------------------------------------
*/

#ifndef ION_CURVE_H
#define ION_CURVE_H

#include <cassert>
#include <vector>

#include "IonShape.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector3.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::shapes
{
	using namespace types::type_literals;
	using namespace utilities;

	namespace curve
	{
		using ControlPoints = std::vector<Vector3>;

		namespace detail
		{	
			constexpr auto default_curve_smoothness = 12;
			constexpr auto min_curve_smoothness = 0;

			constexpr auto max_control_points = 35;
				//Factorial(35 - 1) returns the largest result you can store in a real (32-bit float)


			inline auto curve_smoothness(int smoothness) noexcept
			{
				return smoothness < min_curve_smoothness ? min_curve_smoothness : smoothness;
			}

			mesh::Vertices curve_vertices(const ControlPoints &control_points, const Color &color, int smoothness);
		} //detail
	} //curve


	///@brief A class representing a bezier curve that supports up to 35 control points
	class Curve : public Shape
	{
		protected:

			curve::ControlPoints control_points_;
			int smoothness_ = curve::detail::default_curve_smoothness;


			virtual mesh::Vertices GetVertices() const noexcept override;

		public:
		
			///@brief Constructs a new curve with the given control points, color and visibility
			Curve(curve::ControlPoints control_points, const Color &color, bool visible = true);

			///@brief Constructs a new curve with the given control points, color, thickness and visibility
			Curve(curve::ControlPoints control_points, const Color &color, real thickness, bool visible = true);


			///@brief Constructs a new curve with the given control points, color, smoothness and visibility
			Curve(curve::ControlPoints control_points, const Color &color, int smoothness, bool visible = true);

			///@brief Constructs a new curve with the given control points, color, thickness, smoothness and visibility
			Curve(curve::ControlPoints control_points, const Color &color, real thickness, int smoothness, bool visible = true);


			/**
				@name Modifiers
				@{
			*/

			///@brief Sets Pn (nth point) of this curve to the given point
			inline void P(int n, const Vector3 &p) noexcept
			{
				assert(n < std::ssize(control_points_));

				if (control_points_[n] != p)
				{
					control_points_[n] = p;
					update_vertices_ = true;
				}
			}

			///@brief Sets the thickness of this curve
			inline void Thickness(real thickness) noexcept
			{
				LineThickness(thickness);
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns Pn (nth point) of this curve
			[[nodiscard]] inline auto& P(int n) const noexcept
			{
				assert(n < std::ssize(control_points_));
				return control_points_[n];
			}

			///@brief Returns the thickness of this curve
			[[nodiscard]] inline auto Thickness() const noexcept
			{
				return LineThickness();
			}

			///@brief Returns the smoothness of this curve
			[[nodiscard]] inline auto Smoothness() const noexcept
			{
				return smoothness_;
			}

			///@}
	};
} //ion::graphics::scene::shapes

#endif