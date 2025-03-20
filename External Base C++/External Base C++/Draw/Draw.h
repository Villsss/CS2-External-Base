#pragma once

#pragma once
// types of thread-safe render objects
enum class EDrawType : int
{
	NONE = 0,
	LINE,
	RECT,
	RECT_MULTICOLOR,
	CIRCLE,
	RING,
	TRIANGLE,
	POLYGON,
	TEXT,
	IMAGE,
	ARC
};

struct DrawObject_t
{
	DrawObject_t(const EDrawType nType, std::any&& pObject) :
		nType(nType), pObject(std::move(pObject)) {
	}

	EDrawType nType = EDrawType::NONE;
	std::any pObject = { };
};

#pragma region draw_objects_enumerations
enum ERectRenderFlags : unsigned int
{
	DRAW_RECT_NONE = 0,
	DRAW_RECT_OUTLINE = (1 << 0),
	DRAW_RECT_BORDER = (1 << 1),
	DRAW_RECT_FILLED = (1 << 2),
	DRAW_RECT_ALIGNED = (1 << 3),
};

enum ECircleRenderFlags : unsigned int
{
	DRAW_CIRCLE_NONE = 0,
	DRAW_CIRCLE_OUTLINE = (1 << 0),
	DRAW_CIRCLE_FILLED = (1 << 1)
};

enum ERingRenderFlags : unsigned int
{
	DRAW_RING_NONE = 0,
	DRAW_RING_COLLISIONS = (1 << 1)
};

enum ETriangleRenderFlags : unsigned int
{
	DRAW_TRIANGLE_NONE = 0,
	DRAW_TRIANGLE_OUTLINE = (1 << 0),
	DRAW_TRIANGLE_FILLED = (1 << 1)
};

enum EPolygonRenderFlags : unsigned int
{
	DRAW_POLYGON_NONE = 0,
	DRAW_POLYGON_OUTLINE = (1 << 0),
	DRAW_POLYGON_FILLED = (1 << 1)
};

enum ETextRenderFlags : unsigned int
{
	DRAW_TEXT_NONE = 0,
	DRAW_TEXT_DROPSHADOW = (1 << 0),
	DRAW_TEXT_OUTLINE = (1 << 1)
};
#pragma endregion

#pragma region draw_objects_structures
struct LineObject_t
{
	ImVec2 vecStart = { };
	ImVec2 vecEnd = { };
	ImU32 colLine = 0x0;
	float flThickness = 0.f;
};

struct RectObject_t
{
	ImVec2 vecMin = { };
	ImVec2 vecMax = { };
	ImU32 colRect = 0x0;
	unsigned int uFlags = 0x0;
	ImU32 colOutline = 0x0;
	float flRounding = 0.f;
	float flThickness = 0.f;
};

struct RectMultiColorObject_t
{
	ImVec2 vecMin = { };
	ImVec2 vecMax = { };
	ImU32 colUpperLeft = 0x0;
	ImU32 colUpperRight = 0x0;
	ImU32 colBottomLeft = 0x0;
	ImU32 colBottomRight = 0x0;
	unsigned int uFlags = 0x0;
	ImU32 colOutline = 0x0;
};

struct CircleObject_t
{
	ImVec2 vecCenter = { };
	float flRadius = 0.f;
	ImU32 colCircle = 0x0;
	int nSegments = 0;
	unsigned int uFlags = 0x0;
	ImU32 colOutline = 0x0;
	float flThickness = 0.f;
};

struct RingObject_t
{
	Vec3 vecCenter = { };
	float flRadius = 0.f;
	ImU32 colCircle = 0x0;
	int nSegments = 0;
	unsigned int uFlags = 0x0;
	float flThickness = 0.f;
};

struct TriangleObject_t
{
	ImVec2 vecFirst = { };
	ImVec2 vecSecond = { };
	ImVec2 vecThird = { };
	ImU32 colTriangle = 0x0;
	unsigned int uFlags = 0x0;
	ImU32 colOutline = 0x0;
	float flThickness = 0.f;
};

struct PolygonObject_t
{
	std::vector<ImVec2> vecPoints = { };
	ImU32 colPolygon = 0x0;
	unsigned int uFlags = 0x0;
	ImU32 colOutline = 0x0;
	bool bClosed = false;
	float flThickness = 0.f;
};

struct TextObject_t
{
	const ImFont* pFont = nullptr;
	float flFontSize = 0.f;
	ImVec2 vecPosition = { };
	std::string szText = { };
	ImU32 colText = 0x0;
	unsigned int uFlags = 0x0;
	ImU32 colOutline = 0x0;
};

struct ImageObject_t
{
	ImTextureID pTexture = nullptr;
	ImVec2 vecMin = { };
	ImVec2 vecMax = { };
	ImU32 colImage = 0x0;
	float flRounding = 0.f;
};

