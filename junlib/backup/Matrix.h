#pragma once

#include <iostream>
#include <new>
#include "types.h"
#include "opencv2\core\cvdef.h"

JUN_BEGIN


//管理Matrix类的内存处理工作，是一个策略类，可以根据需要在将来更换。
class AbstractAlloc{
public:

	virtual void* allocate(size_t n) throw(){
		return ::operator new[](n);
	}

	virtual void deallocate(void* p) throw(){
		::operator delete[](p);
	}

};




class Matrix{

private:
	struct Mrep{
		int type;
		size_t sz;
		AbstractAlloc* pAlloc;
		int refcount;
		uchar* data;

		static long long allocatedMem;

		Mrep(){}

		explicit Mrep(int type_,size_t sz_, AbstractAlloc* pAlloc_ = new AbstractAlloc())
			:type(type_),sz(sz_), pAlloc(pAlloc_), refcount(1){
			construct(sz);
			std::memset(data, 0, sz);
		}

		~Mrep(){
			release();
		}

		void addRef(){
			++refcount;
		}

		int reduceRef(){
			return --refcount;
		}
	private:
		//Mrep(const Mrep&); default copy cstor
		//Mrep& operator=(const Mrep&);


		void construct(size_t n){
			data = (uchar*)pAlloc->allocate(n);//只分配内存，不进行构造，因为是简单对象
			allocatedMem += n;
		}

		void release(){
			pAlloc->deallocate(data);
			data = nullptr;
			allocatedMem -= sz;
		}

	};

	int rows_;
	int cols_;
	int originCols_;
	int offset_;
	Mrep *rep;


public:
	Matrix() :rep(nullptr){}
	
	Matrix(int rows, int cols, int type, AbstractAlloc* pAlloc = new AbstractAlloc())
		:rows_(rows), cols_(cols), originCols_(cols), offset_(0){
		rep = new Mrep(type, rows*cols*jun::elemSize(type), pAlloc);
	}
	
	~Matrix(){
		if (rep == nullptr) return;
		if (rep->reduceRef() == 0){
			delete rep;
			rep = nullptr;
		}
	}

	Matrix(const Matrix& m){
		copy0(&m, this);
		rep->addRef();
	}

	Matrix(Matrix&& m){
		copy0(&m, this);
		m.rep = nullptr;
	}

	Matrix& operator=(const Matrix& m){
		if (this != &m){
			if (rep->reduceRef() == 0){
				delete rep;
			}
			copy0(&m, this);
			rep->addRef();
		}
	}


	///////////////////////////////////////  sub matrix //////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
private:
	Matrix(const Matrix&m, int startRow, int startCol, int newRows, int newCols){
		copy0(&m, this);
		rep->addRef();
		offset_ = toIndex(startRow, startCol);
		rows_ = newRows;
		cols_ = newCols;
	}
public:
	Matrix subMatrix(int startRow, int startCol, int newRows, int newCols){
		checkIndex(startRow, startCol);
		checkIndex(startRow + newRows - 1, startCol + newCols - 1);
		return Matrix(*this, startRow, startCol, newRows, newCols);
	}

	Matrix row(int start, int end) { return subMatrix(start, 0, end - start, cols()); }
	Matrix col(int start, int end) { return subMatrix(0, start, rows(), end - start); }
	Matrix row(int r) { return row(r, r + 1); }
	Matrix col(int c) { return col(c, c + 1); }



public:
	// get methods:
	int rows() const{ return rows_; }
	int cols() const{ return cols_; }
	int originCols() const { return originCols_; }
	size_t totalCols() const { return cols()*channels(); }

	int type() const { return rep->type; }
	int channels() const { return getCN(type()); }
	int channelSize() const { return typeSize(type()); }
	int elemSize() const { return channelSize()*channels(); }

	size_t  bpl() const { return cols()*elemSize(); } //bytes per line

	size_t elemNum() const { return rows()*cols(); }
	size_t channelNum() const { return elemNum()*channels(); }

	size_t stride_b() const{ return stride_e()*elemSize(); } //byte stride
	size_t stride_e() const { return originCols(); }	// element stride
	size_t stride() const { return stride_e()*channels(); } // channel stride

	bool isContinues() const { return cols() == originCols(); }

	//////////////////////////////////////////////// access methods ///////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	T* ptr(int r, int c){
		return reinterpret_cast<T*>(data() + r*stride_b() + c*elemSize());
	}

	template<typename T>
	const T* ptr(int r, int c) const {
		return reinterpret_cast<T*>(data() + r*stride_b() + c*elemSize());
	}

	uchar* ptr(int r, int c){
		return (data() + r*stride_b() + c*elemSize());
	}

	const uchar* ptr(int r, int c) const{
		return (data() + r*stride_b() + c*elemSize());
	}


	static long long checkMem(){
		return Mrep::allocatedMem;
	}


private:
	// get channel index
	int toIndex(int rowIndex, int colIndex){
		return offset_ + (rowIndex*originCols() + colIndex)*channels();
	}

	void static copy0(const Matrix* src, Matrix* dest){
		std::memcpy(dest, src, sizeof(Matrix));
	}

	void checkIndex(int rowIndex, int colIndex) const;

	uchar* data() const { return (rep->data); }
};


JUN_END



std::ostream& operator<<(std::ostream& out, jun::Matrix& m);