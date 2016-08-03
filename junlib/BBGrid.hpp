namespace jun{
	template<typename Co>
	void BBGrid<Co>::insertBBox(bool h_spread, bool v_spread,  const value_type& bbox){
		Rect box = bbox->bounding_box();
		int grid_x1, grid_x2, grid_y1, grid_y2;
		gridCoords(box.x, box.y, grid_x1, grid_y1);
		gridCoords(box.xlast() - 1, box.ylast() - 1, grid_x2, grid_y2);

		if (!h_spread) grid_y2 = grid_y1;
		if (!v_spread) grid_x2 = grid_x1;

		for (int y = grid_y1; y <= grid_y2; ++y){
			for (int x = grid_x1; x <= grid_x2; ++x){
				_grid[toIndex(x, y)].add(bbox);
			}
		}
	}
}