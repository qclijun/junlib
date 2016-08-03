#include <list>
#include <set>
#include <unordered_set>
#include <vector>
#include <deque>
#include <valarray>
#include <iostream>
#include "test_JContainer.h"
#include "OUTLINE_LIST.h"
#include "BBGrid.h"

using namespace std;
using jun::JContainer;
using jun::BBGrid;

struct Obj{
	Obj(){ cout << "Obj()" << endl; }
	~Obj(){ cout << "~Obj()" << endl; }
};

struct BigObject{
	int val;
	int *  pval;
	BigObject(int i) :val(i), pval(new int(i)){
		cout << "ctor: " << val << ", " << pval << endl;
	}
	~BigObject(){
		cout << "dstor: " << val << ", " << pval << endl;
		delete pval;
	}
};



void test_JContainer(){

	JContainer<BigObject, std::vector<std::shared_ptr<BigObject>> > cc;
	for (int i = 0; i < 10; ++i){
		cc.add(std::make_shared<BigObject>(i));
	}

	//{
	//	BigObject autoObj(11);
	//	cc.push_back(&autoObj);
	//}


	for (auto it = cc.begin(); it != cc.end(); ++it){
		cout << (*it)->val << ' ';
	}
	//auto cc2 = cc;



}


void test_BBGrid(){
	const int SIZE[] = { 30, 20, 20, 30 };

	BBGrid<jun::OUTLINE_LIST> grid(jun::Rect(0, 0, 100, 100), 10);

	cout << typeid(jun::OUTLINE_LIST::container_type).name() << endl;
	cout << typeid(jun::container_category<std::vector<int>>::container_tag).name() << endl;

	const int N = 30;
	
	vector<jun::OUTLINE*> vec(N);
	for (int i = 0; i < N; ++i){
		jun::PointS startpt(i * 5, i * 5);
		jun::BasicSize<short> sz(SIZE[std::rand() & 0x3]);
		auto ol = jun::OUTLINE::newObj(startpt, jun::RectS(startpt, sz));
		vec[i]=(ol.get());	
		if (ol->bounding_box().xlast() < 100){
			grid.insertBBox(false, false, ol);
		}
	}

	for (auto c : vec)
		cout << c->bounding_box() << ' ';
	cout <<  endl;
	//搜索left>=0 <=50的OUTLINE

	auto ol = vec[10];
	cout << ol->bounding_box() << endl;


	//第一行的元素
	for (int y = 0; y < grid.rows(); ++y){
		for (int x = 0; x < grid.cols(); ++x){
			auto c = grid(x, y);
			
			for (auto iter = c.begin(); iter != c.end(); ++iter){
				if ((*iter).get() == ol) cout << "found at (" << x << ", " << y << ")" << endl;
			}
		}
		cout << endl;
	}


}
