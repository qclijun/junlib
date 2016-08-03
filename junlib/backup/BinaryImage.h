#pragma once
#include "Matrix.h"

JUN_BEGIN

// 不共享数据
class BinaryImage{
public:
	typedef bool element_type;
	typedef uint64_t Ty;
	typedef uint64_t* pointer;

public:
	BinaryImage(int rows, int cols) :rows_(rows), cols_(cols){
		wpl_ = (cols - 1) / BitsPerWord + 1;
		data_ = new Ty[words()];
		clearAll();
	}

	BinaryImage(const BinaryImage& img):rows_(img.rows()),cols_(img.cols()),wpl_(img.wpl_){
		data_ = new Ty[words()];
		std::memcpy(data_, img.data_, bytes());
	}
	BinaryImage(const BinaryImage& img, bool copyData) :rows_(img.rows()), cols_(img.cols()),wpl_(img.wpl_){
		data_ = new Ty[words()];
		if(copyData) std::memcpy(data_, img.data_, bytes());
	}

	BinaryImage(BinaryImage&& img) :rows_(img.rows()), cols_(img.cols()), wpl_(img.wpl_){
		data_ = img.data_;
		img.data_ = nullptr;
	}

	BinaryImage& operator=(const BinaryImage& img){
		if (this != &img){
			delete[] data_;
			rows_ = img.rows();
			cols_ = img.cols();
			wpl_ = img.wpl();
			std::memcpy(data_, img.data_, bytes());
		}
	}

	BinaryImage& operator=(BinaryImage&& img){
		if (this != &img){
			delete[] data_;
			rows_ = img.rows();
			cols_ = img.cols();
			wpl_ = img.wpl();
			data_ = img.data_;
			img.data_ = nullptr;
		}
	}

	~BinaryImage(){
		delete[] data_;
	}

	pointer ptr(int r) const{
		return data_ + r*wpl();
	}

	void setBit(int r, int c){
		setBit(toIndex(r,c));	
	}

	void setBit(size_t pos){
		data_[pos / BitsPerWord] |= ((Ty)1 << (Mask - pos&Mask));
	}

	void clearBit(int r, int c){
		clearBit(toIndex(r,c));
	}

	void clearBit(size_t pos){
		data_[pos / BitsPerWord] &= ~((Ty)1 << (Mask - pos&Mask));
	}

	int getBit(int r, int c) const{
		return getBit(toIndex(r, c));
	}

	int getBit(size_t pos) const{
		return (data_[pos / BitsPerWord] >> (Mask - pos&Mask)) & 1;
	}


	void clearAll(){
		std::memset(data_, 0, bytes());
	}

	void setAll(){
		std::memset(data_, 0xFF, bytes());
	}

	void notAll(){

	}


	bool isSameDimension(const BinaryImage& other) const{
		return rows() == other.rows() && cols() == other.cols();
	}

	//cvt2Gray 
	operator jun::Matrix() const{
		return cvt2Gray();
	}

	jun::Matrix cvt2Gray() const{
		jun::Matrix result(rows(), cols(), 1, 255);
		auto p = result.begin();
		for (size_t pos = 0; pos < elemNum(); ++pos){
			if (getBit(pos)) p[pos] = 0;
		}

		return result;
	}


	static void copy(const BinaryImage& src, BinaryImage& dst){
		std::memcpy(dst.data_, src.data_, src.bytes());
	}

	size_t elemNum() const{
		return rows_*cols_;
	}
	size_t words() const{
		return wpl()*rows();
	}
	size_t bytes() const{
		return wpl()*rows()*sizeof(Ty);
	}
	int rows()const {
		return rows_;
	}
	int cols() const{
		return cols_;
	}
	int wpl() const{
		return wpl_;
	}

private:
	int rows_;
	int cols_;
	int wpl_;
	Ty* data_;

	enum :int{
		BitsPerWord = CHAR_BIT*sizeof(Ty),
		Mask = BitsPerWord - 1,
	};

	size_t toIndex(int r, int c) const{
		return r*wpl()*BitsPerWord + c;
	}

};

void rastorOp(BinaryImage& dst, jun::Rect rect, const BinaryImage& src);



JUN_END
