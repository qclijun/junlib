#pragma once

#include <cstring>
using namespace jun;

template<typename T, typename Alloc>
long long Matrix<T, Alloc>::allocatedMem = 0;

template<typename T, typename Alloc>
inline Matrix<T, Alloc>::Matrix(const Matrix& m)  {
	copy0(m, *this);
}

//template<typename T, typename Alloc>
//inline Matrix<T, Alloc>::Matrix(Matrix&& m) {
//	copy0(m, *this);
//	--*refcount_;
//	m.data_ = nullptr;
//}


template<typename T, typename Alloc>
inline Matrix<T,  Alloc>& Matrix<T,  Alloc>::operator=(const Matrix& m){
	if (this != &m){
		release();
		copy0(m, *this);
	}
	return *this;
}

//template<typename T,typename Alloc> inline
//Matrix<T, Alloc>& Matrix<T,Alloc>::operator=(Matrix&& m){
//	if (this != &m){
//		release();
//		copy0(m, *this);
//		--*refcount_;
//		m.data_ = nullptr;
//	}
//	return *this;
//}


template<typename T, typename Alloc> inline
void Matrix<T, Alloc>::copy0(const Matrix& src, Matrix& dest){
	std::memcpy(&dest, &src, sizeof(Matrix));
	++*dest.refcount_;
}



//template<typename T, size_t Ch, typename Alloc> inline
//Matrix<T,Ch,Alloc>::Matrix(Matrix&& m, std::array<int, 4> rect){
//
//}


template<typename T, typename Alloc>
inline void Matrix<T,Alloc>::checkIndex(int rowIndex, int colIndex) const{
	int startRow = toRowIndex(offset_);
	int startCol = toColIndex(offset_);
	if (rowIndex < 0 || colIndex < 0 || rowIndex + startRow >= rows() || colIndex + startCol >= cols())
		throw out_of_range();
}




template<typename T, typename Alloc>
std::ostream& operator<<(std::ostream& out, const jun::Matrix<T, Alloc>& m){
	out << '[';
	for (int r = 0;; ++r){
		if (r > 0) out << ' ';
		for (int c = 0; c < m.cols(); ++c){
			for (int ch = 0;; ++ch){
				out << m.at(r, c, ch);
				if (ch == m.channels() - 1){ if (c < m.cols() - 1) out << ", "; break; }
				else out << ", ";
			}
		}
		if (r < m.rows() - 1)  out << ";\n";
		else{ out << ']'; break; }
	}
	out << endl;
	return out;
}




