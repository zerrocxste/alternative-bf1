#include "../includes.h"

std::unique_ptr<CDrawing>m_pDrawing = std::make_unique<CDrawing>();

void CDrawing::AddImage(const ImVec2& position, const ImVec2& size, const ImTextureID pTexture, const ImColor& color)
{
	auto window = ImGui::GetCurrentWindow();

	ImRect bb(position, ImVec2(position.x + size.x, position.y + size.y));

	window->DrawList->AddImage(pTexture, bb.Min, bb.Max, ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), ImGui::ColorConvertFloat4ToU32(color));
}

void CDrawing::AddCircleFilled(const ImVec2& position, float radius, const ImColor& color, int segments)
{
	auto window = ImGui::GetCurrentWindow();

	window->DrawList->AddCircleFilled(position, radius, ImGui::ColorConvertFloat4ToU32(color), segments);
}

void CDrawing::AddCircle(const ImVec2& position, float radius, const ImColor& color, int segments)
{
	auto window = ImGui::GetCurrentWindow();

	window->DrawList->AddCircle(position, radius, ImGui::ColorConvertFloat4ToU32(color), segments);
}

void CDrawing::AddRectFilled(const ImVec2& position, const ImVec2& size, const ImColor& color, float rounding)
{
	auto window = ImGui::GetCurrentWindow();

	window->DrawList->AddRectFilled(position, size, ImGui::ColorConvertFloat4ToU32(color), rounding);
}

void CDrawing::AddRect(const ImVec2& position, const ImVec2& size, const ImColor& color, float rounding)
{
	auto window = ImGui::GetCurrentWindow();

	window->DrawList->AddRect(position, size, ImGui::ColorConvertFloat4ToU32(color), rounding);
}

void CDrawing::DrawFillArea(float x, float y, float w, float h, const ImColor& color, float rounding)
{
	AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, rounding);
}

void CDrawing::AddTriangle(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImColor& color)
{
	auto window = ImGui::GetCurrentWindow();

	window->DrawList->AddTriangle(a, b, c, ImGui::ColorConvertFloat4ToU32(color));
}

void CDrawing::AddTriangleFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImColor& color)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	window->DrawList->AddTriangleFilled(a, b, c, ImGui::ColorConvertFloat4ToU32(color));
}

void CDrawing::AddLine(const ImVec2& from, const ImVec2& to, const ImColor& color, float thickness)
{
	auto window = ImGui::GetCurrentWindow();

	window->DrawList->AddLine(from, to, ImGui::ColorConvertFloat4ToU32(color), thickness);
}

void CDrawing::AddCircle3D(Vector position, float points, float radius, ImColor& color)
{
	float step = (float)IM_PI * 2.0f / points;

	for (float a = 0; a < (IM_PI * 2.0f); a += step)
	{
		Vector start(
			radius * cosf(a) + position.x, 
			position.y, 
			radius * sinf(a) + position.z
		);

		Vector end(
			radius * cosf(a + step) + position.x, 
			position.y,
			radius * sinf(a + step) + position.z
		);

		Vector start2d, end2d;

		if (!WorldToScreen(start, start2d) || !WorldToScreen(end, end2d))
			return;

		AddLine(ImVec2(start2d.x, start2d.y), ImVec2(end2d.x, end2d.y), color);
	}
}

void CDrawing::DrawLines(int x0, int y0, int x1, int y1, int r, int g, int b, int a)
{
	AddLine(ImVec2(x0, y0), ImVec2(x1, y1), ImColor(r, g, b));
}

//void CDrawing::DrawSkeleton(int i, int Bone1, int Bone2, float r, float g, float b, float a)
//{
//	float fScreenBone1[2];
//	float fScreenBone2[2];
//
//	if (g_Utils.bCalcScreen(g_PlayerExtraInfoList[i].vBones[Bone1], fScreenBone1) &&
//		g_Utils.bCalcScreen(g_PlayerExtraInfoList[i].vBones[Bone2], fScreenBone2))
//	{
//		AddLine(ImVec2(fScreenBone1[0], fScreenBone1[1]), ImVec2(fScreenBone2[0], fScreenBone2[1]), ImColor(r, g, b));
//	}
//}

