#pragma once

namespace ItemEsp
{
	void RunItemEsp();
	void EachItem(std::vector<DrawObject_t>* vecDrawData, uintptr_t pawn, std::string name, int owner, view_matrix_t matrix);
}