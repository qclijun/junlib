
#include <iostream>
#include <deque>
#include <array>
#include <string>
#include <complex>
#include <vector>
#include <tuple>
#include <typeinfo>
#include <algorithm>
#include <numeric>
#include <future>

#include "junlib\util.h"
#include "junlib\je_allocator.h"
#include "JMatrix.h"
#include "MatrixUtil.h"
#include "filter.h"
#include "threshold.h"
#include "Morph.h"
#include "LineFinder.h"

#include "opencv2\core.hpp"
#include "opencv2\imgproc.hpp"

#include "test_Outline.h"
#include "test_JContainer.h"
#include "BLOBNBOX.h"


using namespace std;
using namespace jun;
using namespace cv;



void test_dilate_h();

void test_rasterOp();
void test_erode_h();

void  test_byteop();

jun::Rect floodfill4(BinaryJMatrix& m,jun::Rect rect, jun::Point seed);

void testFF4(){
	JMatrix m = readImage("ff.png", ReadImageFlag::GRAYSCALE);

	BinaryJMatrix img = gray2b(m);
	writeImage("ffb.png", img);
	jun::Point seed(60, 55);

	jun::Rect result = floodfill4(img,img.rect(), seed);
	writeImage("ff3.png", img);
	cout << result << endl;
}


struct A{
	int x, y;

};

inline bool operator==(const A& left, const A& right){
	return left.x == right.x && left.y == right.y;
}

inline bool operator<(const A& left, const A& right){
	return true;
}

template<typename T>
struct ACmp{
	bool operator()(T* left, T* right){
		if (left->x < right->x) return true;
		if (right->x < left->x) return false;
		return left < right;
	}
};



std::ostream& operator<<(std::ostream&  out, const A& a){
	return out << a.x << ", " << a.y ;
}


template<size_t N>
struct binary{
	static const int value{ binary<N / 10>::value * 2 + N % 10 };
};

template<>
struct binary < 0 > {
	static const int value{ 0 };
};




//#define _DEBUG_LT_PRED(pred, x, y)  pred(x, y)



int main(int argc, char** argv){
	//je_malloc_conf = "narenas:2";
	//int narenas = 0;
	//size_t sz = sizeof(narenas);
	//je_mallctl("opt.narenas", &narenas, &sz, NULL, 0);
	//cout << narenas << endl;



	cout << sizeof(BLOBNBOX) << endl;
	cout << sizeof(OUTLINE_LIST) << endl;
	cout << sizeof(std::shared_ptr<int>) << endl;


	A a1{ 3, 4 };
	A a2{ 3, 5 };
	cout << (a1 < a2) << endl;
	cout << (a2 < a1) << endl;
	ACmp<A> cmp;
	cout << cmp(&a1, &a2) << endl;

	std::set<A*, ACmp<A>> myset;
	myset.insert(&a1);
	myset.insert(&a2);
	myset.insert(&a1);
	for (auto c : myset){
		cout << c->x << ',' << c->y << endl;
	}

	double d = -0.5;
	int i = (d<0?-1:1);
	cout << i << endl;

	cout << binary<101010101>::value << endl;

	//test_BBGrid();
	{
		//testFF4();
	//test_dilate_h();
		// test_byteop();

		//string filename = "test-image_b.png";
		//auto index = filename.find_last_of('.');
		//if (index == string::npos) throw runtime_error("no file extension");

		//string name = filename.substr(0, index);
		//string ext = filename.substr(index);

		//JMatrix m = readImage(filename,ReadImageFlag::GRAYSCALE);
		//BinaryJMatrix binary = gray2b(m);
		//cout << m.rows() << ' ' << m.cols() << ' ' << m.channels() << endl;
		//
		//LineFinder finder(binary);
		//finder.getLineMasks();

		




		//Matrix m_solid = m.cloneEmpty();
		//Morph::openBrick(m_closed, m_solid, max_line_width, max_line_width);
		//display.push_back(m_solid);

		//Matrix m_hollow = Morph::subtract(m_closed, m_solid);
		//display.push_back(m_hollow);
		//writeImage("test-image_hollow.png", m_hollow);

		//Matrix m_vline = m_hollow.cloneEmpty();
		//Matrix m_hline = m_hollow.cloneEmpty();

		//Morph::openBrick(m_hollow,m_vline, 1, min_line_length);
		//Morph::openBrick(m_hollow,m_hline, min_line_length, 1);
		//writeImage("test-image_vline.png", m_hollow);
		//writeImage("test-image_hline.png", m_hollow);

		//m.init(std::rand);
		//cout << m << endl;
		// 
		//MatrixI kernel(3, 3, 1);
		//kernel.init({ 0, -1, 0, -1, 5, -1, 0, -1, 0 });
		//cout << kernel << endl;
		//
		//filter2D(m, m, kernel);
		//cout << m << endl;




		/*BasicMatrix<uchar> m2(3, 4, 1);
		for (int r = 0; r < m2.rows(); ++r){
		m2.copyRow(m2.chIter(r, 0), m.ptr(r, 0));
		}*/


		//BasicMatrix<uchar> result(m.rows, m.cols, CV_MAT_CN(m.type()));
		//for (int r = 0; r < m.rows; ++r){
		//	result.copyRow(result.chIter(r, 0), m.ptr(r, 0));
		//}
		//return result;
		//cout << result << endl;


	}








	cout << "hello" << endl;







	std::getchar();
	return 0;
}










