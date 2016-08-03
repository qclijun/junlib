#include <iostream>
#include <cstdint>
using namespace std;



void test_bitand();
void test_bitor();
void test_bitany();


int count(uint64_t b)
{
	b = (b & 0x5555555555555555ULL) + (b >> 1 & 0x5555555555555555LU);
	b = (b & 0x3333333333333333LU) + (b >> 2 & 0x3333333333333333LU);
	b = b + (b >> 4) & 0x0F0F0F0F0F0F0F0FLU;
	b = b + (b >> 8);
	b = b + (b >> 16);
	b = b + (b >> 32) & 0x0000007F;

	return (int)b;
}

int  main2(int argc, char** argv){


	getchar();
	return 0;
}

