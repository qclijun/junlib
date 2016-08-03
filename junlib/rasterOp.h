#pragma once

#include "JMatrix.h"
#include <ppl.h>

JUN_BEGIN


enum class Raster_OP{
	S_AND_D, S_OR_D,S_XOR_D,
};




////src与dst大小相同
//template<typename T>
//void rasterOp_s(const JMatrixBase<T>& src, jun::Point anchor, Raster_OP op, JMatrixBase<T>& dst){
//	int sx = anchor.x, sy = anchor.y;
//	int dx = 0, dy = 0;
//	int w = src.cols() - std::abs(sx); //重叠部分的宽度
//	int h = src.rows() - std::abs(sy);//重叠部分的高度
//	if (w <= 0 || h <= 0) return;
//
//	if (dx < 0){
//		sx -= dx;
//		dx = 0;
//	}
//	if (dy < 0){
//		sy -= dy;
//		dy = 0;
//	}
//
//	auto ps = src.iter(sy, sx);
//	auto pd = dst.iter(dy, dx);
//	const int cpl = src.cpl();
//	const int nbyte = w*src.channels()*src.BPC;
//
//	switch (op){
//	case jun::Raster_OP::S_AND_D:
//		for (int i = 0; i < h; ++i){
//			bitand(ps, pd, nbyte);
//			pd += cpl;
//			ps += cpl;
//		}
//		break;
//	case jun::Raster_OP::S_OR_D:
//		for (int i = 0; i < h; ++i){
//			bitor(ps, pd, nbyte);
//			pd += cpl;
//			ps += cpl;
//		}
//		break;
//	default:
//		break;
//	}
//}
//
////ppl :
//template<typename T>
//void rasterOp_p(const JMatrixBase<T>& src, jun::Point anchor, Raster_OP op, JMatrixBase<T>& dst){
//	int sx = anchor.x, sy = anchor.y;
//	int dx = 0, dy = 0;
//	int w = src.cols() - std::abs(sx); //重叠部分的宽度
//	int h = src.rows() - std::abs(sy);//重叠部分的高度
//	if (w <= 0 || h <= 0) return;
//
//	if (dx < 0){
//		sx -= dx;
//		dx = 0;
//	}
//	if (dy < 0){
//		sy -= dy;
//		dy = 0;
//	}
//
//	auto ps = src.iter(sy, sx);
//	auto pd = dst.iter(dy, dx);
//	const int cpl = src.cpl();
//	const int nbyte = w*src.channels()*src.BPC;
//
//	switch (op){
//	case jun::Raster_OP::S_AND_D:
//		concurrency::parallel_for(0, h, [=](int r){
//			bitand(ps + r*cpl, pd + r*cpl, nbyte);
//		});
//		break;
//	case jun::Raster_OP::S_OR_D:
//		concurrency::parallel_for(0, h, [=](int r){
//			bitor(ps + r*cpl, pd + r*cpl, nbyte);
//		});
//		break;
//	default:
//		break;
//	}
//}
//

/**
* anchor: src起点相对于dst的起点（rect.x,rect.y)的位置，可以为负值
*/


template<typename T>
void rasterOp_s(const JMatrixBase<T>& src, Point anchor, Raster_OP op, JMatrixBase<T>& dst, Rect rect){
	int sx = 0, sy = 0;
	int dx = rect.x + anchor.x, dy = rect.y + anchor.y;

	if (anchor.x < 0){
		sx = -anchor.x;
		dx = rect.x;
	}
	if (anchor.y < 0){
		sy = -anchor.y;
		dy = rect.y;
	}
	int w = std::min(rect.xlast() - dx, src.cols() - sx);
	int h = std::min(rect.ylast() - dy, src.rows() - sy);

	if (w <= 0 || h <= 0) return;

	auto ps = src.iter(sy, sx);
	auto pd = dst.iter(dy, dx);
	const int scpl = src.cpl();
	const int dcpl = dst.cpl();
	const int nbyte = w*src.channels()*src.BPC;

	switch (op){
	case jun::Raster_OP::S_AND_D:
		for (int i = 0; i < h; ++i){
			bitand(ps, nbyte, pd);
			pd += scpl;
			ps += scpl;
		}
		break;
	case jun::Raster_OP::S_OR_D:
		for (int i = 0; i < h; ++i){
			bitor(ps, nbyte, pd);
			pd += dcpl;
			ps += scpl;
		}
		break;
	case jun::Raster_OP::S_XOR_D:
		for (int i = 0; i < h; ++i){
			bitxor(ps, nbyte, pd);
			pd += dcpl;
			ps += scpl;
		}
		break;
	default:
		throw Never_Reach_Here();
	}
}


template<typename T>
inline void rasterOp_s(const JMatrixBase<T>& src, Point anchor, Raster_OP op, JMatrixBase<T>& dst){
	rasterOp_s(src, anchor, op, dst, dst.rect());
}


template<typename T>
void rasterOp_p(const JMatrixBase<T>& src, Point anchor, Raster_OP op, JMatrixBase<T>& dst, Rect rect){
	int sx = 0, sy = 0;
	int dx = rect.x + anchor.x, dy = rect.y + anchor.y;

	if (anchor.x < 0){
		sx = -anchor.x;
		dx = rect.x;
	}
	if (anchor.y < 0){
		sy = -anchor.y;
		dy = rect.y;
	}
	int w = std::min(rect.xlast() - dx, src.cols() - sx);
	int h = std::min(rect.ylast() - dy, src.rows() - sy);

	if (w <= 0 || h <= 0) return;

	auto ps = src.iter(sy, sx);
	auto pd = dst.iter(dy, dx);
	const int scpl = src.cpl();
	const int dcpl = dst.cpl();
	const int nbyte = w*src.channels()*src.BPC;

	switch (op){
	case jun::Raster_OP::S_AND_D:
		concurrency::parallel_for(0, h, [=](int r){
			bitand(ps + r*scpl, nbyte, pd + r*dcpl);
		});
		break;
	case jun::Raster_OP::S_OR_D:
		concurrency::parallel_for(0, h, [=](int r){
			bitor(ps + r*scpl, nbyte, pd + r*dcpl);
		});
		break;
	case jun::Raster_OP::S_XOR_D:
		concurrency::parallel_for(0, h, [=](int r){
			bitxor(ps + r*scpl, nbyte, pd + r*dcpl);
		});
		break;
	default:
		throw Never_Reach_Here();
	}
}

template<typename T>
inline void rasterOp_p(const JMatrixBase<T>& src, Point anchor, Raster_OP op, JMatrixBase<T>& dst){
	rasterOp_p(src, anchor, op, dst, dst.rect());
}

template<typename T>
inline void rasterOp(const JMatrixBase<T>& src, jun::Point anchor, Raster_OP op, JMatrixBase<T>& dst, Rect rect){
	rasterOp_s(src, anchor, op, dst,rect);
}
template<typename T>
inline void rasterOp(const JMatrixBase<T>& src, Raster_OP op, JMatrixBase<T>& dst, Rect rect){
	rasterOp(src, Point(), op, dst, rect);
}

template<typename T>
inline void rasterOp(const JMatrixBase<T>& src, jun::Point anchor, Raster_OP op, JMatrixBase<T>& dst){
	rasterOp(src, anchor, op, dst, dst.rect());
}




JUN_END
