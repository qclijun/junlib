#pragma once

#include <iostream>
#include "types.h"
#include "byteOp.h"

namespace jun{
	enum class MATRIX_OP{
		SANDD,SORD,DAND_NOTS_,
	};


	//支持submatrix，引用计数，复制操作共享数据
	template<typename T>
	class JMatrixBase{

	public:
		typedef T channel_type;
		typedef T* iterator_type;
		typedef const  T*  const_iterator_type;
		typedef JMatrixBase<T> Self;

		static const int BPC  { sizeof(channel_type) };//bypes per channel

		JMatrixBase(int rows,int cols,int channels);
		JMatrixBase(int rows, int cols, int channels, T val);
		
		JMatrixBase(const Self& other){
			cpymem(other, *this);
			increaseRef();
		}

		JMatrixBase(Self&& other){
			cpymem(other, *this);
			other._data = nullptr;
		}

		Self& operator=(const Self& other){
			if (this != &other){
				release();
				cpymem(&other, *this);
				increaseRef();
			}
			return *this;
		}

		Self& operator=(Self&& other){
			if (this != &other){
				release();
				cpymem(&other, *this);
				other._data = nullptr;
			}
			return *this;
		}

		~JMatrixBase(){
			release();
		}

	public:
		//submatrix
		JMatrixBase(const Self& other, int rowFirst, int colFirst, int newRow, int newCol){
			cpymem(&other, *this);
			increaseRef();
			_offset = toIndex(rowFirst, colFirst);
			_rows = newRow;
			_cols = newCol;
		}

		JMatrixBase(const Self& other, Rect rect) :Self(other, rect.y, rect.x, rect.height, rect.width){}

		Self subMatrix( Rect rect){	return{ *this, rect };}

		Self row(int rowFirst, int rowLast){return{ *this, rowFirst, 0, rowLast - rowFirst, cols() };}
		Self row(int rowFirst) { return row(rowFirst, rowFirst + 1); }
		Self col(int colFirst, int colLast){ return{*this,0,colFirst,rows(),colLast-colFirst}; }
		Self col(int colFirst){ return col(colFirst, colFirst + 1); }
	public:
		//iterator
		iterator_type begin() { return _data; }
		const_iterator_type begin() const { return _data; }

		iterator_type end() { return _data + cpl()*rows(); }
		const_iterator_type end() const { return _data + cpl()*rows(); }

		iterator_type iter(int row, int col=0){
			return begin() + toIndex(row,col);
		}
		const_iterator_type iter(int row, int col=0)const{
			return begin() + toIndex(row,col);
		}

		template<typename T2>
		T2* iter(int row, int col=0){ return (T2*)iter(row, col); }
		template<typename T2>
		const T2* iter(int row, int col=0) const{ return (const T2*)iter(row, col); }

		//access
		channel_type& operator()(int row, int col = 0){
			return *iter(row, col);
		}

		const channel_type& operator()(int row, int col = 0) const{
			return *iter(row, col);
		}

	public:
		void clearAll();
		bool isEmpty() const;
		
		void matrixOp(const Self& src, MATRIX_OP op, Point anchor);
		


	public:
		// get memthods
		int rows() const{ return _rows; }
		int cols() const { return _cols; }
		int channels() const { return _channels; }

		int totalCols() const{ return cols()*channels(); }
		int cpl() const{ return _cpl; } //channels per line 
		int bpl() const { return cpl()*BPC; } //bytes per line
		size_t elemNum() const{ return rows()*cols(); }
		size_t channelNum() const{ return elemNum()*channels(); }		
		size_t bytes() const { return channelNum()*BPC; }

		bool isSameDimension(const Self& other) const{
			return rows() == other.rows() && cols() == other.cols() && channels() == other.channels();
		}
		bool isContinues() const{ return cpl() == totalCols(); }

		Rect rect() const{ return Rect(0, 0, cols(), rows()); }

	private:
		void increaseRef(){ ++*_refcount; }
		int reduceRef(){ return --*_refcount; }
		//copy member
		void cpymem(const Self& m1, Self& m2){ std::memcpy(&m2, &m1, sizeof(Self)); } 
		int toIndex(int row, int col){ return _offset + row*cpl() + col*channels(); }

		void release(){
			if (_data == nullptr) return;
			if (reduceRef() == 0){
				delete[] _data;
				delete _refcount;
				_data = nullptr;
				_refcount = nullptr;
			}
		}
	private:
		int _rows;
		int _cols;
		int _channels;
		int _cpl;
		T* _data;
		int* _refcount;
		int _offset;
	}; // end of class JMatrixBase
	

} // end of namespace jun;



template<typename T>
inline bool operator==(const jun::JMatrixBase<T>& m1, const jun::JMatrixBase<T>& m2){
	if (&m1 == &m2) return true;
	if (!m1.isSameDimension(m2)) return false;
	for (int r = 0; r < m1.rows(); ++r){
		if (std::memcmp(m1.iter(r), m2.iter(r), m1.bpl()) == 0) return true;
	}

	return false;
}

template<typename T>
inline bool operator!=(const jun::JMatrixBase<T>& m1, const jun::JMatrixBase<T>& m2){
	return !(m1 == m2);
}


template<typename T>
std::ostream& operator<<(std::ostream& out, const jun::JMatrixBase<T>& m);



typedef jun::JMatrixBase<unsigned char> JMatrix;
typedef jun::JMatrixBase<unsigned> JMatrixI;
typedef jun::JMatrixBase<unsigned short> JMatrixS;
typedef jun::JMatrixBase<float> JMatrixF;
typedef jun::JMatrixBase<double> JMatrixD;

typedef jun::JMatrixBase<unsigned char> GrayImg;



#include "JMatrixBase.hpp"