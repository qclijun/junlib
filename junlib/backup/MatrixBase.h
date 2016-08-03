#pragma once

#include <iostream>


namespace jun{
	template<typename T, typename word_type = unsigned int>
	class MatrixBase{

	public:
		typedef T channel_type;
		typedef T* iterator_type;
		typedef const  T*  const_iterator_type;
		typedef word_type* word_ptr;
		typedef T* ptr;
		typedef MatrixBase<T,word_type> Self;

		static const int BPC  { sizeof(channel_type) };//bypes per channel
		static const int BPW{ sizeof(word_type) };//bypes per word
		static const int CPW  { BPW / BPC };

		MatrixBase(int rows,int cols,int channels);
		MatrixBase(int rows, int cols, int channels, T val);
		
		MatrixBase(const Self& other) :Self(other.rows(), other.cols(), other.channels()){
			std::memcpy(begin(), other.begin(), words()*CPW);
		}

		MatrixBase(Self&& other) :_rows(other._rows), _cols(other._cols),
			_channels(other._channels), _wpl(other._wpl),
			_data(other._data){
			other._data = nullptr;
		}
	private:
		Self& operator=(const Self&);
	public:
		Self& operator=(Self&& other){
			if (this != &other){
				release();
				_rows = other._rows;
				_cols = other._cols;
				_channels = other._channels;
				_wpl = other._wpl;
				_data = other._data;
				other._data = nullptr;
			}
			return *this;
		}

		~MatrixBase(){
			release();
		}

		void release(){
			delete[] _data;
			_data = nullptr;
		}

	public:
		//iterator
		iterator_type begin() { return (iterator_type)_data; }
		const_iterator_type begin() const { return (const_iterator_type)_data; }

		iterator_type iter(int row, int col=0){
			return (iterator_type)(_data+row*wpl()) + col*channels();
		}

		const_iterator_type iter(int row, int col=0)const{
			return (const_iterator_type)(_data + row*wpl()) + col*channels();
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
		// get memthods
		int rows() const{ return _rows; }
		int cols() const { return _cols; }
		int channels() const { return _channels; }
		int wpl() const { return _wpl; }
		int pad_channels() const{return wpl()*CPW - cols()*channels(); }//用于填充的channel数目

		int totalCols() const{ return cols()*channels(); }
		size_t elemNum() const{ return rows()*cols(); }
		
		size_t words() const{ return rows()*wpl(); }
		size_t bytes() const { return words()*BPW; }
		size_t totalChannels() const { return words()*CPW; }

	private:
		int _rows;
		int _cols;
		int _channels;
		int _wpl;
		word_ptr _data;
	};
	
	template<typename MatrixType>
	inline void copy(const MatrixType& src, MatrixType& dst){
		std::memcpy(dst.begin(), src.begin(), src.bytes());
	}

}

typedef jun::MatrixBase<unsigned char> JMatrix;
typedef jun::MatrixBase<unsigned> JMatrixI;
typedef jun::MatrixBase<unsigned short> JMatrixS;
typedef jun::MatrixBase<float> JMatrixF;
typedef jun::MatrixBase<double> JMatrixD;

typedef jun::MatrixBase<unsigned char> GrayImg_amp;

template<typename T,typename word_type>
std::ostream& operator<<(std::ostream& out, const jun::MatrixBase<T,word_type>& m);





#include "MatrixBase.hpp"