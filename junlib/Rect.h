#pragma once

#include <iostream>
#include <algorithm>

#include "Point.h"


namespace jun{
	template<typename T>
	inline T  mindiff(T t){ return static_cast<T>(1); }
	inline int mindiff(int){ return 1; }
	inline float mindiff(float){ return std::nextafter(0.0f, 1.f); }
	inline double mindiff(double){ return std::nextafter(0, 1.0); }

	


	//////////////////////////////////// Rect /////////////////////////////////
	template<typename T>
	struct BasicRect{
		typedef BasicRect<T> Self;

	public:
		BasicRect() :x(0), y(0), width(0), height(0){}

		BasicRect(T x_, T y_, T width_, T height_)
			:x(x_), y(y_), width(width_), height(height_){}

		BasicRect(BasicPoint2<T> pt, BasicSize<T> sz){
			x = pt.x, y = pt.y;
			width = sz.width;
			height = sz.height;
		}

		BasicRect(BasicPoint2<T> pt1, BasicPoint2<T> pt2){
			int xmin, ymin, xmax, ymax;
			T es = mindiff(T());
			if (pt1.x < pt2.x) {
				xmin = pt1.x; xmax = pt2.x + es;
			}
			else{
				xmin = pt2.x; xmax = pt1.x + es;
			}
			if (pt1.y < pt2.y){
				ymin = pt1.y; ymax = pt2.y + es;
			}
			else{
				ymin = pt2.y; ymax = pt1.y + es;
			}
			x = xmin; y = ymin; 
			width = xmax - xmin;
			height = ymax - ymin;
		}

		BasicPoint2<T> UL() const{
			return{ x, y };
		}

		BasicPoint2<T> UR() const{
			return{ x + width - mindiff(T()), y };
		}

		//lower left
		BasicPoint2<T> LL() const{
			return{ x, y + height - mindiff(T()) };
		}

		//lower right
		BasicPoint2<T> LR() const{
			return{ x + width - mindiff(T()), y + height - mindiff(T()) };
		}

		T left() const{ return x; }
		T top() const{ return y; }


		T xlast() const {
			return x + width;
		}

		T right() const{
			return x + width - mindiff(T());
		}


		T ylast()  const{
			return y + height;
		}

		T bottom() const{
			return y + height - mindiff(T());
		}



		BasicSize<T> size() const{
			return BasicSize<T>(width, height);
		}


		template<typename T2>
		bool contains(BasicPoint2<T2> p) const{
			return x <= p.x && p.x < xlast() && y <= p.y && p.y < ylast();
		}

		template<typename T2>
		bool contains(BasicRect<T2> rect) const{
			return  x <= rect.x && y <= rect.y && rect.xlast() <= xlast() && rect.ylast() <= ylast();
		}

		operator bool() const{
			return height == 0;
		}

		template<typename T2>
		operator BasicRect<T2>() const{
			return{ x, y, width, height };
		}


		BasicRect<T> overlap(BasicRect<T> rect) const{
			if (x >= rect.xlast() || xlast() <= rect.x) return{};
			if (y >= rect.ylast() || ylast() <= rect.y) return{};
			int xmin, xmax, ymin, ymax;
			if (x < rect.xlast()){
				xmin = x; xmax = rect.xlast();
			}
			else{
				xmin = rect.x; xmax = xlast();
			}
			if (y < rect.ylast()){
				ymin = y; ymax = rect.ylast();
			}
			else{
				ymin = rect.y; ymax = ylast();
			}
			return{ xmin, ymin, xmax - xmin, ymax - ymin };

		}

		//负值表示间隙
		int overlap_x(BasicRect<T> rect){
			return std::min(xlast(), rect.xlast()) - std::max(x, rect.x);
		}

		int overlap_y(BasicRect<T> rect){
			return std::min(ylast(), rect.ylast()) - std::max(y, rect.y);
		}


		//union a point
		Self& operator+=(BasicPoint2<T> pt){
			T es = mindiff(T());
			if (pt.x < x) x = pt.x;
			if (pt.y < y) y = pt.y;
			if (pt.x >= xlast()) width = pt.x + es - x;
			if (pt.y >= ylast()) height = pt.y + es - y;
			return *this;
		}

		//union another rect
		Self& operator+=(Self other){
			if (other.x < x) x = other.x;
			if (other.y < y) y = other.y;
			if (other.xlast() > xlast()) width = other.xlast() - x;
			if (other.ylast() > ylast()) height = other.ylast() - y;
			return *this;
		}

		int compare(Self right) const{
			auto result = x - right.x;
			if (result != 0) return result<0?-1:1 ;
			result = xlast() - right.xlast();
			if (result != 0) return result<0 ? -1 : 1;
			result = y - right.y;
			if (result != 0) return result<0 ? -1 : 1;
			return (ylast() - right.ylast()) < 0 ? -1 : 1;
		}

		T area() const{
			return width*height;
		}


	public:
		T x, y;
		T width, height;


	};



	typedef BasicRect<int> Rect;
	typedef BasicRect<short> RectS;
	typedef BasicRect<int> RectI;
	typedef BasicRect<float> RectF;
	typedef BasicRect<double> RectD;

}




template<typename T>
inline std::ostream& operator<<(std::ostream& out, jun::BasicRect<T> rect){
	out << '{' << '(' << rect.x << ", " << rect.y << ") " << rect.width << 'x' << rect.height << '}';
	return out;
}



template<typename T>
inline bool operator==(jun::BasicRect<T> left, jun::BasicRect<T> right){
	return left.x == right.x&&left.y == right.y&&left.width == right.width&&left.height == right.height;
}

template<typename T>
inline bool operator!=(jun::BasicRect<T> left, jun::BasicRect<T> right){
	return !(left == right);
}

template<typename T>
inline bool operator<(jun::BasicRect<T> left, jun::BasicRect<T> right){
	return left.compare(right) < 0;
}

