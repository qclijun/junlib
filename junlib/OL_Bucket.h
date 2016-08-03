#pragma once

#include <list>
#include "types.h"
#include "Outline.h"

namespace jun{

	class OL_BUCKET{
		
	public:
		typedef std::list<Outline> OUTLINE_LIST;


		OL_BUCKET(Rect _box) :box(_box),index(0){
			xdim = (box.width - 1) / BUCKET_SIZE + 1;
			ydim = (box.height - 1) / BUCKET_SIZE + 1;
			buckets = new OUTLINE_LIST[xdim*ydim];
		}
		~OL_BUCKET(){
			delete[] buckets;
		}

		OUTLINE_LIST& operator()(int x, int y){
			return  buckets[(y - box.y) / BUCKET_SIZE*xdim + (x - box.x) / BUCKET_SIZE];
		}



		OUTLINE_LIST* next();



		OL_BUCKET(const OL_BUCKET&) = delete;
		OL_BUCKET& operator=(const OL_BUCKET&) = delete;

		OL_BUCKET(OL_BUCKET&& ) = delete;
		OL_BUCKET& operator=(OL_BUCKET&&) = delete;

	private:
		Rect box;
		int xdim, ydim;
		int index;
		OUTLINE_LIST* buckets;
		static const int BUCKET_SIZE{ 16 };

	};

}