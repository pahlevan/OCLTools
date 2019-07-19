#pragma once

#include "Vec2.h"

template <class T>
class BBox
{
public:
	Vec2D		m_Min;
	Vec2D		m_Max;

public:
	BBox(void){};
	BBox(const Vec2D& _Min ,const Vec2D& _Max):m_Min(_Min),m_Max(_Max){};
	BBox(T minX,T minY,T maxX,T maxY):m_Min(minX,minY),m_Max(maxX,maxY){};



	T x() const { return m_Min.x; }
	T y() const { return m_Min.y; }



	T width() const { return m_Max.x- m_Min.x; }
	T height() const { return m_Max.y-m_Min.y; }


	/** Returns whether or not this box intersects another. */
	bool isOverlapWithBBox(const BBox& BBox) const
	{
		if (m_Max.x < BBox.m_Min.x)
			return false;
		if (m_Max.y < BBox.m_Min.y)
			return false;

		if (m_Min.x > BBox.m_Max.x)
			return false;
		if (m_Min.y > BBox.m_Max.y)
			return false;

		return true;
	}

	/// Calculate the area of intersection of this box and another
	bool intersection(const BBox& b2, BBox& retVal) const
	{

		Vec2D intMin = m_Min;
		Vec2D intMax = m_Max;

		intMin.makeCeil(b2.getMin());

		intMax.makeFloor(b2.getMax());

		// Check intersection isn't null
		if (intMin.x < intMax.x &&	intMin.y < intMax.y )
		{
			 retVal = BBox(intMin, intMax);
			 return true;
		}

		return false;
	}

	/// Gets the center of the box
	Vec2D getCenter(void) const
	{
		return Vec2D((m_Max.x + m_Min.x) * T(0.5),(m_Max.y + m_Min.y) * T(0.5));
	}

	/// Gets the size of the box
	Vec2D getSize(void) const
	{
		return m_Max - m_Min;
	}


	/// Gets the half-size of the box
	Vec2D getHalfSize(void) const
	{
		return (m_Max - m_Min) * T(0.5);
	}


	// Tests whether the given point contained by this box.
	bool contains(const Vec2D& v) const
	{
		return m_Min.x <= v.x && v.x <= m_Max.x && m_Min.y <= v.y && v.y <= m_Max.y;
	}

	// Tests whether another box contained by this box.
	bool contains(const BBox& other) const
	{
		return this->m_Min.x <= other.m_Min.x &&
			this->m_Min.y <= other.m_Min.y &&
			other.m_Max.x <= this->m_Max.x &&
			other.m_Max.y <= this->m_Max.y;
	}

	// Tests 2 boxes for equality
	bool operator== (const BBox& rhs) const
	{
		return this->m_Min == rhs.m_Min && this->m_Max == rhs.m_Max;
	}

	// Tests 2 boxes for inequality.
	bool operator!= (const BBox& rhs) const
	{
		return !(*this == rhs);
	}


	// Gets the minimum corner of the box.
	inline const Vec2D& getMin(void) const
	{ 
		return m_Min; 
	}

	// Gets a modifiable version of the minimum corner of the box.
	inline Vec2D& getMin(void)
	{ 
		return m_Min; 
	}


	// Gets the maximum corner of the box.
	inline const Vec2D& getMax(void) const
	{ 
		return m_Max;
	}

	// Gets a modifiable version of the maximum corner of the box.

	inline Vec2D& getMax(void)
	{ 
		return m_Max;
	}

	/** Merges the passed in box into the current box. The result is the
	box which encompasses both.
	*/
	void merge( const BBox<T>& rhs )
	{
		m_Max.makeCeil(rhs.m_Max);
		m_Min.makeFloor(rhs.m_Min);
	}

};

using BBoxF = BBox<float>;
using BBoxD = BBox<double>;