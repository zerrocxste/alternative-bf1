#include "../includes.h"

std::unique_ptr<CImGuiFonts>m_pMenuFonts = std::make_unique<CImGuiFonts>();

void CImGuiFonts::LoadFonts()
{
	auto& io = ImGui::GetIO();
	this->font_Main = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdana.ttf", 21);
	this->font_Credits = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Verdana.ttf", 15);
}
