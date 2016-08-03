#pragma once

#include <type_traits>
#include <iostream>
#include <cmath>
#include <exception>

namespace jun{

	////////////////////////////////// BasicVec4 //////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	class BasicVec4{
	public:
		typedef T value_type;
		typedef BasicVec4<T> MyType;

		static size_t size() {
			return 4;
		}

		value_type x;
		value_type y;
		value_type z;
		value_type w;

		BasicVec4() :x(0), y(0), z(0),w(0){
			//checkType();
		}
		BasicVec4(value_type x_, value_type y_, value_type z_, value_type w_) :x(x_), y(y_), z(z_), w(w_){
			checkType();
		}




		value_type& operator[](int index) {
			switch (index){
			case 0:return x;
			case 1:return y;
			case 2:return z;
			case 3:return w;
			default:
				throw std::out_of_range("index out of range in vec3");
			}
		}

		const value_type& operator[](int index) const {
			switch (index){
			case 0:return x;
			case 1:return y;
			case 2:return z;
			case 3:return w;
			default:
				throw std::out_of_range("index out of range in vec3");
			}
		}


		//convertor 
		template<typename T2>
		operator BasicVec4<T2>() const {
			return BasicVec4<T2>(static_cast<T2>(x), static_cast<T2>(y), static_cast<T2>(z));
		}

		template<typename T2>
		operator std::array<T2, 4>() const{
			return std::array < T2, 4 > {x, y, z, w};
		}


		MyType& operator+=(MyType a){
			x += a.x; y += a.y; z += a.z;
			return *this;
		}
		MyType& operator-=(MyType a){
			x -= a.x; y -= a.y; z -= a.z;
			return *this;
		}

		MyType& operator*=(double d){
			x *= d; y *= d; z *= d;
			return *this;
		}

		MyType& operator/=(double d){
			x /= d; y /= d; z /= d;
			return *this;
		}

		double norm()const{
			return std::sqrt(x*x + y*y + z*z);
		}

		static double dot(MyType a, MyType b){
			return static_cast<double>((double)a.x*b.x + a.y*b.y + a.z*b.z);
		}

		static MyType cross(MyType a, MyType b){
			return MyType(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
		}


	private:
		void checkType(){
			static_assert(std::is_integral<value_type>::value || std::is_floating_point<value_type>::value,
				"Integer or float point required.");
		}
	};

	////////////////////////////////// BasicVec4 END //////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////

	typedef BasicVec4<int> Vec4i;
	typedef BasicVec4<unsigned char> Vec4b;
	typedef BasicVec4<float> Vec4f;
	typedef BasicVec4<double> Vec4d;


}


template<typename T>
inline std::ostream& operator<<(std::ostream& out, jun::BasicVec4<T> vec){
	out << '{' << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << '}';
	return out;
}

template<>
inline std::ostream& operator<<(std::ostream& out, jun::BasicVec4<unsigned char> vec){
	out << '{' << static_cast<int>(vec.x) << ", " << static_cast<int>(vec.y)
		<< ", " << static_cast<int>(vec.z) << ", " << static_cast<int>(vec.w) << '}';
	return out;
}



template<typename T>
inline bool operator==(jun::BasicVec4<T> left, jun::BasicVec4<T> right){
	return left.x == right.x&&left.y == right.y&&left.z == right.z&&left.w == right.w;
}

template<typename T>
inline bool operator!=(jun::BasicVec4<T> left, jun::BasicVec4<T> right){
	return !(left == right);
}

template<typename T>
inline jun::BasicVec4<T> operator+(jun::BasicVec4<T> a, jun::BasicVec4<T> b){
	return jun::BasicVec4<T>(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

template<typename T>
inline jun::BasicVec4<T> operator-(jun::BasicVec4<T> a, jun::BasicVec4<T> b){
	return jun::BasicVec4<T>(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

template<typename T>
inline jun::BasicVec4<T> operator*(jun::BasicVec4<T> a, double b){
	return jun::BasicVec4<T>(a.x*b, a.y *b, a.z*b, a.w*b);
}

template<typename T>
inline jun::BasicVec4<T> operator*(double b, jun::BasicVec4<T> a){
	return a*b;
}

template<typename T>
inline jun::BasicVec4<T> operator*(jun::BasicVec4<T> a, int b){
	return a*b;
}

template<typename T>
inline jun::BasicVec4<T> operator*(int b, jun::BasicVec4<T> a){
	return a*b;
}
