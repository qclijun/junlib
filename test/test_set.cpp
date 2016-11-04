#include <iostream>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <random>
#include <iterator>
#include "types.h"
#include "test_set.h"
#include "boost\functional\hash.hpp"
#include "junlib\util.h"
#include "junlib\spdlog\mylogger.h"
using namespace std;
using namespace jun;

struct Point_Cmp{
	bool operator()(const jun::Point& p1, const jun::Point& p2){
		return p1.x < p2.x || (p1.x == p2.x && p1.y < p2.y);
	}
};

struct Point_Hash{
	size_t operator()(const Point& p){
		size_t seed = 0;
		boost::hash_combine(seed, p.x);
		boost::hash_combine(seed, p.y);
		return seed;
	}
};

//namespace jun{
//	template<typename T>
//	bool operator==(const BasicVec2<T>& left, const BasicVec2<T>& right){
//		return left.x == right.x&& left.y == right.y;
//	}
//}


void test_set(){
	
	mt19937 eng;
	uniform_int_distribution<int> x_dist(0, 30);
	uniform_int_distribution<int> y_dist(0, 100);
	
	using hasher = Point_Hash ;
	unordered_set<Point, hasher> p_set;
	const int N = 1000;
	for (int i = 0; i < N; ++i){
		p_set.emplace(x_dist(eng), y_dist(eng));
	}
	Point p{ 6, 30 };


	//search x正负3, y正负20的点
	set<Point, Point_Cmp> p_set2(p_set.begin(), p_set.end());

	p_set2.emplace(9, 50);

	int xmin = 6 - 3, xmax = 6 + 3;
	int ymin = 30 - 20, ymax = 30 + 20;


	auto b = p_set2.lower_bound(Point{ xmin,ymin });
	auto e = p_set2.upper_bound(Point{ xmax, ymax });


	auto iter = b;

	for (; iter != e; ++iter){
		if (iter->y > ymax){
			iter = p_set2.lower_bound(Point(iter->x + 1, ymin));
			cout << endl;
		}
		cout << *iter << ' ';
	}
	//cout << *e << endl;
	//while (true){
	//	cout << *iter << endl;
	//	if (++iter == e) break;
	//	if (iter->y > ymax){
	//		iter = p_set2.lower_bound(Point(iter->x + 1, ymin));
	//	}
	//}

}