#include "BBGrid.h"


namespace jun{
	void GridBase::gridCoords(int x, int y, int& grid_x, int& grid_y) const{
		grid_x = (x - bounding_box().x) / gridsize();
		grid_y = (y - bounding_box().y) / gridsize();
		if (grid_x < 0) grid_x = 0;
		else if (grid_x >= cols()) grid_x = cols() - 1;
		if (grid_y < 0) grid_y = 0;
		else if (grid_y >= rows()) grid_y = rows() - 1;
	}



}
