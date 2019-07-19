#pragma once

#include <string>
#include <cassert>

template <class T>
class Vec3
{

public:

	T	x;
	T	y;
	T	z;

	Vec3(void): x(T(0)),y(T(0)),z(T(0)){}
	Vec3(T _x,T _y,T _z):x(_x),y(_y),z(_z){}


	/** Exchange the contents of this vector with another. 
	*/
	inline void swap(Vec3& other)
	{
		std::swap(x, other.x);
		std::swap(y, other.y);
		std::swap(z, other.z);
	}


	/** Assigns the value of the other vector.
	@param
	rkVector The other vector
	*/
	inline Vec3& operator = ( const Vec3& rkVector )
	{
		x = rkVector.x;
		y = rkVector.y;
		z = rkVector.z;

		return *this;
	}

	inline Vec3& operator = ( const T fScalar)
	{
		x = fScalar;
		y = fScalar;
		z = fScalar;

		return *this;
	}

	inline bool operator == ( const Vec3& rkVector ) const
	{
		return ( x == rkVector.x && y == rkVector.y && z == rkVector.z );
	}

	inline bool operator != ( const Vec3& rkVector ) const
	{
		return ( x != rkVector.x || y != rkVector.y || z != rkVector.z  );
	}

	T length()const
	{
		return sqrt( x * x + y * y + z * z );
	}

	T normalise()
	{
		T fLength = sqrt( x * x + y * y + z * z );

		// Will also work for zero-sized vectors, but will change nothing
		if ( fLength > 1e-08 )
		{
			T fInvLength = 1 / fLength;
			x *= fInvLength;
			y *= fInvLength;
			z *= fInvLength;
		}
		return fLength;
	}

	Vec3<T> get_normalise() const
	{
		T fLength = sqrt( x * x + y * y + z * z );

		// Will also work for zero-sized vectors, but will change nothing
		if ( fLength > 1e-08 )
		{
			T fInvLength = 1 / fLength;
			return Vec3<T>(x * fInvLength,y * fInvLength,z * fInvLength);
		}
		return Vec3<T>(0,0,0);
	}


	void abs()
	{
		x = ::abs(x);
		y = ::abs(y);
		z = ::abs(z);
	}



	//---------------------------------------------------------------------------------------------------

	// arithmetic operations
	inline Vec3 operator + ( const Vec3& rkVector ) const
	{
		return Vec3(
			x + rkVector.x,
			y + rkVector.y,
			z + rkVector.z);
	}

	inline Vec3 operator - ( const Vec3& rkVector ) const
	{
		return Vec3(
			x - rkVector.x,
			y - rkVector.y,
			z - rkVector.z);
	}

	inline Vec3 operator * ( const T fScalar ) const
	{
		return Vec3(
			x * fScalar,
			y * fScalar,
			z * fScalar);
	}

	inline Vec3 operator * ( const Vec3& rhs) const
	{
		return Vec3(
			x * rhs.x,
			y * rhs.y,
			z * rhs.z);
	}

	inline Vec3 operator / ( const T fScalar ) const
	{
		assert( fScalar != 0.0 );

		T fInv = 1.0f / fScalar;

		return Vec3(
			x * fInv,
			y * fInv,
			z * fInv);
	}

	inline Vec3 operator / ( const Vec3& rhs) const
	{
		return Vec3(
			x / rhs.x,
			y / rhs.y,
			z / rhs.z);
	}

	inline const Vec3& operator + () const
	{
		return *this;
	}

	inline Vec3 operator - () const
	{
		return Vec3(-x, -y,-z);
	}




	// overloaded operators to help fssVector3
	inline friend Vec3 operator * ( const T fScalar, const Vec3& rkVector )
	{
		return Vec3(
			fScalar * rkVector.x,
			fScalar * rkVector.y,
			fScalar * rkVector.z);
	}

	inline friend Vec3 operator / ( const T fScalar, const Vec3& rkVector )
	{
		return Vec3(
			fScalar / rkVector.x,
			fScalar / rkVector.y,
			fScalar / rkVector.z);
	}

	inline friend Vec3 operator + (const Vec3& lhs, const T rhs)
	{
		return Vec3(
			lhs.x + rhs,
			lhs.y + rhs,
			lhs.z + rhs);
	}

	inline friend Vec3 operator + (const T lhs, const Vec3& rhs)
	{
		return Vec3(
			lhs + rhs.x,
			lhs + rhs.y,
			lhs + rhs.z);
	}

	inline friend Vec3 operator - (const Vec3& lhs, const T rhs)
	{
		return Vec3(
			lhs.x - rhs,
			lhs.y - rhs,
			lhs.z - rhs);
	}

	inline friend Vec3 operator - (const T lhs, const Vec3& rhs)
	{
		return Vec3(
			lhs - rhs.x,
			lhs - rhs.y,
			lhs - rhs.z);
	}
	// arithmetic updates
	inline Vec3& operator += ( const Vec3& rkVector )
	{
		x += rkVector.x;
		y += rkVector.y;
		z += rkVector.z;

		return *this;
	}

	inline Vec3& operator += ( const T fScaler )
	{
		x += fScaler;
		y += fScaler;
		z += fScaler;

		return *this;
	}

	inline Vec3& operator -= ( const Vec3& rkVector )
	{
		x -= rkVector.x;
		y -= rkVector.y;
		z -= rkVector.z;

		return *this;
	}

	inline Vec3& operator -= ( const T fScaler )
	{
		x -= fScaler;
		y -= fScaler;
		z -= fScaler;

		return *this;
	}

	inline Vec3& operator *= ( const T fScalar )
	{
		x *= fScalar;
		y *= fScalar;
		z *= fScalar;

		return *this;
	}

	inline Vec3& operator *= ( const Vec3& rkVector )
	{
		x *= rkVector.x;
		y *= rkVector.y;
		z *= rkVector.z;

		return *this;
	}

	inline Vec3& operator /= ( const T fScalar )
	{
		assert( fScalar != 0.0 );

		T fInv = 1.0f / fScalar;

		x *= fInv;
		y *= fInv;
		z *= fInv;

		return *this;
	}

	inline Vec3& operator /= ( const Vec3& rkVector )
	{
		x /= rkVector.x;
		y /= rkVector.y;
		z /= rkVector.z;

		return *this;
	}


	template<class T>
	static Vec3<T> fromHexString(const std::string& strVal)
	{
		assert( strVal.size() == 24 );

		return Vec3<T>(
			utils::fromHexFloat(strVal.substr(0,8)),
			utils::fromHexFloat(strVal.substr(8,8)),
			utils::fromHexFloat(strVal.substr(16,8))
			);
	}


};





using Vec3D =  Vec3<double>;