#include "BLOBNBOX.h"
#include "JContainer.h"

namespace jun{
	class BLOBNBOX_LIST : public JContainer<BLOBNBOX, std::set<std::shared_ptr<BLOBNBOX>,BoxCmp<BLOBNBOX> > >{
	public:
		typedef JContainer<BLOBNBOX, std::set<std::shared_ptr<BLOBNBOX>, BoxCmp<BLOBNBOX> > > BaseTy;
		typedef BaseTy::container_type container_type;
		typedef BaseTy::iterator iterator;
		typedef BaseTy::value_type value_type;


		int lineCrossing() const{
			int total = 0;
			for (auto it = begin(); it != end(); ++it){
				total += (*it)->line_crossings;
			}
			return total;
		}
		bool atLeast2LineCrossing() const{
			return lineCrossing() >= 2;
		}

	};

}