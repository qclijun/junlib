#pragma once

#include <set>
#include <memory>

#include "JContainer.h"
#include "Outline.h"

namespace jun{

	class OUTLINE_LIST : public JContainer<OUTLINE,std::set<std::shared_ptr<OUTLINE>, BoxCmp<OUTLINE> > >{
	public:
		typedef JContainer<OUTLINE, std::set<std::shared_ptr<OUTLINE>, BoxCmp<OUTLINE> > > BaseTy;
		typedef  BaseTy::container_type container_type;

		typedef  BaseTy::value_type value_type;
		typedef  BaseTy::iterator iterator;
		typedef  BaseTy::const_iterator const_iterator;

		int area() const{
			int result = 0;
			for (auto it = begin(); it != end(); ++it){
				result += (*it)->area();
			}
			return result;
		}

		RectS bounding_box() const{
			auto it = begin();
			RectS rect = (*it)->bounding_box();
			for (++it; it != end(); ++it){
				rect += (*it)->bounding_box();
			}
			return rect;
		}
	};

}
