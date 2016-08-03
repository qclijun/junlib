#include "BLOBNBOX.h"

namespace jun{

	BLOBNBOX::BLOBNBOX(){
		init();
	}


	BLOBNBOX::~BLOBNBOX(){
		outlines.reset();
		base_char_blob_ = nullptr;
		std::fill(std::begin(neighbours_), std::end(neighbours_), nullptr);
	}

	void BLOBNBOX::init(){
		area = 0;
		area_stroke_width = 0.0f;
		horz_stroke_width = 0.0f;
		vert_stroke_width = 0.0f;
		reInit();
	}

	void BLOBNBOX::reInit(){
		joined = false;
		reduced = false;
		repeated_set = 0;
		left_tab_type = TabType::TT_NONE;
		right_tab_type = TabType::TT_NONE;
		region_type = BlobRegionType::BRT_UNKNOWN;
		flow = BlobTextFlowType::NONE;
		spt_type = BlobSpecialTextType::SKIP;
		left_rule = 0;
		right_rule = 0;
		left_crossing_rule = 0;
		right_crossing_rule = 0;

	}
}