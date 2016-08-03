#include "LineFinder.h"
#include "Morph.h"
#include "MatrixUtil.h"
#include "floodfill.h"

#include <iostream>
#include <algorithm>

using std::max_element;
using std::deque;
using std::cout;
using std::endl;

using namespace jun::Morph;
using jun::LineFinder;

const  double LineFinder::kMaxStaveHeight = { 1.0 };// Max height of a music stave in inches.

const  double LineFinder::kThickLengthMultiple = { 0.75 };

const  double LineFinder::kMaxNonLineDensity = { 0.25 };

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

	v_empty = pix_vline.isEmpty();
	h_empty = pix_hline.isEmpty();

	//if (!v_empty&&!h_empty){
	//	FilterMusic();
	//}

	if (h_empty&&v_empty) return;
	BinaryJMatrix pix_nonlines(binary);
	if (!v_empty){
		pix_nonlines.substract(pix_vline);
	}
	if (!h_empty){
		pix_nonlines.substract(pix_hline);
	}

	if (!v_empty){
		writeImage("test-image_nonlines.png", pix_nonlines);
		Morph::erode_brick(pix_nonlines, pix_non_vline, kMaxLineResidue, 1);
		//writeImage("test-image_non_vline.png", pix_non_vline);

		FloodFill::seedFillBinary(pix_non_vline, pix_nonlines);
		//writeImage("test-image_non_vline2.png", pix_non_vline);
	}

	if (!h_empty){
		Morph::erode_brick(pix_nonlines, pix_non_hline, 1, kMaxLineResidue);
		FloodFill::seedFillBinary(pix_non_hline, pix_nonlines);
	}

	if (!h_empty&&!v_empty){
		matrixAnd(pix_vline, pix_hline, pix_intersections);
		pix_non_vline.or(pix_hline);
		pix_non_vline.substract(pix_intersections);
		writeImage("test-image_non_vline.png", pix_non_vline);

		pix_non_hline.or(pix_vline);
		pix_non_hline.substract(pix_intersections);
		writeImage("test-image_non_hline.png", pix_non_hline);

		if (filterFalsePositives(resolution, pix_non_vline, pix_vline, &pix_intersections) == 0)
			v_empty = true;
		if (filterFalsePositives(resolution, pix_non_hline, pix_hline, &pix_intersections) == 0)
			h_empty = true;
	}
	else if (!h_empty){ //v_empty == true
		if (filterFalsePositives(resolution, pix_non_hline, pix_hline, &pix_intersections) == 0)
			h_empty = true;
	}
	else{ // h_empty  == true;
		if (filterFalsePositives(resolution, pix_non_vline, pix_vline, &pix_intersections) == 0)
			v_empty = true;
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
		FloodFill::getConnComp(intersection_pix,box, rects2, FloodFill::Connectivity::EIGHT);
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


// Helper erases false-positive line segments from the input/output line_pix.
// 1. Since thick lines shouldn't really break up, we can eliminate some false
//    positives by marking segments that are at least kMinThickLineWidth
//    thickness, yet have a length less than min_thick_length.
// 2. Lines that don't have at least 2 intersections with other lines and have
//    a lot of neighbouring non-lines are probably not lines (perhaps arabic
//    or Hindi words, or underlines.)
// Bad line components are erased from line_pix.
// Returns the number of remaining connected components.
int LineFinder::filterFalsePositives(int resolution, Ty& nonline_pix,
	Ty& line_pix, Ty* intersection_pix ) {
	int min_thick_length = static_cast<int>(resolution*kThickLengthMultiple);

	deque<Rect> boxes;
	deque<BinaryJMatrix> comp_pixes;
	FloodFill::getConnComp(line_pix, boxes,comp_pixes, FloodFill::Connectivity::EIGHT);
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
			if (count > box.height * box.width * kMaxNonLineDensity) bad_line = true;
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

