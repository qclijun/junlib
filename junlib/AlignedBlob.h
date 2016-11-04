#pragma once

#include "BlobGrid.h"

namespace jun{

	//Fraction  of resolution  used as aligment tolerance for aligned tabs.
	const double kAlignedFraction = 0.03125;
	//Fraction of resolution used as alignment tolerance for ragged tabs.
	const double kRaggedFraction = 2.5;
	//Fraction of height used as a minimum gutter gap for aligned blobs.
	const double kAlignedGapFraction = 0.75;
	//Fraction of height used as a minimum gutter gap for ragged blobs.
	const double kRaggedGapFraction = 1.0;
	//constant number of pixels used as alignment tolerance for line finding.
	const int kVlineAlignment = 3;
	//constant number of pixels used as  gutter gap tolerance for line finding.
	const int kVLineGutter = 1;
	//constant number of pixels used as the search size for line finding.
	const int kVLineSearchSize = 150;
	//Min number of points to accept for a aligned tab stop.
	const int kMinAlignedTabs = 4;
	//Min number of points to accept for a ragged tab stop.
	const int kMinRaggedTabs = 5;
	//Constant number of pixels minimum height of a vertical line.
	const int kVLineMinLength = 500;

	const double kMinTabGradient = 4.0;

	// Tolerance to skew on top of current estimate of skew.Divide x or y length
	// by kMaxSkewFactor to get the y or x skew distance.
	// If the angle is small, the angle in degrees is roughly 60/kMaxSkewFactor
	// (3.8 degree)
	const int kMaxSkewFactor = 15;



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