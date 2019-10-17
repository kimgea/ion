/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/utilities
File:	IonMatrix3.cpp
-------------------------------------------
*/

#include "IonMatrix3.h"

#include <cmath>
#include "utilities/IonMath.h"

namespace ion::graphics::utilities
{

using namespace matrix3;
using namespace ion::utilities;

namespace matrix3::detail
{
} //matrix3::detail

Matrix3::Matrix3(real m00, real m01, real m02,
				 real m10, real m11, real m12,
				 real m20, real m21, real m22) noexcept :
	m_{{m00, m01, m02},
	   {m10, m11, m12},
	   {m20, m21, m22}}
{
	//Empty
}

Matrix3::Matrix3(real m00, real m01,
				 real m10, real m11,
				 real m20, real m21) noexcept :
	m_{{m00, m01, 0.0_r},
	   {m10, m11, 0.0_r},
	   {m20, m21, 1.0_r}}
{
	//Empty
}


/*
	Static matrix conversions
*/

Matrix3 Matrix3::Reflection(real angle) noexcept
{
	auto sin_of_angle = math::Sin(angle * 2.0_r);
	auto cos_of_angle = math::Cos(angle * 2.0_r);

	//Same for both left and right handed rotation
	return {cos_of_angle, sin_of_angle,
			sin_of_angle, -cos_of_angle,
			0.0_r, 0.0_r};
}

Matrix3 Matrix3::Rotation(real angle) noexcept
{
	auto sin_of_angle = math::Sin(angle);
	auto cos_of_angle = math::Cos(angle);
	
	#ifdef ION_LEFT_HAND_ROTATION
	//Left-hand rotation CW (Direct3D)
	// cos, sin
	//-sin, cos
	return {cos_of_angle, sin_of_angle,
			-sin_of_angle, cos_of_angle,
			0.0_r, 0.0_r};
	#else
	//Right-hand rotation CCW (OpenGL)
	//cos, -sin
	//sin, cos
	return {cos_of_angle, -sin_of_angle,
			sin_of_angle, cos_of_angle,
			0.0_r, 0.0_r};
	#endif
}

Matrix3 Matrix3::Rotation(real angle, const Vector2 &origin) noexcept
{
	return Translation(origin).Rotate(angle).Translate(-origin);
}

Matrix3 Matrix3::Scaling(const Vector2 &vector) noexcept
{
	return {vector.X(), 0.0_r,
			0.0_r, vector.Y(),
			0.0_r, 0.0_r};
}

Matrix3 Matrix3::Shearing(const Vector2 &vector) noexcept
{
	return {1.0_r, vector.Y(),
			vector.X(), 1.0_r,
			0.0_r, 0.0_r};
}

Matrix3 Matrix3::Translation(const Vector2 &vector) noexcept
{
	return {1.0_r, 0.0_r,
			0.0_r, 1.0_r,
			vector.X(), vector.Y()};
}


/*
	Matrix conversions
*/

real Matrix3::ToReflection() const noexcept
{
	//Same for both left and right handed rotation
	return std::atan2(m_[1][0], m_[0][0]) / 2.0_r;
}

real Matrix3::ToRotation() const noexcept
{
	#ifdef ION_LEFT_HAND_ROTATION
	//Left-hand rotation CW (Direct3D)
	return std::atan2(m_[0][1], m_[0][0]);
	#else
	//Right-hand rotation CCW (OpenGL)
	return std::atan2(m_[1][0], m_[0][0]);
	#endif
}

Vector2 Matrix3::ToScaling() const noexcept
{
	return {m_[0][0], m_[1][1]};
}

Vector2 Matrix3::ToShearing() const noexcept
{
	return {m_[1][0], m_[0][1]};
}

Vector2 Matrix3::ToTranslation() const noexcept
{
	return {m_[2][0], m_[2][1]};
}


/*
	Adjoint/adjugate
*/

Matrix3& Matrix3::Adjoint() noexcept
{
	return *this = AdjointCopy();
}

Matrix3 Matrix3::AdjointCopy() const noexcept
{
	Matrix3 matrix{*this};
	return matrix.Cofactor().Transpose();
}


/*
	Affine
*/

Matrix3& Matrix3::Affine() noexcept
{
	m_[2][0] = 0.0_r;
	m_[2][1] = 0.0_r;
	m_[2][2] = 1.0_r;
	return *this;
}

Matrix3 Matrix3::AffineCopy() const noexcept
{
	Matrix3 matrix{*this};
	return matrix.Affine();
}

bool Matrix3::IsAffine() const noexcept
{
	return m_[2][0] == 0.0_r && m_[2][1] == 0.0_r && m_[2][2] == 1.0_r;
}


/*
	Concatenating
*/

Matrix3& Matrix3::Concatenate(const Matrix3 &matrix) noexcept
{
	return *this = ConcatenateCopy(matrix);
}

Matrix3 Matrix3::ConcatenateCopy(const Matrix3 &matrix) const noexcept
{
	return *this * matrix;
}


Matrix3& Matrix3::ConcatenateAffine(const Matrix3 &matrix) noexcept
{
	return *this = ConcatenateAffineCopy(matrix);
}

Matrix3 Matrix3::ConcatenateAffineCopy(const Matrix3 &matrix) const noexcept
{
	//Matrices must be affine
	if (!IsAffine() || !matrix.IsAffine())
		return Zero;
	else
	{
		return {m_[0][0] * matrix.m_[0][0] + m_[0][1] * matrix.m_[1][0] + m_[0][2],
				m_[0][0] * matrix.m_[0][1] + m_[0][1] * matrix.m_[1][1] + m_[0][2],
				m_[0][0] * matrix.m_[0][2] + m_[0][1] * matrix.m_[1][2] + m_[0][2] + m_[0][2],
			
				m_[1][0] * matrix.m_[0][0] + m_[1][1] * matrix.m_[1][0] + m_[1][2],
				m_[1][0] * matrix.m_[0][1] + m_[1][1] * matrix.m_[1][1] + m_[1][2],
				m_[1][0] * matrix.m_[0][2] + m_[1][1] * matrix.m_[1][2] + m_[1][2] + m_[1][2],
			
				0.0_r,
				0.0_r,
				1.0_r};
	}
}


/*
	Cofactoring
*/

Matrix3& Matrix3::Cofactor() noexcept
{
	return *this = CofactorCopy();
}

Matrix3 Matrix3::CofactorCopy() const noexcept
{
	return {  m_[1][1] * m_[2][2] - m_[2][1] * m_[1][2],
			-(m_[1][0] * m_[2][2] - m_[2][0] * m_[1][2]),
			  m_[1][0] * m_[2][1] - m_[2][0] * m_[1][1],

			-(m_[0][1] * m_[2][2] - m_[2][1] * m_[0][2]),
			  m_[0][0] * m_[2][2] - m_[2][0] * m_[0][2],
			-(m_[0][0] * m_[2][1] - m_[2][0] * m_[0][1]),

			  m_[0][1] * m_[1][2] - m_[1][1] * m_[0][2],
			-(m_[0][0] * m_[1][2] - m_[1][0] * m_[0][2]),
			  m_[0][0] * m_[1][1] - m_[1][0] * m_[0][1]};
}


/*
	Determinant and inversing
*/

real Matrix3::Determinant() const noexcept
{
	return m_[0][0] * (m_[1][1] * m_[2][2] - m_[1][2] * m_[2][1]) -
		   m_[0][1] * (m_[1][0] * m_[2][2] - m_[1][2] * m_[2][0]) +
		   m_[0][2] * (m_[1][0] * m_[2][1] - m_[1][1] * m_[2][0]);
}


Matrix3& Matrix3::Inverse() noexcept
{
	return *this = InverseCopy();
}

Matrix3 Matrix3::InverseCopy() const noexcept
{
	Matrix3 matrix{*this};
	return matrix.Adjoint() * (1.0_r / Determinant());
}


/*
	Reflecting
*/

Matrix3& Matrix3::Reflect(real angle) noexcept
{
	return *this = ReflectCopy(angle);
}

Matrix3 Matrix3::ReflectCopy(real angle) const noexcept
{
	return Reflection(angle) * (*this);
}


/*
	Rotating
*/

Matrix3& Matrix3::Rotate(real angle) noexcept
{
	return *this = RotateCopy(angle);
}

Matrix3 Matrix3::RotateCopy(real angle) const noexcept
{
	return Rotation(angle) * (*this);
}


/*
	Scaling
*/

Matrix3& Matrix3::Scale(const Vector2 &vector) noexcept
{
	return *this = ScaleCopy(vector);
}

Matrix3 Matrix3::ScaleCopy(const Vector2 &vector) const noexcept
{
	return Scaling(vector) * (*this);
}


/*
	Shearing
*/

Matrix3& Matrix3::Shear(const Vector2 &vector) noexcept
{
	return *this = ShearCopy(vector);
}

Matrix3 Matrix3::ShearCopy(const Vector2 &vector) const noexcept
{
	return Shearing(vector) * (*this);
}


/*
	Transforming
*/

Vector2 Matrix3::TransformPoint(const Vector2 &point) const noexcept
{
	auto [x, y] = point.XY();

	#ifdef ION_LEFT_HAND_ROTATION
	//Left-hand rotation CW (Direct3D)
	return {(m_[0][0] * x + m_[1][0] * y) + m_[2][0],
			(m_[0][1] * x + m_[1][1] * y) + m_[2][1]};
	#else
	//Right-hand rotation CCW (OpenGL)
	return {(m_[0][0] * x + m_[0][1] * y) + m_[0][2],
			(m_[1][0] * x + m_[1][1] * y) + m_[1][2]};
	#endif
}


/*
	Translating
*/

Matrix3& Matrix3::Translate(const Vector2 &vector) noexcept
{
	return *this = TranslateCopy(vector);
}

Matrix3 Matrix3::TranslateCopy(const Vector2 &vector) const noexcept
{
	return Translation(vector) * (*this);
}


/*
	Transposition
*/

Matrix3& Matrix3::Transpose() noexcept
{
	return *this = TransposeCopy();
}

Matrix3 Matrix3::TransposeCopy() const noexcept
{
	return {m_[0][0], m_[1][0], m_[2][0],
			m_[0][1], m_[1][1], m_[2][1],
			m_[0][2], m_[1][2], m_[2][2]};
}

} //ion::graphics::utilities