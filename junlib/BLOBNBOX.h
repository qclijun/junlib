#pragma once
#include "OUTLINE_LIST.h"

namespace jun{
	enum class TabType : uchar{
		TT_NONE,
		TT_DELETED,
		TT_MAYBE_RAGGED,
		TT_MAYBE_ALIGNED,
		TT_CONFIRMED,
		TT_VLINE,
	};

	enum class BlobRegionType :uchar{
		BRT_NOISE,
		BRT_HLINE,
		BRT_VLINE,
		BRT_RECTIMAGE,
		BRT_POLYIMAGE,
		BRT_UNKNOWN,
		BRT_VERT_TEXT,
		BRT_TEXT,

		BRT_COUNT
	};

	enum class BlobNeighbourDir :uchar{
		LEFT, BELLOW, RIGHT, ABOVE, COUNT
	};

	enum class BlobSpecialTextType :uchar{
		NONE, ITALIC, DIGIT, MATH, UNCLEAR, SKIP, COUNT
	};

	enum class BlobTextFlowType :uchar{
		NONE, NONTEXT, NEIGHBOURS, CHAIN, STRONG_CHAIN,
		TEXT_ON_IMAGE, LEADER, COUNT
	};


	class BLOBNBOX{
	public:
		BLOBNBOX();
		~BLOBNBOX();

		static std::shared_ptr<BLOBNBOX> newObj(){
			return std::shared_ptr<BLOBNBOX>(new BLOBNBOX);
		}

		BLOBNBOX(const BLOBNBOX&) = delete;
		BLOBNBOX(BLOBNBOX&&) = delete;
		BLOBNBOX& operator=(const BLOBNBOX&) = delete;
		BLOBNBOX& operator=(BLOBNBOX&&) = delete;



		RectS bounding_box() const{ return box; }
		void set_bounding_box(RectS new_box){
			box = new_box;
			base_char_top_ = box.y;
			base_char_bottom_ = box.ylast()-1;
		}

		void compute_bounding_box(){
			box = outlines.bounding_box();
			base_char_top_ = box.y;
			base_char_bottom_ = box.ylast() - 1;
			baseline_y_ = box.ylast() - 1;
		}

		RectS reduced_box() const{
			return red_box;
		}
		void set_reduced_box(RectS new_box){
			red_box = new_box;
			reduced = 1;
		}


		OUTLINE_LIST get_outlines() const{ return outlines; }
		BLOBNBOX* neighbour(BlobNeighbourDir n) const{
			return neighbours_[static_cast<int>(n)];
		}
		bool good_stroke_neighbour(BlobNeighbourDir n) const{
			return good_stroke_neighbours_[static_cast<int>(n)];
		}
		void set_neighbour(BlobNeighbourDir n, BLOBNBOX* neighbour, bool good){
			neighbours_[static_cast<int>(n)] = neighbour;
			good_stroke_neighbours_[static_cast<int>(n)] = good;
		}
		bool isDiacritic() const{
			return base_char_top_ != box.y || base_char_bottom_ != box.ylast() - 1;
		}
		int base_char_top() const{ return base_char_top_; }
		int base_char_bottom() const{ return base_char_bottom_; }
		int baseline_position() const{ return baseline_y_; }
		void set_diacritic_box(RectS diacritic_box){
			base_char_top_ = box.y;
			base_char_bottom_ = box.ylast() - 1;
		}


	private:
		void init();
		void reInit();



	private:
		OUTLINE_LIST outlines;
		RectS box;
		RectS red_box;

		BLOBNBOX* base_char_blob_;




		int16_t base_char_top_;
		int16_t base_char_bottom_;
		int16_t baseline_y_;


		BLOBNBOX* neighbours_[4];
		bool good_stroke_neighbours_[4];


	public:
		int area : 30;
		int joined : 1;
		int reduced : 1;
		int repeated_set;
		float horz_stroke_width;
		float vert_stroke_width;
		float area_stroke_width;
		int line_crossings;
		int16_t left_rule;
		int16_t right_rule;
		int16_t left_crossing_rule;
		int16_t right_crossing_rule;
		TabType left_tab_type;
		TabType right_tab_type;
		BlobRegionType region_type;
		BlobTextFlowType flow;
		BlobSpecialTextType spt_type;
		bool horz_possible;
		bool vert_possible;
		bool leader_on_left;
		bool leader_on_right;
	};

	typedef std::shared_ptr<BLOBNBOX> PBLOBNBOX;

}
