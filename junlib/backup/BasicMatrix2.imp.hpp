#pragma once

#include <iomanip>

template<typename T, typename Alloc>
inline void BasicMatrix<T,Alloc>::checkIndex(int rowIndex, int colIndex) const{
	Assert<Index_Out_Of_Range>(!INDEX_CHECK || (rowIndex >= 0 0 && colIndex >= 0 && rowIndex < rows() && colIndex < cols()));
}

template<typename T, typename Alloc>
template<typename InputIt>
inline BasicMatrix<T,Alloc>::BasicMatrix(int rows, int cols, InputIt first, InputIt last, const Alloc& a = Alloc()){
	init(rows, cols, a);
	chPtr p = chIter(0, 0);
	for (size_t i = 0; i < channelNum(); ++i){
		p[i] = static_cast<channelType>(*first);
		if (++first == last) break;
	}
}

template<typename T, typename Alloc>
template<typename InputIt>
inline BasicMatrix<T, Alloc>::BasicMatrix(InputIt first, InputIt last, const Alloc& a = Alloc()){
	init(1, static_cast<int>(last - first)/channels, a);
	chPtr p = chIter(0, 0);
	for (size_t i = 0; i < channelNum(); ++i){
		p[i] = static_cast<channelType>(*first);
		if (++first == last) break;
	}
}

template<typename T, typename Alloc>
inline BasicMatrix<T, Alloc>::BasicMatrix(int rows, int cols, std::initializer_list<double> dl, const Alloc& a = Alloc()){
	init(rows, cols, a);
	chPtr p = chIter(0, 0);
	auto b = dl.begin(), e = dl.end();
	for (size_t i = 0; i < channelNum(); ++i){
		p[i] = static_cast<channelType>(*b);
		if (++b == e) break;
	}
}
template<typename T, typename Alloc>
inline BasicMatrix<T, Alloc>::BasicMatrix(std::initializer_list<double> dl, const Alloc& a = Alloc()){
	init(1, static_cast<int>(dl.size() / channels), a);
	chPtr p = chIter(0, 0);
	auto b = dl.begin(), e = dl.end();
	for (size_t i = 0; i < channelNum(); ++i){
		p[i] = static_cast<channelType>(*b);
		if (++b == e) break;
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
	BasicMatrix<T, Alloc> result(rows(), cols());
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
		auto it = m.elemIter(r, 0);
		for (int c = 0; c < m.cols(); ++c){			
			out << it[c];		
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
		auto it = m.elemIter(r, 0);
		for (int c = 0; c < m.cols(); ++c){
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
		auto it = m.elemIter(r, 0);
		for (int c = 0; c < m.cols(); ++c){
			out << std::setw(5) << (int)it[c];
		}
		if (r < m.rows() - 1)  out << ";\n";
		else{ out << ']'; break; }
	}
	out << endl;
	return out;
}

template<typename Alloc>
std::ostream& operator<<(std::ostream& out, const jun::BasicMatrix<int, Alloc>& m){

	out << '[';
	for (int r = 0;; ++r){
		if (r > 0) out << ' ';
		auto it = m.elemIter(r, 0);
		for (int c = 0; c < m.cols(); ++c){
			out << std::setw(5) << (int)it[c];
		}
		if (r < m.rows() - 1)  out << ";\n";
		else{ out << ']'; break; }
	}
	out << endl;
	return out;
}

template<typename Alloc>
std::ostream& operator<<(std::ostream& out, const jun::BasicMatrix<float, Alloc>& m){

	out << '[';
	for (int r = 0;; ++r){
		if (r > 0) out << ' ';
		auto it = m.elemIter(r, 0);
		for (int c = 0; c < m.cols(); ++c){
			out << std::setw(5) << it[c];
		}
		if (r < m.rows() - 1)  out << ";\n";
		else{ out << ']'; break; }
	}
	out << endl;
	return out;
}

template<typename Alloc>
std::ostream& operator<<(std::ostream& out, const jun::BasicMatrix<double, Alloc>& m){

	out << '[';
	for (int r = 0;; ++r){
		if (r > 0) out << ' ';
		auto it = m.elemIter(r, 0);
		for (int c = 0; c < m.cols(); ++c){
			out << std::setw(5) << it[c];
		}
		if (r < m.rows() - 1)  out << ";\n";
		else{ out << ']'; break; }
	}
	out << endl;
	return out;
}






