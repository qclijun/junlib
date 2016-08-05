#pragma once
#include <set>
#include "PtrCmp.h"

namespace jun{

	// A shared_ptr set.
	template<typename Key, typename Cmp = PtrCmp<Key,std::less<Key>>, typename Alloc = std::allocator<std::shared_ptr<Key>> >
	class JSet{
	public:
		typedef std::shared_ptr<Key> value_type;
		typedef value_type key_type;
		typedef Cmp key_compare;
		typedef key_compare value_compare;
		typedef Alloc allocator_type;
		typedef std::set < key_type, key_compare, allocator_type> container_type;
		typedef typename container_type::iterator iterator;
		typedef typename container_type::const_iterator const_iterator;

		JSet() :ptr(std::make_shared<container_type>()){}

		iterator begin(){ return ptr->begin(); }
		const_iterator begin() const{ return ptr->begin(); }

		iterator end(){ return ptr->end(); }
		const_iterator end() const{ return ptr->end(); }

		std::pair<iterator, bool> insert(value_type value){ return ptr->insert(value); }
	
	private:
		std::shared_ptr < container_type > ptr;
	};


}