struct ArcObject_t
{
	ImVec2 vecPosition = { };
	float flRadius = 0.f;
	float flMinAngle = 0.f;
	float flMaxAngle = 0.f;
	ImU32 colFill = 0x0;
	float flThickness = 0.f;
};

//@Credits: Qo0 ( https://github.com/rollraw/qo0-csgo/blob/v1/base/utilities/draw.h )
namespace Draw
{
	void RenderDrawData(ImDrawList* pDrawList, std::vector<DrawObject_t> passeddata);

	void AddLine(std::vector<DrawObject_t>* vecDrawData, const ImVec2& vecStart, const ImVec2& vecEnd, const ImColor& colLine, float flThickness = 1.0f);
	void AddRect(std::vector<DrawObject_t>* vecDrawData, const ImVec2& vecMin, const ImVec2& vecMax, const ImColor& colRect, unsigned int uFlags = DRAW_RECT_NONE, const ImColor& colOutline = ImColor(0, 0, 0, 255), float flRounding = 0.f, float flThickness = 1.0f);
	void AddRectMultiColor(std::vector<DrawObject_t>* vecDrawData, const ImVec2& vecMin, const ImVec2& vecMax, const ImColor& colUpperLeft, const ImColor& colUpperRight, const ImColor& colBottomLeft, const ImColor& colBottomRight, unsigned int uFlags = DRAW_RECT_NONE, const ImColor& colOutline = ImColor(0, 0, 0, 255));
	void AddCircle(std::vector<DrawObject_t>* vecDrawData, const ImVec2& vecCenter, float flRadius, const ImColor& colCircle, int nSegments = 12, unsigned int uFlags = DRAW_CIRCLE_NONE, const ImColor& colOutline = ImColor(0, 0, 0, 255), float flThickness = 1.0f);
	void AddTriangle(std::vector<DrawObject_t>* vecDrawData, const ImVec2& vecFirst, const ImVec2& vecSecond, const ImVec2& vecThird, const ImColor& colTriangle, unsigned int uFlags = DRAW_TRIANGLE_NONE, const ImColor& colOutline = ImColor(0, 0, 0, 255), float flThickness = 1.0f);
	void AddPolygon(std::vector<DrawObject_t>* vecDrawData, std::vector<ImVec2>& vecPoints, const ImColor& colPolygon, unsigned int uFlags = DRAW_POLYGON_FILLED, const ImColor& colOutline = ImColor(0, 0, 0, 255), bool bClosed = true, float flThickness = 1.0f);
	void AddText(std::vector<DrawObject_t>* vecDrawData, const ImFont* pFont, float flFontSize, const ImVec2& vecPosition, const std::string& szText, const ImColor& colText, unsigned int uFlags = DRAW_TEXT_NONE, const ImColor& colOutline = ImColor(0, 0, 0, 255));
	void AddText(std::vector<DrawObject_t>* vecDrawData, const ImVec2& vecPosition, const std::string& szText, const ImColor& colText, int iFlags = DRAW_TEXT_NONE, const ImColor& colOutline = ImColor(0, 0, 0, 255));
	void AddImage(std::vector<DrawObject_t>* vecDrawData, ImTextureID pTexture, const ImVec2& vecMin, const ImVec2& vecMax, const ImColor& colImage = ImColor(255, 255, 255, 255), float flRounding = 0.f);
	void AddArcFilled(std::vector<DrawObject_t>* vecDrawData, const ImVec2& vecPosition, float flRadius, float flMinAngle, float flMaxAngle, const ImColor& colFill = ImColor(255, 255, 255, 255), float flThickness = 1.f);

	void AddDrawListRect(ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, ImU32 colRect, unsigned int uFlags = DRAW_RECT_NONE, ImU32 colOutline = IM_COL32(0, 0, 0, 255), float flRounding = 0.f, float flThickness = 1.0f);
	void AddDrawListRectMultiColor(ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, ImU32 colUpperLeft, ImU32 colUpperRight, ImU32 colBottomLeft, ImU32 colBottomRight, unsigned int uFlags = DRAW_RECT_NONE, ImU32 colOutline = IM_COL32(0, 0, 0, 255));
	void AddDrawListText(ImDrawList* pDrawList, const ImFont* pFont, float flFontSize, const ImVec2& vecPosition, const std::string& szText, ImU32 colText, unsigned int uFlags = DRAW_TEXT_NONE, ImU32 colOutline = IM_COL32(0, 0, 0, 255));

	inline bool	bInitialized = false;

	inline std::shared_mutex PlayerEspMutex = { };
	inline std::shared_mutex AimbotMutex = { };
	inline std::shared_mutex ItemMutex = { };
	inline std::shared_mutex SpectatorMutex = { };

	inline std::vector<DrawObject_t> playerEspData;
	inline std::vector<DrawObject_t> aimbotData;
	inline std::vector<DrawObject_t> ItemData;
}