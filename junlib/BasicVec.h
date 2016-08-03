#include <array>

namespace jun{
	///////////////////////////////////////////////// BasicVec /////////////////////////////////////////////

	template<typename T, size_t N>
	class BasicVec : public std::array < T, N > {
	public:
		typedef std::array<T, N> MyBase;
		typedef typename MyBase::value_type value_type;
		typedef typename value_type channel_type;
		enum {
			channels = N,
			depth = sizeof(value_type) * 8,
		};


		BasicVec(const array<T, N>& arr) :MyBase(arr){
		}

		BasicVec(std::initializer_list<T> l) {
			std::copy_n(l.begin(), N, begin());
		}

		template<typename T2>
		BasicVec(const BasicVec<T2, N>& other){
			auto it = begin();
			for (int i = 0; i < N; ++i, ++it){
				*it = static_cast<T>(other[i]);
			}
		}


		BasicVec() :array(){}

		double norm() const{
			double res = 0.0;
			for (int i = 0; i < N; ++i){
				res += (double)a[i] * (double)a[i];
			}
			return std::sqrt(res);
		}

	};



	template<typename T, size_t N>
	inline BasicVec<T, N>& operator+=(BasicVec<T, N>& a, const BasicVec<T, N>& b){
		for (int i = 0; i < N; ++i){
			a[i] += b[i];
		}
		return a;
	}

	template<typename T, size_t N>
	inline BasicVec<T, N>& operator-=(BasicVec<T, N>& a, const BasicVec<T, N>& b){
		for (int i = 0; i < N; ++i){
			a[i] -= b[i];
		}
		return a;
	}

	template<typename T, size_t N>
	inline BasicVec<T, N>& operator*=(BasicVec<T, N>& a, double d){
		for (int i = 0; i < N; ++i){
			a[i] = static_cast<T>((double)a[i] * d);
		}
		return a;
	}

	template<typename T, size_t N>
	inline BasicVec<T, N>& operator/=(BasicVec<T, N>& a, const BasicVec<T, N>& b){
		for (int i = 0; i < a.size()++i){
			a[i] = static_cast<T>((double)a[i] / d);
		}
		return a;
	}


}