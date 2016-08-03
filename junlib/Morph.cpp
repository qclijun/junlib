#include <chrono>
#include <future>
#include <iostream>
//#include <amp.h>

#include "Morph.h"
#include "MatrixUtil.h"
#include "junlib\util.h"
#include "rasterOp.h"

using jun::BinaryJMatrix;
using jun::Point;


/**
************************************* Helper functions ********************************
*/

//in-place not supported.
static void dilate_h1(const BinaryJMatrix& src, BinaryJMatrix& dst, int hsz){
	//内部函数，不检查参数
	//Assert<Bad_Arg>(!ARG_CHECK || (hsz > 0 && src.isSameDimension(dst) && (&src != &dst)));
	jun::copy(src, dst);
	if (hsz == 1) {
		return;
	}
	int center = hsz / 2;
	for (int r = 0; r < src.rows(); ++r){
		auto ps = src.iter(r, 0);
		for (int c = 0; c < src.cols(); ++c){
			if (c >= center && (c + hsz - center <= src.cols())){
				for (int i = c - center; i < c - center + hsz; ++i){

					if (ps[i]){
						dst(r, c) = 1;
						break;
					}
				}

			}
			else if (c < center){
				for (int i = 0; i < std::min(c + hsz - center, src.cols()); ++i){
					if (ps[i]){
						dst(r, c) = 1;
						break;
					}
				}
			}
			else{
				for (int i = c - center; i < src.cols(); ++i){
					if (ps[i]){
						dst(r, c) = 1;
						break;
					}
				}
			}
		}



	}

}

static void dilate_h2(const BinaryJMatrix& src, BinaryJMatrix& dst, int hsz){

	//Assert<Bad_Arg>(!ARG_CHECK || (hsz > 0 && src.isSameDimension(dst) && (&src != &dst)));
	jun::copy(src, dst);
	//TO: if hsz>cols() ???
	if (hsz == 1) {
		return;
	}
	int center = hsz / 2;
	for (int r = 0; r < src.rows(); ++r){
		auto ps = src.iter(r, 0);
		for (int c = 0; c < src.cols(); ++c){
			if (c >= center && (c + hsz - center <= src.cols())){
				if (jun::bitany(ps + c - center, hsz))
					dst(r, c) = 1;
			}
			else if (c < center){ // left border
				if (jun::bitany(ps, std::min(c + hsz - center, src.cols()))) dst(r, c) = 1;
			}
			else{ //right border
				if (jun::bitany(ps + c - center, src.cols() - c + center)) dst(r, c) = 1;
			}
		}


	}
}



static void dilate_h3(const BinaryJMatrix& src, BinaryJMatrix& dst, int hsz){
	//Assert<Bad_Arg>(!ARG_CHECK || (hsz > 0 && src.isSameDimension(dst) && (&src != &dst)));
	jun::copy(src, dst);
	if (hsz == 1) {
		return;
	}
	int center = hsz / 2;
	for (int x = 0; x < hsz; ++x){
		if (x == center) continue;
		jun::rasterOp(src, Point(center-x, 0), jun::Raster_OP::S_OR_D, dst);
	}
}


static void dilate_h40(const BinaryJMatrix& src, int begin, int end, int center, BinaryJMatrix& dst){
	int len = end - begin;
	const int SZ = 16;
	if (len == 1) {
		jun::copy(src, dst);
		return;
	}
	dst.clearAll();
	if (len <= SZ){
		for (int i = begin; i < end; ++i){
			jun::rasterOp(src, Point(center - i, 0), jun::Raster_OP::S_OR_D, dst);
		}
		return;
	}

	int mid = begin + len / 2;
	BinaryJMatrix tmp(src, false);
	BinaryJMatrix tmp2(src, false);

	concurrency::parallel_invoke(
		[=, &src, &tmp]{dilate_h40(src, begin, mid, center, tmp); },
		[=, &src, &tmp2]{dilate_h40(src, mid, end, center, tmp2); }
	);

	jun::bitor(tmp2.begin(), tmp.bytes(), tmp.begin());
	jun::copy(tmp, dst);
}

static void dilate_h4(const BinaryJMatrix& src, BinaryJMatrix& dst, int hsz){
	//Assert<Bad_Arg>(!ARG_CHECK || (hsz > 0 && src.isSameDimension(dst) && (&src != &dst)));

	if (hsz == 1) {
		jun::copy(src, dst);
		return;
	}
	int center = hsz / 2;
	dilate_h40(src, 0, hsz, center, dst);
}

