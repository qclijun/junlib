#pragma once

#include <iostream>
#include <memory>
#include <iterator>
#include <initializer_list>
#include <array>

namespace jun{

	struct ChannelOutOfBound{};

	template<typename T, typename Alloc = std::allocator<T> >
	class MatrixBase{
	protected:

		int rows_, cols_;
		Alloc alloc_;

		MatrixBase(){}

		MatrixBase(int rows, int cols, const Alloc& alloc = Alloc())
			:rows_(rows), cols_(cols), alloc_(alloc)
		{}


	};

	template<typename T, size_t Ch, typename Alloc = std::allocator<T> >
	class Matrix : public MatrixBase < T, Alloc > {

friend Matrix<T,1,Alloc> Matrix<T,Ch,Alloc>::channelMatrix(int);
friend class Matrix < T, 1, Alloc > ;
		//typedefs:
	public:
		enum { channels=Ch };
		typedef Matrix<T, Ch, Alloc> MyType;
		typedef T value_type;
		typedef typename Alloc alloc_type;


		typedef  T* iterator_type;
		typedef  const T* const_iterator_type;


	private:
		typedef MatrixBase<T, Alloc> MyBase;
		

		int offset_{ 0 };
		int* refcount_ = new int{ 1 };
		T** data_;	//all channels data;
		static long long allocatedMem;

	public:


		Matrix() :MyBase(0,0),data_(nullptr){}

		Matrix(int rows, int cols, const Alloc& alloc = Alloc())
			:MyBase(rows, cols, alloc){
			construct();
		}

		template<typename InputIt>
		Matrix(int rows, int cols, InputIt first, InputIt last, const Alloc& alloc = Alloc())
			: MyBase(rows, cols, alloc){
			construct();
			assign(first, last);
		}

		Matrix(int rows, int cols, std::initializer_list<value_type> l, const Alloc& alloc = Alloc())
			: MyBase(rows, cols, alloc){
			construct();
			assign(l.begin(), l.end());
		}

		template<typename InputIt>
		Matrix(InputIt first, InputIt last, const Alloc& alloc = Alloc())
			: MyBase(1, (last - first) / channels, alloc){
			if (elemSize() < last - first) ++cols_;
			construct();
			assign(first, last);
		}


		explicit Matrix(std::initializer_list<value_type> l, const Alloc& alloc = Alloc())
			: MyBase(1, static_cast<int>(l.size() / channels), alloc){
			if (elemSize() < l.size()) ++cols_;
			construct();
			assign(l.begin(), l.end());
		}


		Matrix(const Matrix& m);
		Matrix(Matrix&& m);
		~Matrix(){ release(); }

		Matrix& operator=(const Matrix& m);
		Matrix& operator=(Matrix&& m);


		//-----------------------------------sub matrix ----------------------------------

	

		/*Matrix(Matrix&& m, std::array<int, 4> rect);*/

		Matrix<T, 1, Alloc> channelMatrix(int index);


		Matrix row(int start, int end) ;
		Matrix col(int start, int end) ;
		Matrix row(int r) { return row(r, r + 1); }
		Matrix col(int c) { return col(c, c + 1); }



		template<typename InputIt>
		void assign(InputIt first, InputIt last){
			switch (channels){
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
			for (int i = 0; i < channels; ++i){
				std::fill_n(data_[i], val);
			}
		}

		int rows() const{ return rows_; }
		int cols() const{ return cols_; }

		int elemSize() const { return rows()*cols(); }

		alloc_type get_allocator() const { return alloc_; }

		T& at(int row, int col, int channel = 0){
			return data_[channel][toIndex(row, col) + offset_];
		}
		const T& at(int row, int col, int channel = 0) const{
			return data_[channel][toIndex(row, col) + offset_];
		}




		void printDataAddr() const{
			std::cout << "Data Address: " << data_ << std::endl;
		}

		static bool checkMem(){
			return allocatedMem == 0;
		}

	private:
		int toIndex(int row, int col) const{
			return row*rows() + col;
		}

		void release(){
			if (--*refcount_ == 0){
				std::cout << "Release Data: " << data_ << std::endl;

				if (data_ != nullptr){
					for (int i = 0; i < channels; ++i){
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
			data_ = (T**)new void*[channels];
			for (int i = 0; i < channels; ++i){
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

template<typename T, size_t Ch, typename Alloc> 
std::ostream& operator<<(std::ostream& out, const jun::Matrix<T, Ch, Alloc>& m);

#include "Matrix.imp.hpp"

