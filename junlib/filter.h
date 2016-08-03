#include "JMatrixBase.h"
#include "MatrixUtil.h"


JUN_BEGIN


// colIndex: channel index, not element index
template<typename T>
inline double convolution(const JMatrixBase<T>& src, const JMatrixBase<int>& kernel, typename JMatrixBase<T>::iter pos, Point anchor = Point(-1, -1)){
	static_assert(sizeof(JMatrixBase<T>::channel_type) == sizeof(uchar), "src channalType must be char");
	
	int dx = anchor.x, dy = anchor.y;
	double sum = 0.0;

	const int ch = src.channels();
	const int s = src.cpl();

	for (int r = 0; r < kernel.rows(); ++r){
		for (int c = 0; c < kernel.cols(); ++c){
			sum += kernel(r, c)* pos[(r + dy)*s + (c + dx)*ch];
		}
	}
	return sum;
}

template<typename T>
void filter2D(const JMatrixBase<T>& src, JMatrixBase<T>& dst, const JMatrixBase<int>& kernel){
	Assert<Bad_Arg>(!ARG_CHECK || (kernel.rows() == 3) && kernel.cols() == 3);

	typedef typename JMatrixBase<T>::channel_type cty;
	JMatrixBase<T> buf = addBorder(src, 1);
	cout << buf << endl;

	const int ch = buf.channels();
	const size_t s = buf.cpl();

	for (int r = 1; r < buf.rows(); ++r){
		auto p = buf.iter(r, 0);
		auto pd = dst.iter(r - 1, 0);
		for (int c = 1 * ch; c < (buf.cols() - 1)*ch; ++c){
			double val = kernel(0, 0) * p[c - s - ch] + kernel(0, 1) * p[c - s] + kernel(0, 2) * p[c - s + ch];
			val += kernel(1, 0)*p[c - ch] + kernel(1, 1)*p[c] + kernel(1, 2)*p[c + ch];
			val += kernel(2, 0)*p[c + s - ch] + kernel(2, 1)*p[c + s] + kernel(2, 2)*p[c + s + ch];

			//if (val < 0) val = 0;
			//else if (val > 255) val = 255;
			pd[c-ch] = static_cast<cty>(val);
		}
	}

}




JUN_END


