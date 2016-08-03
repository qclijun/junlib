#pragma once

#include <iostream>
#include <memory>
#include <iterator>
#include <initializer_list>
#include <array>

using std::array;

namespace jun{

	struct ChannelOutOfBound{};

	struct MatrixBase{
		int rows, cols;
		int channels;		
	public:
		MatrixBase(){}

		MatrixBase(int rows, int cols,int channels=1)
			:rows(rows), cols(cols),channels(channels)
		{}

		MatrixBase(array<int, 3> arr) :rows(arr[0]), cols(arr[1]), channels(arr[2]){}
		MatrixBase(array<int, 2> arr) :rows(arr[0]), cols(arr[1]), channels(1){}
	};

	inline bool operator==(MatrixBase left, MatrixBase right){
		return left.rows == right.rows&&left.cols == right.cols&&left.channels == right.channels;
	}

	inline bool operator!=(MatrixBase left, MatrixBase right){
		return !(left == right);
	}

	////--------------------------------Matrix------------------------------------//
	/////////////////////////////////////////////////////////////////////////////////

	template<typename T, typename Alloc = std::allocator<T> >
	class Matrix{
		//typedefs:
	public:

		typedef Matrix<T, Alloc> MyType;
		typedef T value_type;
		typedef typename Alloc alloc_type;


		typedef  T* iterator_type;
		typedef  const T* const_iterator_type;


	private:

		MatrixBase base;
		alloc_type alloc_;
		bool isContinue_{ true };

		int offset_{ 0 };
		int* refcount_ = new int{ 1 };
		T** data_;	//all channels data;

		static long long allocatedMem;

	public:


		Matrix() :base(0,0,0),data_(nullptr){}

		Matrix(int rows, int cols, int channels = 1, const Alloc& a = Alloc())
			:base(rows,cols,channels), alloc_(a){
			construct();
		}

		template<typename InputIt>
		Matrix(MatrixBase dimension, InputIt first, InputIt last, const Alloc& alloc = Alloc())
			: base(dimension){
			construct();
			assign(first, last);
		}



		Matrix(MatrixBase dimension, std::initializer_list<value_type> l, const Alloc& alloc = Alloc())
			: base(dimension){
			construct();
			assign(l.begin(), l.end());
		}

		template<typename InputIt>
		Matrix(InputIt first, InputIt last, const Alloc& alloc = Alloc())
			: base(1, (last - first),1){
			construct();
			assign(first, last);
		}

		Matrix(std::initializer_list<value_type> l, const Alloc& alloc = Alloc())
			: base(1, l.size(), 1){
			construct();
			assign(l.begin(), l.end());
		}

		Matrix(const Matrix& m);

		///复制构造函数复杂度O(1)，语义是share resources，没有必要使用move constructor(steal resources)
		//Matrix(Matrix&& m);

		~Matrix(){ release(); }

		Matrix& operator=(const Matrix& m);
		//Matrix& operator=(Matrix&& m);


		//---------------------------sub matrix ----------------------------------
		///////////////////////////////////////////////////////////////////////////
		Matrix(const Matrix&m, array<int, 4> rect){

		}


		////--------------------------get function --------------------------
		int rows() const{ return base.rows; }
		int cols() const{ return base.cols; }
		int channels() const { return base.channels; }
		int elemSize() const { return rows()*cols(); }
		bool isContinue() const { return isContinue_; }

		alloc_type get_allocator() const { return alloc_; }



		//-----------------------------------sub matrix ----------------------------------

	

		/*Matrix(Matrix&& m, std::array<int, 4> rect);*/




		Matrix row(int start, int end) ;
		Matrix col(int start, int end) ;
		Matrix row(int r) { return row(r, r + 1); }
		Matrix col(int c) { return col(c, c + 1); }



		template<typename InputIt>
		void assign(InputIt first, InputIt last){
			switch (channels()){
			case 1:
				assign1(first, last);
				break;
			case 2:
				assign2(first, last);
				break;
			case 3:
				assign3(first, last);
				break;
			case 4:
				assign4(first, last);
			default:
				throw ChannelOutOfBound();
			}
		}

