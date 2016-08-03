#pragma once

#include "JMatrix.h"
#include "junlib\util.h"

namespace  jun{

	//src可以与dst为同一对象,dst大小足够容纳src
	template<typename MatrixType>
	inline void copy(const MatrixType& src, MatrixType& dst){
		if (&src != &dst)
			std::memcpy(dst.begin(), src.begin(), src.bytes());
	}

	template<typename MatrixType>
	inline void clipRect(const MatrixType& src, Rect rect, MatrixType& dst){
		Assert<Bad_Arg>(!ARG_CHECK || (src.rect().contains(rect)));
		Assert<Bad_Arg>(!ARG_CHECK || (dst.rows() == rect.height&&dst.cols() == rect.width));

		auto pd = dst.begin();
		auto ps = src.iter(rect.y, rect.x);
		const int nbyte = rect.width*src.channels()*src.BPC;
		for (int r = 0; r < rect.height; ++r){
			std::memcpy(pd, ps, nbyte);
			pd += dst.cpl();
			ps += src.cpl();
		}
	}

	template<typename MatrixType>
	inline MatrixType clipRect(const MatrixType& src, Rect rect){
		Assert<Bad_Arg>(!ARG_CHECK || (src.rect().contains(rect)));

		MatrixType dst(rect.height, rect.width, src.channels());
		clipRect(src, rect, dst);
		return dst;
	}


	template<typename MatrixType>
	inline void matrixAnd(const MatrixType& src1, const MatrixType& src2, MatrixType& dst){
		copy(src1, dst);
		dst.and(src2);
	}

	template<typename MatrixType>
	inline MatrixType matrixAnd(const MatrixType& src1, const MatrixType& src2){
		MatrixType result(src1);
		dst.and(src2);
		return result;
	}

	template<typename MatrixType>
	inline void matrixOr(const MatrixType& src1, const MatrixType& src2, MatrixType& dst){
		copy(src1, dst);
		dst.or(src2);
	}

	template<typename MatrixType>
	inline MatrixType matrixOr(const MatrixType& src1, const MatrixType& src2){
		MatrixType result(src1);
		dst.or(src2);
		return result;
	}

	template<typename MatrixType>
	inline void matrixXor(const MatrixType& src1, const MatrixType& src2, MatrixType& dst){
		copy(src1, dst);
		dst.xor(src2);
	}

	template<typename MatrixType>
	inline MatrixType matrixXor(const MatrixType& src1, const MatrixType& src2){
		MatrixType result(src1);
		dst.xor(src2);
		return result;
	}

	inline BinaryJMatrix matrixSubstract(const BinaryJMatrix& m1, const BinaryJMatrix& m2){
		auto result(m1);
		result.substract(m2);
		return result;
	}

	
	inline size_t countPixes(const BinaryJMatrix& src, Rect rect){
		size_t sum = 0;
		auto p = src.iter(rect.y, rect.x);
		for (int r = 0; r < rect.height; ++r, p += src.cpl()){
			for (int c = 0; c < rect.width; ++c){
				if (p[c]) ++sum;
			}
		}
		return sum;
	}


}