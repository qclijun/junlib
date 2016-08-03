#pragma once

#include <iostream>
#include <memory>
#include <iterator>
#include <initializer_list>
#include <array>
#include "types.h"
#include "junlib\je_allocator.h"
#include "junlib\util.h"

using std::array;

namespace jun{

	////--------------------------------Matrix------------------------------------//
	/////////////////////////////////////////////////////////////////////////////////

	//T 必须是简单类型，
	template<typename T, typename Alloc = JUN_ je_allocator<T> >
	class BasicMatrix{
		
	public:
		//typedefs:
		typedef BasicMatrix<T, Alloc> MyType;
		typedef Alloc allocType;
		typedef T  channelType;
		typedef T* chPtr;
		typedef const T* const_chPtr;


	private:
		struct Mrep{			
			size_t sz_;
			allocType alloc_;
			int refcount_;
			T* data_;	

			static long long allocatedMem;

			Mrep(){}

			Mrep(size_t sz, T val, allocType alloc = allocType())
				:sz_(sz), alloc_(alloc),refcount_(1){
				construct(sz);
				std::uninitialized_fill_n(data_, sz, val);
			}

			Mrep(size_t sz, allocType alloc = allocType())
				:sz_(sz), alloc_(alloc), refcount_(1){
				construct(sz);
				//std::uninitialized_fill_n(data_, sz, T());
			}

			~Mrep(){
				release();
			}

			void addref(){
				++refcount_;
			}

			int reduceRef(){
				return --refcount_;
			}
		private:
			//Mrep(const Mrep&); default copy cstor
			//Mrep& operator=(const Mrep&);


			void construct(size_t n){
				data_ = alloc_.allocate(n);//只分配内存，不进行构造，因为是简单对象
				allocatedMem += n*sizeof(T);
			}

			void release(){			
					alloc_.deallocate(data_, sz_);
					data_ = nullptr;
					allocatedMem -= sz_*sizeof(T);				
			}

		};

		int rows_;
		int cols_;
		int channels_;
		int originCols_;
		int offset_;
		Mrep *rep;

		typedef typename allocType::template rebind<Mrep>::other repAllocType;
		repAllocType repAlloc;
	public:
		BasicMatrix():rep(nullptr){	
		}

		BasicMatrix(int rows, int cols, int channels, T val , const Alloc& a = Alloc()){
			create(rows, cols, channels, val, a);
		}

		BasicMatrix(int rows, int cols, int channels, const Alloc& a = Alloc()){
			create(rows, cols, channels, a);
		}

		~BasicMatrix(){
			if (rep == nullptr)
				return;
			if ((rep->reduceRef() == 0)){
				repAlloc.destroy(rep);
				repAlloc.deallocate(rep, 1);
				rep = nullptr;
			}
				
		}


		BasicMatrix(const BasicMatrix& m){
			copy0(&m, this);
			rep->addref();
		}

		///复制构造函数复杂度O(1)，语义是share resources，没有必要使用move constructor(steal resources)
		BasicMatrix(BasicMatrix&& m) {
			copy0(&m, this);
			m.rep = nullptr;
		}

	
		BasicMatrix& operator=(const BasicMatrix& m){
			if (this != &m){
				if (rep->reduceRef() == 0){
					//delete rep;
					repAlloc.destroy(rep);
					repAlloc.deallocate(rep, 1);
				}					
				copy0(&m, this);
				rep->addref();
			}
		}
		BasicMatrix& operator=(BasicMatrix&& m){
			if (this != &m){
				if (rep->reduceRef() == 0){
					//delete rep;
					repAlloc.destroy(rep);
					repAlloc.deallocate(rep, 1);
				}
				copy0(&m, this);
				m.rep = nullptr;
			}
		}

		template<typename InputIt>
		void init(InputIt first, InputIt last);

		void init(std::initializer_list<double> dl);

		template<typename Func>
		void init(Func f);


		//---------------------------sub BasicMatrix ----------------------------------
		///////////////////////////////////////////////////////////////////////////

private:
		BasicMatrix(const BasicMatrix&m, int startRow, int startCol, int newRows, int newCols){			
			copy0(&m, this);
			rep->addref();
			offset_ = toIndex(startRow, startCol);
			rows_ = newRows;
			cols_ = newCols;
		}
public:
		BasicMatrix subMatrix(int startRow, int startCol, int newRows, int newCols){
			checkIndex(startRow, startCol);
			checkIndex(startRow + newRows - 1, startCol + newCols - 1);
			return BasicMatrix(*this, startRow, startCol, newRows, newCols);
		}

		BasicMatrix row(int start, int end) { return subMatrix(start, 0, end - start, cols()); }
		BasicMatrix col(int start, int end) { return subMatrix(0, start, rows(), end - start); }
		BasicMatrix row(int r) { return row(r, r + 1); }
		BasicMatrix col(int c) { return col(c, c + 1); }



		////--------------------------get function --------------------------
		int rows() const{ return rows_; }
		int cols() const{ return cols_; }
		int channels() const { return channels_; }
		int originCols() const { return originCols_; }
		int totalCols() const { return cols()*channels(); }
		size_t  bpl() const { return totalCols()*sizeof(channelType); } //bytes per line