		void assign(const T& val){
			for (int i = 0; i < channels(); ++i){
				std::fill_n(data_[i], val);
			}
		}



		T& at(int row, int col, int channel = 0){
			return data_[channel][toIndex(row, col) + offset_];
		}
		const T& at(int row, int col, int channel = 0) const{
			return data_[channel][toIndex(row, col) + offset_];
		}




		void printDataAddr() const{
			std::cout << "Data Address: " << data_ << std::endl;
		}

		static long long checkMem(){
			return allocatedMem;
		}

	private:

		

		int toIndex(int row, int col) const{
			return row*cols() + col;
		}

		int toRowIndex(int index) const{
			return index / cols();
		}
		int toColIndex(int index) const{
			return index%cols();
		}

		int offset() const{ return offset_; }

		void checkIndex(int rowIndex, int colIndex) const;

		void release(){
			if (--*refcount_ == 0){
				std::cout << "Release Data: " << data_ << std::endl;

				if (data_ != nullptr){
					for (int i = 0; i < channels(); ++i){
						deallocate(data_[i], elemSize());
						//data_[i] = nullptr;
					}
					delete refcount_;
					delete[] data_;
					data_ = nullptr;
				}
			}
		}

		void construct(){
			data_ = (T**)new void*[channels()];
			for (int i = 0; i < channels(); ++i){
				data_[i] = allocate(elemSize());
			}
		}



		value_type* allocate(size_t n) throw(){
			//cout << "Allocate Memory: " << allocatedMem << "  ->  ";
			allocatedMem += n*sizeof(value_type);
			//cout << allocatedMem << endl;
			return get_allocator().allocate(n);
		}

		void deallocate(value_type* ptr, size_t n) throw(){
			get_allocator().deallocate(ptr, n);

			//cout << "Allocate Memory: " << allocatedMem << "  ->  ";
			allocatedMem -= n*sizeof(value_type);
			//cout << allocatedMem << endl;

		}

		void static copy0(const Matrix& src, Matrix& dest);

		template<typename InputIt>
		void assign1(InputIt first, InputIt last){
			value_type* p1 = data_[0] + offset_;
			InputIt pIn = first;
			for (int i = 0; i < elemSize() && pIn != last; ++i){
				*p1++ = *pIn++;
			}
		}

		template<typename InputIt>
		void assign2(InputIt first, InputIt last){
			value_type* p1 = data_[0] + offset_;
			value_type* p2 = data_[1] + offset_;
			InputIt pIn = first;
			for (int i = 0; i < elemSize() && pIn != last; ++i){
				*p1++ = *pIn++;
				*p2++ = *pIn++;
			}
		}

		template<typename InputIt>
		void assign3(InputIt first, InputIt last){
			value_type* p1 = data_[0] + offset_;
			value_type* p2 = data_[1] + offset_;
			value_type* p3 = data_[2] + offset_;
			InputIt pIn = first;
			for (int i = 0; i < elemSize() && pIn != last; ++i){
				*p1++ = *pIn++;
				*p2++ = *pIn++;
				*p3++ = *pIn++;
			}
		}

		template<typename InputIt>
		void assign4(InputIt first, InputIt last){
			value_type* p1 = data_[0] + offset_;
			value_type* p2 = data_[1] + offset_;
			value_type* p3 = data_[2] + offset_;
			value_type* p4 = data_[3] + offset_;
			InputIt pIn = first;
			for (int i = 0; i < elemSize() && pIn != last; ++i){
				*p1++ = *pIn++;
				*p2++ = *pIn++;
				*p3++ = *pIn++;
				*p4++ = *pIn++;
			}
		}

	};




}

template<typename T, typename Alloc> 
std::ostream& operator<<(std::ostream& out, const jun::Matrix<T, Alloc>& m);

#include "Matrix.imp.hpp"

