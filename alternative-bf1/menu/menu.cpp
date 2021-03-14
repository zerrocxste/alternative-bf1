#include "../includes.h"

std::unique_ptr<CMenu>m_pMenu = std::make_unique<CMenu>();

namespace menu_param
{
	ImVec2 vWindowSize;
	ImVec2 vWindowPos;
	bool wantRageQuit;
	bool hahaThisNiggaRealWantRageQuit;
}

#define WindowName  "alternative hack for battlefield 1"

ImVec2 operator+(ImVec2 p1, ImVec2 p2) { return ImVec2(p1.x + p2.x, p1.y + p2.y); }
ImVec2 operator-(ImVec2 p1, ImVec2 p2) { return ImVec2(p1.x - p2.x, p1.y - p2.y); }
ImVec2 operator/(ImVec2 p1, ImVec2 p2) { return ImVec2(p1.x / p2.x, p1.y / p2.y); }
ImVec2 operator*(ImVec2 p1, int value) { return ImVec2(p1.x * value, p1.y * value); }
ImVec2 operator*(ImVec2 p1, float value) { return ImVec2(p1.x * value, p1.y * value); }

ImVec4 operator+(float val, ImVec4 p2) { return ImVec4(val + p2.x, val + p2.y, val + p2.z, val + p2.w); }
ImVec4 operator*(float val, ImVec4 p2) { return ImVec4(val * p2.x, val * p2.y, val * p2.z, val * p2.w); }
ImVec4 operator*(ImVec4 p2, float val) { return ImVec4(val * p2.x, val * p2.y, val * p2.z, val * p2.w); }
ImVec4 operator-(ImVec4 p1, ImVec4 p2) { return ImVec4(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z, p1.w - p2.w); }
ImVec4 operator*(ImVec4 p1, ImVec4 p2) { return ImVec4(p1.x * p2.x, p1.y * p2.y, p1.z * p2.z, p1.w * p2.w); }
ImVec4 operator/(ImVec4 p1, ImVec4 p2) { return ImVec4(p1.x / p2.x, p1.y / p2.y, p1.z / p2.z, p1.w / p2.w); }

ImVec4 boxGaussianIntegral(ImVec4 x)
{
	const ImVec4 s = ImVec4(x.x > 0 ? 1.0f : -1.0f, x.y > 0 ? 1.0f : -1.0f, x.z > 0 ? 1.0f : -1.0f, x.w > 0 ? 1.0f : -1.0f);
	const ImVec4 a = ImVec4(fabsf(x.x), fabsf(x.y), fabsf(x.z), fabsf(x.w));
	const ImVec4 res = 1.0f + (0.278393f + (0.230389f + 0.078108f * (a * a)) * a) * a;
	const ImVec4 resSquared = res * res;
	return s - s / (resSquared * resSquared);
}

ImVec4 boxLinearInterpolation(ImVec4 x)
{
	const float maxClamp = 1.0f;
	const float minClamp = -1.0f;
	return ImVec4(x.x > maxClamp ? maxClamp : x.x < minClamp ? minClamp : x.x,
		x.y > maxClamp ? maxClamp : x.y < minClamp ? minClamp : x.y,
		x.z > maxClamp ? maxClamp : x.z < minClamp ? minClamp : x.z,
		x.w > maxClamp ? maxClamp : x.w < minClamp ? minClamp : x.w);
}

float boxShadow(ImVec2 lower, ImVec2 upper, ImVec2 point, float sigma, bool linearInterpolation)
{
	const ImVec2 pointLower = point - lower;
	const ImVec2 pointUpper = point - upper;
	const ImVec4 query = ImVec4(pointLower.x, pointLower.y, pointUpper.x, pointUpper.y);
	const ImVec4 pointToSample = query * (sqrtf(0.5f) / sigma);
	const ImVec4 integral = linearInterpolation ? 0.5f + 0.5f * boxLinearInterpolation(pointToSample) : 0.5f + 0.5f * boxGaussianIntegral(pointToSample);
	return (integral.z - integral.x) * (integral.w - integral.y);
}

struct RectangleShadowSettings
{
	// Inputs
	bool    linear = false;
	float   sigma = 11.585f;
	ImVec2  padding = ImVec2(50, 50);
	ImVec2  rectPos = ImVec2(50, 50);
	ImVec2  rectSize = ImVec2(120, 120);
	ImVec2  shadowOffset = ImVec2(0, 0);
	ImVec2  shadowSize = ImVec2(120, 50);
	ImColor shadowColor = ImColor(0.6f, 0.6f, 0.6f, 1.0f);

