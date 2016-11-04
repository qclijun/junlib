#include <iostream>
#include <algorithm>
#include <set>

#include "LineFinder.h"
#include "Morph.h"
#include "MatrixUtil.h"
#include "floodfill.h"
#include "BoxCmp.h"
#include "AlignedBlob.h"


using std::max_element;
using std::deque;
using std::cout;
using std::endl;
using std::set;

namespace jun{

	int LINE_FINDER_DEBUG_LEVEL = 3;

	const  double LineFinder::kMaxStaveHeight = { 1.0 };// Max height of a music stave in inches.

	const  double LineFinder::kThickLengthMultiple = { 0.75 };

	const  double LineFinder::kMaxNonLineDensiBinaryJMatrix = { 0.25 };

	void LineFinder::getLineMasks(){
		int max_line_width = resolution / kThinLineFraction;
		int min_line_length = resolution / kMinLineLengthFraction;
		int closing_brick = max_line_width / 3;

		Morph::close_brick(binary, pix_closed, closing_brick, closing_brick);
		//pixa_display.push_back(&pix_closed);

		BinaryJMatrix pix_solid(binary, false);
		Morph::open_brick(pix_closed, pix_solid, max_line_width, max_line_width);
		//BinaryJMatrix pix_hollow(pix_closed);
		//pix_hollow.substract(pix_solid);

		auto pix_hollow = matrixSubstract(pix_closed, pix_solid);


		Morph::open_brick(pix_hollow, pix_vline, 1, min_line_length);//长度超过min_line_length的垂直线
		Morph::open_brick(pix_hollow, pix_hline, min_line_length, 1);//长度超过min_line_length的水平线

		jun::writeImage("test-image_vline.png", pix_vline);
		jun::writeImage("test-image_hline.png", pix_hline);

		v_empBinaryJMatrix = pix_vline.isEmpty();
		h_empBinaryJMatrix = pix_hline.isEmpty();

		//if (!v_empBinaryJMatrix&&!h_empBinaryJMatrix){
		//	FilterMusic();
		//}

		if (h_empBinaryJMatrix&&v_empBinaryJMatrix) return;
		BinaryJMatrix pix_nonlines(binary);
		if (!v_empBinaryJMatrix){
			pix_nonlines.substract(pix_vline);
		}
		if (!h_empBinaryJMatrix){
			pix_nonlines.substract(pix_hline);
		}

		if (!v_empBinaryJMatrix){
			writeImage("test-image_nonlines.png", pix_nonlines);
			Morph::erode_brick(pix_nonlines, pix_non_vline, kMaxLineResidue, 1);
			//writeImage("test-image_non_vline.png", pix_non_vline);

			FloodFill::seedFillBinary(pix_non_vline, pix_nonlines);
			//writeImage("test-image_non_vline2.png", pix_non_vline);
		}

		if (!h_empBinaryJMatrix){
			Morph::erode_brick(pix_nonlines, pix_non_hline, 1, kMaxLineResidue);
			FloodFill::seedFillBinary(pix_non_hline, pix_nonlines);
		}

		if (!h_empBinaryJMatrix&&!v_empBinaryJMatrix){
			matrixAnd(pix_vline, pix_hline, pix_intersections);
			pix_non_vline.or(pix_hline);
			pix_non_vline.substract(pix_intersections);
			writeImage("test-image_non_vline.png", pix_non_vline);

			pix_non_hline.or(pix_vline);
			pix_non_hline.substract(pix_intersections);
			writeImage("test-image_non_hline.png", pix_non_hline);

			if (filterFalsePositives(resolution, pix_non_vline, pix_vline, &pix_intersections) == 0)
				v_empBinaryJMatrix = true;
			if (filterFalsePositives(resolution, pix_non_hline, pix_hline, &pix_intersections) == 0)
				h_empBinaryJMatrix = true;
		}
		else if (!h_empBinaryJMatrix){ //v_empBinaryJMatrix == true
			if (filterFalsePositives(resolution, pix_non_hline, pix_hline, &pix_intersections) == 0)
				h_empBinaryJMatrix = true;
		}
		else{ // h_empBinaryJMatrix  == true;
			if (filterFalsePositives(resolution, pix_non_vline, pix_vline, &pix_intersections) == 0)
				v_empBinaryJMatrix = true;
		}


	}

