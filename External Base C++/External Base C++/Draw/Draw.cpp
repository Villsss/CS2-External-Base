#include "../include.h"

#define M_DEG2RAD( x )		DirectX::XMConvertToRadians( x )

std::vector<DrawObject_t> drawDataTemp;

void Draw::RenderDrawData(ImDrawList* pDrawList)
{
	{
		std::unique_lock<std::shared_mutex> lock(Draw::drawMutex);
		drawDataTemp.clear();
		drawDataTemp.reserve(1024);
		drawDataTemp = playerEspData;
	}

	for (const auto& data : drawDataTemp)
	{
		if (!data.pObject.has_value())
			continue;

		switch (data.nType)
		{
		case EDrawType::LINE:
		{
			const auto& pObject = std::any_cast<LineObject_t>(data.pObject);
			pDrawList->AddLine(pObject.vecStart, pObject.vecEnd, pObject.colLine, pObject.flThickness);
			break;
		}
		case EDrawType::RECT:
		{
			const auto& pObject = std::any_cast<RectObject_t>(data.pObject);

			if (pObject.uFlags & DRAW_RECT_ALIGNED)
				AddDrawListRect(pDrawList,
					{ floor(pObject.vecMin.x), floor(pObject.vecMin.y) },
					{ floor(pObject.vecMax.x), floor(pObject.vecMax.y) },
					pObject.colRect, pObject.uFlags, pObject.colOutline, pObject.flRounding, pObject.flThickness);
			else
				AddDrawListRect(pDrawList, pObject.vecMin, pObject.vecMax, pObject.colRect, pObject.uFlags, pObject.colOutline, pObject.flRounding, pObject.flThickness);

			break;
		}
		case EDrawType::RECT_MULTICOLOR:
		{
			const auto& pObject = std::any_cast<RectMultiColorObject_t>(data.pObject);

			if (pObject.uFlags & DRAW_RECT_ALIGNED)
				AddDrawListRectMultiColor(pDrawList,
					{ floor(pObject.vecMin.x), floor(pObject.vecMin.y) },
					{ floor(pObject.vecMax.x), floor(pObject.vecMax.y) },
					pObject.colUpperLeft, pObject.colUpperRight,
					pObject.colBottomLeft, pObject.colBottomRight,
					pObject.uFlags, pObject.colOutline);
			else
				AddDrawListRectMultiColor(pDrawList,
					pObject.vecMin,
					pObject.vecMax,
					pObject.colUpperLeft, pObject.colUpperRight,
					pObject.colBottomLeft, pObject.colBottomRight,
					pObject.uFlags, pObject.colOutline);

			break;
		}
		case EDrawType::CIRCLE:
		{
			const auto& pObject = std::any_cast<CircleObject_t>(data.pObject);

			if (pObject.uFlags & DRAW_CIRCLE_FILLED)
				pDrawList->AddCircleFilled(pObject.vecCenter, pObject.flRadius, pObject.colCircle, pObject.nSegments);
			else
				pDrawList->AddCircle(pObject.vecCenter, pObject.flRadius, pObject.colCircle, pObject.nSegments, pObject.flThickness);

			if (pObject.uFlags & DRAW_CIRCLE_OUTLINE)
				pDrawList->AddCircle(pObject.vecCenter, pObject.flRadius + 1.0f, pObject.colOutline, pObject.nSegments, pObject.flThickness + 1.0f);

			break;
		}
		case EDrawType::TRIANGLE:
		{
			const auto& pObject = std::any_cast<TriangleObject_t>(data.pObject);

			if (pObject.uFlags & DRAW_TRIANGLE_FILLED)
				pDrawList->AddTriangleFilled(pObject.vecFirst, pObject.vecSecond, pObject.vecThird, pObject.colTriangle);
			else
				pDrawList->AddTriangle(pObject.vecFirst, pObject.vecSecond, pObject.vecThird, pObject.colTriangle, pObject.flThickness);

			if (pObject.uFlags & DRAW_TRIANGLE_OUTLINE)
				pDrawList->AddTriangle(pObject.vecFirst, pObject.vecSecond, pObject.vecThird, pObject.colOutline, pObject.flThickness + 1.0f);

			break;
		}
		case EDrawType::POLYGON:
		{
			const auto& pObject = std::any_cast<PolygonObject_t>(data.pObject);

			if (pObject.uFlags & DRAW_POLYGON_FILLED)
				pDrawList->AddConvexPolyFilled(pObject.vecPoints.data(), static_cast<int>(pObject.vecPoints.size()), pObject.colPolygon);
			else
				pDrawList->AddPolyline(pObject.vecPoints.data(), static_cast<int>(pObject.vecPoints.size()), pObject.colPolygon, pObject.bClosed, pObject.flThickness);

			if (pObject.uFlags & DRAW_POLYGON_OUTLINE)
				pDrawList->AddPolyline(pObject.vecPoints.data(), static_cast<int>(pObject.vecPoints.size()), pObject.colOutline, pObject.bClosed, pObject.flThickness + 1.0f);

			break;
		}
		case EDrawType::TEXT:
		{
			const auto& pObject = std::any_cast<TextObject_t>(data.pObject);
			AddDrawListText(pDrawList, pObject.pFont, pObject.flFontSize, pObject.vecPosition, pObject.szText, pObject.colText, pObject.uFlags, pObject.colOutline);
			break;
		}
		case EDrawType::IMAGE:
		{
			const auto& pObject = std::any_cast<ImageObject_t>(data.pObject);
			pDrawList->AddImageRounded(pObject.pTexture, pObject.vecMin, pObject.vecMax, ImVec2(0, 0), ImVec2(1, 1), pObject.colImage, pObject.flRounding);
			break;
		}
		case EDrawType::ARC:
		{
			const auto& pObject = std::any_cast<ArcObject_t>(data.pObject);
			pDrawList->PathArcTo(pObject.vecPosition, pObject.flRadius, M_DEG2RAD(pObject.flMinAngle), M_DEG2RAD(pObject.flMaxAngle), 32);
			pDrawList->PathStroke(pObject.colFill, false, pObject.flThickness);
			break;
		}
		default:
			break;
		}
	}
}

