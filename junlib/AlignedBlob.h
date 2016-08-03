#pragma once

#include "BlobGrid.h"

namespace jun{

	enum class TabAlignment : unsigned char{
		LEFT_ALIGNED,
		LEFT_RAGGED,
		CENTER_JUSTIFIED,
		RIGHT_ALIGNED,
		RIGHT_RAGGED,
		SEPARATOR,
		COUNT,
	};

	struct AlignedBlobParams{
		AlignedBlobParams(int vertical_x, int vertical_y,
			int height, int v_gap_multiple,
			int min_gutter_width, int resolution,
			TabAlignment align0);
		AlignedBlobParams(int vertical_x, int vertical_y, int width);


		void set_vertical(int vertical_x, int vertical_y){
			vertical.x = static_cast<short>(vertical_x);
			vertical.y = static_cast<short>(vertical_y);
		}

		double gutter_fraction;// Multiple of height used for min_gutter.
		bool right_tab; // looking at right edges?
		bool ragged;  // ragged edge
		TabAlignment alignment; 
		TabType confirmed_type; // Type to flag blobs if accepted.
		int max_v_gap; // max vertical gap to be tolerated.
		int min_gutter; // min gutter between columns.

		// tolerances allowed on horizontal alignment of aligned edges.
		int l_align_tolerance;
		int r_align_tolerance;

		int min_points; 
		int min_length;

		// current estimate of logical  vertical.
		// 表明方向
		PointS  vertical;
	};


	class AlignedBlob : public BlobGrid{
	public:
		AlignedBlob(RectS box, int gridsize) :BlobGrid(box, gridsize){}
		~AlignedBlob(){}

		// find a set of blobs that are aligned in the given vertical direction(params.vertical 表明方向)
		// with the given blob.
		BLOBNBOX_LIST findAlignedBlobs(const AlignedBlobParams& params,BLOBNBOX* bbox);
	};

}