#include <array>
#include <deque>
#include <cassert>

#include "MatrixOp.h"
#include "rasterOp.h"
#include "floodfill.h"
#include "MatrixUtil.h"
#include "junlib\util.h"

using namespace std;

using jun::BinaryJMatrix;
using jun::Point;
using jun::Rect;


struct seg{
	int x1, x2, y, dy;
	seg(int x1, int x2, int y, int  dy)
		:x1(x1), x2(x2), y(y), dy(dy){}
};

Rect floodfill4(BinaryJMatrix& m, Rect imgRegion, Point seed){
	assert(imgRegion.contains(seed));
	const int r0 = imgRegion.y, c0 = imgRegion.x;
	const int rows = imgRegion.ylast(), cols = imgRegion.xlast();



	int x = seed.x;
	int y = seed.y;
	int x1, x2, dy;
	int xmin, xmax, ymin = y, ymax = y;

	auto p = m.iter(seed.y, 0);
	assert(p[x] == 1);
	p[x] = 0;

	for (x1 = x - 1; x1 >= c0 && p[x1] == 1; --x1)
		p[x1] = 0;
	for (x2 = x + 1; x2 < cols && p[x2] == 1; ++x2)
		p[x2] = 0;
	xmin = ++x1;
	xmax = --x2;

	deque<seg> stack;
	stack.emplace_back(x1, x2, y, 1);
	stack.emplace_back(x1, x2, y, -1);


#pragma region main-loop

	while (!stack.empty()){
		auto elem = stack.back();
		stack.pop_back();
		x1 = elem.x1, x2 = elem.x2, y = elem.y, dy = elem.dy;
		if (x1 < xmin) xmin = x1;
		if (x2 > xmax) xmax = x2;
		if (y < ymin) ymin = y;
		if (y > ymax) ymax = y;

		y += dy;
		p = m.iter(y, 0);
		if (y < r0 || y >= rows) continue;

		int j1 = x1;
		for (; j1 >= c0 && p[j1] == 1; --j1)
			p[j1] = 0;
		if (j1 == x1) goto skip; //左边第一个像素为边界

		++j1;
		if (x1 - j1 >= 2) stack.emplace_back(j1, x1 - 1, y, -dy);//向左突出至少2

		int j2 = x1 + 1;
		for (; j2 < cols && p[j2] == 1; ++j2)
			p[j2] = 0;
		--j2;
		stack.emplace_back(j1, j2, y, dy);
		if (j2 - x2 >= 2) stack.emplace_back(x2 + 1, j2, y, -dy);//向右突出至少2
		j1 = j2 + 2;

	skip:
		//只向右搜素
		while (j1 <= x2){
			for (; j1 <= x2 && p[j1] != 1; ++j1);
			if (j1 > x2) break;
			assert(p[j1] == 1);
			p[j1] = 0;
			j2 = j1 + 1;
			for (; j2 < cols && p[j2] == 1; ++j2){
				p[j2] = 0;
			}
			--j2;
			stack.emplace_back(j1, j2, y, dy);
			if (j2 - x2 >= 2) stack.emplace_back(x2 + 1, j2, y, -dy);//向右突出至少2
			j1 = j2 + 2;
		}

	}

	//for (int i = x1; i <= x2; ++i){
	//	if (i < m.cols() && p[i] == 1){
	//		p[i] = 0;
	//		int j1 = i - 1;
	//		for (; j1 >= 0 && p[j1] == 1; --j1)
	//			p[j1] = 0;
	//		int j2 = i + 1;
	//		for (; j2 < m.cols() && p[j2] == 1; ++j2)
	//			p[j2] = 0;
	//		stack.emplace_back(j1 + 1, j2 - 1, y, dy);
	//		i = j2;
	//	}
	//}



#pragma endregion 

	return Rect(xmin, ymin, xmax - xmin + 1, ymax - ymin + 1);

}

