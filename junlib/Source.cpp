#include <iostream>
#include <algorithm>
#include <vector>
#include  "JMatrixBase.h"
#include "types.h"

#include  <amp.h>



using namespace std;
using namespace jun;

using namespace concurrency;

using Array = concurrency::array < unsigned, 2 > ;

void test_dilate_h();


int mainssss(){
	{
		/*accelerator acc = accelerator(accelerator::default_accelerator);
		cout << acc.supports_cpu_shared_memory << endl;
		

		BinaryImg img(4, 5);
		std::generate_n(img.begin(), 20, []{return std::rand() & 1; });

		cout << img << endl;

		concurrency::extent<2> ext(4, 5);

		Array data(ext, img.begin());
		Array data2(data);

		const int  hsz = 3;
		const int center = hsz / 1;

		parallel_for_each(data.extent, [=,&data2,&data](index<2> idx) restrict(amp){
			for (int i = 0; i < hsz; ++i){
				int mo = center - i;
				int c = idx[1];
				int w = data.extent[1];
				if (0 <= c&&c < w && 0 <= c + mo&&c + mo < w){
					data2(idx[0], idx[1] + mo) |= data[idx];
				}
			}

		});

		vector<unsigned char> result(20);
		concurrency::copy(data2, result.begin());

		for (auto val : result){
			cout << (int)val << ' ';
		}

		cout << endl;*/
		test_dilate_h();
	}




	getchar();
	return 0;
}



