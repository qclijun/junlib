#include "je_new.h"
#include <iostream>

void* operator new(size_t n) throw (std::bad_alloc){

	while (true){
		if (auto p = je_malloc(n)){
			//std::cout << "new " << n << ": " << p << std::endl;
			return p;
		}
		auto handler = std::get_new_handler();
		if (handler) (*handler)();
		else throw std::bad_alloc();
	}
}

void* operator new[](size_t n) throw (std::bad_alloc){
	while (true){
		if (auto p = je_malloc(n)){
			//std::cout << "new[] " << n << ": " << p << std::endl;
			return p;
		}
		auto handler = std::get_new_handler();
		if (handler) (*handler)();
		else throw std::bad_alloc();
	}
}


void operator delete(void* p) throw(){
	je_free(p);
}

void operator delete[](void* p) throw(){
	je_free(p);
}