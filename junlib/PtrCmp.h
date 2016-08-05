#pragma once

#include <memory>
#include <functional>

namespace jun{
	template<typename T, typename Cmp = std::less<T> >
	struct PtrCmp{
		PtrCmp() :cmp(){}
		bool operator()(const T* left, const T* right){
			return cmp(*left, *right);
		}
		bool operator()(std::shared_ptr<T> left, std::shared_ptr<T> right){
			return this->operator()(left.get(), right.get());
		}
	private:
		Cmp cmp;
	};

}
