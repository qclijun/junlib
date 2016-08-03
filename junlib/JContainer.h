#pragma once

#include <vector>
#include <list>
#include <forward_list>
#include <deque>
#include <set>
#include <unordered_set>

#include <memory>


namespace jun{




	struct list_tag{};
	struct table_tag{};


	template<typename Container>
	struct container_category{
		typedef list_tag container_tag;
	};

	template<typename Key, typename Cmp, typename Alloc>
	struct container_category < std::set<Key, Cmp, Alloc> > {
		typedef table_tag container_tag;
	};

	template<typename Key, typename KeyHash, typename KeyEqual, typename Alloc>
	struct container_category < std::unordered_set<Key, KeyHash,KeyEqual,Alloc> > {
		typedef table_tag container_tag;
	};


	template<typename Container>
	inline typename container_category<Container>::container_tag get_co_tag(const Container& c){
		typedef typename container_category<Container>::container_tag _Cat;
		return (_Cat);
	}



	//   a java-like container to contain big objects, this container
	//   is a shared ptr, so use direct not refrence or pointer.
	//   Ö¸ÕëÈİÆ÷
	template<typename T, typename Co = std::vector<std::shared_ptr<T>> >
	class JContainer{
	private:
		typedef JContainer<T, Co> Self;

	public:
		typedef Co container_type;
		typedef typename Co::value_type value_type;
		typedef typename Co::size_type size_type;
		typedef typename Co::difference_type difference_type;
		typedef typename Co::iterator iterator;
		typedef typename Co::const_iterator const_iterator;



		iterator begin() { return ptr->begin(); }
		const_iterator begin() const { return ptr->begin(); }
		iterator end(){ return ptr->end(); }
		const_iterator end() const{ return ptr->end(); }

		JContainer() :ptr(std::make_shared<Co>()){}

		void reset(){ ptr.reset(); }

		void push_back(value_type p){
			ptr->push_back(p);
		}

		void  insert(value_type p){
			ptr->insert(p);
		}


		void add(value_type p, list_tag){
			ptr->push_back(p);
		}

		void add(value_type p, table_tag){
			ptr->insert(p);
		}

		void add(value_type p){
			add(p, container_category<container_type>::container_tag());
		}


	private:
		std::shared_ptr<Co> ptr;
	};

}