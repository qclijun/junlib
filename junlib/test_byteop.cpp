#include "byteOp.h"
#include <iostream>
#include <algorithm>
#include <chrono>

using  namespace std;
using namespace  jun;

void  test_byteop(){
	const size_t SZ = 1 << 20;
	const size_t  TIMES = 10000;
	
	uchar* src = new  uchar[SZ];
	uchar* dst = new uchar[SZ];
	uchar*  tmp = new uchar[SZ];
	uchar*  tmp2 = new uchar[SZ];

	generate(src, src+SZ, []()->uchar{return std::rand() % UCHAR_MAX; });
	generate(dst, dst+SZ, []()->uchar{return std::rand() % UCHAR_MAX; });

	
	double jizhun = 1.0;
	memcpy(tmp, dst, SZ);
	auto start = chrono::high_resolution_clock::now();
	for (auto i = 0; i < TIMES; ++i){
		jun::bitand(src,  SZ,tmp);
	}
	auto diff = chrono::high_resolution_clock::now() - start;
	cout << "bitand: " << diff.count() / TIMES << endl;
	jizhun = static_cast<double>(diff.count());

	memcpy(tmp2, dst, SZ);

	//for (auto i = 0; i < TIMES; ++i){
	//	jun::bitand2(src, tmp2, SZ);
	//}
	//diff = chrono::high_resolution_clock::now() - start;
	//cout << "bitand2: " << diff.count() / jizhun << endl;

	//assert(memcmp(tmp, tmp2, SZ) == 0);

	delete[]  src;
	delete[] dst;
	delete[]  tmp;
	delete[]  tmp2;

}

