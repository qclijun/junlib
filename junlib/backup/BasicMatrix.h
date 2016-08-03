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

		typedef T* iterator_type;
		typedef const T* const_iterator_type;


	private:
		struct Mrep{			
			int rows_;
			int cols_;
			int channels_;
			allocType alloc_;
			int refcount_;
			T* data_;	

			static long long allocatedMem;

			Mrep():rows_(0),cols_(0),channels_(0),refcount_(1),data_(nullptr){}

			Mrep(int rows, int cols, int channels, T val, allocType alloc = allocType())
				:rows_(rows),cols_(cols),channels_(channels), alloc_(alloc),refcount_(1){
				construct(rows*cols*channels);
				std::uninitialized_fill_n(data_, rows*cols*channels, val);
			}

			// uninitialized.
			Mrep(int rows, int cols, int channels, allocType alloc = allocType())
				:rows_(rows), cols_(cols), channels_(channels), alloc_(alloc), refcount_(1){
				construct(rows*cols*channels);
				//std::uninitialized_fill_n(data_, sz, T());
			}

			~Mrep(){
				release();
			}

			void addRef(){
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
					alloc_.deallocate(data_, rows_*cols_*channels_);
					data_ = nullptr;
					allocatedMem -= rows_*cols_*channels_*sizeof(T);
			}

		};

		Mrep *rep;

		typedef typename allocType::template rebind<Mrep>::other repAllocType;
		repAllocType repAlloc;


	public:
		BasicMatrix(){
			rep = repAlloc.allocate(1); 
			new(rep)Mrep();
		}

		BasicMatrix(int rows, int cols, int channels, T val , const Alloc& a = Alloc()){
			create(rows, cols, channels, val, a);
		}

		BasicMatrix(int rows, int cols, int channels, const Alloc& a = Alloc()){
			create(rows, cols, channels, a);
		}

		~BasicMatrix(){
			if (rep == nullptr) return;
			if ((rep->reduceRef() == 0)){
				repAlloc.destroy(rep);
				repAlloc.deallocate(rep, 1);
				rep = nullptr;
			}
				
		}


		BasicMatrix(const BasicMatrix& m){
			rep = m.rep;
			rep->addRef();
		}


		BasicMatrix(BasicMatrix&& m) {
			rep = m.rep;
			m.rep = nullptr;
		}

	
		BasicMatrix& operator=(const BasicMatrix& m){
			if (this != &m){
				if (rep->reduceRef() == 0){
					repAlloc.destroy(rep);
					repAlloc.deallocate(rep, 1);
				}					
				rep = m.rep;
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
				rep = m.rep;
				m.rep = nullptr;
			}
		}


		template<typename InputIt>
		void init(InputIt first, InputIt last);

		void init(std::initializer_list<double> dl);

		template<typename Func>
		void init(Func f);

		/**
		*** 不支持submatrix, 为了保证matrix底层数据的连续性，这样不用每次检查是否连续
		
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
		*/


		////--------------------------get function --------------------------
		int rows() const{ return rep->rows_; }
		int cols() const{ return rep->cols_; }
		int channels() const { return rep->channels_; }
		int totalCols() const { return cols()*channels(); }


		size_t elemNum() const { return rows()*cols(); }
		size_t channelNum() const { return elemNum()*channels(); }
		size_t bytes() const { return channelNum()*sizeof(channelType); }

		size_t  bpl() const { return totalCols()*sizeof(channelType); } //bytes per line
		size_t stride_b() const{ return stride()*sizeof(channelType); } //byte stride
		size_t stride_e() const { return cols(); }	// element stride
		size_t stride() const { return totalCols(); } // channel stride

		allocType get_allocator() const { return rep->alloc_; }

		void setAll(double val);

		BasicMatrix<T, Alloc> clone() const;
		BasicMatrix<T, Alloc> cloneEmpty() const{
			return BasicMatrix(rows(), cols(), channels());
		}

		//iterator:
		iterator_type begin(){ return rep->data_; }
		const_iterator_type begin() const { return rep->data_; }
		iterator_type end(){ return rep->data_ + channelNum(); }
		const_iterator_type end() const { return rep->data_ + channelNum(); }

		iterator_type iter(int rowIndex, int colIndex = 0){
			return rep->data_ + toIndex(rowIndex, colIndex);
		}

		const_iterator_type iter(int rowIndex, int colIndex = 0)  const{
			return rep->data_ + toIndex(rowIndex, colIndex);
		}

		// access methods

		//access channels
		iterator_type chIter(int rowIndex, int colIndex = 0){
			return rep->data_ + toIndex(rowIndex, colIndex);
		}

		const_iterator_type chIter(int rowIndex, int colIndex = 0)  const{
			return rep->data_ + toIndex(rowIndex, colIndex);
		}

		 channelType& operator()(int r, int c, int ch=0){
			 return rep->data_[toIndex(r, c) + ch];
		}

		 const channelType& operator()(int r, int c, int ch = 0) const{
			return rep->data_[toIndex(r, c)+ch];
		}

		 channelType& at(int row, int col,int ch=0){
			 checkIndex(row, col);
			return rep->data_[toIndex(row, col)+ch];
		}

		 const channelType& at(int row, int col,int ch=0) const{
			 checkIndex(row, col);
			return rep->data_[toIndex(row, col)+ch];
		}

		 void copyPix(iterator_type dst, const_iterator_type src){
			 std::memcpy(dst, src, channels()*sizeof(channelType));
		 }

		 void copyRow(iterator_type dst, const_iterator_type src){
			 std::memcpy(dst, src, bpl());
		 }

		 static void copy(const BasicMatrix& src, BasicMatrix& dst){			 
			std::memcpy(dst.begin(), src.begin(), src.bytes());	 
		 }


		 //将dl复制给从（row,col)开始的连续的dl.size()个channel,仅限一行
		 void put(int row, int col, std::initializer_list<double> dl){
			 if (col*channels() + dl.size() > totalCols())
				 throw out_of_range("");
			 auto p = chIter(row, col);
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
			rep = repAlloc.allocate(1);
			new(rep)Mrep(rows, cols, channels, val, a);
		}

		void create(int rows, int cols, int channels, const Alloc& a = Alloc()){
			rep = repAlloc.allocate(1);
			new(rep)Mrep(rows, cols, channels, a);
		}

		//取得在原始数组中的下标
		size_t toIndex(int row, int col) const{
			return row*stride() + col*channels();
		}

		void checkIndex(int rowIndex, int colIndex) const;

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
	return std::memcmp(m1.begin(), m2.begin(), m1.bytes()) == 0;
}

template<typename T, typename A1, typename A2>
bool operator!=(const jun::BasicMatrix<T, A1>& m1, const jun::BasicMatrix<T, A2>& m2){
	return !(m1 == m2);
}






#include "BasicMatrix.imp.hpp"