	int  rings = 10;
	int  spacingBetweenRings = 1;
	int  samplesPerCornerSide = 20;
	int  spacingBetweenSamples = 4;

	// Outputs
	int totalVertices = 0;
	int totalIndices = 0;
};

void drawRectangleShadowVerticesAdaptive(RectangleShadowSettings& settings)
{
	const int    samplesSpan = settings.samplesPerCornerSide * settings.spacingBetweenSamples;
	const int    halfWidth = static_cast<int>(settings.rectSize.x / 2);
	const int    numSamplesInHalfWidth = (halfWidth / settings.spacingBetweenSamples) == 0 ? 1 : halfWidth / settings.spacingBetweenSamples;
	const int    numSamplesWidth = samplesSpan > halfWidth ? numSamplesInHalfWidth : settings.samplesPerCornerSide;
	const int    halfHeight = static_cast<int>(settings.rectSize.y / 2);
	const int    numSamplesInHalfHeight = (halfHeight / settings.spacingBetweenSamples) == 0 ? 1 : halfHeight / settings.spacingBetweenSamples;
	const int    numSamplesHeight = samplesSpan > halfHeight ? numSamplesInHalfHeight : settings.samplesPerCornerSide;
	const int    numVerticesInARing = numSamplesWidth * 4 + numSamplesHeight * 4 + 4;
	const ImVec2 whiteTexelUV = ImGui::GetIO().Fonts->TexUvWhitePixel;
	const ImVec2 origin = ImGui::GetCursorScreenPos();
	const ImVec2 rectangleTopLeft = origin + settings.rectPos;
	const ImVec2 rectangleBottomRight = rectangleTopLeft + settings.rectSize;
	const ImVec2 rectangleTopRight = rectangleTopLeft + ImVec2(settings.rectSize.x, 0);
	const ImVec2 rectangleBottomLeft = rectangleTopLeft + ImVec2(0, settings.rectSize.y);

	ImColor shadowColor = settings.shadowColor;
	settings.totalVertices = numVerticesInARing * settings.rings;
	settings.totalIndices = 6 * (numVerticesInARing) * (settings.rings - 1);

	ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList->PrimReserve(settings.totalIndices, settings.totalVertices);
	const ImDrawVert* shadowVertices = drawList->_VtxWritePtr;
	ImDrawVert* vertexPointer = drawList->_VtxWritePtr;

	for (int r = 0; r < settings.rings; ++r)
	{
		const float  adaptiveScale = (r / 2.5f) + 1;
		const ImVec2 ringOffset = ImVec2(adaptiveScale * r * settings.spacingBetweenRings, adaptiveScale * r * settings.spacingBetweenRings);
		for (int j = 0; j < 4; ++j)
		{
			ImVec2      corner;
			ImVec2      direction[2];
			const float spacingBetweenSamplesOnARing = static_cast<float>(settings.spacingBetweenSamples);
			switch (j)
			{
			case 0:
				corner = rectangleTopLeft + ImVec2(-ringOffset.x, -ringOffset.y);
				direction[0] = ImVec2(1, 0) * spacingBetweenSamplesOnARing;
				direction[1] = ImVec2(0, 1) * spacingBetweenSamplesOnARing;
				for (int i = 0; i < numSamplesWidth; ++i)
				{
					const ImVec2 point = corner + direction[0] * (numSamplesWidth - i);
					shadowColor.Value.w = boxShadow(rectangleTopLeft, rectangleBottomRight, point - settings.shadowOffset, settings.sigma, settings.linear);
					vertexPointer->pos = point;
					vertexPointer->uv = whiteTexelUV;
					vertexPointer->col = shadowColor;
					vertexPointer++;
				}

				shadowColor.Value.w = boxShadow(rectangleTopLeft, rectangleBottomRight, corner - settings.shadowOffset, settings.sigma, settings.linear);
				vertexPointer->pos = corner;
				vertexPointer->uv = whiteTexelUV;
				vertexPointer->col = shadowColor;
				vertexPointer++;

				for (int i = 0; i < numSamplesHeight; ++i)
				{
					const ImVec2 point = corner + direction[1] * (i + 1);
					shadowColor.Value.w = boxShadow(rectangleTopLeft, rectangleBottomRight, point - settings.shadowOffset, settings.sigma, settings.linear);
					vertexPointer->pos = point;
					vertexPointer->uv = whiteTexelUV;
					vertexPointer->col = shadowColor;
					vertexPointer++;
				}
				break;
			case 1:
				corner = rectangleBottomLeft + ImVec2(-ringOffset.x, +ringOffset.y);
				direction[0] = ImVec2(1, 0) * spacingBetweenSamplesOnARing;
				direction[1] = ImVec2(0, -1) * spacingBetweenSamplesOnARing;
				for (int i = 0; i < numSamplesHeight; ++i)
				{
					const ImVec2 point = corner + direction[1] * (numSamplesHeight - i);
					shadowColor.Value.w = boxShadow(rectangleTopLeft, rectangleBottomRight, point - settings.shadowOffset, settings.sigma, settings.linear);
					vertexPointer->pos = point;
					vertexPointer->uv = whiteTexelUV;
					vertexPointer->col = shadowColor;
					vertexPointer++;
				}

				shadowColor.Value.w = boxShadow(rectangleTopLeft, rectangleBottomRight, corner - settings.shadowOffset, settings.sigma, settings.linear);
				vertexPointer->pos = corner;
				vertexPointer->uv = whiteTexelUV;
				vertexPointer->col = shadowColor;
				vertexPointer++;

				for (int i = 0; i < numSamplesWidth; ++i)
				{
					const ImVec2 point = corner + direction[0] * (i + 1);
					shadowColor.Value.w = boxShadow(rectangleTopLeft, rectangleBottomRight, point - settings.shadowOffset, settings.sigma, settings.linear);
					vertexPointer->pos = point;
					vertexPointer->uv = whiteTexelUV;
					vertexPointer->col = shadowColor;
					vertexPointer++;
				}
				break;
			case 2:
				corner = rectangleBottomRight + ImVec2(+ringOffset.x, +ringOffset.y);
				direction[0] = ImVec2(-1, 0) * spacingBetweenSamplesOnARing;
				direction[1] = ImVec2(0, -1) * spacingBetweenSamplesOnARing;
				for (int i = 0; i < numSamplesWidth; ++i)
				{
					const ImVec2 point = corner + direction[0] * (numSamplesWidth - i);
					shadowColor.Value.w = boxShadow(rectangleTopLeft, rectangleBottomRight, point - settings.shadowOffset, settings.sigma, settings.linear);
					vertexPointer->pos = point;
					vertexPointer->uv = whiteTexelUV;
					vertexPointer->col = shadowColor;
					vertexPointer++;
				}

				shadowColor.Value.w = boxShadow(rectangleTopLeft, rectangleBottomRight, corner - settings.shadowOffset, settings.sigma, settings.linear);
				vertexPointer->pos = corner;
				vertexPointer->uv = whiteTexelUV;
				vertexPointer->col = shadowColor;
				vertexPointer++;

				for (int i = 0; i < numSamplesHeight; ++i)
				{
					const ImVec2 point = corner + direction[1] * (i + 1);
					shadowColor.Value.w = boxShadow(rectangleTopLeft, rectangleBottomRight, point - settings.shadowOffset, settings.sigma, settings.linear);
					vertexPointer->pos = point;
					vertexPointer->uv = whiteTexelUV;
					vertexPointer->col = shadowColor;
					vertexPointer++;
				}
				break;
			case 3:
				corner = rectangleTopRight + ImVec2(+ringOffset.x, -ringOffset.y);
				direction[0] = ImVec2(-1, 0) * spacingBetweenSamplesOnARing;
				direction[1] = ImVec2(0, 1) * spacingBetweenSamplesOnARing;
				for (int i = 0; i < numSamplesHeight; ++i)
				{
					const ImVec2 point = corner + direction[1] * (numSamplesHeight - i);
					shadowColor.Value.w = boxShadow(rectangleTopLeft, rectangleBottomRight, point - settings.shadowOffset, settings.sigma, settings.linear);
					vertexPointer->pos = point;
					vertexPointer->uv = whiteTexelUV;
					vertexPointer->col = shadowColor;
					vertexPointer++;
				}

				shadowColor.Value.w = boxShadow(rectangleTopLeft, rectangleBottomRight, corner - settings.shadowOffset, settings.sigma, settings.linear);
				vertexPointer->pos = corner;
				vertexPointer->uv = whiteTexelUV;
				vertexPointer->col = shadowColor;
				vertexPointer++;

				for (int i = 0; i < numSamplesWidth; ++i)
				{
					const ImVec2 point = corner + direction[0] * (i + 1);
					shadowColor.Value.w = boxShadow(rectangleTopLeft, rectangleBottomRight, point - settings.shadowOffset, settings.sigma, settings.linear);
					vertexPointer->pos = point;
					vertexPointer->uv = whiteTexelUV;
					vertexPointer->col = shadowColor;
					vertexPointer++;
				}
				break;
			}
		}
	}

	ImDrawIdx idx = (ImDrawIdx)drawList->_VtxCurrentIdx;

	for (int r = 0; r < settings.rings - 1; ++r)
	{
		const ImDrawIdx startOfRingIndex = idx;
		for (int i = 0; i < numVerticesInARing - 1; ++i)
		{
			drawList->_IdxWritePtr[0] = idx + 0;
			drawList->_IdxWritePtr[1] = idx + 1;
			drawList->_IdxWritePtr[2] = idx + numVerticesInARing;
			drawList->_IdxWritePtr[3] = idx + 1;
			drawList->_IdxWritePtr[4] = idx + numVerticesInARing + 1;
			drawList->_IdxWritePtr[5] = idx + numVerticesInARing;

			idx += 1;
			drawList->_IdxWritePtr += 6;
		}

		drawList->_IdxWritePtr[0] = idx + 0;
		drawList->_IdxWritePtr[1] = startOfRingIndex + 0;
		drawList->_IdxWritePtr[2] = startOfRingIndex + numVerticesInARing;
		drawList->_IdxWritePtr[3] = idx + 0;
		drawList->_IdxWritePtr[4] = startOfRingIndex + numVerticesInARing;
		drawList->_IdxWritePtr[5] = idx + numVerticesInARing;

		drawList->_IdxWritePtr += 6;
		idx += 1;
	}
	drawList->_VtxCurrentIdx += settings.totalVertices;
}

