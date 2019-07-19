#pragma once


template <class T>
class Vec4

{
public:

	T	x;
	T	y;
	T	z;
	T	w;

	Vec4(void)				: x(T(0)),y(T(0)),z(T(0)),w(T(0))	{}
	Vec4(T _x,T _y,T _z,T _w)	: x(_x)  ,y(_y)  ,z(_z)  ,w(_w)		{}


	/** Assigns the value of the other vector.
	@param
	rkVector The other vector
	*/
	inline Vec4& operator = ( const Vec4& rkVector )
	{
		x = rkVector.x;
		y = rkVector.y;
		z = rkVector.z;
		w = rkVector.w;

		return *this;
	}

	inline Vec4& operator = ( const T fScalar)
	{
		x = fScalar;
		y = fScalar;
		z = fScalar;
		w = fScalar;

		return *this;
	}

	// arithmetic operations
	inline Vec4 operator + ( const Vec4& rkVector ) const
	{
		return Vec4(
			x + rkVector.x,
			y + rkVector.y,
			z + rkVector.z,
			w + rkVector.w);
	}

	inline bool operator == ( const Vec4& rkVector ) const
	{
		return ( x == rkVector.x && y == rkVector.y && z == rkVector.z && w == rkVector.w );
	}

	inline bool operator != ( const Vec4& rkVector ) const
	{
		return ( x != rkVector.x || y != rkVector.y || z != rkVector.z || w != rkVector.w  );
	}

	T normalise()
	{
		T fLength = sqrt( x * x + y * y + z * z + w * w );

		// Will also work for zero-sized vectors, but will change nothing
		if ( fLength > 1e-08 )
		{
			T fInvLength = 1.0 / fLength;
			x *= fInvLength;
			y *= fInvLength;
			z *= fInvLength;
			w *= fInvLength;
		}
		return fLength;
	}

};


typedef  Vec4<double>  Vec4D;
typedef  Vec4<float>  Vec4F;