#pragma once
#include <jemalloc/jemalloc.h>
#include "Matrix.h"
#include "byteOp.h"

#include  <cassert>
#include <thread>

using std::cout;
using std::endl;

JUN_BEGIN


/**
********************************* class BinaryImg *********************************************
*/


class BinaryImg{
public:
	BinaryImg() :rows_(0), cols_(0){
		data_ = nullptr;
	}

	BinaryImg(int rows, int cols) :rows_(rows), cols_(cols){		
		data_ = alloc_(bytes());
	}

	void release(){
		dealloc_(data_);
		data_ = nullptr;
	}
	~BinaryImg(){
		release();
	}

	BinaryImg(const BinaryImg& img, bool copyData) :rows_(img.rows()), cols_(img.cols()){
		data_ = alloc_(bytes());
		if (copyData) std::memcpy(begin(), img.begin(), bytes());
	}


	BinaryImg(BinaryImg&& img) :rows_(img.rows()), cols_(img.cols()){
		//cout << "move cstor: " << this<<" <- "<<&img << " thread: " << std::this_thread::get_id() << endl;
		data_ = img.data_;
		img.data_ = nullptr;
	}

	BinaryImg(const BinaryImg& img) :rows_(img.rows()), cols_(img.cols()){
		data_ = alloc_(bytes());
		std::memcpy(begin(), img.begin(), bytes());
	}

	BinaryImg& operator=(BinaryImg&& other){
		//cout << "move operator=: " << this << " <- " << &other << " thread: " << std::this_thread::get_id() << endl;
		if (this != &other){
			release();
			rows_ = other.rows();
			cols_ = other.cols();
			data_ = other.data_;
			other.data_ = nullptr;
		}
		return *this;
	}

private:
	//disable  copy operator 

	BinaryImg& operator=(const BinaryImg&);



	//BinaryImg& operator=(const BinaryImg& other){
	//	if (this != &other){
	//		release();
	//		rows_ = other.rows();
	//		cols_ = other.cols();
	//		data_ = new uchar[bytes()];
	//		std::memcpy(begin(), other.begin(), bytes());
	//	}
	//	return *this;
	//}

public:



	void create(int rows, int cols){
		rows_ = rows;
		cols_ = cols;
		data_ = alloc_(rows*cols);
	}


	uchar* iter(int r, int c = 0){
		return  data_ + r*cols() + c;
	}

	const uchar* iter(int r, int c = 0) const{
		return  data_ + r*cols() + c;
	}

	uchar& operator()(int r, int c){
		return data_[r*cols() + c];
	}

	const uchar& operator()(int r, int c) const{
		return data_[r*cols() + c];
	}

	void clearBit(int r, int c){
		data_[r*cols() + c] = 0;
	}

	void setBit(int r, int c){
		data_[r*cols() + c] = 1;
	}

	void clearAll(){
		std::memset(begin(), 0, bytes());
	}

	void setAll(){
		std::memset(begin(), 1, bytes());
	}

	void notAll(){
		uint64_t* p = reinterpret_cast<uint64_t*>(begin());
		size_t sz = bytes();
		size_t n = sz / sizeof(uint64_t);
		size_t remaining = sz % sizeof(uint64_t);
		for (size_t i = 0; i < n; ++n){
			*p = ((~*p) & ALL_ONE);
			++p;
		}
		uchar* p2 = reinterpret_cast<uchar*>(p);
		for (size_t i = 0; i < remaining; ++i){
			*p = 1 - *p;
			++p;
		}
	}

	void substract(const BinaryImg& src){
		auto ps = reinterpret_cast<const uint64_t*>(src.begin());
		auto pd = reinterpret_cast<uint64_t*>(begin());
		size_t sz = bytes();
		size_t n = sz / sizeof(uint64_t);
		size_t remaining = sz % sizeof(uint64_t);
		for (size_t i = 0; i < n; ++i){
			*pd &= ~(*ps);
			++pd;
			++ps;
		}
		auto ps2 = reinterpret_cast<const uchar*>(ps);
		auto pd2 = reinterpret_cast<uchar*>(pd);
		for (size_t i = 0; i < remaining; ++i){
			*pd2 &= ~(*ps2);
			++pd2;
			++ps2;
		}
	}

	bool isEmpty() const;

	int rows() const { return rows_; }
	int cols() const { return cols_; }

	Rect getRect() const{ return Rect(0, 0, cols(), rows()); }

	size_t bytes() const { return rows_*cols_; }
	uchar* begin(){ return data_; }
	uchar* end()  { return data_ + bytes(); }
	const uchar* begin()const{ return data_; }
	const uchar* end() const { return data_ + bytes(); }

	static void copy(const BinaryImg& src, BinaryImg& dst){
		Assert<Bad_Arg>(!ARG_CHECK || (src.isSameDimension(dst)));
		std::memcpy(dst.begin(), src.begin(), src.bytes());
	}

	bool isSameDimension(const BinaryImg& other)const{ return rows() == other.rows() && cols() == other.cols(); }

public:
	static const uint64_t ALL_ONE = 0x0101010101010101ULL;


private:
	uchar* alloc_(size_t n) {
		return new uchar[n];
	}

	void dealloc_(void* p){
		delete[] p;
	}


private:
	uchar* data_;
	int rows_;
	int cols_;
};




inline bool BinaryImg::isEmpty() const{
	return !(bitany(begin(), bytes()));
}


JUN_END


inline bool operator==(const jun::BinaryImg& img1, const jun::BinaryImg& img2){
	if (&img1 == &img2) return true;
	if (!img1.isSameDimension(img2)) return false;
	return std::memcmp(img1.begin(), img2.begin(), img1.bytes()) == 0;
}

inline bool operator!=(const jun::BinaryImg& img1, const jun::BinaryImg& img2){
	return !(img1 == img2);
}

 std::ostream& operator<<(std::ostream& out, const jun::BinaryImg& img);



#include "rasterOp.h"