void CDrawing::AddText(float x, float y, const ImColor& color, float fontSize, int flags, const char* format, ...)
{
	if (!format)
		return;

	auto& io = ImGui::GetIO();
	auto DrawList = ImGui::GetWindowDrawList();
	auto Font = io.Fonts->Fonts[0];

	char szBuff[256] = { '\0' };
	va_list vlist = nullptr;
	va_start(vlist, format);
	vsprintf_s(szBuff, format, vlist);
	va_end(vlist);

	DrawList->PushTextureID(io.Fonts->TexID);

	float size_font = fontSize;

	float size = size_font == 0.f ? Font->FontSize : size_font;
	ImVec2 text_size = Font->CalcTextSizeA(size, FLT_MAX, 0.f, szBuff);

	ImColor Color = ImColor(0.f, 0.f, 0.f, color.Value.w);

	if (flags & FL_CENTER_X)
		x -= text_size.x / 2.f;

	if (flags & FL_CENTER_Y)
		y -= text_size.x / 2.f;

	if (vars::font::style == 1)
		DrawList->AddText(Font, size, ImVec2(x + 1.f, y + 1.f), ImGui::ColorConvertFloat4ToU32(Color), szBuff);

	if (vars::font::style == 2)
	{
		DrawList->AddText(Font, size, ImVec2(x, y - 1.f), ImGui::ColorConvertFloat4ToU32(Color), szBuff);
		DrawList->AddText(Font, size, ImVec2(x, y + 1.f), ImGui::ColorConvertFloat4ToU32(Color), szBuff);
		DrawList->AddText(Font, size, ImVec2(x + 1.f, y), ImGui::ColorConvertFloat4ToU32(Color), szBuff);
		DrawList->AddText(Font, size, ImVec2(x - 1.f, y), ImGui::ColorConvertFloat4ToU32(Color), szBuff);

		DrawList->AddText(Font, size, ImVec2(x - 1.f, y - 1.f), ImGui::ColorConvertFloat4ToU32(Color), szBuff);
		DrawList->AddText(Font, size, ImVec2(x + 1.f, y - 1.f), ImGui::ColorConvertFloat4ToU32(Color), szBuff);
		DrawList->AddText(Font, size, ImVec2(x - 1.f, y + 1.f), ImGui::ColorConvertFloat4ToU32(Color), szBuff);
		DrawList->AddText(Font, size, ImVec2(x + 1.f, y + 1.f), ImGui::ColorConvertFloat4ToU32(Color), szBuff);
	}

	DrawList->AddText(Font, size, ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(color), szBuff);
	DrawList->PopTextureID();
}

void CDrawing::DrawBox(float x, float y, float w, float h, const ImColor& color)
{
	AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color);
}

void CDrawing::DrawBoxOutline(float x, float y, float w, float h, const ImColor& color)
{
	DrawBox(x + 1.f, y + 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
	DrawBox(x - 1.f, y - 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
	DrawBox(x - 1.f, y + 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
	DrawBox(x + 1.f, y - 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));

	DrawBox(x + 1.f, y, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
	DrawBox(x - 1.f, y, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
	DrawBox(x, y + 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
	DrawBox(x, y - 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));

	DrawBox(x, y, w, h, color);
}

void CDrawing::DrawRoundBox(float x, float y, float w, float h, const ImColor& color, float rounding)
{
	AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color, rounding);
}

void CDrawing::DrawRoundBoxOutline(float x, float y, float w, float h, const ImColor& color, float rounding)
{
	DrawRoundBox(x + 1.f, y + 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w), rounding);
	DrawRoundBox(x - 1.f, y - 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w), rounding);
	DrawRoundBox(x - 1.f, y + 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w), rounding);
	DrawRoundBox(x + 1.f, y - 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w), rounding);

	DrawRoundBox(x + 1.f, y, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w), rounding);
	DrawRoundBox(x - 1.f, y, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w), rounding);
	DrawRoundBox(x, y + 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w), rounding);
	DrawRoundBox(x, y - 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w), rounding);

	DrawRoundBox(x, y, w, h, color, rounding);
}

void CDrawing::DrawCornerBox(float x, float y, float w, float h, const ImColor& color)
{
	AddLine(ImVec2(x, y), ImVec2(x + w / 4.f, y), color);
	AddLine(ImVec2(x, y), ImVec2(x, y + h / 4.f), color);

	AddLine(ImVec2(x + w, y), ImVec2(x + w - w / 4.f, y), color);
	AddLine(ImVec2(x + w, y), ImVec2(x + w, y + h / 4.f), color);

	AddLine(ImVec2(x, y + h), ImVec2(x + w / 4.f, y + h), color);
	AddLine(ImVec2(x, y + h), ImVec2(x, y + h - h / 4.f), color);

	AddLine(ImVec2(x + w, y + h), ImVec2(x + w, y + h - h / 4.f), color);
	AddLine(ImVec2(x + w, y + h), ImVec2(x + w - w / 4.f, y + h), color);
}

