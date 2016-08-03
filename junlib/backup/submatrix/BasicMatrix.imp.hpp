#pragma once

#include <iomanip>

using jun::BasicMatrix;

template<typename T, typename Alloc>
inline void BasicMatrix<T,Alloc>::checkIndex(int rowIndex, int colIndex) const{
	Assert<Index_Out_Of_Range>(!INDEX_CHECK || (rowIndex >= 0 0 && colIndex >= 0 && rowIndex < rows() && colIndex < cols()));
}

template<typename T, typename Alloc>
template<typename InputIt>
inline void BasicMatrix<T, Alloc>::init(InputIt first, InputIt last){
	chPtr p = chIter(0, 0);
	for (size_t i = 0; i < channelNum(); ++i){
		p[i] = static_cast<channelType>(*first);
		if (++first == last) break;
	}
}

template<typename T, typename Alloc>
inline void BasicMatrix<T, Alloc>::init(std::initializer_list<double> dl){
	chPtr p = chIter(0, 0);
	auto b = dl.begin(), e = dl.end();
	for (size_t i = 0; i < channelNum(); ++i){
		p[i] = static_cast<channelType>(*b);
		if (++b == e) break;
	}
}

template<typename T, typename Alloc>
template<typename Func>
inline void BasicMatrix<T, Alloc>::init(Func f){
	chPtr p = chIter(0, 0);
	for (size_t i = 0; i < channelNum(); ++i){
		p[i] = static_cast<channelType>(f());		
	}
}


template<typename T, typename Alloc>
inline void BasicMatrix<T, Alloc>::setAll(double val){
	int nRow = rows();
	int nCol = totalCols();
	if (isContinues()){
		nCol *= nRow;
		nRow = 1;
	}
	for (int r = 0; r < nRow; ++r){
		chPtr p = chIter(r, 0);
		for (int c = 0; c < nCol; ++c){
			p[c] = static_cast<channelType>(val);
		}
	}
}


template<typename T, typename Alloc>
inline BasicMatrix<T,Alloc> BasicMatrix<T, Alloc>::clone() const{
	BasicMatrix<T, Alloc> result(rows(), cols(),channels());
	int nRow = rows();
	size_t nCol = bpl();
	if (isContinues()){
		nCol *= nRow;
		nRow = 1;
	}
	for (int r = 0; r < nRow; ++r){
		std::memcpy(result.chIter(r, 0), this->chIter(r, 0), nCol);
	}

	return result;
}



template<typename T, typename Alloc>
std::ostream& operator<<(std::ostream& out, const jun::BasicMatrix<T, Alloc>& m){	
	out << '[';
	for (int r = 0;; ++r){
		if (r > 0) out << ' ';
		auto it = m.chIter(r, 0);
		for (int c = 0; c < m.totalCols(); ++c){
			out <<  it[c]<<' ';
		}
		if (r < m.rows() - 1)  out << ";\n";
		else{ out << ']'; break; }
	}
	out << endl;
	return out;
}

template<typename Alloc>
std::ostream& operator<<(std::ostream& out, const jun::BasicMatrix<uchar, Alloc>& m){

	out << '[';
	for (int r = 0;; ++r){
		if (r > 0) out << ' ';
		auto it = m.chIter(r, 0);
		for (int c = 0; c < m.totalCols(); ++c){
			out << std::setw(5)<<(int)it[c];
		}
		if (r < m.rows() - 1)  out << ";\n";
		else{ out << ']'; break; }
	}
	out << endl;
	return out;
}

template<typename Alloc>
std::ostream& operator<<(std::ostream& out, const jun::BasicMatrix<char, Alloc>& m){

	out << '[';
	for (int r = 0;; ++r){
		if (r > 0) out << ' ';
		auto it = m.chIter(r, 0);
		for (int c = 0; c < m.totalCols(); ++c){
			out << std::setw(5) << (int)it[c];
		}
		if (r < m.rows() - 1)  out << ";\n";
		else{ out << ']'; break; }
	}
	out << endl;
	return out;
}