Rect floodfill8(BinaryJMatrix& m, Rect imgRegion, Point seed){
	assert(imgRegion.contains(seed));
	const int r0 = imgRegion.y, c0 = imgRegion.x;
	const int rows = imgRegion.ylast(), cols = imgRegion.xlast();

	int x = seed.x;
	int y = seed.y;
	int x1, x2, dy;
	int xmin, xmax, ymin = y, ymax = y;

	auto p = m.iter(seed.y, 0);
	assert(p[x] == 1);
	p[x] = 0;

	for (x1 = x - 1; x1 >= c0 && p[x1] == 1; --x1)
		p[x1] = 0;
	for (x2 = x + 1; x2 < cols && p[x2] == 1; ++x2)
		p[x2] = 0;
	xmin = ++x1;
	xmax = --x2;

	deque<seg> stack;
	stack.emplace_back(x1, x2, y, 1);
	stack.emplace_back(x1, x2, y, -1);

#pragma region main-loop

	while (!stack.empty()){
		auto elem = stack.back();
		stack.pop_back();
		x1 = elem.x1, x2 = elem.x2, y = elem.y, dy = elem.dy;
		if (x1 < xmin) xmin = x1;
		if (x2 > xmax) xmax = x2;
		if (y < ymin) ymin = y;
		if (y > ymax) ymax = y;

		y += dy;
		p = m.iter(y, 0);
		if (y < r0 || y >= rows) continue;


		int j1 = x1 - 1;
		for (; j1 >= c0 && p[j1] == 1; --j1)
			p[j1] = 0;
		if (j1 == x1 - 1) goto skip; //左边第一个像素为边界

		++j1;
		if (x1 - j1 >= 1) stack.emplace_back(j1, x1 - 1, y, -dy);//向左突出至少1

		int j2 = x1;
		for (; j2 < cols && p[j2] == 1; ++j2)
			p[j2] = 0;
		--j2;
		stack.emplace_back(j1, j2, y, dy);
		if (j2 - x2 >= 1) stack.emplace_back(x2 + 1, j2, y, -dy);//向右突出至少1
		j1 = j2 + 2;

	skip:
		//只向右搜素
		while (j1 <= x2 + 1){
			for (; j1 <= x2 + 1 && p[j1] != 1; ++j1);
			if (j1 > x2 + 1) break;
			assert(p[j1] == 1);
			p[j1] = 0;
			j2 = j1 + 1;
			for (; j2 < cols && p[j2] == 1; ++j2){
				p[j2] = 0;
			}
			--j2;
			stack.emplace_back(j1, j2, y, dy);
			if (j2 - x2 >= 1) stack.emplace_back(x2 + 1, j2, y, -dy);//向右突出至少1
			j1 = j2 + 2;
		}

	}

#pragma endregion 

	return Rect(xmin, ymin, xmax - xmin + 1, ymax - ymin + 1);


}

Rect jun::FloodFill::floodfill(BinaryJMatrix& m, Rect imgRegion, Point seed, Connectivity connectivity){
	jun::Assert<Bad_Arg>(imgRegion.contains(seed));
	switch (connectivity){
	case Connectivity::FOUR:
		return floodfill4(m, imgRegion, seed);
	case Connectivity::EIGHT:
		return floodfill8(m, imgRegion, seed);
	default:
		throw Never_Reach_Here();
	}
}

bool nextPixOn(const jun::BinaryJMatrix& src, Rect imgRegion, int xstart, int ystart, int& x, int& y){
	auto p = src.iter(ystart, 0);
	for (auto c = xstart; c < imgRegion.xlast(); ++c){
		if (p[c]){
			y = ystart;
			x = c;
			return true;
		}
	}
	for (auto r = ystart + 1; r < imgRegion.ylast(); ++r){
		p = src.iter(r, 0);
		for (auto c = imgRegion.x; c < imgRegion.xlast(); ++c){
			if (p[c]){
				y = r;
				x = c;
				return true;
			}
		}
	}
	return false;
}

