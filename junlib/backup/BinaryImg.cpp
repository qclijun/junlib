#include "BinaryImg.h"
#include <cassert>

using  jun::BinaryImg;
using namespace std;

std::ostream& operator<<(std::ostream& out, const jun::BinaryImg& img){
	out << '[';
	for (int r = 0;; ++r){
		if (r > 0) out << ' ';
		auto p = img.iter(r, 0);
		for (int c = 0; c < img.cols(); ++c){
			out << std::setw(3) << (int)*p++;

		}
		if (r == img.rows() - 1){
			out << ']';
			break;
		}
		else{
			out << ";\n";
		}
	}
	out << std::endl;
	return out;
}


/*
********************************* BinaryImg member methods *****************************
*/


/**
***************************************** test code ********************************************
*/


void test_bitand(){
	const int SZ = 237;
	uchar src[SZ];
	uchar dst[SZ];
	std::generate_n(src, SZ, [](){return std::rand() & 1; });
	std::generate_n(dst, SZ, [](){return std::rand() & 1; });
	uchar tmp[SZ];

	memcpy(tmp, dst, SZ);
	jun::bitand(src, tmp, SZ);

	for (int i = 0; i < SZ; ++i){
		assert(tmp[i] == (src[i] & dst[i]));
	}
	cout << "test_bitand() passed." << endl;

}

void test_bitor(){
	const int SZ = 503;
	uchar src[SZ];
	uchar dst[SZ];
	std::generate_n(src, SZ, [](){return std::rand() & 1; });
	std::generate_n(dst, SZ, [](){return std::rand() & 1; });
	uchar tmp[SZ];

	memcpy(tmp, dst, SZ);
	jun::bitor(src, tmp, SZ);

	for (int i = 0; i < SZ; ++i){
		assert(tmp[i] == (src[i] | dst[i]));
	}
	cout << "test_bitor() passed." << endl;
}

void test_bitany(){

	uchar p[] = { 0, 0, 0, 0, 0, 1, 0, 0 };

	bool result1 = jun::bitany(p, 4);
	cout << boolalpha << result1 << endl;
}

void test_rasterOp(){
	BinaryImg img1(5, 5);
	BinaryImg img2(5, 5);

	std::generate_n(img1.begin(), img1.bytes(), [](){return rand() & 1; });
	std::generate_n(img2.begin(), img2.bytes(), [](){return rand() & 1; });

	BinaryImg result(img2,true);

	jun::rasterOp(img1, jun::Point(-1, 0), jun::Raster_OP::S_OR_D, result);

	std::cout << img1 << std::endl;
	std::cout << img2 << std::endl;
	std::cout << result << std::endl;

}