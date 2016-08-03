#pragma once


#define BEGIN(space_name) namespace space_name{
#define END(space_name) }




#define JUN_	jun::
#define JUN_BEGIN	namespace jun{
#define JUN_END		}

#ifdef _DEBUG
const bool ARG_CHECK = true;
const bool INDEX_CHECK = true;
const bool TYPE_CHECK = true;

#else
const bool ARG_CHECK = false;
const bool INDEX_CHECK = false;
const bool TYPE_CHECK = false;
#endif

JUN_BEGIN

enum DataType{
	T_8U	= 0x10,
	T_8S	= 0x11,
	T_16U	= 0x22,
	T_16S	= 0x23, 
	T_32S	= 0x44, 
	T_32F	= 0x45, 
	T_64F	= 0x86
};


const int CN_SHIFT = 8;

inline int makeType(DataType t, int cn){
	return ((cn - 1) << CN_SHIFT) + t;
}

inline int getCN(int type){
	return (type >> CN_SHIFT) + 1;
}

inline int typeSize(int type){
	return (type & 0xf0) >> 4;
}

inline int elemSize(int type){
	return typeSize(type)*getCN(type);
}

#define T_8UC1 makeType(DataType::T_8U,1)
#define T_8UC2 makeType(DataType::T_8U,2)
#define T_8UC3 makeType(DataType::T_8U,3)
#define T_8UC4 makeType(DataType::T_8U,4)
#define T_8UC(n) makeType(DataType::T_8U,(n))

#define T_8SC1 makeType(DataType::T_8S,1)
#define T_8SC2 makeType(DataType::T_8S,2)
#define T_8SC3 makeType(DataType::T_8S,3)
#define T_8SC4 makeType(DataType::T_8S,4)
#define T_8SC(n) makeType(DataType::T_8S,(n))

#define T_16UC1 makeType(DataType::T_16U,1)
#define T_16UC2 makeType(DataType::T_16U,2)
#define T_16UC3 makeType(DataType::T_16U,3)
#define T_16UC4 makeType(DataType::T_16U,4)
#define T_16UC(n) makeType(DataType::T_16U,(n))

#define T_16SC1 makeType(DataType::T_16S,1)
#define T_16SC2 makeType(DataType::T_16S,2)
#define T_16SC3 makeType(DataType::T_16S,3)
#define T_16SC4 makeType(DataType::T_16S,4)
#define T_16SC(n) makeType(DataType::T_16S,(n))

#define T_32SC1 makeType(DataType::T_32S,1)
#define T_32SC2 makeType(DataType::T_32S,2)
#define T_32SC3 makeType(DataType::T_32S,3)
#define T_32SC4 makeType(DataType::T_32S,4)
#define T_32SC(n) makeType(DataType::T_32S,(n))

#define T_32FC1 makeType(DataType::T_32F,1)
#define T_32FC2 makeType(DataType::T_32F,2)
#define T_32FC3 makeType(DataType::T_32F,3)
#define T_32FC4 makeType(DataType::T_32F,4)
#define T_32FC(n) makeType(DataType::T_32F,(n))

#define T_64FC1 makeType(DataType::T_64F,1)
#define T_64FC2 makeType(DataType::T_64F,2)
#define T_64FC3 makeType(DataType::T_64F,3)
#define T_64FC4 makeType(DataType::T_64F,4)
#define T_64FC(n) makeType(DataType::T_64F,(n))

JUN_END