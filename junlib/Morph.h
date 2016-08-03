#pragma once
#include "JMatrix.h"

JUN_BEGIN

//enum class Shape{
//	MORPH_RECT, MORPH_CROSS, MORPH_ELLIPSE
//};


using jun::BinaryJMatrix;


//binary image needed.in-place not supported.接口尽量简化
namespace Morph{

	void dilate_brick(const BinaryJMatrix& src, BinaryJMatrix& dst, int hsz, int vsz);

	void erode_brick(const BinaryJMatrix& src, BinaryJMatrix& dst, int hsz, int vsz);

	void open_brick(const BinaryJMatrix& src, BinaryJMatrix& dst, int hsz, int vsz);

	void close_brick(const BinaryJMatrix& src, BinaryJMatrix& dst, int hsz, int vsz);
	
};

JUN_END