void jun::FloodFill::getConnComp( const BinaryJMatrix& m, Rect imgRegion, std::deque<Rect>& rects, Connectivity connectivity){
	int xstart = imgRegion.x, ystart = imgRegion.y;
	auto tmp(m);
	int x = 0, y = 0;
	while (true){
		if (!nextPixOn(tmp, imgRegion, xstart, ystart, x, y)) break;
		rects.push_back(floodfill(tmp, imgRegion, Point(x, y), connectivity));
		xstart = x;
		ystart = y;
	}
}

void jun::FloodFill::getConnComp( const BinaryJMatrix& m, Rect imgRegion, std::deque<Rect>& rects,
	std::deque<BinaryJMatrix>& pixes, Connectivity connectivity){
	int xstart = imgRegion.x, ystart = imgRegion.y;
	auto tmp1(m), tmp2(m);
	int x = 0, y = 0;
	while (true){
		if (!nextPixOn(tmp1, imgRegion, xstart, ystart, x, y)) break;
		Rect box = floodfill(tmp1, imgRegion, Point(x, y), connectivity);
		rects.push_back(box);

		auto tmp3 = clipRect(tmp1, box);
		auto tmp4 = clipRect(tmp2, box);

		tmp3.xor(tmp4); //被填充的像素

		rasterOp(tmp3, { 0, 0 }, Raster_OP::S_XOR_D, tmp2, box);
		pixes.emplace_back(std::move(tmp3));

		xstart = x;
		ystart = y;
	}
}

jun::JMatrixI jun::FloodFill::computeDistance(const BinaryJMatrix& mask, Connectivity connectivity){
	JMatrixI dist(mask.rows(), mask.cols(), 1);
	const int ymax = mask.rows() - 1, xmax = mask.cols() - 1;

	//边框初始化距离
	for (int r = 1; r < ymax; ++r){
		dist(r, 0) = mask(r, 0); //FG初始距离1，BG初始距离0
		dist(r, xmax) = mask(r, xmax);
	}
	auto pm = mask.iter(0, 0), pm2 = mask.iter(ymax, 0);
	auto pd = dist.iter(0, 0), pd2 = dist.iter(ymax, 0);
	for (int c = 0; c <= xmax; ++c){
		pd[c] = pm[c]; pd2[c] = pm2[c];
	}

	const int cpl = dist.cpl();

	switch (connectivity)
	{
	case jun::FloodFill::Connectivity::FOUR:
		//UL->LR
		for (int r = 1; r <ymax; ++r){
			pm = mask.iter(r);
			pd = dist.iter(r);
			for (int c = 1; c < xmax; ++c){
				if (pm[c]){
					pd[c] = std::min(pd[c - 1], pd[c - cpl]) + 1;
				}
			}
		}
		//LR->UL
		for (int r = ymax - 1; r >0; --r){
			pm = mask.iter(r);
			pd = dist.iter(r);
			for (int c = xmax - 1; c > 1; --c){
				if (pm[c]){
					int minval = std::min(pd[c + 1], pd[c + cpl]) + 1;
					if (pd[c] > minval) pd[c] = minval;
				}
			}
		}

		break;
	case jun::FloodFill::Connectivity::EIGHT:
		//UL->LR
		for (int r = 1; r <ymax; ++r){
			pm = mask.iter(r);
			pd = dist.iter(r);
			for (int c = 1; c < xmax; ++c){
				if (pm[c]){
					pd[c] = std::min( { pd[c - 1], pd[c - cpl], pd[c - cpl - 1], pd[c - cpl + 1]
						} ) + 1;
				}
			}
		}
		//LR->UL
		for (int r = ymax - 1; r >0; --r){
			pm = mask.iter(r);
			pd = dist.iter(r);
			for (int c = xmax - 1; c > 1; --c){
				if (pm[c]){

					int minval = std::min({ pd[c + 1], pd[c + cpl], pd[c + cpl - 1], pd[c + cpl + 1] }) + 1;
					if (pd[c] > minval) pd[c] = minval;
				}
			}
		}

		break;
	default:
		throw Never_Reach_Here();
	}


	return dist;
}


