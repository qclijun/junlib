#pragma once

#include <string>
#include <typeinfo>
#include <cstring>

#include "JMatrix.h"
#include "opencv2\core.hpp"
#include "opencv2\imgcodecs.hpp"
#include "junlib\util.h"

JUN_BEGIN

template<typename T>
JMatrixBase<T> addBorder(const JMatrixBase<T>& src, int n){
	Assert<Bad_Arg>(!ARG_CHECK || (n < src.rows() && n < src.cols()));

	JMatrixBase<T> result(src.rows() + 2 * n, src.cols() + 2 * n, src.channels());
	for (int r = 0; r < src.rows(); ++r){
		std::memcpy(result.iter(n + r, n), src.iter(r, 0), src.bpl());
	}
	cout << result << endl;
	/*auto origin = result.elemIter(n,n);
	for (int i = 0; i < n; ++i){
		origin = addBorder0(result, origin, result.rows() - 2 * (n - i), result.cols() - 2 * (n - i));
	}*/

	const int nR = src.rows();
	const int nC = src.cols();
	const int ch = src.channels();
	for (int r = 0; r < nR; ++r){
		auto p = result.iter(n+r, 0);
		for (int i = 0; i < n; ++i){
			std::memcpy(p + i*ch, p + (2 * n - i)*ch, ch*sizeof(T));
			//result.copyPix(p + i*ch, p + (2 * n - i)*ch);
			//result.copyPix(p + (n + nC + i)*ch, p + (n + nC - 2 - i)*ch);
			//p[i] = p[(2 * n - i)]; //对称线为col = n
			//p[(n + nC + i)] = p[n + nC - 2 - i]; //对称线为col = n+nC-1
			std::memcpy(p + (n + nC + i)*ch, p + (n + nC - 2 - i)*ch, ch*sizeof(T));
		}
	}

	for (int i = 0; i < n; ++i){
		std::memcpy(result.iter(i, 0), result.iter(2 * n - i), result.bpl()); //对称线为row = n
		std::memcpy(result.iter(n + nR + i), result.iter(n + nR - 2 - i), result.bpl());//对称线为row = n+nR-1
		//result.copyRow(result.chIter(i, 0), result.chIter(2 * n - i));
		//result.copyRow(result.chIter(n + nR + i), result.chIter(n + nR - 2 - i));
	}

	//cout << result << endl;

	return result;
}




const int depthSize[] = { 1, 1, 2, 2, 4, 4, 8 };

template<typename T>
JMatrixBase<T> cvtFromMat(const cv::Mat& m){
	Assert<Bad_Type>(!TYPE_CHECK || (depthSize[m.depth()]==sizeof(uchar)));

	JMatrixBase<T> result(m.rows, m.cols, CV_MAT_CN(m.type()));
	int nRow = m.rows;
	size_t sz = result.bpl();
	if (m.isContinuous()){
		sz *= nRow;
		nRow = 1;
	}

	for (int r = 0; r < nRow; ++r){
		std::memcpy(result.iter(r,0), m.ptr(r, 0), sz);
	}
	return result;
}

template<typename T>
inline cv::Mat cvtToMat(const JMatrixBase<T>& m){
	return cvtToMat(m, T());
}

template<typename T>
cv::Mat cvtToMat(const JMatrixBase<T>& m, unsigned char){
	int ch = m.channels();
	cv::Mat result(m.rows(), m.cols(), CV_8UC(ch));
	std::memcpy(result.ptr(), m.begin(), m.bytes());
	return result;
}

template<typename T>
cv::Mat cvtToMat(const JMatrixBase<T>& m, char){
	int ch = m.channels();
	cv::Mat result(m.rows(), m.cols(), CV_8SC(ch));
	std::memcpy(result.ptr(), m.begin(), m.bytes());
	return result;
}

template<typename T>
cv::Mat cvtToMat(const JMatrixBase<T>& m, unsigned short){
	int ch = m.channels();
	cv::Mat result(m.rows(), m.cols(), CV_16UC(ch));
	std::memcpy(result.ptr(), m.begin(), m.bytes());
	return result;
}

template<typename T>
cv::Mat cvtToMat(const JMatrixBase<T>& m, short){
	int ch = m.channels();
	cv::Mat result(m.rows(), m.cols(), CV_16SC(ch));
	std::memcpy(result.ptr(), m.begin(), m.bytes());
	return result;
}

template<typename T>
cv::Mat cvtToMat(const JMatrixBase<T>& m, int){
	int ch = m.channels();
	cv::Mat result(m.rows(), m.cols(), CV_32SC(ch));
	std::memcpy(result.ptr(), m.begin(), m.bytes());
	return result;
}

template<typename T>
cv::Mat cvtToMat(const JMatrixBase<T>& m, float){
	int ch = m.channels();
	cv::Mat result(m.rows(), m.cols(), CV_32FC(ch));
	std::memcpy(result.ptr(), m.begin(), m.bytes());
	return result;
}

template<typename T>
cv::Mat cvtToMat(const JMatrixBase<T>& m, double){
	int ch = m.channels();
	cv::Mat result(m.rows(), m.cols(), CV_64FC(ch));
	std::memcpy(result.ptr(), m.begin(), m.bytes());
	return result;
}

enum class ReadImageFlag{
	UNCHANGED=-1,GRAYSCALE=0,COLOR=1,ANYDEPTH=2,ANYCOLOR=4,LOAD_GDAL=8
};

inline JMatrix readImage(const std::string& filename, ReadImageFlag flag = ReadImageFlag::COLOR) {
	cv::Mat m = cv::imread(filename, static_cast<int>(flag));
	if (m.rows == 0) throw std::ios::failure("cannot read the image: " + filename);
	return cvtFromMat<uchar>(m);
}

inline void writeImage(const std::string&  filename,const JMatrix& img){
	cv::imwrite(filename, cvtToMat(img));
}



inline JMatrix b2gray(const BinaryJMatrix& binary){
	JMatrix result(binary.rows(), binary.cols(), 1);
	auto pd = result.begin();
	for (auto p = binary.begin(); p != binary.end(); ++p,++pd){
		*pd = (*p == 0) ? 255 : 0;	
	}

	return result;
}

inline BinaryJMatrix gray2b(const JMatrix&  gray, int  threshold = 125){
	Assert<Bad_Arg>(gray.channels() == 1);
	BinaryJMatrix result(gray.rows(), gray.cols());
	auto pd = result.begin();
	for (auto p = gray.begin(); p != gray.end(); ++p,++pd)
		*pd = *p < threshold ? 1 : 0;

	return result;
}

//binary image
inline void writeImage(const std::string&  filename, const BinaryJMatrix& img){
	writeImage(filename, b2gray(img));
}




JUN_END
