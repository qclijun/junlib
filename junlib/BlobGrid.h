#pragma once


#include "BBGrid.h"
#include "BLOBNBOX_LIST.h"

namespace jun{

	class BlobGrid : public BBGrid<BLOBNBOX_LIST>{
	public:
		BlobGrid(RectS box, int gridsize) :BBGrid(box, gridsize){}
		virtual ~BlobGrid(){}

		void insertBlobList(BLOBNBOX_LIST bloblist);
	};
}

