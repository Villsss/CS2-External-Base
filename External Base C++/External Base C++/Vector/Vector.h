inline float ScreenSizeX = GetSystemMetrics(SM_CXSCREEN);
inline float ScreenSizeY = GetSystemMetrics(SM_CYSCREEN);

struct view_matrix_t {
	float* operator[ ](int index) {
		return matrix[index];
	}
	float matrix[4][4];
};

struct C_UTL_VECTOR
{
	DWORD64 Count = 0;
	DWORD64 Data = 0;
};

class myWeapon {
public:
	size_t size;
	uintptr_t handle;
};

class Vec2
{
public:
	float x, y;
public:
	Vec2() :x(0.f), y(0.f) {}
	Vec2(float x_, float y_) :x(x_), y(y_) {}
	Vec2(ImVec2 ImVec2_) :x(ImVec2_.x), y(ImVec2_.y) {}
	Vec2 operator=(ImVec2 ImVec2_)
	{
		x = ImVec2_.x;
		y = ImVec2_.y;
		return *this;
	}
	Vec2 operator+(Vec2 Vec2_)
	{
		return { x + Vec2_.x,y + Vec2_.y };
	}
	Vec2 operator-(Vec2 Vec2_)
	{
		return { x - Vec2_.x,y - Vec2_.y };
	}
	Vec2 operator*(Vec2 Vec2_)
	{
		return { x * Vec2_.x,y * Vec2_.y };
	}
	Vec2 operator/(Vec2 Vec2_)
	{
		return { x / Vec2_.x,y / Vec2_.y };
	}
	Vec2 operator*(float n)
	{
		return { x / n,y / n };
	}
	Vec2 operator/(float n)
	{
		return { x / n,y / n };
	}
	bool operator==(Vec2 Vec2_)
	{
		return x == Vec2_.x && y == Vec2_.y;
	}
	bool operator!=(Vec2 Vec2_)
	{
		return x != Vec2_.x || y != Vec2_.y;
	}
	ImVec2 ToImVec2()
	{
		return ImVec2(x, y);
	}
	float Length()
	{
		return sqrtf(powf(x, 2) + powf(y, 2));
	}
	float DistanceTo(const Vec2& Pos)
	{
		return sqrtf(powf(Pos.x - x, 2) + powf(Pos.y - y, 2));
	}
};

class Vec3
{
public:
	float x, y, z;
public:
	Vec3() :x(0.f), y(0.f), z(0.f) {}
	Vec3(float x_, float y_, float z_) :x(x_), y(y_), z(z_) {}
	Vec3 operator+(Vec3 Vec3_)
	{
		return { x + Vec3_.x,y + Vec3_.y,z + Vec3_.z };
	}
	Vec3 operator-(Vec3 Vec3_)
	{
		return { x - Vec3_.x,y - Vec3_.y,z - Vec3_.z };
	}
	Vec3 operator*(Vec3 Vec3_)
	{
		return { x * Vec3_.x,y * Vec3_.y,z * Vec3_.z };
	}
	Vec3 operator/(Vec3 Vec3_)
	{
		return { x / Vec3_.x,y / Vec3_.y,z / Vec3_.z };
	}
	Vec3 operator*(float n)
	{
		return { x * n,y * n,z * n };
	}
	Vec3 operator/(float n)
	{
		return { x / n,y / n,z / n };
	}
	bool operator==(Vec3 Vec3_)
	{
		return x == Vec3_.x && y == Vec3_.y && z == Vec3_.z;
	}
	bool operator!=(Vec3 Vec3_)
	{
		return x != Vec3_.x || y != Vec3_.y || z != Vec3_.z;
	}
	float Length()
	{
		return sqrtf(powf(x, 2) + powf(y, 2) + powf(z, 2));
	}
	float DistanceTo(const Vec3& Pos)
	{
		return sqrtf(powf(Pos.x - x, 2) + powf(Pos.y - y, 2) + powf(Pos.z - z, 2));
	}
	Vec3 ToAngle()
	{
		return Vec3{
			std::atan2(-z, std::hypot(x, y)) * (180.0f / std::numbers::pi_v<float>),
			std::atan2(y, x) * (180.0f / std::numbers::pi_v<float>),
			0.0f
		};
	}
	Vec3 worldToScreen(view_matrix_t matrix) {
		float _x = matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z + matrix[0][3];
		float _y = matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z + matrix[1][3];

		float w = matrix[3][0] * x + matrix[3][1] * y + matrix[3][2] * z + matrix[3][3];

		float inv_w = 1.f / w;
		_x *= inv_w;
		_y *= inv_w;

		float screen_x = ScreenSizeX * 0.5f;
		float screen_y = ScreenSizeY * 0.5f;

		screen_x += 0.5f * _x * ScreenSizeX + 0.5f;
		screen_y -= 0.5f * _y * ScreenSizeY + 0.5f;

		return { screen_x, screen_y, w };
	}
	float distanceTo(Vec3 other) {
		return std::sqrt((other.x - x) * (other.x - x) +
			(other.y - y) * (other.y - y) +
			(other.z - z) * (other.z - z));
	}
	bool IsZero() const {
		return x == 0.0f && y == 0.0f && z == 0.0f;
	}
};

inline Vec3 CalculateAngle(Vec3 localPosition, Vec3 enemyPosition, Vec3 viewAngles)
{
	return (enemyPosition - localPosition).ToAngle() - viewAngles;
};

inline Vec3 clampAngles(Vec3 angles) {
	if (angles.x > 89.0f && angles.x <= 180.0f) {
		angles.x = 89.0f;
	}

	if (angles.x > 180.0f) {
		angles.x -= 360.0f;
	}

	if (angles.x < -89.0f) {
		angles.x = -89.0f;
	}

	if (angles.y > 180.0f) {
		angles.y -= 360.0f;
	}

	if (angles.y < -180.0f) {
		angles.y += 360.0f;
	}
	angles.z = 0;

	return angles;
};

inline Vec3 normalizeAngles(Vec3 angle) {
	while (angle.x > 180.f)
		angle.x -= 360.0f;

	while (angle.x < -180.0f)
		angle.x += 360.0f;

	while (angle.y > 180.0f)
		angle.y -= 360.0f;

	while (angle.y < -180.0f)
		angle.y += 360.0f;

	return angle;
};

inline bool isWithinScreenFov(const Vec2& screenPos, float fovRadius) {
	Vec2 screenCenter = { ScreenSizeX / 2.0f, ScreenSizeY / 2.0f };
	float dist = std::sqrt(std::pow(screenPos.x - screenCenter.x, 2) + std::pow(screenPos.y - screenCenter.y, 2));
	return dist <= fovRadius;
}

inline Vec3 calculateBestAngle(Vec3 angle, float configFov) {
	Vec3 newAngle;

	float calcFov = std::sqrt(angle.x * angle.x + angle.y * angle.y);
	float fov = configFov;

	if (calcFov > fov)
		return { 0,0,0 };

	if (calcFov < fov) {
		calcFov = fov;
		newAngle = angle;
	}
	return newAngle;
}

template <typename T>
class Singleton
{
public:
	static T& get()
	{
		static T instance;
		return instance;
	}
};