RectangleShadowSettings shadowSettings;

void drawShadow()
{
	/*ImGui::SetNextWindowSize(ImVec2(400, 800), ImGuiCond_Once);
	ImGui::Begin("Test Shadows");
	ImGui::Checkbox("Linear Falloff", &shadowSettings.linear);
	ImGui::SliderFloat2("Rectangle Size", &shadowSettings.rectSize.x, 10, 256);
	ImGui::SliderFloat("Shadow Sigma", &shadowSettings.sigma, 0, 50);
	ImGui::SliderFloat2("Shadow Offset", &shadowSettings.shadowOffset.x, -10, 10);
	ImGui::SliderInt("Rings number", &shadowSettings.rings, 1, 10);
	ImGui::SliderInt("Rings spacing", &shadowSettings.spacingBetweenRings, 1, 20);
	ImGui::SliderInt("Corner samples", &shadowSettings.samplesPerCornerSide, 1, 20);
	ImGui::SliderInt("Corner spacing", &shadowSettings.spacingBetweenSamples, 1, 20);
	ImGui::ColorPicker3("Shadow Color", &shadowSettings.shadowColor.Value.x, ImGuiColorEditFlags_PickerHueWheel);
	ImGui::End();*/

	static ImColor backgroundColor(255, 255, 255, 0);
	//shadowSettings.shadowColor = ImColor(255, 172, 19);
	shadowSettings.shadowColor = ImColor(0, 0, 0);
	shadowSettings.rectPos = shadowSettings.padding;
	shadowSettings.rectSize = menu_param::vWindowSize;
	shadowSettings.shadowSize.x = menu_param::vWindowSize.x + 100.f;
	shadowSettings.shadowSize.y = menu_param::vWindowSize.y + 100.f;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImU32)backgroundColor);
	ImGui::PushStyleColor(ImGuiCol_Border, (ImU32)backgroundColor);

	ImGui::Begin("##MainWindowShadow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowPos(ImVec2(menu_param::vWindowPos.x - 50.f, menu_param::vWindowPos.y - 50.f));
	ImGui::SetWindowSize(shadowSettings.shadowSize);
	drawRectangleShadowVerticesAdaptive(shadowSettings);
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	const ImVec2 origin(ImGui::GetCursorScreenPos());
	drawList->AddRect(origin, origin + shadowSettings.shadowSize, ImColor(255, 0, 0, 1));
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}

