#pragma once

#include <algorithm>

#include "JMatrixBase.h"

namespace jun{


	class BinaryJMatrix : public JMatrix{
	public:
		typedef JMatrix BaseTy;
		typedef BinaryJMatrix Self;
		BinaryJMatrix(int row, int col) :BaseTy(row, col, 1){}
		BinaryJMatrix(int row, int col, int) :BaseTy(row, col, 1){}

		BinaryJMatrix(const Self& m) :BaseTy(m){}

		BinaryJMatrix(const Self& m, bool copyData):BaseTy(m,copyData){
		}

		BinaryJMatrix(Self&& m):BaseTy(std::move(m)){
		}
	
		BinaryJMatrix& operator=(const Self& m){
			//if (this == &m) return *this;
			return *(Self*)&BaseTy::operator=(m);
		}
	public:
		BinaryJMatrix& operator=(Self&& m){
			//if (this == &m) return *this;
			return  *(Self*)&BaseTy::operator=(std::move(m));
		}


		void substract(const Self& src){
			bitandnot(src.begin(), src.bytes(), begin());
		}
		
		void setAll(Rect rect){
			Assert<Bad_Arg>(!ARG_CHECK || (this->rect().contains(rect)));
			auto p = iter(rect.y, rect.x);
			for (int r = 0; r < rect.height; ++r){
				std::memset(p, 1, rect.width*BPC);
				p += cpl();
			}
		}

		void setAll(){
			memset(begin(), 1, bytes());
		}

		 int channels() const{ return 1; }
	};

}
