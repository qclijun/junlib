#ifndef _BYTEOP_H_
#define _BYTEOP_H_

#include <cstdint>


typedef unsigned char uchar;

namespace jun{

	/*
	*********************************** bit operations **********************************
	*/

	const static std::uint64_t ALL_ONE = { 0x0101010101010101ULL };

	inline void bitand(const uchar* src, size_t sz, uchar* dst){
		uint64_t* ps = (uint64_t*)(src);
		uint64_t* pd = (uint64_t*)(dst);
		size_t n = sz / sizeof(uint64_t);
		size_t remaining = sz %sizeof(uint64_t);
		for (size_t i = 0; i < n; ++i){
			*pd++ &= *ps++;
		}
		uchar* psuc = (uchar*)(ps);
		uchar* pduc = (uchar*)(pd);
		for (size_t i = 0; i < remaining; ++i){
			*pduc++ &= *psuc++;
		}
	}

	inline void bitor(const uchar* src, size_t sz, uchar* dst){
		auto ps = reinterpret_cast<const uint64_t*>(src);
		auto pd = reinterpret_cast<uint64_t*>(dst);
		size_t n = sz / sizeof(uint64_t);
		size_t remaining = sz %sizeof(uint64_t);
		for (size_t i = 0; i < n; ++i){
			*pd++ |= *ps++;
		}
		auto ps2 = reinterpret_cast<const uchar*>(ps);
		auto pd2 = reinterpret_cast<uchar*>(pd);
		for (size_t i = 0; i < remaining; ++i){
			*pd2++ |= *ps2++;
		}
	}

	inline void bitxor(const uchar* src, size_t sz, uchar* dst){
		auto ps = reinterpret_cast<const uint64_t*>(src);
		auto pd = reinterpret_cast<uint64_t*>(dst);
		size_t n = sz / sizeof(uint64_t);
		size_t remaining = sz %sizeof(uint64_t);
		for (size_t i = 0; i < n; ++i){
			*pd++ ^= *ps++;
		}
		auto ps2 = reinterpret_cast<const uchar*>(ps);
		auto pd2 = reinterpret_cast<uchar*>(pd);
		for (size_t i = 0; i < remaining; ++i){
			*pd2++ ^= *ps2++;
		}
	}

	inline void bitandnot(const uchar* src, size_t sz, uchar* dst){
		auto ps = reinterpret_cast<const uint64_t*>(src);
		auto pd = reinterpret_cast<uint64_t*>(dst);
		size_t n = sz / sizeof(uint64_t);
		size_t remaining = sz %sizeof(uint64_t);
		for (size_t i = 0; i < n; ++i,++pd,++ps){
			*pd &= (~*ps);
		}
		auto ps2 = reinterpret_cast<const uchar*>(ps);
		auto pd2 = reinterpret_cast<uchar*>(pd);
		for (size_t i = 0; i < remaining; ++i,++pd2,++ps2){
			*pd2 &= (~*ps2);
		}
	}

	inline bool bitany(const uchar* src, size_t sz){
		auto  p = reinterpret_cast<const uint64_t*>(src);
		auto end = src + sz;
		for (; (const uchar*)(p + 1) < end; ++p){
			if (*p) return true;
		}
		//auto p2 = (const uchar*)p;
		//if (p2 == end) return false;
		//return (*p & (UINT64_MAX >> (8 - (end - p2)) * 8)) != 0;

		for (auto p2 = (const uchar*)(p); p2 < end; ++p2){
			if (*p2) return true;
		}

		return false;
	}

	inline bool bitall(const uchar* src, size_t sz){
		auto p = reinterpret_cast<const uint64_t*>(src);
		auto end = src + sz;

		for (; (const uchar*)(p + 1) < end; ++p){
			if (*p != ALL_ONE) return false;
		}
		for (auto p2 = (const uchar*)(p); p2 < end; ++p2){
			if (*p2 != 1) return false;
		}
		return true;

	}


}
#endif