#pragma region draw_render

void Draw::AddLine(std::vector<DrawObject_t>* vecDrawData, const ImVec2& vecStart, const ImVec2& vecEnd, const ImColor& colLine, float flThickness)
{
	vecDrawData->emplace_back(EDrawType::LINE, std::make_any<LineObject_t>(LineObject_t{ vecStart, vecEnd, colLine, flThickness }));
}

void Draw::AddRect(std::vector<DrawObject_t>* vecDrawData, const ImVec2& vecMin, const ImVec2& vecMax, const ImColor& colRect, unsigned int uFlags, const ImColor& colOutline, float flRounding, float flThickness)
{
	vecDrawData->emplace_back(EDrawType::RECT, std::make_any<RectObject_t>(RectObject_t{ vecMin, vecMax, colRect, uFlags, colOutline, flRounding, flThickness }));
}

void Draw::AddRectMultiColor(std::vector<DrawObject_t>* vecDrawData, const ImVec2& vecMin, const ImVec2& vecMax, const ImColor& colUpperLeft, const ImColor& colUpperRight, const ImColor& colBottomLeft, const ImColor& colBottomRight, unsigned int uFlags, const ImColor& colOutline)
{
	vecDrawData->emplace_back(EDrawType::RECT_MULTICOLOR, std::make_any<RectMultiColorObject_t>(RectMultiColorObject_t{ vecMin, vecMax, colUpperLeft, colUpperRight, colBottomLeft, colBottomRight, uFlags, colOutline }));
}

void Draw::AddCircle(std::vector<DrawObject_t>* vecDrawData, const ImVec2& vecCenter, float flRadius, const ImColor& colCircle, int nSegments, unsigned int uFlags, const ImColor& colOutline, float flThickness)
{
	vecDrawData->emplace_back(EDrawType::CIRCLE, std::make_any<CircleObject_t>(CircleObject_t{ vecCenter, flRadius, colCircle, nSegments, uFlags, colOutline, flThickness }));
}

void Draw::AddTriangle(std::vector<DrawObject_t>* vecDrawData, const ImVec2& vecFirst, const ImVec2& vecSecond, const ImVec2& vecThird, const ImColor& colTriangle, unsigned int uFlags, const ImColor& colOutline, float flThickness)
{
	vecDrawData->emplace_back(EDrawType::TRIANGLE, std::make_any<TriangleObject_t>(TriangleObject_t{ vecFirst, vecSecond, vecThird, colTriangle, uFlags, colOutline, flThickness }));
}

void Draw::AddPolygon(std::vector<DrawObject_t>* vecDrawData, std::vector<ImVec2>& vecPoints, const ImColor& colPolygon, unsigned int uFlags, const ImColor& colOutline, bool bClosed, float flThickness)
{
	vecDrawData->emplace_back(EDrawType::POLYGON, std::make_any<PolygonObject_t>(PolygonObject_t{ std::move(vecPoints), colPolygon, uFlags, colOutline, bClosed, flThickness }));
}

void Draw::AddText(std::vector<DrawObject_t>* vecDrawData, const ImFont* pFont, float flFontSize, const ImVec2& vecPosition, const std::string& szText, const ImColor& colText, unsigned int uFlags, const ImColor& colOutline)
{
	if (pFont->ContainerAtlas == nullptr)
		return;

	// check is only one flag selected
	IM_ASSERT(ImIsPowerOfTwo(uFlags == DRAW_TEXT_NONE || uFlags & (DRAW_TEXT_DROPSHADOW | DRAW_TEXT_OUTLINE)));

	vecDrawData->emplace_back(EDrawType::TEXT, std::make_any<TextObject_t>(TextObject_t{ pFont, flFontSize, vecPosition, szText, colText, uFlags, colOutline }));
}

void Draw::AddText(std::vector<DrawObject_t>* vecDrawData, const ImVec2& vecPosition, const std::string& szText, const ImColor& colText, int iFlags, const ImColor& colOutline)
{
	AddText(vecDrawData, nullptr, 0.f, vecPosition, szText, colText, iFlags, colOutline);
}

