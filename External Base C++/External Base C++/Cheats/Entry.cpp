#include "../include.h"

void __fastcall Run::Cheats() 
{
	uintptr_t client = memory.clientDLL;

	Draw::RenderDrawData(ImGui::GetBackgroundDrawList());
}