		size_t elemNum() const { return rows()*cols(); }
		size_t channelNum() const { return elemNum()*channels(); }

		size_t stride_b() const{ return stride()*sizeof(channelType); } //byte stride
		size_t stride_e() const { return originCols(); }	// element stride
		size_t stride() const { return stride_e()*channels(); } // channel stride

		bool isContinues() const { return cols() == originCols(); }

		allocType get_allocator() const { return rep->alloc_; }

		void setAll(double val);

		BasicMatrix<T, Alloc> clone() const;
		BasicMatrix<T, Alloc> cloneEmpty() const{
			return BasicMatrix(rows(), cols(), channels());
		}


		// access methods

		//access channels
		chPtr chIter(int rowIndex, int colIndex = 0){
			return rep->data_ + toIndex(rowIndex, colIndex);
		}

		const_chPtr chIter(int rowIndex, int colIndex = 0)  const{
			return rep->data_ + toIndex(rowIndex, colIndex);
		}

		 channelType& operator()(int r, int c){
			return rep->data_[toIndex(r, c)];
		}

		 const channelType& operator()(int r, int c) const{
			return rep->data_[toIndex(r, c)];
		}

		 channelType& at(int row, int col){
			 checkIndex(row, col);
			return rep->data_[toIndex(row, col)];
		}

		 const channelType& at(int row, int col) const{
			 checkIndex(row, col);
			return rep->data_[toIndex(row, col)];
		}

		 void copyPix(chPtr dst, const_chPtr src){
			 std::memcpy(dst, src, channels()*sizeof(channelType));
		 }

		 void copyRow(chPtr dst, const_chPtr src){
			 std::memcpy(dst, src, bpl());
		 }

		 static void copy(const BasicMatrix& src, BasicMatrix& dst){
			 for (int r = 0; r < src.rows(); ++r){
				 std::memcpy(dst.chIter(r, 0), src.chIter(r, 0), dst.bpl());
			 }
		 }


		 //将dl复制给从（row,col)开始的连续的dl.size()个channel,仅限一行
		 void put(int row, int col, std::initializer_list<double> dl){
			 if (col*channels() + dl.size() > totalCols())
				 throw out_of_range("");
			 chPtr p = chIter(row, col);
			 for (auto p2 = begin(dl); p2 != end(dl);)
				 *p++ = static_cast<channelType>(*p2++);
		 }
		 
		 bool isSameDimension(const BasicMatrix& m) const{
			 return rows() == m.rows() && cols() == m.cols() && channels() == m.channels();
		 }

		static long long checkMem(){
			return Mrep::allocatedMem;
		}



	private:

		void create(int rows, int cols, int channels, T val , const Alloc& a = Alloc()){
			rows_ = rows;
			cols_ = cols;
			channels_ = channels;
			originCols_ = cols;
			offset_ = 0;
			rep = repAlloc.allocate(1);
			new(rep)Mrep(rows_*cols_*channels_,val, a);
		}

		void create(int rows, int cols, int channels, const Alloc& a = Alloc()){
			rows_ = rows;
			cols_ = cols;
			channels_ = channels;
			originCols_ = cols;
			offset_ = 0;
			rep = repAlloc.allocate(1);
			new(rep)Mrep(rows_*cols_*channels_, a);
		}


		int offset() const{
			return offset_;
		}

		//取得在原始数组中的下标
		int toIndex(int row, int col) const{
			return (row*originCols() + col)*channels() + offset();
		}

		void checkIndex(int rowIndex, int colIndex) const;
		void static copy0(const BasicMatrix* src, BasicMatrix* dest){
			std::memcpy(dest, src, sizeof(BasicMatrix));
		}
	};

	template<typename T, typename Alloc>
	long long BasicMatrix<T,Alloc>:: Mrep::allocatedMem = 0;



	typedef BasicMatrix<uchar> Matrix;
	typedef BasicMatrix<int> MatrixI;
	typedef BasicMatrix<float> MatrixF;
	typedef BasicMatrix<double> MatrixD;

}

template<typename T, typename Alloc> 
std::ostream& operator<<(std::ostream& out, const jun::BasicMatrix<T, Alloc>& m);


template<typename T, typename A1, typename A2>
bool operator==(const jun::BasicMatrix<T, A1>& m1, const jun::BasicMatrix<T, A2>& m2){
	if (&m1==&m2) return true;
	if (!m1.isSameDimension(m2)) return false;
	for (int r = 0; r < m1.rows(); ++r){
		auto p1 = m1.chIter(r, 0);
		auto p2 = m2.chIter(r, 0);
		for (int c = 0; c < m1.totalCols(); ++c){
			if (*p1++ != *p2++) return false;
		}
	}
	return true;
}

template<typename T, typename A1, typename A2>
bool operator!=(const jun::BasicMatrix<T, A1>& m1, const jun::BasicMatrix<T, A2>& m2){
	return !(m1 == m2);
}






#include "BasicMatrix.imp.hpp"

