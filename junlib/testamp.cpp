#include <cstdint>
#include <amp.h>
#include <algorithm>
using namespace std;
using  namespace concurrency;


template<typename T, int rank>
using Array = concurrency::array < T, ranl >;

void test_amp(){
	const int ROW = 6;
	const int COL = 6;
	unsigned char*  src[ROW*COL];
	unsigned char*  dst[ROW*COL];

	generate_n(src, ROW*COL, []{return rand() & 1; });
	generate_n(dst, ROW*COL, []{return rand() & 1; });


	Array<unsigned, 2> sdata(4, 1);
	auto ext = sdata.extent;
	for (int r = 0; r < ext[0]; ++r){

	}

}
