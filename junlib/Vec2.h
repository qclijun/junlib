#pragma once

#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <type_traits>
#include <exception>

namespace jun{
	////////////////////////////////// BasicVec2 //////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	class BasicVec2{
	public:
		typedef T value_type;
		typedef BasicVec2<T> MyType;

		static int size(){
			return 2;
		}

		value_type x;
		value_type y;

		BasicVec2() :x(0), y(0){
			//checkType();
		}
		BasicVec2(value_type x_, value_type y_) :x(x_), y(y_){
			checkType();
		}

		value_type& operator[](int index) {
			switch (index){
			case 0:return x;
			case 1:return y;
			default:
				throw std::out_of_range("index out of range in vec2");
			}
		}

		const value_type& operator[](int index) const {
			switch (index){
			case 0:return x;
			case 1:return y;
			default:
				throw std::out_of_range("index out of range in vec2");
			}
		}


		//convertor 
		template<typename T2>
		operator BasicVec2<T2>() const {
			return BasicVec2<T2>(static_cast<T2>(x), static_cast<T2>(y));
		}

		template<typename T2>
		operator std::array<T2, 2>() const{
			return array < T2, 2 > {x, y};
		}


		MyType& operator+=(MyType a){
			x += a.x;
			y += a.y;
			return *this;
		}
		MyType& operator-=(MyType a){
			x -= a.x;
			y -= a.y;
			return *this;
		}

		//MyType& operator*=(int i){
		//	x *= i; y *= i;
		//	return *this;
		//}


		MyType& operator*=(double d){
			x = static_cast<T>(x*d);
			y = static_cast<T>(y*d);
			//x *= d; y *= d;
			return *this;
		}

		MyType& operator/=(double d){
			x = static_cast<T>(x / d);
			y = static_cast<T>(y / d);
			//x /= d; y /= d;
			return *this;
		}

		double norm()const{
			return std::sqrt(x*x + y*y);
		}


		T dot_product(MyType a){
			return (x*a.x + y*a.y);
		}
		T cross_product(MyType a){
			return (x*a.y - y*a.x);
		}


	private:
		void checkType(){
			static_assert(std::is_integral<value_type>::value || std::is_floating_point<value_type>::value,
				"Integer or float point required.");
		}
	};

	////////////////////////////////// BasicVec2 END //////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////

	typedef BasicVec2<int> Vec2i;
	typedef BasicVec2<unsigned char> Vec2b;
	typedef BasicVec2<float> Vec2f;
	typedef BasicVec2<double> Vec2d;










	template<typename T>
	inline bool operator==(const jun::BasicVec2<T>& left, const jun::BasicVec2<T>& right){
		return left.x == right.x&&left.y == right.y;
	}

	template<typename T>
	inline bool operator!=(const jun::BasicVec2<T>& left, const jun::BasicVec2<T>& right){
		return !(left == right);
	}

	template<typename T>
	inline bool operator<(jun::BasicVec2<T> left, jun::BasicVec2<T> right){
		return (left.x < right.x) || ((left.x == right.x) && (left.y < right.y));
	}


	template<typename T>
	inline jun::BasicVec2<T> operator+(jun::BasicVec2<T> a, jun::BasicVec2<T> b){
		return jun::BasicVec2<T>(a.x + b.x, a.y + b.y);
	}

	template<typename T>
	inline jun::BasicVec2<T> operator-(jun::BasicVec2<T> a, jun::BasicVec2<T> b){
		return jun::BasicVec2<T>(a.x - b.x, a.y - b.y);
	}

	template<typename T>
	inline jun::BasicVec2<double> operator*(jun::BasicVec2<T> a, double b){
		return jun::BasicVec2<double>(a.x*b, a.y *b);
	}

	template<typename T>
	inline jun::BasicVec2<double> operator*(double b, jun::BasicVec2<T> a){
		return a*b;
	}

	template<typename T>
	inline jun::BasicVec2<T> operator*(jun::BasicVec2<T> a, int b){
		return jun::BasicVec2<T>(a.x*b, a.y *b);
	}

	template<typename T>
	inline jun::BasicVec2<T> operator*(int b, jun::BasicVec2<T> a){
		return a*b;
	}





}

	template<typename T>
	inline std::ostream& operator<<(std::ostream& out, jun::BasicVec2<T> vec){
		out << '{' << std::setw(4) << vec.x << "," << std::setw(4) << vec.y << '}';
		return out;
	}

	inline std::ostream& operator<<(std::ostream& out, jun::BasicVec2<unsigned char> vec){
		out << '{' << std::setw(4) << static_cast<int>(vec.x) << "," << std::setw(4) << static_cast<int>(vec.y) << '}';
		return out;
	}
