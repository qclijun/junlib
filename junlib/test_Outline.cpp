#include <iostream>
#include <vector>


#include "Outline.h"
#include "test_Outline.h"

using namespace std;
using namespace jun;

void test_OL(){
	PointS startpt{ 1, 2 };


	DIR steps[]{
		DIR::EAST, DIR::NORTH, DIR::EAST, DIR::SOUTH, DIR::EAST, DIR::SOUTH,
			DIR::SOUTH, DIR::WEST, DIR::WEST,DIR::NORTH,DIR::WEST,DIR::NORTH
	};	
	vector<DIR> dirs(rbegin(steps), rend(steps));
	auto ol = OUTLINE::newObj(startpt, dirs);

	Point pos = ol->start_pos();
	for (int i = 0; i < ol->path_length(); ++i){
		DIR d = ol->step_dir(i);
		cout << "DIR: " << (Point)d;
		pos += d;
		cout << " pos: " << pos << endl;
	}
	//auto child = ol.child();
	//if (child!=nullptr)
	
	

	OUTLINE* ol2 = ol.get();
	//ol2.set_step(1, DIR::SOUTH);
	cout << ol->area() << endl;
	//ol.addChild(ol2);
	//ol.child()->push_back(ol2);//自我循环，将自己添加到children导致无限循环
	//cout << ol2.area() << endl;
	//cout << (ol == ol2) << endl;
}

