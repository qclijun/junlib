#pragma once

#include <deque>
#include <set>

#include "JMatrix.h"
#include "BLOBNBOX.h"
#include "AlignedBlob.h"
#include "junlib\spdlog\mylogger.h"
#include "fmt\ostream.h"

namespace jun{

	class LineFinder{
	private:

	public:


		LineFinder(BinaryJMatrix& binary_) :binary(std::move(binary_)),
			pix_closed(binary, false), pix_vline(binary, false), pix_non_vline(binary, false),
			pix_hline(binary, false), pix_non_hline(binary, false), pix_intersections(binary, false),
			pix_music_mask(binary, false)
		{
			v_empBinaryJMatrix = false;
			h_empBinaryJMatrix = false;
		}



		void getLineMasks();
		void findAndRemoveVLines();
		

	private:
		void FilterMusic();
		static int filterFalsePositives(int resolution, BinaryJMatrix& nonline_pix,
			BinaryJMatrix& line_pix, BinaryJMatrix* intersection_pix = nullptr);

		template<typename Cmp>
		void getLineBlobs(bool horizontal_lines, BinaryJMatrix* pix_lines, BinaryJMatrix* pix_intersections,
			std::set<PBLOBNBOX, Cmp>& result);


		PBLOBNBOX findAlignedBlob(const jun::AlignedBlobParams& p, std::set<PBLOBNBOX, BoxCmp_LT<BLOBNBOX>>& bset, PBLOBNBOX bbox);

		void findAlignedBlobs(std::deque<PBLOBNBOX>& aligned_blobs,const AlignedBlobParams& p, std::set<PBLOBNBOX, BoxCmp_LT<BLOBNBOX>>& bset, PBLOBNBOX bbox);


		void findVerticalAlignment(const AlignedBlobParams& p, std::set<PBLOBNBOX, BoxCmp_LT<BLOBNBOX>>& bset, PBLOBNBOX bbox);


		static int numTouchingIntersections(Rect box, BinaryJMatrix* intersection_pix);

		static mylogger::spdlog_ptr& logger(){
			static mylogger::spdlog_ptr s_logger{ mylogger::get_logger("jun::LineFinder") };
			return s_logger;
		}

	private:

		BinaryJMatrix  binary;
		BinaryJMatrix  pix_closed;
		BinaryJMatrix  pix_vline;        //  candidate vertical lines.
		BinaryJMatrix  pix_non_vline;    // pixels that didn't look like vertical lines.
		BinaryJMatrix  pix_hline;    //   candidate horizontal lines.
		BinaryJMatrix  pix_non_hline;  //   pixels that didn't look like horizontal lines.
		BinaryJMatrix  pix_intersections; //pixels where vertical and horizontal lines meet.
		BinaryJMatrix  pix_music_mask;  //candidate music staves.
		bool v_empBinaryJMatrix; //vline is empBinaryJMatrix?
		bool h_empBinaryJMatrix;

		int resolution{ 300 };
		//std::deque<BinaryJMatrix*> pixa_display;

		const static int kThinLineFraction{ 20 };
		const static int kMinLineLengthFraction{ 4 };
		// Max size of line residue. (The pixels that fail the long thin opening, and
		// therefore don't make it to the candidate line mask, but are nevertheless
		// part of the line.)
		const static int kMaxLineResidue{ 6 };
		const static int kMinThickLineWidth{ 12 };
		const static int kCrackSpacing{ 100 };

		

		const static double kThickLengthMultiple;
		const static double kMaxStaveHeight;
		const static double kMaxNonLineDensiBinaryJMatrix;

	};



	//将line_pix转换为blobnbox的集合,由cmp指定排序算法
	template<typename Cmp>
	void LineFinder::getLineBlobs(bool horizontal_lines, BinaryJMatrix* pix_lines, BinaryJMatrix* pix_intersections,
		std::set<PBLOBNBOX, Cmp>& result){

		const int h = pix_lines->rows();
		const int w = pix_lines->cols();
		const int cpl = pix_lines->cpl();

		if (horizontal_lines){ //水平线
			auto it = pix_lines->begin();
			for (int r = 0; r < h; ++r, it += cpl){
				for (int c = 0; c < w; c += kCrackSpacing){
					it[c] = 0;   //clear bit
				}
			}
		}
		else{
			auto it = pix_lines->begin();
			for (int r = 0; r < h; r += kCrackSpacing){
				std::memset(it + r*cpl, 0, cpl);
			}
		}
		std::deque<Rect> boxes;
		FloodFill::getConnComp(*pix_lines, boxes, FloodFill::Connectivity::EIGHT);
		while (!boxes.empty()){
			Rect& curr = boxes.back();
			PBLOBNBOX newBLOB = BLOBNBOX::newObj();
			newBLOB->line_crossings = numTouchingIntersections(curr, pix_intersections);
			if (horizontal_lines){ //flip xy
				newBLOB->set_bounding_box(Rect{ curr.y, curr.x, curr.height, curr.width });
			}
			else{
				newBLOB->set_bounding_box(curr);
			}
			newBLOB->left_tab_type = TabType::TT_MAYBE_ALIGNED;
			result.insert(newBLOB);
			boxes.pop_back();
		}

	}

}