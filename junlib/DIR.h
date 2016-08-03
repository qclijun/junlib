#pragma once

#include "Point.h"
#include <cstdint>

namespace{
	//0-w,1-n,2-e,3-s
	jun::Point directions[4] = { { -1, 0 }, { 0, -1 }, { 1, 0 }, { 0, 1 } };
}

namespace jun{
	//direction class
	class DIR{

	public:
		DIR(int val = 0){
			_val = static_cast<int8_t>(val & 0x3);
		}

		operator Point() const{
			return directions[_val];
		}

		operator PointS() const{
			return static_cast<PointS>(directions[_val]);
		}


		int x() const{ return static_cast<Point>(*this).x; } //use operator Point()
		int y() const{ return static_cast<Point>(*this).y; }

		int val() const{ return _val; }

		bool isReverse(DIR dir) const{
			return val() - dir.val() == 2 || (val() - dir.val() == -2);
		}

		static const DIR WEST;
		static const DIR NORTH;
		static const DIR EAST;
		static const DIR SOUTH;

	private:
		std::int8_t _val;

		
	};




}

