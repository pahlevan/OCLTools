#pragma once

#include <cassert>
#include <cmath>
//#include <float.h>

template <class T>
class Vec2
{
public:

	T	x;
	T	y;
public:
	Vec2(void) : x(T(0)), y(T(0)) {}
	Vec2(T _x, T _y) :x(_x), y(_y) {};

	/** Exchange the contents of this vector with another.
	*/
	inline void swap(Vec2& other)
	{
		std::swap(x, other.x);
		std::swap(y, other.y);
	}

	/** Assigns the value of the other vector.
	@param
	rkVector The other vector
	*/
	inline Vec2& operator = (const Vec2& rkVector)
	{
		x = rkVector.x;
		y = rkVector.y;

		return *this;
	}

	inline Vec2& operator = (const T fScalar)
	{
		x = fScalar;
		y = fScalar;

		return *this;
	}

	inline bool operator == (const Vec2& rkVector) const
	{
		return (x == rkVector.x && y == rkVector.y);
	}

	inline bool operator != (const Vec2& rkVector) const
	{
		return (x != rkVector.x || y != rkVector.y);
	}

	/** Sets this vector's components to the minimum of its own and the
	ones of the passed in vector.
	@remarks
	'Minimum' in this case means the combination of the lowest
	value of x, y and z from both vectors. Lowest is taken just
	numerically, not magnitude, so -1 < 0.
	*/
	inline void makeFloor(const Vec2& cmp)
	{
		if (cmp.x < x) x = cmp.x;
		if (cmp.y < y) y = cmp.y;
	}

	/** Sets this vector's components to the maximum of its own and the
	ones of the passed in vector.
	@remarks
	'Maximum' in this case means the combination of the highest
	value of x, y and z from both vectors. Highest is taken just
	numerically, not magnitude, so 1 > -3.
	*/
	void makeCeil(const Vec2& cmp)
	{
		if (cmp.x > x) x = cmp.x;
		if (cmp.y > y) y = cmp.y;
	}

	// arithmetic operations
	inline Vec2 operator + (const Vec2& rkVector) const
	{
		return Vec2(
			x + rkVector.x,
			y + rkVector.y);
	}

	inline Vec2 operator - (const Vec2& rkVector) const
	{
		return Vec2(
			x - rkVector.x,
			y - rkVector.y);
	}

	inline Vec2 operator * (const T fScalar) const
	{
		return Vec2(
			x * fScalar,
			y * fScalar);
	}

	inline Vec2 operator * (const Vec2& rhs) const
	{
		return Vec2(
			x * rhs.x,
			y * rhs.y);
	}

	inline Vec2 operator / (const T fScalar) const
	{
		assert(fScalar != 0.0);

		T fInv = 1.0f / fScalar;

		return Vec2(
			x * fInv,
			y * fInv);
	}

	inline Vec2 operator / (const Vec2& rhs) const
	{
		return Vec2(
			x / rhs.x,
			y / rhs.y);
	}

	inline const Vec2& operator + () const
	{
		return *this;
	}

	inline Vec2 operator - () const
	{
		return Vec2(-x, -y);
	}

	// overloaded operators to help fssVector2
	inline friend Vec2 operator * (const T fScalar, const Vec2& rkVector)
	{
		return Vec2(
			fScalar * rkVector.x,
			fScalar * rkVector.y);
	}

	inline friend Vec2 operator / (const T fScalar, const Vec2& rkVector)
	{
		return Vec2(
			fScalar / rkVector.x,
			fScalar / rkVector.y);
	}

	inline friend Vec2 operator + (const Vec2& lhs, const T rhs)
	{
		return Vec2(
			lhs.x + rhs,
			lhs.y + rhs);
	}

	inline friend Vec2 operator + (const T lhs, const Vec2& rhs)
	{
		return Vec2(
			lhs + rhs.x,
			lhs + rhs.y);
	}

	inline friend Vec2 operator - (const Vec2& lhs, const T rhs)
	{
		return Vec2(
			lhs.x - rhs,
			lhs.y - rhs);
	}

	inline friend Vec2 operator - (const T lhs, const Vec2& rhs)
	{
		return Vec2(
			lhs - rhs.x,
			lhs - rhs.y);
	}
	// arithmetic updates
	inline Vec2& operator += (const Vec2& rkVector)
	{
		x += rkVector.x;
		y += rkVector.y;

		return *this;
	}

	inline Vec2& operator += (const T fScaler)
	{
		x += fScaler;
		y += fScaler;

		return *this;
	}

	inline Vec2& operator -= (const Vec2& rkVector)
	{
		x -= rkVector.x;
		y -= rkVector.y;

		return *this;
	}

	inline Vec2& operator -= (const T fScaler)
	{
		x -= fScaler;
		y -= fScaler;

		return *this;
	}

	inline Vec2& operator *= (const T fScalar)
	{
		x *= fScalar;
		y *= fScalar;

		return *this;
	}

	inline Vec2& operator *= (const Vec2& rkVector)
	{
		x *= rkVector.x;
		y *= rkVector.y;

		return *this;
	}

	inline Vec2& operator /= (const T fScalar)
	{
		assert(fScalar != 0.0);

		T fInv = 1.0f / fScalar;

		x *= fInv;
		y *= fInv;

		return *this;
	}

	inline Vec2& operator /= (const Vec2& rkVector)
	{
		x /= rkVector.x;
		y /= rkVector.y;

		return *this;
	}

	/** Returns the length (magnitude) of the vector.
			@warning
				This operation requires a square root and is expensive in
				terms of CPU operations. If you don't need to know the exact
				length (e.g. for just comparing lengths) use squaredLength()
				instead.
		*/
	inline T length() const
	{
		return sqrt(x * x + y * y);
	}

	/** Returns the square of the length(magnitude) of the vector.
		@remarks
			This  method is for efficiency - calculating the actual
			length of a vector requires a square root, which is expensive
			in terms of the operations required. This method returns the
			square of the length of the vector, i.e. the same as the
			length but before the square root is taken. Use this if you
			want to find the longest / shortest vector without incurring
			the square root.
	*/
	inline T squaredLength() const
	{
		return x * x + y * y;
	}
	/** Returns the distance to another vector.
		@warning
			This operation requires a square root and is expensive in
			terms of CPU operations. If you don't need to know the exact
			distance (e.g. for just comparing distances) use squaredDistance()
			instead.
	*/
	inline T distance(const Vec2& rhs) const
	{
		return (*this - rhs).length();
	}

	static  T Sum(const Vec2& rhs)
	{
		return rhs.x + rhs.y;
	}



	static Vec2 normalize(const Vec2& vec)
	{
		T Length = vec.length();
		if (Length < std::numeric_limits<T>::epsilon())
		{
			return Vec2(0, 0);
		}
		T invLength = 1.0f / Length;
		return Vec2(vec.x * invLength, vec.y*invLength);
	}

	static T dot(const Vec2& v1, const Vec2& v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}
};

using Vec2F = Vec2<float>;
using Vec2D = Vec2<double>;