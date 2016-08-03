#pragma once

#include <type_traits>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <exception>

namespace jun{

	////////////////////////////////// BasicVec3 //////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	class BasicVec3{
	public:
		typedef T value_type;
		typedef BasicVec3<T> MyType;
		typedef T channelType;

		static const int channels = 3;

		static size_t size() {
			return 3;
		}

		value_type x;
		value_type y;
		value_type z;

		BasicVec3() :x(0), y(0),z(0){
			//checkType();
		}
		BasicVec3(value_type x_, value_type y_, value_type z_ = 0) :x(x_), y(y_), z(z_){
			checkType();
		}

		value_type& operator[](int index) {
			switch (index){
			case 0:return x;
			case 1:return y;
			case 2:return z;
			default:
				throw std::out_of_range("index out of range in vec3");
			}
		}

		const value_type& operator[](int index) const {
			switch (index){
			case 0:return x;
			case 1:return y;
			case 2:return z;
			default:
				throw std::out_of_range("index out of range in vec3");
			}
		}


		//convertor 
		template<typename T2>
		operator BasicVec3<T2>() const {
			return BasicVec3<T2>(static_cast<T2>(x), static_cast<T2>(y),static_cast<T2>(z));
		}

		template<typename T2>
		operator std::array<T2, 3>() const{
			return array < T2, 3 > {x, y, z};
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

	////////////////////////////////// BasicVec3 END //////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////

	typedef BasicVec3<int> Vec3i;
	typedef BasicVec3<unsigned char> Vec3b;
	typedef BasicVec3<float> Vec3f;
	typedef BasicVec3<double> Vec3d;


}


template<typename T>
inline std::ostream& operator<<(std::ostream& out, jun::BasicVec3<T> vec){
	out << '{' << std::setw(4) << vec.x << ", " << std::setw(4) << vec.y << ", " << std::setw(4) << vec.z << '}';
	return out;
}

template<>
inline std::ostream& operator<<(std::ostream& out, jun::BasicVec3<unsigned char> vec){
	out << '{' << std::setw(4) << static_cast<int>(vec.x) << ", " << std::setw(4) << static_cast<int>(vec.y)
		<< ", " << std::setw(4) << static_cast<int>(vec.z) << '}';
	return out;
}



template<typename T>
inline bool operator==(jun::BasicVec3<T> left, jun::BasicVec3<T> right){
	return left.x == right.x&&left.y == right.y&&left.z==right.z;
}

template<typename T>
inline bool operator!=(jun::BasicVec3<T> left, jun::BasicVec3<T> right){
	return !(left == right);
}

template<typename T>
inline jun::BasicVec3<T> operator+(jun::BasicVec3<T> a, jun::BasicVec3<T> b){
	return jun::BasicVec3<T>(a.x + b.x, a.y + b.y,a.z+b.z);
}

template<typename T>
inline jun::BasicVec3<T> operator-(jun::BasicVec3<T> a, jun::BasicVec3<T> b){
	return jun::BasicVec3<T>(a.x - b.x, a.y - b.y,a.z-b.z);
}

template<typename T>
inline jun::BasicVec3<T> operator*(jun::BasicVec3<T> a, double b){
	return jun::BasicVec3<T>(a.x*b, a.y *b,a.z*b);
}

template<typename T>
inline jun::BasicVec3<T> operator*(double b, jun::BasicVec3<T> a){
	return jun::BasicVec3<T>(a.x*b, a.y *b,a.z*b);
}

template<typename T>
inline jun::BasicVec3<T> operator*(jun::BasicVec3<T> a, int b){
	return jun::BasicVec3<T>(a.x*b, a.y *b,a.z*b);
}

template<typename T>
inline jun::BasicVec3<T> operator*(int b, jun::BasicVec3<T> a){
	return jun::BasicVec3<T>(a.x*b, a.y *b,a.z*b);
}