void Draw::AddImage(std::vector<DrawObject_t>* vecDrawData, ImTextureID pTexture, const ImVec2& vecMin, const ImVec2& vecMax, const ImColor& colImage, float flRounding)
{
	vecDrawData->emplace_back(EDrawType::IMAGE, std::make_any<ImageObject_t>(ImageObject_t{ pTexture, vecMin, vecMax, colImage, flRounding }));
}

void Draw::AddArcFilled(std::vector<DrawObject_t>* vecDrawData, const ImVec2& vecPosition, float flRadius, float flMinAngle, float flMaxAngle, const ImColor& colFill, float flThickness) {
	vecDrawData->emplace_back(EDrawType::ARC, std::make_any<ArcObject_t>(ArcObject_t{ vecPosition,flRadius,flMinAngle,flMaxAngle,colFill,flThickness }));
}
#pragma endregion

#pragma region draw_bindings
void Draw::AddDrawListRect(ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, ImU32 colRect, unsigned int uFlags, ImU32 colOutline, float flRounding, float flThickness)
{
	if (uFlags & DRAW_RECT_FILLED)
		pDrawList->AddRectFilled(vecMin, vecMax, colRect, flRounding);
	else
		pDrawList->AddRect(vecMin, vecMax, colRect, flRounding, 0, flThickness);

	if (uFlags & DRAW_RECT_BORDER)
		pDrawList->AddRect({ vecMin.x + 1.0f, vecMin.y + 1.0f }, { vecMax.x - 1.0f, vecMax.y - 1.0f }, colOutline, flRounding, 0, 1.0f);

	if (uFlags & DRAW_RECT_OUTLINE)
		pDrawList->AddRect({ vecMin.x - 1.0f, vecMin.y - 1.0f }, { vecMax.x + 1.0f, vecMax.y + 1.0f }, colOutline, flRounding, 0, 1.0f);
}

void Draw::AddDrawListRectMultiColor(ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, ImU32 colUpperLeft, ImU32 colUpperRight, ImU32 colBottomLeft, ImU32 colBottomRight, unsigned int uFlags, ImU32 colOutline)
{
	// @todo:
	// implement thickness

	if (uFlags & DRAW_RECT_FILLED)
		pDrawList->AddRectFilledMultiColor(vecMin, vecMax, colUpperLeft, colUpperRight, colBottomRight, colBottomLeft);
	else
	{
		pDrawList->AddRectFilledMultiColor(vecMin, { vecMax.x, vecMin.y + 1.f }, colUpperLeft, colUpperRight, colUpperRight, colUpperLeft); // top
		pDrawList->AddRectFilledMultiColor(ImVec2(vecMin.x, vecMax.y), { vecMax.x, vecMax.y - 1.f }, colBottomLeft, colBottomRight, colBottomRight, colBottomLeft); // bottom

		pDrawList->AddRectFilledMultiColor(vecMin, ImVec2(vecMin.x + 1.f, vecMax.y), colUpperLeft, colUpperLeft, colBottomLeft, colBottomLeft); // left
		pDrawList->AddRectFilledMultiColor(ImVec2(vecMax.x, vecMin.y), { vecMax.x - 1.f, vecMax.y }, colUpperRight, colUpperRight, colBottomRight, colBottomRight); // right
	}

	if (uFlags & DRAW_RECT_BORDER)
		pDrawList->AddRect({ vecMin.x + 1.0f, vecMin.y + 1.0f }, { vecMax.x - 1.0f, vecMax.y - 1.0f }, colOutline);

	if (uFlags & DRAW_RECT_OUTLINE)
		pDrawList->AddRect({ vecMin.x - 1.0f, vecMin.y - 1.0f }, { vecMax.x + 1.0f, vecMax.y + 1.0f }, colOutline);
}

void Draw::AddDrawListText(ImDrawList* pDrawList, const ImFont* pFont, float flFontSize, const ImVec2& vecPosition, const std::string& szText, ImU32 colText, unsigned int uFlags, ImU32 colOutline)
{
	pDrawList->PushTextureID(pFont->ContainerAtlas->TexID);

	if (uFlags & DRAW_TEXT_OUTLINE)
	{
		static const ImVec2 offsets[] = { {-1.0f, -1.0f}, {1.0f, -1.0f}, {-1.0f, 1.0f}, {1.0f, 1.0f} };
		for (const auto& offset : offsets)
			pDrawList->AddText(pFont, flFontSize, { vecPosition.x + offset.x, vecPosition.y + offset.y }, colOutline, szText.c_str());
	}
	else if (uFlags & DRAW_TEXT_DROPSHADOW)
	{
		pDrawList->AddText(pFont, flFontSize, { vecPosition.x + 1.0f, vecPosition.y + 1.0f }, colOutline, szText.c_str());
	}

	pDrawList->AddText(pFont, flFontSize, vecPosition, colText, szText.c_str());
	pDrawList->PopTextureID();
}
#pragma endregion