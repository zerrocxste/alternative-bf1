class CImGuiFonts
{
public:
	void LoadFonts();
	ImFont* font_Main;
	ImFont* font_Credits;
};
extern std::unique_ptr<CImGuiFonts>m_pMenuFonts;
