#include <iostream>
#include <set>
#include <algorithm>
#include <random>
#include <iterator>
#include "test_set.h"
#include "junlib\util.h"
using namespace std;

void test_set(){
	set<int> myset;
	mt19937 eng;
	uniform_int_distribution<int> dist(0, 100);
	const int N = 20;
	for (int i = 0; i < N; ++i){
		myset.insert(dist(eng));
	}




	auto b = myset.lower_bound(22);
	auto e = myset.lower_bound(60);
	jun::print_list(b, e);

	jun::print_list(myset);

}