	void LineFinder::FilterMusic(){
		int max_stave_height = static_cast<int>(resolution * kMaxStaveHeight);

		auto intersection_pix(pix_vline);
		intersection_pix.and(pix_hline);

		//jun::writeImage("test-image_inter.png", intersection_pix);

		std::deque<Rect> rects;
		std::deque<Rect> rects2;
		jun::FloodFill::getConnComp(pix_vline, rects, FloodFill::Connectivity::EIGHT);
		pix_music_mask.clearAll();

		while (!rects.empty()){
			Rect& box = rects.back();
			rects2.clear();
			FloodFill::getConnComp(intersection_pix, box, rects2, FloodFill::Connectivity::EIGHT);
			auto joins = rects2.size();
			cout << "Box: " << box << ", joins = " << joins << endl;

			if (joins > 5 && (joins - 1) * max_stave_height >= 4 * box.height){
				pix_music_mask.setAll(box);
			}

			rects.pop_back();
		}
		jun::writeImage("test-image_music_mask.png", pix_music_mask);


	}


	//helper functions:
	namespace {
		size_t countPixelsAdjacentToLine(int line_width, jun::Rect box, const jun::BinaryJMatrix& nonline_pix){
			bool hline = box.width > box.height;
			int x = box.x, y = box.y;
			int w = box.width, h = box.height;
			if (hline){
				int bottom = std::min(nonline_pix.rows(), y + h + line_width);
				y = std::max(0, y - line_width);
				h = bottom - y;
			}
			else{
				int right = std::min(nonline_pix.cols(), x + w + line_width);
				x = std::max(0, x - line_width);
				w = right - x;
			}
			return jun::countPixes(nonline_pix, { x, y, w, h });
		}
	}

	int LineFinder::numTouchingIntersections(Rect box, jun::BinaryJMatrix* intersection_pix){
		if (intersection_pix == nullptr) return 0;
		std::deque<Rect> result;
		jun::FloodFill::getConnComp(*intersection_pix, box, result, FloodFill::Connectivity::EIGHT);
		return static_cast<int>(result.size());
	}

	// Helper erases false-positive line segments from the input/output line_pix.
	// 1. Since thick lines shouldn't really break up, we can eliminate some false
	//    positives by marking segments that are at least kMinThickLineWidth
	//    thickness, yet have a length less than min_thick_length.
	// 2. Lines that don't have at least 2 intersections with other lines and have
	//    a lot of neighbouring non-lines are probably not lines (perhaps arabic
	//    or Hindi words, or underlines.)
	// Bad line components are erased from line_pix.
	// Returns the number of remaining connected components.
	int LineFinder::filterFalsePositives(int resolution, BinaryJMatrix& nonline_pix,
		BinaryJMatrix& line_pix, BinaryJMatrix* intersection_pix) {
		int min_thick_length = static_cast<int>(resolution*kThickLengthMultiple);

		deque<Rect> boxes;
		deque<BinaryJMatrix> comp_pixes;
		FloodFill::getConnComp(line_pix, boxes, comp_pixes, FloodFill::Connectivity::EIGHT);
		int n = 0;
		while (!boxes.empty()){
			Rect& box = boxes.back();
			BinaryJMatrix& comp_pix = comp_pixes.back();
			auto dist = FloodFill::computeDistance(comp_pix, FloodFill::Connectivity::FOUR);
			int max_width = 2 * (*std::max_element(dist.begin(), dist.end()));
			bool bad_line = false;
			bool hline = box.width > box.height; //水平线

			//宽长比太大 
			if (hline){
				if (box.height >= kMinThickLineWidth && box.width<min_thick_length && max_width>kMinThickLineWidth)
					bad_line = true;
			}
			else{
				if (box.width >= kMinThickLineWidth && box.height<min_thick_length && max_width>kMinThickLineWidth)
					bad_line = true;
			}

			//线周围的像素太多
			if (!bad_line){
				auto count = countPixelsAdjacentToLine(max_width, box, nonline_pix);
				if (count > box.height * box.width * kMaxNonLineDensiBinaryJMatrix) bad_line = true;
			}

			if (bad_line){
				line_pix.clearAll(box);
			}
			else{
				++n;
			}

			boxes.pop_back();
			comp_pixes.pop_back();

		}
		return n;
	}


