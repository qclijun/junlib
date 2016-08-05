#pragma once
#include "JMatrix.h"
#include <deque>

JUN_BEGIN

namespace  FloodFill{

	enum class Connectivity{
		FOUR = 4, EIGHT = 8
	};

	Rect floodfill(BinaryJMatrix& m, Rect imgRegion, Point seed, Connectivity connectivity = Connectivity::EIGHT);

	inline Rect floodfill(BinaryJMatrix& m, Point seed, Connectivity connectivity = Connectivity::EIGHT){
		return floodfill(m, m.rect(),  seed,connectivity);
	}

	void getConnComp(const BinaryJMatrix& m, Rect imgRegion, std::deque<Rect>& rects, Connectivity connectivity = Connectivity::EIGHT);

	inline void getConnComp(const BinaryJMatrix& m, std::deque<Rect>& rects, Connectivity connectivity = Connectivity::EIGHT){
		getConnComp(m, m.rect(),rects, connectivity);
	}

	void getConnComp(const BinaryJMatrix& m, Rect imgRegion, std::deque<Rect>& conn_boxes,
		std::deque<BinaryJMatrix>& conn_pixes, Connectivity connectivity);

	inline void getConnComp(const BinaryJMatrix& m, std::deque<Rect>& conn_boxes,
		std::deque<BinaryJMatrix>& conn_pixes, Connectivity connectivity){
		getConnComp(m, m.rect(), conn_boxes, conn_pixes, connectivity);
	}

	//�õ��������ÿ�����ص��ֵ��ʾ�Ӹõ��������ڣ�4��ͨ�����ͨ�������ƶ���BG���ص���̾���
	//���Ӷ�4*O(N)��8*O(N)
	JMatrixI computeDistance(const BinaryJMatrix& src, Connectivity connectivity);

	void seedFillBinary( BinaryJMatrix& dst, BinaryJMatrix& mask, Connectivity connectivity = Connectivity::EIGHT);


}

JUN_END
