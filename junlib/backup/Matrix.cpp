#include "Matrix.h"
#include "junlib\util.h"


long long Matrix::Mrep::allocatedMem = 0;

inline void Matrix::checkIndex(int rowIndex, int colIndex) const{
	jun::Assert<Index_Out_Of_Range>(!INDEX_CHECK || (rowIndex >= 0  && colIndex >= 0 && rowIndex < rows() && colIndex < cols()));
}


std::ostream& operator<<(std::ostream& out, jun::Matrix& m){
	out << '[';
	for (int r = 0;; ++r){
		if (r > 0) out << ' ';
		auto it = m.ptr(r, 0);
		for (int c = 0; c < m.cols(); ++c){
			out << (int)it[c] << ' ';
		}
		if (r < m.rows() - 1)  out << ";\n";
		else{ out << ']'; break; }
	}
	out << std::endl;
	return out;
}