	// 左对齐
	PBLOBNBOX LineFinder::findAlignedBlob(const AlignedBlobParams& p, std::set<PBLOBNBOX, BoxCmp_LT<BLOBNBOX>>& bset, PBLOBNBOX bbox){
		if (bbox == nullptr) return nullptr;
		int dx = p.vertical.x, dy = p.vertical.y;
		int dir_y = dy < 0 ? -1 : 1;

		int skew_tolerance = p.max_v_gap / jun::kMaxSkewFactor;
		int x2 = (p.max_v_gap*dx + dy / 2) / dy;
		Rect box = bbox->bounding_box();
		int x_start = box.x;
		x2 += x_start;
		int xmin, xmax, ymin, ymax;
		if (x2 < x_start){ //向左
			xmin = x2; xmax = x_start;
		}
		else{
			xmin = x_start; xmax = x2;
		}
		if (dy > 0){ //向下
			ymin = box.ylast();
			ymax = ymin + p.max_v_gap;
		}
		else{
			ymax = box.y;
			ymin = ymax - p.max_v_gap;
		}
		xmin -= skew_tolerance - p.min_gutter;
		xmax += skew_tolerance + p.r_align_tolerance;

		logger()->debug("Starting {} {}  search at {}-{},{}  dy={} search_size={}, gutter={}\n",
			p.ragged ? "Ragged" : "Aligned", "Left", xmin, xmax, box.ylast(), dir_y, p.max_v_gap, p.min_gutter
			);
		auto begin_blob = std::make_shared<BLOBNBOX>();
		begin_blob->set_bounding_box(Rect{ xmin, ymin, 1, 1 });
		auto end_blob = std::make_shared<BLOBNBOX>();
		end_blob->set_bounding_box(Rect{ xmax, ymax, 1, 1 });
		auto beginIter = bset.lower_bound(begin_blob);
		auto endIter = bset.upper_bound(end_blob);

		PBLOBNBOX result = nullptr;
		int min_dist = std::numeric_limits<int>::max();
		for (auto iter = beginIter; iter != endIter; ++iter){
			Rect nbox = (*iter)->bounding_box();
			int n_y = (nbox.y + nbox.ylast()) / 2;
			if ((dy > 0) && (n_y<ymin || n_y>ymax)) continue;
			if (dy < 0 && (n_y < ymin || n_y > ymax)) continue;

			int x_at_ny = x_start + (n_y - ymin)*dx / dy;
			int n_x = nbox.x;
			//aligned so keep it.
			if (n_x <= x_at_ny + p.r_align_tolerance&&n_x >= x_at_ny - p.l_align_tolerance){
				logger()->debug("aligned, seeking left, box={}\n", nbox);
				TabType n_type = (*iter)->left_tab_type;
				if (n_type != TabType::TT_NONE && (p.ragged || n_type != TabType::TT_MAYBE_RAGGED)){

					int x_dist = n_x - x_at_ny;
					int y_dist = n_y - ymin;
					int new_dist = x_dist*x_dist + y_dist*y_dist;
					if (new_dist < min_dist) {
						min_dist = new_dist;
						result = (*iter);
					}

				}
			}
		}
		return result;

	}

