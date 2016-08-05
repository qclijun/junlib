#pragma once

#include <vector>
#include <memory>
#include "types.h"
#include "DIR.h"
#include <iostream>


namespace jun{
	using std::vector;


	struct EdgeOffset{
		int8_t offset_numerator;
		uint8_t pixel_diff;
		uint8_t direction;
	};

	enum class OUTLINE_FLAGS :int16_t{
		NORMAL, INVERSE
	};


	// 一组封闭的轮廓线（可以为空），一般作为OUTLINEList或BLOBNBOX的一部分
	// 不允许创建OUTLINE的自动变量，只允许在动态空间中（使用newObj）创建
	// 由OUTLINEList或BLOBNBOX来释放空间



	class OUTLINE{
	private:
		OUTLINE() :stepcount(0){}

		OUTLINE(PointS startpt, RectS bound) :box(bound), start(startpt), stepcount(0){
			std::cout << "ctor: " << this << std::endl;
		}

		OUTLINE(PointS startpt, const vector<DIR>& new_steps);

	public:

		static std::shared_ptr<OUTLINE> newObj(){ return std::shared_ptr<OUTLINE>(new OUTLINE()); }
		static std::shared_ptr<OUTLINE> newObj(PointS startpt, RectS bound){ 
			return std::shared_ptr<OUTLINE>(new OUTLINE(startpt, bound));
		}
		static std::shared_ptr<OUTLINE> newObj(PointS startpt, const vector<DIR>& new_steps){
			return std::shared_ptr<OUTLINE>(new OUTLINE(startpt, new_steps));
		}

		~OUTLINE(){
			std::cout << "~OUTLINE(): "<<this << std::endl;
		}

		void set_step(int stepIndex, DIR stepdir){
			int shift = stepIndex % 4 * 2;
			uint8_t mask = 3 << shift;
			steps[stepIndex / 4] = ((stepdir.val() << shift) & mask) |
				(steps[stepIndex / 4] & ~mask);
		}

		DIR step_dir(int index) const {
			return (steps[index / 4] >> (index % 4 * 2)) & STEP_MASK;
		}

		int path_length() const{ return stepcount; }
		Point start_pos() const{ return static_cast<Point>(start); }
		Rect bounding_box() const{ return box; }

		Point position_at(int index) const{
			Point pos = start;
			for (int i = 0; i < index; ++i)
				pos += step_dir(index);
			return pos;
		}

		int area() const; //total area(include all level children)
		int perimeter() const; // perimeter(include first level children)

		const static int INTERSECTING = INT_MAX;

		//0:  pt在外面; 1: OUTLINE逆时针包围pt; -1: OUTLINE顺时针包围pt
		//INTERSECTING : OUTLINE穿过pt
		int winding_num(Point pt) const;

	private:
		static int elem_num(int stepcount){
			return (stepcount + 3) / 4;
		}
	private:

		RectS box;
		PointS start;
		int16_t stepcount;
		OUTLINE_FLAGS flags;
		std::vector<uchar> steps;
		std::vector<EdgeOffset> offsets;

		static const int STEP_MASK{ 3 };
	};

	typedef std::shared_ptr<OUTLINE> POUTLINE;




}




