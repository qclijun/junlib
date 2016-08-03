#include "Outline.h"
#include <cassert>

namespace jun{

	OUTLINE::OUTLINE(PointS startpt, const vector<DIR>& new_steps)
		:start(startpt), stepcount(static_cast<int16_t>(new_steps.size())), steps(elem_num(stepcount)){
		PointS pos = start;
		box = RectS(pos, pos);
		auto lastdir = new_steps.back();
		auto prevdir = lastdir;
		int stepindex = 0;
		for (int srcindex = 0; srcindex < new_steps.size(); ++srcindex, ++stepindex){
			auto dir = new_steps[srcindex];
			set_step(stepindex, dir);
			pos += dir;
			box += pos;
			if (dir.isReverse(prevdir) && stepindex > 0){
				stepindex -= 2;
				prevdir = stepindex == -1 ? lastdir : step_dir(stepindex);
			}
			else{
				prevdir = dir;
			}
		}

		assert(pos.x == start.x&&pos.y == start.y);//轮廓必须封闭
		int begin = 0;
		while (stepindex - 1 > begin && step_dir(begin).isReverse(step_dir(stepindex - 1))){
			start += step_dir(begin); //起点向前移
			++begin;
			--stepindex; //cancel there-and-back
		}
		for (int i = begin; i < stepindex; ++i){
			set_step(i - begin, step_dir(i));
		}
		stepcount = stepindex - begin;
		assert(stepcount >= 4);

	}

	int OUTLINE::area() const{
		Point pos = start_pos();
		int total = 0;
		for (int i = 0; i < path_length(); ++i){
			DIR dir = step_dir(i);
			Point dir_vec = dir;
			if (dir_vec.x > 0)
				total += pos.y;
			else if (dir_vec.x < 0)
				total -= pos.y;
			pos += dir_vec;

		}
		return total;
	}

	inline int OUTLINE::perimeter() const{
		int total = path_length();
		return total;
	}

	int OUTLINE::winding_num(Point pt) const{
		Point vec = start_pos() - pt;
		Point step_vec;
		int cross_product;
		int count = 0;

		for (int i = 0; i < path_length(); ++i){
			step_vec = step_dir(i);
			if (vec.y <= 0 && vec.y + step_vec.y > 0){
				cross_product = vec.cross_product(step_vec);
				if (cross_product > 0) //右边向上
					++count;
				else if (cross_product == 0)
					return INTERSECTING;
			}
			else if (vec.y > 0 && vec.y + step_vec.y <= 0){
				cross_product = vec.cross_product(step_vec);
				if (cross_product < 0) //右边向下
					--count;
				else if (cross_product == 0)
					return INTERSECTING;
			}
			vec += step_vec;
		}
		return count;
	}
}

