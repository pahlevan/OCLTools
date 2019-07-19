#pragma once

#include "fssVec2.h"

template <class T>
class fssBBox
{
public:
	fssVec2D		m_Min;
	fssVec2D		m_Max;

public:
	fssBBox(void){};
	fssBBox(const fssVec2D& _Min ,const fssVec2D& _Max):m_Min(_Min),m_Max(_Max){};
	fssBBox(T minX,T minY,T maxX,T maxY):m_Min(minX,minY),m_Max(maxX,maxY){};



	T x() const { return m_Min.x; }
	T y() const { return m_Min.y; }



	T width() const { return m_Max.x- m_Min.x; }
	T height() const { return m_Max.y-m_Min.y; }


	/** Returns whether or not this box intersects another. */
	bool isOverlapWithBBox(const fssBBox& BBox) const
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
	bool intersection(const fssBBox& b2, fssBBox& retVal) const
	{

		fssVec2D intMin = m_Min;
		fssVec2D intMax = m_Max;

		intMin.makeCeil(b2.getMin());

		intMax.makeFloor(b2.getMax());

		// Check intersection isn't null
		if (intMin.x < intMax.x &&	intMin.y < intMax.y )
		{
			 retVal = fssBBox(intMin, intMax);
			 return true;
		}

		return false;
	}

	/// Gets the center of the box
	fssVec2D getCenter(void) const
	{
		return fssVec2D((m_Max.x + m_Min.x) * T(0.5),(m_Max.y + m_Min.y) * T(0.5));
	}

	/// Gets the size of the box
	fssVec2D getSize(void) const
	{
		return m_Max - m_Min;
	}


	/// Gets the half-size of the box
	fssVec2D getHalfSize(void) const
	{
		return (m_Max - m_Min) * T(0.5);
	}


	// Tests whether the given point contained by this box.
	bool contains(const fssVec2D& v) const
	{
		return m_Min.x <= v.x && v.x <= m_Max.x && m_Min.y <= v.y && v.y <= m_Max.y;
	}

	// Tests whether another box contained by this box.
	bool contains(const fssBBox& other) const
	{
		return this->m_Min.x <= other.m_Min.x &&
			this->m_Min.y <= other.m_Min.y &&
			other.m_Max.x <= this->m_Max.x &&
			other.m_Max.y <= this->m_Max.y;
	}

	// Tests 2 boxes for equality
	bool operator== (const fssBBox& rhs) const
	{
		return this->m_Min == rhs.m_Min && this->m_Max == rhs.m_Max;
	}

	// Tests 2 boxes for inequality.
	bool operator!= (const fssBBox& rhs) const
	{
		return !(*this == rhs);
	}


	// Gets the minimum corner of the box.
	inline const fssVec2D& getMin(void) const
	{ 
		return m_Min; 
	}

	// Gets a modifiable version of the minimum corner of the box.
	inline fssVec2D& getMin(void)
	{ 
		return m_Min; 
	}


	// Gets the maximum corner of the box.
	inline const fssVec2D& getMax(void) const
	{ 
		return m_Max;
	}

	// Gets a modifiable version of the maximum corner of the box.

	inline fssVec2D& getMax(void)
	{ 
		return m_Max;
	}

	/** Merges the passed in box into the current box. The result is the
	box which encompasses both.
	*/
	void merge( const fssBBox<T>& rhs )
	{
		m_Max.makeCeil(rhs.m_Max);
		m_Min.makeFloor(rhs.m_Min);
	}

};

typedef  fssBBox<double> fssBBoxD;