//
//static void dilate_h5(const BinaryJMatrix& src, BinaryJMatrix& dst, int hsz){
//	//Assert<Bad_Arg>(!ARG_CHECK || (hsz > 0 && src.isSameDimension(dst) && (&src != &dst)));
//	if (hsz == 1) {
//		jun::copy(src, dst);
//		return;
//	}
//	const int center = hsz / 2;
//	const int w = src.cols();
//
//	using namespace concurrency;
//	using Array = concurrency::array < int, 2 > ;
//	concurrency::extent<2> ext(src.rows(), src.cols());
//
//	Array sArr(ext, src.begin());
//	Array dArr(sArr);
//
//	for (int i = 0; i < hsz; ++i){
//		int mov = center - i;
//		if (mov == 0) continue;
//		concurrency::parallel_for_each(ext, [=, &sArr, &dArr](index<2> idx) restrict(amp){
//			int c = idx[1];
//			if (0 <= c&&c < w && 0 <= c + mov&&c + mov < w){
//				dArr(idx[0], c + mov) |= sArr(idx);
//			}
//		});
//	}
//
//	//concurrency::parallel_for_each(ext, [=, &sArr, &dArr](index<2> idx) restrict(amp){
//	//	for (int i = 0; i < hsz; ++i){
//	//		int mov = center - i;
//	//		if (mov == 0) continue;
//	//		int c = idx[1];
//	//		if (0 <= c&&c < w && 0 <= c + mov&&c + mov < w){
//	//			dArr(idx[0], c + mov) |= sArr(idx);
//	//		}
//	//	}
//	//});
//
//	concurrency::copy(dArr, dst.begin());
//}
//


static inline void dilate_h(const BinaryJMatrix& src, BinaryJMatrix& dst, int hsz){
	dilate_h3(src, dst, hsz);
}

static void dilate_v(const BinaryJMatrix& src, BinaryJMatrix& dst, int vsz){
	//Assert<Bad_Arg>(!ARG_CHECK || (vsz > 0 && src.isSameDimension(dst) && (&src != &dst)));
	jun::copy(src, dst);
	if (vsz == 1) {
		return;
	}
	int center = vsz / 2;
	for (int y = 0; y < vsz; ++y){
		if (y == center) continue;
		jun::rasterOp(src, Point(0, center - y), jun::Raster_OP::S_OR_D, dst);
	}
}

static void erode_h(const BinaryJMatrix& src, BinaryJMatrix& dst, int hsz){
	//Assert<Bad_Arg>(!ARG_CHECK || (hsz > 0 && src.isSameDimension(dst) && (&src != &dst)));
	jun::copy(src, dst);
	if (hsz == 1) {
		return;
	}
	int center = hsz / 2;
	for (int x = 0; x < hsz; ++x){
		if (x == center) continue;
		jun::rasterOp(src, Point(center - x, 0), jun::Raster_OP::S_AND_D, dst);
	}
}

static void erode_v(const BinaryJMatrix& src, BinaryJMatrix& dst, int vsz){
	//Assert<Bad_Arg>(!ARG_CHECK || (vsz > 0 && src.isSameDimension(dst) && (&src != &dst)));
	jun::copy(src, dst);
	if (vsz == 1) {
		return;
	}
	int center = vsz / 2;
	for (int y = 0; y < vsz; ++y){
		if (y == center) continue;
		jun::rasterOp(src, Point(0, center - y), jun::Raster_OP::S_AND_D, dst);
	}
}

/**
************************************* interface implemention ********************************
*/



void jun::Morph::dilate_brick(const BinaryJMatrix& src, BinaryJMatrix& dst, int hsz, int vsz){
	Assert<Bad_Arg>((hsz > 0 && vsz > 0 && src.isSameDimension(dst) && (&src != &dst)));
	if (hsz == 1 && vsz == 1){
		jun::copy(src, dst);
		return;
	}
	if (hsz == 1){
		dilate_v(src, dst, vsz);
		return;
	}
	if (vsz == 1){
		dilate_h(src, dst, hsz);
		return;
	}
	BinaryJMatrix tmp(src);
	dilate_h(src, tmp, hsz);
	dilate_v(tmp, dst, vsz);
}

void jun::Morph::erode_brick(const BinaryJMatrix& src, BinaryJMatrix& dst, int hsz, int vsz){
	Assert<Bad_Arg>(!ARG_CHECK || (hsz > 0 && vsz > 0 && src.isSameDimension(dst) && (&src != &dst)));
	if (hsz == 1 && vsz == 1){
		jun::copy(src, dst);
		return;
	}
	if (hsz == 1){
		erode_v(src, dst, vsz);
		return;
	}
	if (vsz == 1){
		erode_h(src, dst, hsz);
		return;
	}
	BinaryJMatrix tmp(src, true);
	erode_h(src, tmp, hsz);
	erode_v(tmp, dst, vsz);
}

