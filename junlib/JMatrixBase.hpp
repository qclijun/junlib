#pragma once

#include <algorithm>
#include <iomanip>

template<typename T>
jun::JMatrixBase<T>::JMatrixBase(int rows, int cols, int channels)
	:_rows(rows), _cols(cols), _channels(channels){
	_data = new T[channelNum()];
}


template<typename T>
jun::JMatrixBase<T>::JMatrixBase(int rows, int cols, int channels, T val)
	:JMatrixBase(rows,cols,channels){
	std::fill_n(begin(), channelNum(), val);
}

template<typename T> inline
void jun::JMatrixBase<T>::clearAll(jun::Rect rect){
	auto p = iter(rect.y, rect.x);
	const int nbyte = rect.width*channels()*BPC;
	for (int r = 0; r < rect.height; ++r){
		std::memset(p, 0, nbyte);
		p += cpl();
	}
}
template<typename T> inline
void jun::JMatrixBase<T>::setAll(T val){
	if (BPC == sizeof(char)){
		std::memset(begin(), val, bytes());
		return;
	}
	std::fill_n(begin(), channelNum(), val);
}

template<typename T> inline
void jun::JMatrixBase<T>::setAll(Rect rect, T val){
	auto p = iter(rect.y, rect.x);
	const int ch = channels()*rect.width;
	for (int r = 0; r < rect.height; ++r){
		std::fill_n(p, ch, val);
	}
}

template<typename T> inline
void jun::JMatrixBase<T>::setMasked(const BinaryJMatrix& mask, T val){
	auto ps = mask.begin();
	auto pd = begin();
	while (ps != end()){
		if (*ps) *pd = val;
		++ps; ++pd;
	}
}



template<typename T>
std::ostream& operator<<(std::ostream& out, const jun::JMatrixBase<T>& m){
	out << '[';
	for (int r = 0; ; ++r){
		if (r>0) out << ' ';
		auto p = m.iter(r);
		for (int c = 0; c < m.totalCols(); ++c){
			out << std::setw(5) << (int)p[c];
		}
		if (r == m.rows() - 1){
			out << ']';
			break;
		}
		else{
			out << ";\n";
		}
	}
	out << endl;
	return out;
}




