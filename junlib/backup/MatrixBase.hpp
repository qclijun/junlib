#include "MatrixBase.h"
#include <algorithm>
#include <iomanip>
using namespace jun;

template<typename T,typename word_type>
MatrixBase<T,word_type>::MatrixBase(int rows, int cols, int channels)
	:_rows(rows), _cols(cols), _channels(channels),
	_wpl((cols*channels - 1) / CPW + 1),
	_data(new word_type[words()]){

}


template<typename T, typename word_type>
MatrixBase<T, word_type>::MatrixBase(int rows, int cols, int channels, T val)
	:MatrixBase(rows,cols,channels){
	std::fill_n(begin(), words()*CPW, val);
}


template<typename T, typename word_type>
std::ostream& operator<<(std::ostream& out, const MatrixBase<T,word_type>& m){
	out << '[';
	for (int r = 0; ; ++r){
		if (r>0) out << ' ';
		auto p = m.iter(r, 0);
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


