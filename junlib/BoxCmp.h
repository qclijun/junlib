#pragma once
#include <memory>

#include "Rect.h"

namespace jun{
	template<typename T>
	struct BoxCmp{
		bool operator()(T* left, T* right){
			if (left->bounding_box() < right->bounding_box()) return true;
			if (right->bounding_box() < left->bounding_box()) return false;
			return left < right;
		}
		bool operator()(std::shared_ptr<T> left, std::shared_ptr<T> right){
			return this->operator()(left.get(), right.get());
		}
	};

	//先比较左边，后比较上边
	template<typename T>
	struct BoxCmp_LT : public std::binary_function<std::shared_ptr<T>,std::shared_ptr<T>,bool>{
		bool operator()(std::shared_ptr<T> left, std::shared_ptr<T> right){
			Rect box1 = left->bounding_box();
			Rect box2 = right->bounding_box();
			int result = box1.left() - box2.left();
			if (result != 0) return result < 0;
			result = box1.top() - box2.top();
			if (result != 0) return result < 0;
			return left->get() < right->get();
		}
	};
}


