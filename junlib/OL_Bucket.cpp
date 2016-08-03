#include "OL_Bucket.h"

namespace jun{

	
	//next  not-empty bucket
	OL_BUCKET::OUTLINE_LIST* OL_BUCKET::next(){
		for (; buckets[index].empty() && index < xdim*ydim; ++index);
		if (index == xdim*ydim) return  nullptr;
		return &buckets[index];
	}

}