void jun::Morph::open_brick(const BinaryJMatrix& src, BinaryJMatrix& dst, int hsz, int vsz){
	Assert<Bad_Arg>(!ARG_CHECK || (hsz > 0 && vsz > 0 && src.isSameDimension(dst) && (&src != &dst)));
	if (hsz == 1 && vsz == 1){
		jun::copy(src, dst);
		return;
	}

	BinaryJMatrix tmp(src);
	if (hsz == 1){
		erode_v(src, tmp, vsz);
		dilate_v(tmp, dst, vsz);
		return;
	}
	if (vsz == 1){
		erode_h(src, tmp, hsz);
		dilate_h(tmp, dst, hsz);
		return;
	}

	erode_h(src, tmp, hsz);
	erode_v(tmp, dst, vsz);
	dilate_h(dst, tmp, hsz);
	dilate_v(tmp, dst, vsz);

}

void jun::Morph::close_brick(const BinaryJMatrix& src, BinaryJMatrix& dst, int hsz, int vsz){
	Assert<Bad_Arg>(!ARG_CHECK || (hsz > 0 && vsz > 0 && src.isSameDimension(dst) && (&src != &dst)));
	if (hsz == 1 && vsz == 1){
		jun::copy(src, dst);
		return;
	}

	BinaryJMatrix tmp(src);
	if (hsz == 1){
		dilate_v(src, tmp, vsz);
		erode_v(tmp, dst, vsz);
		return;
	}
	if (vsz == 1){
		dilate_h(src, tmp, hsz);
		erode_h(tmp, dst, hsz);
		return;
	}

	dilate_h(src, tmp, hsz);
	dilate_v(tmp, dst, vsz);
	erode_h(dst, tmp, hsz);
	erode_v(tmp, dst, vsz);

}

/**
***************************************** test code ********************************************
*/

void test_erode_h(){
	jun::JMatrix b = jun::readImage("test-image_b.png", jun::ReadImageFlag::GRAYSCALE);
	BinaryJMatrix img = gray2b(b);

	const int hsz = 5;
	BinaryJMatrix tmp(img, false);
	erode_h(img, tmp, hsz);
	writeImage("test-image_erode_h.png", tmp);
}


void test_dilate_h(){
	jun::JMatrix b = jun::readImage("test-image_b.png", jun::ReadImageFlag::GRAYSCALE);
	BinaryJMatrix img = gray2b(b);

	/*const int SZ = 3600;
	BinaryJMatrix img(SZ, SZ);
	std::srand((unsigned int)::time(nullptr));
	std::generate_n(img.begin(), img.bytes(), []()-> uchar{return rand() & 1; });*/

	//writeImage("rand.png", img);
	std::cout << img.rows() << ' ' << img.cols() << std::endl;
	const int hsz = 10;
	const int times = 10;

	BinaryJMatrix h1(img, false);

	auto d = jun::time_call(times, [&]{
		h1.clearAll();
		dilate_h1(img, h1, hsz);
	});
	auto jizhun = (double)d;
	std::cout << "time(h1): " << d << std::endl;
	//writeImage("rand_h1.png", h1);


	BinaryJMatrix h2(img, false);
	d = jun::time_call(times, [&]{
		h2.clearAll();
		dilate_h2(img, h2, hsz);
	});

	std::cout << "time(h2): " << d / jizhun << std::endl;
	//writeImage("rand_h2.png", h2);

	BinaryJMatrix h3(img, false);
	d = jun::time_call(times, [&]{
		h3.clearAll();
		dilate_h3(img, h3, hsz);
	});
	std::cout << "time(h3): " << d / jizhun << std::endl;

	BinaryJMatrix h4(img, false);
	d = jun::time_call(times, [&]{
		h4.clearAll();
		dilate_h4(img, h4, hsz);
	});
	std::cout << "time(h4): " << d / jizhun << std::endl;

	//BinaryJMatrix h5(img, false);
	//d = jun::time_call(times, [&]{
	//	h5.clearAll();
	//	dilate_h5(img, h5, hsz);
	//});
	//std::cout << "time(h5): " << d / jizhun << std::endl;

	//writeImage("rand_h3.png", h3);

	//assert(h4 == h3);
	std::cout << std::boolalpha << (h1 == h2) << " " << (h2 == h3) << " " << (h3 == h4) << "  " 
		<< std::endl;
}


