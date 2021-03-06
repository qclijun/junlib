#include "AlignedBlob.h"

namespace jun{

	


	AlignedBlobParams::AlignedBlobParams(int vertical_x, int vertical_y,
		int height, int v_gap_multiple,
		int min_gutter_width, int resolution,
		TabAlignment align0)
	:right_tab(align0==TabAlignment::RIGHT_ALIGNED || align0==TabAlignment::RIGHT_RAGGED),
	ragged(align0==TabAlignment::LEFT_RAGGED || align0==TabAlignment::RIGHT_RAGGED),
	alignment(align0),
	confirmed_type(TabType::TT_CONFIRMED),
	min_length(0){
		max_v_gap = height*v_gap_multiple;
		if (ragged){
			gutter_fraction = kRaggedGapFraction;
			if (alignment == TabAlignment::RIGHT_RAGGED){
				l_align_tolerance = static_cast<int>(resolution*kRaggedFraction + 0.5);
				r_align_tolerance = static_cast<int>(resolution*kAlignedFraction + 0.5);
			}
			else{
				l_align_tolerance = static_cast<int>(resolution*kAlignedFraction + 0.5);
				r_align_tolerance = static_cast<int>(resolution*kRaggedFraction + 0.5);
			}
			min_points = kMinRaggedTabs;

		}
		else{
			gutter_fraction = kAlignedFraction;
			l_align_tolerance = static_cast<int>(resolution*kAlignedFraction + 0.5);
			r_align_tolerance = static_cast<int>(resolution*kAlignedFraction + 0.5);
			min_points = kMinAlignedTabs;
		}
		min_gutter = static_cast<int>(height*gutter_fraction + 0.5);
		if (min_gutter < min_gutter_width){
			min_gutter = min_gutter_width;

			set_vertical(vertical_x, vertical_y);
		}

	}

	AlignedBlobParams::AlignedBlobParams(int vertical_x, int vertical_y,
		int width){
		gutter_fraction = 0.0;
		right_tab = false;
		ragged = false;
		alignment = TabAlignment::SEPARATOR;
		confirmed_type = TabType::TT_VLINE;
		max_v_gap = kVLineSearchSize;
		min_gutter = kVLineGutter;
		min_points = 1;
		min_length = kVLineMinLength;
		l_align_tolerance =r_align_tolerance= std::max(kVlineAlignment, width);
		set_vertical(vertical_x, vertical_y);

		
	}

	static bool isAlignedBlob(const AlignedBlobParams& p, BLOBNBOX* bbox,  const BLOBNBOX* neighbour){
		Rect box = bbox->bounding_box();
		Rect nbox = neighbour->bounding_box();
		int n_y = (nbox.top() + nbox.bottom()) / 2;
		int dy = p.vertical.y>0 ? 1 : -1;  // delta y
		int x_start = p.right_tab ? box.right() : box.left();
		int y_start = dy>0 ? box.bottom() : box.top();
		if (std::abs(n_y - y_start) > p.max_v_gap) return false; // too far 
		if ((n_y - y_start)*dy < 0 || (nbox.overlap_y(box))) return false;


		int x_at_n_y = x_start + (n_y - y_start)*p.vertical.x / p.vertical.y;
		if (x_at_n_y<neighbour->left_crossing_rule || x_at_n_y> neighbour->right_crossing_rule)
			return false;
		int n_left = nbox.left(), n_right = nbox.right();
		int n_x = p.right_tab ? n_right : n_left;

		if (p.right_tab){
			if (n_left<x_at_n_y + p.min_gutter && n_right>x_at_n_y + p.r_align_tolerance
				&& (p.ragged || n_left < x_at_n_y + p.gutter_fraction*nbox.height)){
				if (bbox->right_tab_type >= TabType::TT_MAYBE_ALIGNED){

				}
			}
		}
		return true;
	}

	BLOBNBOX_LIST AlignedBlob::findAlignedBlobs(const AlignedBlobParams& p, BLOBNBOX* bbox){
		BLOBNBOX_LIST result;
		if (bbox == nullptr) return result;

		Rect box = bbox->bounding_box();
		if (!bounding_box().contains(box)) return result;  // box not in the grid.
		int x_start = p.right_tab ? box.right() : box.left();
		int left_col_edge = bbox->left_rule;
		int right_col_edge = bbox->right_rule;
		int dy = p.vertical.y>0 ? 1 : -1;  // delta y
		int y_start = dy>0 ? box.bottom() : box.top();

		int skew_tolerance = p.max_v_gap / kMaxSkewFactor;

		int x2 = (p.max_v_gap*p.vertical.x + std::abs(p.vertical.y) / 2) / std::abs(p.vertical.y);
		x2 += x_start;
		int y_end = y_start + p.max_v_gap*dy;
		int xmin, xmax;
		if (x2 < x_start){
			xmin = x2 - skew_tolerance;
			xmax = x_start + skew_tolerance;
		}
		else{
			xmin = x_start - skew_tolerance;
			xmax = x2 + skew_tolerance;
		}

		if (p.right_tab){
			xmax += p.min_gutter;
			xmin -= p.l_align_tolerance;
		}
		else{
			xmax += p.r_align_tolerance;
			xmin -= p.min_gutter;
		}
		int grid_x1, grid_y1, grid_x2, grid_y2;
		gridCoords(xmin, y_start, grid_x1, grid_y1);
		gridCoords(xmax, y_end, grid_x2, grid_y2);

		//start searching...
		for (auto y = grid_y1; y <= grid_y2; ++y){
			for (auto x = grid_x1; x <= grid_x2; ++x){
				auto ls = this->operator()(x, y);
				for (auto it = ls.begin(); it != ls.end(); ++it){
					PBLOBNBOX curr = *it;
					if (curr.get() == bbox) continue;
					Rect nbox = curr->bounding_box();


				}
			}
		}

		return result;

	}

}