static const int MAX_ITERS = { 40 };
void seedFillBinary0(BinaryJMatrix& dst,BinaryJMatrix& mask_img,jun::FloodFill::Connectivity connectivity){
	assert(dst.isSameDimension(mask_img));

	int w = dst.cols();
	int h = dst.rows();
	const int bpw = sizeof(uint64_t); //byte per word
	int wpl = w / bpw;
	int rem = w % bpw;
	uint64_t word, mask;
	uint64_t wordabove, wordleft, wordbelow, wordright;
	uint64_t wordprev;

	switch (connectivity)
	{
	case jun::FloodFill::Connectivity::FOUR:
#pragma region "UL -> LR scan"
		for (int r = 0; r < h; ++r){
			auto p = (uint64_t*)dst.iter(r, 0);
			auto pm = (uint64_t*)mask_img.iter(r, 0);
#pragma region full word
			for (auto i = 0; i < wpl; ++i, ++p, ++pm){
				word = *p, mask = *pm;
				if (r > 0){
					wordabove = *(uint64_t*)((uchar*)p - dst.cols());
					word |= wordabove;
				}
				if (i > 0){
					wordleft = *(p - 1);
					word |= (wordleft >> (bpw - 1)*CHAR_BIT);
				}
				word &= mask;
				if (word == 0 || (word == jun::ALL_ONE)){
					*p = word;
					continue;
				}

				while (true){
					wordprev = word;
					word = (word | (word >> CHAR_BIT) | (word << CHAR_BIT))&mask;
					if (word == wordprev){
						*p = word;
						break;
					}
				}

			}
#pragma endregion
			//remaining bytes
			auto p2 = (uchar*)p, pm2 = (uchar*)pm;
			for (; pm2 < pm2 + rem; ++pm2, ++p2){
				*p2 = *pm&(*p2 | *(p2 - 1) | *(p2 - dst.cols()));
			}



		}
#pragma endregion

		//LR->UL  scan
		for (int r = h - 1; r >= 0; --r){
			auto p = (uint64_t*)dst.iter(r, dst.cols()) - 1;
			auto pm = (uint64_t*)mask_img.iter(r, mask_img.cols()) - 1;
			for (int i = wpl-1; i >= 0; --i,--p,--pm){
				word = *p, mask = *pm;
				if (r < h - 1){
					wordbelow = *(uint64_t*)((uchar*)p + dst.cols());
					word |= wordbelow;
				}
				if (i < wpl - 1){
					wordright = (*(p + 1)&0xff) << (bpw-1)*CHAR_BIT;
					word |= wordright;
				}
				word &= mask;
				if (word == 0 || (word == jun::ALL_ONE)){
					*p = word;
					continue;
				}
				while (true){
					wordprev = word;
					word = (word | (word >> CHAR_BIT) | (word << CHAR_BIT))&mask;
					if (word == wordprev){
						*p = word;
						break;
					}
				}

			}
			//remaining bytes
			auto p2 = (uchar*)p, pm2 = (uchar*)pm;
			for (; pm2 > pm2 - rem; --pm2, --p2){
				*p2 = *pm&(*p2 | *(p2 + 1) | *(p2 + dst.cols()));
			}
		}


		break;
	case jun::FloodFill::Connectivity::EIGHT:
		break;
	default:
		break;
	}


}
void jun::FloodFill::seedFillBinary(BinaryJMatrix& dst, BinaryJMatrix& mask, Connectivity connectivity){
	BinaryJMatrix tmp(dst.rows(),dst.cols());
	for (auto i = 0; i < MAX_ITERS; ++i){
		jun::copy(dst, tmp);
		seedFillBinary0(dst, mask, connectivity);
		if (dst == tmp) break;
	}

}