namespace menu_utils
{
	void tabs(const char* lebel[], const int arr_, ImVec2 size, int& cur_page, float x_offset = 0.f, bool* click_callback = false)
	{
		for (int i = 0; i < arr_; i++)
		{
			auto& style = ImGui::GetStyle();
			ImVec4 save_shit[2];
			save_shit[0] = style.Colors[ImGuiCol_Button];
			save_shit[1] = style.Colors[ImGuiCol_ButtonHovered];
			if (i == cur_page)
			{
				style.Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_ButtonActive];
				style.Colors[ImGuiCol_ButtonHovered] = style.Colors[ImGuiCol_ButtonActive];
			}

			if (ImGui::Button(lebel[i], ImVec2(size.x + x_offset, size.y)))
			{
				if (cur_page != i && click_callback != nullptr)
					*click_callback = true;
				cur_page = i;
			}

			if (i == cur_page)
			{
				style.Colors[ImGuiCol_Button] = save_shit[0];
				style.Colors[ImGuiCol_ButtonHovered] = save_shit[1];
			}

			if (i != arr_ - 1)
				ImGui::SameLine();
		}
	}

	void set_color(bool white)
	{
		auto& style = ImGui::GetStyle();
		if (white)
		{
			style.Colors[ImGuiCol_FrameBg] = ImColor(200, 200, 200);
			style.Colors[ImGuiCol_FrameBgHovered] = ImColor(220, 220, 220);
			style.Colors[ImGuiCol_FrameBgActive] = ImColor(230, 230, 230);
			style.Colors[ImGuiCol_Separator] = ImColor(180, 180, 180);
			style.Colors[ImGuiCol_CheckMark] = ImColor(255, 172, 19);
			style.Colors[ImGuiCol_SliderGrab] = ImColor(255, 172, 19);
			style.Colors[ImGuiCol_SliderGrabActive] = ImColor(255, 172, 19);
			style.Colors[ImGuiCol_ScrollbarBg] = ImColor(120, 120, 120);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImColor(255, 172, 19);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImGui::GetStyleColorVec4(ImGuiCol_ScrollbarGrab);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImGui::GetStyleColorVec4(ImGuiCol_ScrollbarGrab);
			style.Colors[ImGuiCol_Header] = ImColor(160, 160, 160);
			style.Colors[ImGuiCol_HeaderHovered] = ImColor(200, 200, 200);
			style.Colors[ImGuiCol_Button] = ImColor(180, 180, 180);
			style.Colors[ImGuiCol_ButtonHovered] = ImColor(200, 200, 200);
			style.Colors[ImGuiCol_ButtonActive] = ImColor(230, 230, 230);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.78f, 0.78f, 0.78f, 1.f);
			style.Colors[ImGuiCol_WindowBg] = ImColor(220, 220, 220, 0.7 * 255);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
			style.Colors[ImGuiCol_Border] = ImVec4(0.72f, 0.72f, 0.72f, 0.70f);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.83f);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.75f, 0.75f, 0.75f, 0.87f);
			style.Colors[ImGuiCol_Text] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
			style.Colors[ImGuiCol_ChildBg] = ImVec4(0.72f, 0.72f, 0.72f, 0.76f);
			style.Colors[ImGuiCol_PopupBg] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.81f, 0.81f, 0.81f, 1.00f);
			style.Colors[ImGuiCol_Tab] = ImVec4(0.61f, 0.61f, 0.61f, 0.79f);
			style.Colors[ImGuiCol_TabHovered] = ImVec4(0.71f, 0.71f, 0.71f, 0.80f);
			style.Colors[ImGuiCol_TabActive] = ImVec4(0.77f, 0.77f, 0.77f, 0.84f);
			style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.73f, 0.73f, 0.73f, 0.82f);
			style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.58f, 0.58f, 0.58f, 0.84f);
		}
		else
		{
			ImGui::StyleColorsClassic();
			style.Colors[ImGuiCol_FrameBg] = ImColor(3, 3, 3);
			style.Colors[ImGuiCol_FrameBgHovered] = ImColor(20, 20, 20);
			style.Colors[ImGuiCol_FrameBgActive] = ImColor(30, 30, 30);
			style.Colors[ImGuiCol_Separator] = ImColor(8, 8, 8);
			style.Colors[ImGuiCol_CheckMark] = ImColor(255, 172, 19);
			style.Colors[ImGuiCol_SliderGrab] = ImColor(255, 172, 19);
			style.Colors[ImGuiCol_SliderGrabActive] = ImColor(255, 172, 19);
			style.Colors[ImGuiCol_PopupBg] = ImColor(12, 12, 12);
			style.Colors[ImGuiCol_ScrollbarBg] = ImColor(12, 12, 12);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImColor(255, 172, 19);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImGui::GetStyleColorVec4(ImGuiCol_ScrollbarGrab);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImGui::GetStyleColorVec4(ImGuiCol_ScrollbarGrab);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImGui::GetStyleColorVec4(ImGuiCol_ScrollbarGrab);
			style.Colors[ImGuiCol_Border] = ImColor(12, 12, 12);
			style.Colors[ImGuiCol_ChildBg] = ImColor(16, 16, 16);
			style.Colors[ImGuiCol_Header] = ImColor(16, 16, 16);
			style.Colors[ImGuiCol_HeaderHovered] = ImColor(20, 20, 20);
			style.Colors[ImGuiCol_HeaderActive] = ImColor(30, 30, 30);
			style.Colors[ImGuiCol_Button] = ImColor(8, 8, 8);
			style.Colors[ImGuiCol_ButtonHovered] = ImColor(20, 20, 20);
			style.Colors[ImGuiCol_ButtonActive] = ImColor(30, 30, 30);
			style.Colors[ImGuiCol_Text] = ImColor(255, 255, 255);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.78f, 0.78f, 0.78f, 1.f);
		}
	}
}

