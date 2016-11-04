#pragma once
#include "Vec.h"


namespace jun{

	//////////////////////////////////// BasicPoint /////////////////////////////////
	template<typename T>
	class BasicPoint2 : public BasicVec2 < T > {

	public:
		typedef BasicVec2<T> baseType;

		BasicPoint2() :baseType(0, 0){}

		BasicPoint2(T x , T y ) :baseType(x, y){}

		BasicPoint2(BasicVec2<T> v) :baseType(v){}
	};

	//typedef BasicPoint2<int> Point;
	using Point = BasicPoint2 < int > ;
	typedef BasicPoint2<short> PointS;
	typedef BasicPoint2<int> PointI;
	typedef BasicPoint2<float> PointF;
	typedef BasicPoint2<double> PointD;



	template<typename T>
	class BasicPoint3 : public BasicVec3 < T > {

	public:
		typedef BasicVec3<T> baseType;

		BasicPoint3(T x, T y, T z) :baseType(x, y, z){}
		BasicPoint3() :baseType(0, 0, 0){}

		BasicPoint3(BasicVec3<T> v) :baseType(v){}
	};

	typedef BasicPoint3<int> Point3;
	typedef BasicPoint3<int> Point3I;
	typedef BasicPoint3<float> Point3F;
	typedef BasicPoint3<double> Point3D;

	template<typename T>
	class BasicSize {

	public:
		BasicSize() :width(0), height(0){}

		BasicSize(T sz) :width(sz), height(sz){}

		BasicSize(T w , T h) :width(w), height(h){}
		T width;
		T height;
	};

	typedef BasicSize<int> Size;
	typedef BasicSize<short> SizeS;
	typedef BasicSize<int> SizeI;
	typedef BasicSize<float> SizeF;
	typedef BasicSize<double> SizeD;

}


template<typename T>
inline std::ostream& operator<<(std::ostream& out, jun::BasicPoint2<T> vec){
	out << '{' << std::setw(4) << vec.x << "," << std::setw(4) << vec.y << '}';
	return out;
}
