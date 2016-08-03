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

	struct ChannelOutOfBound{};


	////--------------------------------Matrix------------------------------------//
	/////////////////////////////////////////////////////////////////////////////////

	//T �����Ǽ����ͣ�trivially-copyable objects
	template<typename T, typename Alloc = JUN_ je_allocator<T> >
	class BasicMatrix{
		
	public:
		//typedefs:
		typedef BasicMatrix<T, Alloc> MyType;
		typedef Alloc allocType;
		typedef T elemType;
		typedef elemType* elemPtr;
		typedef typename MatrixTypeTraits<T>::channelType channelType;
		typedef typename channelType* chPtr;

		static const int channels{ MatrixTypeTraits < T >::channels };


	private:
		struct Mrep{			
			size_t sz_;
			allocType alloc_;
			int refcount_;
			elemPtr data_;	

			static long long allocatedMem;

			Mrep(){}

			Mrep(size_t sz, const elemType& val, allocType alloc = allocType())
				:sz_(sz), alloc_(alloc),refcount_(1){
				construct(sz);
				std::uninitialized_fill_n(data_, sz, val);
			}

			Mrep(size_t sz, allocType alloc = allocType())
				:sz_(sz), alloc_(alloc), refcount_(1){
				construct(sz);
				std::uninitialized_fill_n(data_, sz, elemType());
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
				data_ = alloc_.allocate(n);//ֻ�����ڴ棬�����й��죬��Ϊ�Ǽ򵥶���
				allocatedMem += n*sizeof(elemType);
			}

			void release(){			
					alloc_.deallocate(data_, sz_);
					data_ = nullptr;
					allocatedMem -= sz_*sizeof(elemType);				
			}

		};

		int rows_;
		int cols_;
		int originCols_;
		int offset_;
		Mrep *rep;

		typedef typename allocType::template rebind<Mrep>::other repAllocType;
		repAllocType repAlloc;
	public:


		BasicMatrix():rep(nullptr){	
		}

		BasicMatrix(int rows, int cols, const Alloc& a = Alloc()){
			init(rows, cols, a);
		}

		BasicMatrix(int rows, int cols, const T& val , const Alloc& a = Alloc())
		{
			init(rows, cols, val, a);
		}

		template<typename InputIt>
		BasicMatrix(int rows, int cols, InputIt first, InputIt last, const Alloc& a = Alloc());

		template<typename InputIt>
		BasicMatrix(InputIt first, InputIt last, const Alloc& a = Alloc());

		BasicMatrix(int rows, int cols, std::initializer_list<double> dl, const Alloc& a = Alloc());
		
		BasicMatrix(std::initializer_list<double> dl, const Alloc& a = Alloc());


		~BasicMatrix(){
			if (rep == nullptr)
				return;
			if ((rep->reduceRef() == 0)){
				//delete rep;
				repAlloc.destroy(rep);
				repAlloc.deallocate(rep, 1);
				rep = nullptr;
			}
				
		}


		BasicMatrix(const BasicMatrix& m){
			//cout << "call copy ctor" << endl;
			copy0(&m, this);
			rep->addref();
		}

		///���ƹ��캯�����Ӷ�O(1)��������share resources��û�б�Ҫʹ��move constructor(steal resources)
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
		//BasicMatrix& operator=(BasicMatrix&& m);


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
		int originCols() const { return originCols_; }
		int totalCols() const { return cols()*channels; }
		size_t  bpl() const { return totalCols()*sizeof(channelType); } //bytes per line

		size_t elemNum() const { return rows()*cols(); }
		size_t channelNum() const { return elemNum()*channels; }

		size_t stride_b() const{ return stride()*sizeof(channelType); } //byte stride
		size_t stride_e() const { return originCols(); }	// element stride
		size_t stride() const { return stride_e()*channels; } // channel stride

		bool isContinues() const { return cols() == originCols(); }

		allocType get_allocator() const { return rep->alloc_; }

		void setAll(double val);

		BasicMatrix<T, Alloc> clone() const;


		void assign(const T& val){
			for (int r = 0; r < rows(); ++r){
				std::fill_n(elemIter(r, 0), cols(), val);
			}
		}

		// access methods

		//access elements
		elemPtr elemIter(int rowIndex, int colIndex = 0){
			//checkIndex(rowIndex, colIndex);
			return rep->data_ + toIndex(rowIndex, colIndex);
		}

		const elemPtr elemIter(int rowIndex, int colIndex = 0) const{
			//checkIndex(rowIndex, colIndex);
			return rep->data_ + toIndex(rowIndex, colIndex);
		}

		//access channels
		chPtr chIter(int rowIndex, int colIndex = 0){
			return reinterpret_cast<chPtr>(elemIter(rowIndex, colIndex));
		}

		const chPtr chIter(int rowIndex, int colIndex = 0)  const{
			return reinterpret_cast<const chPtr>(elemIter(rowIndex, colIndex));
		}


		elemPtr moveIter(elemPtr it, int r, int c = 0){
			return it + r*originCols() + c;
		}

		const elemPtr moveIter(elemPtr it, int r, int c = 0) const{
			return it + r*originCols() + c;
		}

		 elemType& operator()(int r, int c){
			return rep->data_[toIndex(r, c)];
		}

		 const elemType& operator()(int r, int c) const{
			return rep->data_[toIndex(r, c)];
		}


		 elemType& at(int row, int col){
			 checkIndex(row, col);
			return rep->data_[toIndex(row, col)];
		}

		 const elemType& at(int row, int col) const{
			 checkIndex(row, col);
			return rep->data_[toIndex(row, col)];
		}

		 //��dl���Ƹ��ӣ�row,col)��ʼ��������dl.size()��channel,����һ��
		 void put(int row, int col, std::initializer_list<double> dl){
			 if (col*channels + dl.size() > totalCols())
				 throw out_of_range("");
			 chPtr p = chIter(row, col);
			 for (auto p2 = begin(dl); p2 != end(dl); ++p2, ++p)
				 *p = static_cast<channelType>(*p2);
		 }
		 

		 bool isSameDimension(const BasicMatrix& m) const{
			 return rows() == m.rows() && cols() == m.cols();
		 }


		static long long checkMem(){
			return Mrep::allocatedMem;
		}



	private:

		void init(int rows, int cols, const T& val , const Alloc& a = Alloc()){
			rows_ = rows;
			cols_ = cols;
			originCols_ = cols;
			offset_ = 0;
			rep = repAlloc.allocate(1);
			new(rep)Mrep(rows*cols,val, a);
		}

		void init(int rows, int cols, const Alloc& a = Alloc()){
			init(rows, cols, T(), a);
		}

		int offset() const{
			return offset_;
		}

		//ȡ����ԭʼ�����е��±�
		int toIndex(int row, int col) const{
			return row*originCols() + col + offset();
		}

		//int toRowIndex(int index) const{
		//	return (index-offset()) / originCols();
		//}
		//int toColIndex(int index) const{
		//	return (index - offset()) % originCols();
		//}



		void checkIndex(int rowIndex, int colIndex) const;
		void static copy0(const BasicMatrix* src, BasicMatrix* dest){
			std::memcpy(dest, src, sizeof(BasicMatrix));
		}
	};

	template<typename T, typename Alloc>
	long long BasicMatrix<T,Alloc>:: Mrep::allocatedMem = 0;


}

template<typename T, typename Alloc> 
std::ostream& operator<<(std::ostream& out, const jun::BasicMatrix<T, Alloc>& m);


template<typename T, typename A1, typename A2>
bool operator==(const BasicMatrix<T, A1>& m1, const BasicMatrix<T, A2>& m2){
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
bool operator!=(const BasicMatrix<T, A1>& m1, const BasicMatrix<T, A2>& m2){
	return !(m1 == m2);
}








#include "BasicMatrix.imp.hpp"

