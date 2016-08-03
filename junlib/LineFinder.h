#pragma once

#include <deque>
#include "JMatrix.h"

JUN_BEGIN

class LineFinder{
private:
	typedef BinaryJMatrix Ty;
public:
	

	LineFinder(BinaryJMatrix& binary_) :binary(std::move(binary_)),
		pix_closed(binary, false), pix_vline(binary, false), pix_non_vline(binary, false),
		pix_hline(binary, false), pix_non_hline(binary, false), pix_intersections(binary, false),
		pix_music_mask(binary, false)
	{
		v_empty = false;
		h_empty = false;
	}



	void getLineMasks();
	

private:
	void FilterMusic();
	static int filterFalsePositives(int resolution, Ty& nonline_pix,
		Ty& line_pix, Ty* intersection_pix = nullptr);
private:

	Ty  binary;
	Ty  pix_closed;
	Ty  pix_vline;        //  candidate vertical lines.
	Ty  pix_non_vline;    // pixels that didn't look like vertical lines.
	Ty  pix_hline;    //   candidate horizontal lines.
	Ty  pix_non_hline;  //   pixels that didn't look like horizontal lines.
	Ty  pix_intersections; //pixels where vertical and horizontal lines meet.
	Ty  pix_music_mask;  //candidate music staves.
	bool v_empty; //vline is empty?
	bool h_empty;

	int resolution = { 300 };
	//std::deque<BinaryJMatrix*> pixa_display;

	const static int kThinLineFraction = { 20 };
	const static int kMinLineLengthFraction = { 4 };
	// Max size of line residue. (The pixels that fail the long thin opening, and
	// therefore don't make it to the candidate line mask, but are nevertheless
	// part of the line.)
	const static int kMaxLineResidue = { 6 };
	const static int kMinThickLineWidth = { 12 };
	const static double kThickLengthMultiple;
	const static double kMaxStaveHeight; 
	const static double kMaxNonLineDensity;

};

JUN_END