	void  LineFinder::findAlignedBlobs(std::deque<PBLOBNBOX>& aligned_blobs,const AlignedBlobParams& p, std::set<PBLOBNBOX, BoxCmp_LT<BLOBNBOX>>& bset, PBLOBNBOX bbox){
		const int dy = p.vertical.y;
		auto tmp = bbox;
		while ((tmp = findAlignedBlob(p, bset, tmp)) != nullptr){
			TabType type = bbox->left_tab_type;
			if (p.ragged || (type != TabType::TT_NONE && type != TabType::TT_MAYBE_RAGGED)){
				logger()->debug("found aligned blob: {}\n", tmp->bounding_box());

				if (dy > 0) aligned_blobs.push_back(bbox);
				else aligned_blobs.push_front(bbox);
			}		
		}


	}

	void LineFinder::findVerticalAlignment(const AlignedBlobParams& p, std::set<PBLOBNBOX, BoxCmp_LT<BLOBNBOX>>& bset, PBLOBNBOX bbox){

	}

	namespace {
		inline bool atLeast2LineCrossing(const std::deque<PBLOBNBOX>& que){
			int total = 0;
			for (auto iter = que.begin(); iter != que.end(); ++iter){
				total += (*iter)->line_crossings;
			}
			return total >= 2;
		}
	}


	void  LineFinder::findAndRemoveVLines(){
		set<PBLOBNBOX, jun::BoxCmp_LT<BLOBNBOX>> bset;
		getLineBlobs(false, &pix_vline, &pix_intersections, bset);
		if (bset.empty()) return;
		int vertical_x = 0, vertical_y = 1;
		deque<PBLOBNBOX> alignedblobs;
		for (auto it = bset.begin(); it != bset.end(); ++it){
			if ((*it)->left_tab_type == TabType::TT_MAYBE_ALIGNED){
				PBLOBNBOX bbox = *it;
				Rect box = bbox->bounding_box();
				logger()->info("Finding live vector starting at ({},{}) \n", box.x, box.y);
				AlignedBlobParams align_params(vertical_x, vertical_y, box.width);
				alignedblobs.clear();
				alignedblobs.push_back(bbox);
				findAlignedBlobs(alignedblobs, align_params, bset, bbox);
				align_params.vertical *= -1; //反向
				findAlignedBlobs(alignedblobs, align_params, bset, bbox);

				auto box = alignedblobs.front()->bounding_box();
				int start_x = box.x;
				int start_y = box.y;
				box = alignedblobs.back()->bounding_box();
				int end_x = box.x;
				int end_y = box.ylast();
				bool at_least_2_crossings = atLeast2LineCrossing(alignedblobs);
				if ((alignedblobs.size() >= align_params.min_points && end_y - start_y >= align_params.min_length
					&& (align_params.ragged || end_y - start_y >= std::abs(end_x - start_x)*kMinTabGradient))
					|| at_least_2_crossings){
					int confirmed_pts = 0;
					for (auto iter = alignedblobs.begin(); iter != alignedblobs.end(); ++iter){
						auto bbox = *iter;
						if (align_params.right_tab){
							if (bbox->right_tab_type == align_params.confirmed_type)
								++confirmed_pts;
						}
						else{
							if (bbox->left_tab_type == align_params.confirmed_type)
								++confirmed_pts;
						}
					}
					if (!align_params.ragged || confirmed_pts * 2 < alignedblobs.size()){
						for (auto iter = alignedblobs.begin(); iter != alignedblobs.end(); ++iter){
							PBLOBNBOX bbox = *iter;
							if (align_params.right_tab){
								bbox->right_tab_type = align_params.confirmed_type;
							}
							else{
								bbox->left_tab_type = align_params.confirmed_type;
							}

						}
					}
				}
				


			}
		}


	}

}