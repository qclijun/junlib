#pragma once

#include "je_new.h"
#include <type_traits>
#include <iostream>
#include "types.h"
#include "byteOp.h"
#include "junlib\util.h"


namespace jun{

	class BinaryJMatrix;

	template<typename T>
	class JMatrixBase{
		static_assert(std::is_arithmetic<T>::value, "T must be arithmetic.");
	public:
		typedef T channel_type;
		typedef T* iterator_type;
		typedef const  T*  const_iterator_type;
		typedef JMatrixBase<T> Self;

		static const int BPC  { sizeof(channel_type) };//bypes per channel


		JMatrixBase(int rows,int cols,int channels = 1);
		JMatrixBase(int rows, int cols, int channels, T val);

		
		JMatrixBase(const Self& other) :Self(other.rows(), other.cols(), other.channels()){
			
			std::memcpy(begin(), other.begin(), bytes());
		}

		JMatrixBase(const Self& other, bool copyData) :Self(other.rows(), other.cols(), other.channels()){
			if(copyData) std::memcpy(begin(), other.begin(), bytes());
		}

		JMatrixBase(Self&& other) :_rows(other._rows), _cols(other._cols),
			_channels(other._channels), 
			_data(other._data){
			other._data = nullptr;
		}

		Self& operator=(const Self& other){
			if (this != &other){
				release();
				_rows = other._rows;
				_cols = other._cols;
				_channels = other._channels;
				_data = new T[channelNum()];
				std::memcpy(_data, other._data, bytes());
			}
			return *this;
		}
	public:
		Self& operator=(Self&& other){
			if (this != &other){
				release();
				_rows = other._rows;
				_cols = other._cols;
				_channels = other._channels;
				_data = other._data;
				other._data = nullptr;
			}
			return *this;
		}

		~JMatrixBase(){
			release();
		}

		void release(){
			delete[] _data;
			_data = nullptr;
		}

	public:
		//iterator
		iterator_type begin() { return _data; }
		const_iterator_type begin() const { return _data; }

		iterator_type end() { return _data + channelNum(); }
		const_iterator_type end() const { return _data + channelNum(); }


		iterator_type iter(int row, int col=0){
			return begin() + row*cpl() + col*channels();
		}

		const_iterator_type iter(int row, int col=0)const{
			return begin() + row*cpl() + col*channels();
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

		bool isEmpty() const{
			return !bitany(begin(), bytes());
		}

		void and(const Self& src){ bitand(src.begin(), src.bytes(), begin()); }
		void or(const Self& src){ bitor(src.begin(), src.bytes(), begin()); }
		void xor(const Self& src){ bitxor(src.begin(), src.bytes(), begin()); }

		void clearAll(){
			std::memset(begin(), 0, bytes());
		}
		void clearAll(Rect rect);

		void setAll(T val);
		void setAll(Rect rect, T val);
		void setMasked(const BinaryJMatrix& mask, T val);


	public:
		// get memthods
		int rows() const{ return _rows; }
		int cols() const { return _cols; }
		int channels() const { return _channels; }

		int totalCols() const{ return cols()*channels(); }
		int cpl() const{ return cols()*channels(); } //channels per line 
		int bpl() const { return cpl()*BPC; } //bytes per line
		size_t elemNum() const{ return rows()*cols(); }
		size_t channelNum() const{ return elemNum()*channels(); }		
		size_t bytes() const { return channelNum()*BPC; }

		bool isSameDimension(const Self& other) const{
			return rows() == other.rows() && cols() == other.cols() && channels() == other.channels();
		}

		Rect rect() const{ return Rect(0, 0, cols(), rows()); }

	private:
		int _rows;
		int _cols;
		int _channels;
		T* _data;
	}; // end of class JMatrixBase
	

	typedef jun::JMatrixBase<unsigned char> JMatrix;

	typedef jun::JMatrixBase<short> JMatrixS;
	typedef jun::JMatrixBase<unsigned short> JMatrixUS;
	typedef jun::JMatrixBase<int> JMatrixI;
	typedef jun::JMatrixBase<unsigned int> JMatrixUI;
	typedef jun::JMatrixBase<float> JMatrixF;
	typedef jun::JMatrixBase<double> JMatrixD;

	typedef jun::JMatrixBase<unsigned char> GrayImg;

} // end of namespace jun;



template<typename T>
inline bool operator==(const jun::JMatrixBase<T>& m1, const jun::JMatrixBase<T>& m2){
	if (&m1 == &m2) return true;
	if (!m1.isSameDimension(m2)) return false;
	return std::memcmp(m1.begin(), m2.begin(), m1.bytes()) == 0;
}

template<typename T>
inline bool operator!=(const jun::JMatrixBase<T>& m1, const jun::JMatrixBase<T>& m2){
	return !(m1 == m2);
}
//
//
//inline bool operator==(const jun::JMatrix& m1, const jun::JMatrix& m2){
//	if (&m1 == &m2) return true;
//	if (!m1.isSameDimension(m2)) return false;
//	return std::memcmp(m1.begin(), m2.begin(), m1.bytes()) == 0;
//}


//inline bool operator!=(const jun::JMatrix& m1, const jun::JMatrix& m2){
//	return !(m1 == m2);
//}


template<typename T>
std::ostream& operator<<(std::ostream& out, const jun::JMatrixBase<T>& m);







#include "JMatrixBase.hpp"