enum GLOBALS_PAGES
{
	PAGE_GLOBALS_AIMBOT,
	PAGE_GLOBALS_VISUALS,
	PAGE_GLOBALS_MISC,
	PAGE_GLOBALS_CONFIG
};

void CMenu::Run()
{
	static int m_Page = 0;

	ImGui::SetNextWindowSizeConstraints(ImVec2(499.f, 399.f), ImVec2(999.f, 899.f));
	ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x - 22.f, ImGui::GetIO().DisplaySize.y - 65.f), ImGuiCond_Once);

	auto WndFlags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
	ImGui::Begin(WindowName, nullptr, WndFlags);

	menu_param::vWindowSize = ImGui::GetWindowSize();
	menu_param::vWindowPos = ImGui::GetWindowPos();

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 4);
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 17);

	ImGui::PushFont(m_pMenuFonts->font_Main);
	ImGui::Text("alternative");
	ImGui::PopFont();

	ImGui::SameLine();

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 17);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);

	if (ImGui::GetWindowSize().x < 618.f)
		ImGui::GetFont()->FontSize = ImClamp(abs(634.f - ImGui::GetWindowSize().x) / 4.f, 16.f, 22.f);

	const char* lebels[] = {
		"Aimbot", "Visuals", "Misc", "Config"
	};

	menu_utils::tabs(lebels, IM_ARRAYSIZE(lebels), ImVec2(ImGui::GetWindowSize().x / 4.f - 44.f, 20), m_Page);

	ImGui::GetFont()->FontSize = 15.f;
	ImGui::BeginChild("1", ImVec2(0, ImGui::GetWindowSize().y - 60.f), true);	

	if (m_Page == PAGE_GLOBALS_AIMBOT)
	{
		ImGui::BeginChild("Aimbot", ImVec2(), true);

		static int i_aimbot_tabs = 0;
		const char* csz_aimbot_tabs[] = { "Undetected functional", "Potentially unsafe functionality (!)" };
		menu_utils::tabs(csz_aimbot_tabs, IM_ARRAYSIZE(csz_aimbot_tabs), ImVec2(ImGui::GetContentRegionAvail().x / 2.f, 20), i_aimbot_tabs, -4.f);

		if (i_aimbot_tabs == 0)
		{
			ImGui::Checkbox("Enable", &vars::aimbot::enable);
			ImGui::SameLine();
			ImGui::Checkbox("Only enemy", &vars::aimbot::only_enemy);
			ImGui::SliderFloat("Field of view", &vars::aimbot::fov, 0.f, 90.f);
			ImGui::SliderFloat("Smooth", &vars::aimbot::smooth, 0.f, 5.f);
			ImGui::Checkbox("Recoil compesation", &vars::aimbot::enable_recoil_compesation);
			if (vars::aimbot::enable_recoil_compesation)
				ImGui::SliderFloat("Recoil compesation value", &vars::aimbot::recoil_compesation, 0.f, 22.f);
			const char* pcszBones[] = { "Head", "Neck", "Body" };
			ImGui::Combo("Bone", &vars::aimbot::bone, pcszBones, IM_ARRAYSIZE(pcszBones));
			ImGui::Checkbox("Aiming on RMB", &vars::aimbot::aiming_on_rmb);
			ImGui::Checkbox("Disable if yourself in vehicle", &vars::aimbot::disable_on_vehicle);
			ImGui::Checkbox("Aim from vehicles only at the horse ", &vars::aimbot::aim_from_vehicles_only_horse);
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.5f, 0.5f, 1.f));
			std::string increase_fire_rate_label = vars::aimbot::increase_fire_rate == false ? "Enable increase fire rate"  : "Disable increase fire rate";
			if (ImGui::Button(increase_fire_rate_label.c_str(), ImVec2(ImGui::GetWindowSize().x - 15.f, 40)))
			{
				vars::aimbot::increase_fire_rate = !vars::aimbot::increase_fire_rate;
				m_pFeatures->InrecreaseFireRate(vars::aimbot::increase_fire_rate);
			}
			std::string no_recoil_label = vars::aimbot::no_recoil == false ? "Enable no recoil" : "Disable no recoil";
			if (ImGui::Button(no_recoil_label.c_str(), ImVec2(ImGui::GetWindowSize().x - 15.f, 40)))
			{
				vars::aimbot::no_recoil = !vars::aimbot::no_recoil;
				m_pFeatures->NoRecoil(vars::aimbot::no_recoil);
			}
			std::string no_weapon_overheating_label = vars::aimbot::weapon_no_overheating == false ? "Enable weapon no overheating" : "Disable weapon no overheating";
			if (ImGui::Button(no_weapon_overheating_label.c_str(), ImVec2(ImGui::GetWindowSize().x - 15.f, 40)))
			{
				vars::aimbot::weapon_no_overheating = !vars::aimbot::weapon_no_overheating;
				m_pFeatures->NoOverheatingWeapon(vars::aimbot::weapon_no_overheating);
			}
			ImGui::PopStyleColor();
		}

		ImGui::EndChild();
	}
	else if (m_Page == PAGE_GLOBALS_VISUALS)
	{
		ImGui::BeginChild("Visuals", ImVec2(), true);

		if (ImGui::TreeNode("Globals"))
		{
			ImGui::ColorEdit3("##invis enemy", vars::visuals::col_enemy_invis);
			ImGui::SameLine();
			ImGui::Text("enemy   ");
			ImGui::SameLine();
			ImGui::ColorEdit3("##invis teammate", vars::visuals::col_teammate_invis);
			ImGui::SameLine();
			ImGui::Text("teammate    ");
			ImGui::SameLine();
			ImGui::ColorEdit3("##vis enemy", vars::visuals::col_enemy_vis);
			ImGui::SameLine();
			ImGui::Text("vis enemy   ");
			ImGui::SameLine();
			ImGui::ColorEdit3("##vis teammate", vars::visuals::col_teammate_vis);
			ImGui::SameLine();
			ImGui::Text("vis teammate  ");

			ImGui::ColorEdit3("##invis enemy vehicle", vars::visuals::col_enemy_vehicle);
			ImGui::SameLine();
			ImGui::Text("enemy vehicle  ");
			ImGui::SameLine();
			ImGui::ColorEdit3("##vis teammate vehicle", vars::visuals::col_teammate_vehicle);
			ImGui::SameLine();
			ImGui::Text("teammate vehicle   ");

			ImGui::ColorEdit3("##invis enemy horse", vars::visuals::col_enemy_horse);
			ImGui::SameLine();
			ImGui::Text("enemy horse  ");
			ImGui::SameLine();
			ImGui::ColorEdit3("##vis teammate horse", vars::visuals::col_teammate_horse);
			ImGui::SameLine();
			ImGui::Text("teammate horse   ");

			const char* pcszBoxesType[] = { "Off", "Box", "Box outline", "Corner box", "Corner box outline",  "Round box", "Round box outline"  };
			ImGui::Combo("Bounding box", &vars::visuals::box_type, pcszBoxesType, IM_ARRAYSIZE(pcszBoxesType));

			ImGui::TreePop();
		}

		static char* pcszPlayerType[] = { "Enemies", "Enemies and teammates", "Off", };
		ImGui::Combo("Player type", &vars::visuals::player_type, pcszPlayerType, IM_ARRAYSIZE(pcszPlayerType));

		if (ImGui::TreeNode("ESP"))
		{
			ImGui::Checkbox("Name", &vars::visuals::name);
			ImGui::Checkbox("Health", &vars::visuals::health);
			ImGui::Checkbox("3D radar", &vars::visuals::radar_3d);
			ImGui::Checkbox("Offscreen", &vars::visuals::offscreen);
			ImGui::SliderFloat("Max distance offscreen", &vars::visuals::max_distance_offscreen, 10.f, 1000.f);
			ImGui::Checkbox("Distance", &vars::visuals::distance);
			ImGui::Checkbox("Vehicle", &vars::visuals::vehicle_esp);
			/*ImGui::Checkbox("Skeleton", &cvar.esp_hitboxes);
			ImGui::SameLine();
			ImGui::ColorEdit4("##skeletoncol", cvar.col_skeleton, ImGuiColorEditFlags_NoAlpha);*/
			ImGui::Spacing();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Font"))
		{
			static char* pcszFontType[] = { "Default", "Shadow", "Outline" };
			ImGui::SliderFloat("Font size", &vars::font::size, 0.f, 20.f, "%.0f", 1.0f);
			ImGui::Combo("Font type", &vars::font::style, pcszFontType, IM_ARRAYSIZE(pcszFontType));

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Extra"))
		{
			ImGui::Checkbox("Debug", &vars::extra::debug);

			ImGui::TreePop();
		}

		/*if (ImGui::TreeNode("Visual offscreen settings"))
		{
			ImGui::SliderFloat("Offscreen distance", &cvar.radar_range, 25.f, 150.f, "%.0f", 1.0f);
			ImGui::Checkbox("Draw distance", &cvar.offscreen_distance);
			ImGui::Combo("Visual style", &cvar.offscreen_visual_style, szOffscreenType, IM_ARRAYSIZE(szOffscreenType));
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Radar settings"))
		{
			ImGui::Checkbox("Enable", &cvar.radar);
			ImGui::Checkbox("Only enemy", &cvar.radar_teammates);
			ImGui::SliderFloat("Zoom", &cvar.radar_zoom, 0.1, 3.f);
			ImGui::Text("Radar background");
			ImGui::SameLine();
			ImGui::ColorEdit3("##radar_bg", cvar.radar_col);
			ImGui::TreePop();
		}*/

		ImGui::EndChild();
	}
	else if (m_Page == PAGE_GLOBALS_MISC)
	{
		ImGui::BeginChild("Misc", ImVec2(), true);

		std::string szEnableAllFunc = vars::global::enable ? "Turn off all cheat functions" : "Enable all cheat functions";
		if (ImGui::Button(szEnableAllFunc.c_str(), ImVec2(ImGui::GetWindowSize().x - 15.f, 40)))
			vars::global::enable = !vars::global::enable;
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.f, 0.5f, 1.5f, 1.f));
		if (ImGui::Button("Quit", ImVec2(ImGui::GetWindowSize().x - 15.f, 40)))
			globals::unload_dll = true;
		ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.5f, 0.5f, 1.f));
		if (ImGui::Button("Rage quit", ImVec2(ImGui::GetWindowSize().x - 15.f, 40)))
			menu_param::wantRageQuit = true;
		ImGui::PopStyleColor();

		ImGui::EndChild();
	}
	else if (m_Page == PAGE_GLOBALS_CONFIG)
	{
		ImGui::BeginChild("Config", ImVec2(), true);

		ImGui::Text("Coming soon...");

		if (ImGui::Button("Color style"))
		{
			static bool isWhite = true;
			isWhite = !isWhite;
			menu_utils::set_color(isWhite);
		}

		ImGui::EndChild();
	}

	ImGui::EndChild();
	ImGui::PushFont(m_pMenuFonts->font_Credits);
	ImGui::Text("Build date: %s, %s.", __DATE__, __TIME__);
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetWindowSize().x - ImGui::CalcTextSize("by zerrocxste").x - 15.f);
	ImGui::Text("by zerrocxste");
	ImGui::PopFont();

	ImGui::End();

	drawShadow();

	if (menu_param::wantRageQuit)
	{
		auto WndFlags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
		ImGui::SetNextWindowSize(ImVec2(350.f, 200.f));
		ImGui::Begin("stop stop stop", &menu_param::wantRageQuit, WndFlags);
		ImGui::PushFont(m_pMenuFonts->font_Main);
		ImGui::Text("stop stop stop");
		ImGui::PopFont();
		ImGui::Spacing();
		ImGui::BeginChild("rage quit", ImVec2(0, 0), true);
		ImGui::Text("u realy wants to rage quit:?");
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.f);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.5f, 0.5f, 1.f));
		if (ImGui::Button("YUP", ImVec2(ImGui::GetWindowSize().x - 15.f, 40)))
			menu_param::hahaThisNiggaRealWantRageQuit = true;
		ImGui::PopStyleColor();
		if (ImGui::Button("nope", ImVec2(ImGui::GetWindowSize().x - 15.f, 40)))
			menu_param::wantRageQuit = false;
		ImGui::EndChild();
		ImGui::End();

		if (menu_param::hahaThisNiggaRealWantRageQuit)
		{
			auto WndFlags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
			ImGui::SetNextWindowSize(ImVec2(300.f, 170.f));
			ImGui::Begin("ohh", &menu_param::wantRageQuit, WndFlags);
			ImGui::PushFont(m_pMenuFonts->font_Main);
			ImGui::Text("wow");
			ImGui::PopFont();
			ImGui::Spacing();
			ImGui::BeginChild("rage quit1", ImVec2(0, 0), true);
			ImGui::Text("haha this nigga really wants to rage quit");
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.5f, 0.5f, 1.f));
			if (ImGui::Button("YES", ImVec2(ImGui::GetWindowSize().x - 15.f, 40)))
				TerminateProcess(GetCurrentProcess(), 0);
			ImGui::PopStyleColor();
			if (ImGui::Button("ok i changed my choice", ImVec2(ImGui::GetWindowSize().x - 15.f, 40)))
			{
				menu_param::wantRageQuit = false;
				menu_param::hahaThisNiggaRealWantRageQuit = false;
			}	
			ImGui::EndChild();
			ImGui::End();
		}
	}
}
