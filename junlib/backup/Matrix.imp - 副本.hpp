#pragma once

#include <cstring>
using namespace jun;

template<typename T, size_t Ch, typename Alloc>
long long Matrix<T, Ch, Alloc>::allocatedMem = 0;

template<typename T, size_t Ch, typename Alloc> inline
Matrix<T, Ch, Alloc>::Matrix(const Matrix& m)  {
	copy0(m, *this);
}

template<typename T, size_t Ch, typename Alloc> inline
Matrix<T, Ch, Alloc>::Matrix(Matrix&& m) {
	copy0(m, *this);
	--*refcount_;
	m.data_ = nullptr;
}


template<typename T, size_t Ch, typename Alloc> inline
Matrix<T, Ch, Alloc>& Matrix<T, Ch, Alloc>::operator=(const Matrix& m){
	if (this != &m){
		release();
		copy0(m, *this);
	}
	return *this;
}

template<typename T, size_t Ch, typename Alloc> inline
Matrix<T, Ch, Alloc>& Matrix<T, Ch, Alloc>::operator=(Matrix&& m){
	if (this != &m){
		release();
		copy0(m, *this);
		--*ref_count_;
		m.data_ = nullptr;
	}
	return *this;
}


template<typename T, size_t Ch, typename Alloc> inline
void Matrix<T, Ch, Alloc>::copy0(const Matrix& src, Matrix& dest){
	//dest.rows_ = src.rows();
	//dest.cols_ = src.cols();
	//dest.alloc_ = src.get_allocator();
	//dest.refcount_ = src.refcount_;
	//dest.data_ = src.data_;
	//dest.offset_ = src.offset_;
	std::memcpy(&dest, &src, sizeof(Matrix));
	++*dest.refcount_;
}
template<typename T, size_t Ch, typename Alloc>
inline Matrix<T, 1, Alloc> Matrix<T,Ch,Alloc>::channelMatrix(int index){
	if (index >= channels) throw ChannelOutOfBound();

	Matrix<T, 1, Alloc> ret;
	ret.data_[0] = this->data_[index];
	return ret;

}


//// N channel -> 1 channel
//template<typename T, size_t Ch, typename Alloc> 
//template<size_t N> 
//inline Matrix(const Matrx<T, N, Alloc>& m, int channelIndex){
//
//}


//template<typename T, size_t Ch, typename Alloc> inline
//Matrix<T,Ch,Alloc>::Matrix(Matrix&& m, std::array<int, 4> rect){
//
//}





template<typename T, size_t Ch, typename Alloc>
std::ostream& operator<<(std::ostream& out, const jun::Matrix<T, Ch, Alloc>& m){
	out << '[';
	for (int r = 0;; ++r){
		if (r > 0) out << ' ';
		for (int c = 0; c < m.cols(); ++c){
			for (int ch = 0;; ++ch){
				out << m.at(r, c, ch);
				if (ch == m.channels - 1){ if (c < m.cols() - 1) out << ", "; break; }
				else out << ", ";
			}
		}
		if (r < m.rows() - 1)  out << ";\n";
		else{ out << ']'; break; }
	}
	out << endl;
	return out;
}




