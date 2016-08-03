#include "BlobGrid.h"

namespace jun{

	void BlobGrid::insertBlobList(BLOBNBOX_LIST bloblist){
		for (auto it = bloblist.begin(); it != bloblist.end(); ++it){
			if (!(*it)->joined){
				insertBBox(true, true, *it);
			}
		}
	}
}