void CDrawing::DrawCornerBoxOutline(float x, float y, float w, float h, const ImColor& color)
{
	DrawCornerBox(x - 1.f, y + 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
	DrawCornerBox(x - 1.f, y - 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
	DrawCornerBox(x + 1.f, y + 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
	DrawCornerBox(x + 1.f, y - 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));

	DrawCornerBox(x - 1.f, y, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
	DrawCornerBox(x, y - 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
	DrawCornerBox(x, y + 1.f, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));
	DrawCornerBox(x + 1.f, y, w, h, ImColor(0.f, 0.f, 0.f, color.Value.w));

	DrawCornerBox(x, y, w, h, color);
}

void CDrawing::DrawEspBox(int box_type, float x, float y, float w, float h, float r, float g, float b, float a)
{
	switch (box_type)
	{
	case 1:
		DrawBox(x, y, w, h, ImColor(r, g, b, a));
		break;
	case 2:
		DrawBoxOutline(x, y, w, h, ImColor(r, g, b, a));
		break;
	case 3:
		DrawCornerBox(x, y, w, h, ImColor(r, g, b, a));
		break;
	case 4:
		DrawCornerBoxOutline(x, y, w, h, ImColor(r, g, b, a));
		break;
	case 5:
		DrawRoundBox(x, y, w, h, ImColor(r, g, b, a), 3.f);
		break;
	case 6:
		DrawRoundBoxOutline(x, y, w, h, ImColor(r, g, b, a), 3.f);
		break;
	}
}

//void CDrawing::DrawCrosshair(int x, int y, const ImColor& color)
//{
//	int Size, Gap;
//	int Thickness;
//
//	Size = (int)cvar.crosshair_Size;
//	Gap = (int)cvar.crosshair_Gap;
//	Thickness = (int)cvar.crosshair_Thickness;
//
//	int thickness = Thickness - 1;
//
//	if (cvar.crosshair_Dynamic)
//	{
//		int Recoil = int(g_Local.vPunchangle[0] * cvar.crosshair_DynamicScale);
//
//		DrawFillArea(x + Recoil - Gap - Size + 1 - thickness, y - thickness, Size, Thickness, color);
//		DrawFillArea(x - Recoil + Gap, y - thickness, Size, Thickness, color);
//		DrawFillArea(x - thickness, y + Recoil - Gap - Size + 1 - thickness, Thickness, Size, color);
//		DrawFillArea(x - thickness, y - Recoil + Gap, Thickness, Size, color);
//	}
//	else
//	{
//		DrawFillArea(x - Gap - Size + 1 - thickness, y - thickness, Size, Thickness, color);
//		DrawFillArea(x + Gap, y - thickness, Size, Thickness, color);
//		DrawFillArea(x - thickness, y - Gap - Size + 1 - thickness, Thickness, Size, color);
//		DrawFillArea(x - thickness, y + Gap, Thickness, Size, color);
//	}
//}

void CDrawing::DrawDot(int x, int y, const ImColor& color)
{
	int Thickness;
	Thickness = (int)5;
	int thickness = Thickness - 1;

	DrawFillArea(x - thickness, y - thickness, Thickness, Thickness, color);
}

void CDrawing::HitMaker()
{
	float flScreenPosCenter[2];
	flScreenPosCenter[0] = ImGui::GetIO().DisplaySize.x / 2;
	flScreenPosCenter[1] = ImGui::GetIO().DisplaySize.y / 2;

	const int iLineSize = 8;

	const ImColor col(255, 255, 255);

	AddLine(ImVec2(flScreenPosCenter[0] - iLineSize, flScreenPosCenter[1] - iLineSize), ImVec2(flScreenPosCenter[0] - (iLineSize / 4), flScreenPosCenter[1] - (iLineSize / 4)), col);
	AddLine(ImVec2(flScreenPosCenter[0] - iLineSize, flScreenPosCenter[1] + iLineSize), ImVec2(flScreenPosCenter[0] - (iLineSize / 4), flScreenPosCenter[1] + (iLineSize / 4)), col);
	AddLine(ImVec2(flScreenPosCenter[0] + iLineSize, flScreenPosCenter[1] + iLineSize), ImVec2(flScreenPosCenter[0] + (iLineSize / 4), flScreenPosCenter[1] + (iLineSize / 4)), col);
	AddLine(ImVec2(flScreenPosCenter[0] + iLineSize, flScreenPosCenter[1] - iLineSize), ImVec2(flScreenPosCenter[0] + (iLineSize / 4), flScreenPosCenter[1] - (iLineSize / 4)), col);
}