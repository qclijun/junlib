#pragma once

#include "Rect.h"

namespace jun{
	class GridBase{
	public:
		GridBase(Rect region, int gridsize) : box_(region), gridsize_(gridsize),
			rows_((region.height - 1) / gridsize_ + 1),
			cols_((region.width - 1) / gridsize_ + 1),
			gridnum_(rows_*cols_){}

		virtual ~GridBase(){}

		Rect bounding_box() const{ return box_; }
		int gridsize() const{ return gridsize_; }
		int rows() const{ return rows_; }
		int cols() const{ return cols_; }
		int gridnum() const{ return gridnum_; }

		void gridCoords(int x, int y, int& grid_x, int& grid_y) const;
		int toIndex(int grid_x, int grid_y) const{
			return grid_y*cols_ + grid_x;
		}
	private:
		const Rect box_;
		const int gridsize_;
		const int rows_;
		const int cols_;
		const int gridnum_;
	};

	template<typename Container>
	class BBGrid : public GridBase{
		typedef BBGrid<Container> Self;
	public:
		typedef typename Container::value_type value_type;
	public:
		BBGrid(Rect box, int gridsize) :GridBase(box, gridsize){
			grid = new Container[gridnum()];
		}


		virtual ~BBGrid(){
			delete[] grid;
		}

		BBGrid(const Self&) = delete;
		BBGrid(Self&&) = delete;

		BBGrid& operator=(const Self&) = delete;
		BBGrid& operator=(Self&&) = delete;

		void insertBBox(bool h_spread, bool v_spread, const value_type& bbox);

		Container& operator()(int x, int y){
			return grid[toIndex(x, y)];
		}


	private:
		Container* grid;
	};


}

#include "BBGrid.hpp"
