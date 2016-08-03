#include <algorithm>
#include <iomanip>

using namespace jun;

template<typename T>
JMatrixBase<T>::JMatrixBase(int rows, int cols, int channels)
	:_rows(rows), _cols(cols), _channels(channels),
	_cpl(cols*channels),_offset(0){
	_data = new T[channelNum()];
	_refcount = new int(1);
}


template<typename T>
JMatrixBase<T>::JMatrixBase(int rows, int cols, int channels, T val)
	:JMatrixBase(rows,cols,channels){
	std::fill_n(begin(), channelNum(), val);
}

template<typename T>
inline void JMatrixBase<T>::clearAll(){
	int nbyte = bpl();
	int nRow = rows();
	if (isContinues()){
		nbyte *= nRow;
		nRow = 1;
	}
	for (int r = 0; r < nRow; ++r){
		std::memset(iter(r), 0, nbyte);
	}
}

template<typename T>
inline bool JMatrixBase<T>::isEmpty() const{
	int nbyte = bpl();
	int nRow = rows();
	if (isContinues()){
		nbyte *= nRow;
		nRow = 1;
	}
	for (int r = 0; r < nRow; ++r){
		jun::bitany(iter(r), nbyte);
	}
}


template<typename T>
void JMatrixBase<T>::matrixOp(const JMatrixBase<T>& src,MATRIX_OP op ,Point anchor){
	int dx = anchor.x, dy = anchor.y;
	int sx = 0, sy = 0;
	if (dx < 0){
		sx = -dx;
		dx = 0;
	}
	if (dy < 0){
		sy = -dy;
		dy = 0;
	}
	int w = std::min(cols() - dx, src.cols() - sx);
	int h = std::min(rows() - dy, src.rows() - sy);
	auto ps = src.iter(sy, sx);
	auto pd = iter(dy, dx);
	switch (op){
	case MATRIX_OP::SANDD:

		break;
	}
}


template<typename T>
std::ostream& operator<<(std::ostream& out, const JMatrixBase<T>& m){
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


