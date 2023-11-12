#include "stdafx.h"

Vector2 Rotate(Vector2 vector, float degree)
{
	Vector2 result = { 0, };

	float theta = DEGREE_TO_RADIAN(degree);

	result.x = (vector.x * cos(theta)) - (vector.y * sin(theta));
	result.y = (vector.x * sin(theta)) + (vector.y * cos(theta));

	return result;
}
void Rotate(const Vector2& vSrc, const Vector2& vDst, Vector2& unitVector, float t)
{
	if (t < 0)
	{
		t = 0;
	}
	else if (t > 1)
	{
		t = 1;
	}

	const Vector2 vToTarget = (vDst - vSrc).Normalized();
	const float a = atan2(unitVector.x, unitVector.y);
	const float b = atan2(vToTarget.x, vToTarget.y);
	const float theta = a - b;

	float rotationDegree = RADIAN_TO_DEGREE(theta);
	if (abs(rotationDegree) > 180)
	{
		rotationDegree -= GetSign(rotationDegree) * 360;
	}
	if (abs(rotationDegree) > 45)
	{
		rotationDegree = GetSign(rotationDegree) * 45;
	}
	unitVector = Rotate(unitVector, rotationDegree * t);
}


bool OutOfRange(const RECT& rect, const RECT& rectRange)
{
	if (rect.right < rectRange.left || rect.bottom < rectRange.top || rect.left > rectRange.right || rect.top > rectRange.bottom)
	{
		return true;
	}

	return false;
}

void GetRotationPos(const RECT& rect, const Vector2& unitVector, Vector2 basisVector, Vector2 vPoints[4])
{
	const int rotationSign = unitVector.x > 0 ? 1 : -1;
	const float theta = Vector2::GetTheta(unitVector, basisVector);
	const float rotationDegree = RADIAN_TO_DEGREE(theta) * rotationSign;

	Vector2 posCenter = { 0, };
	posCenter.x = rect.left + ((float)(rect.right - rect.left) / 2);
	posCenter.y = rect.top + ((float)(rect.bottom - rect.top) / 2);

	vPoints[0] = { (float)rect.left, (float)rect.top };
	vPoints[1] = { (float)rect.right, (float)rect.top };
	vPoints[2] = { (float)rect.left, (float)rect.bottom };
	vPoints[3] = { (float)rect.right, (float)rect.bottom };

	Vector2 directionVec[4] = { 0, };
	float distance[4] = { 0, };
	for (int i = 0; i < 4; ++i)
	{
		directionVec[i] = (vPoints[i] - posCenter).Normalized();
		distance[i] = (vPoints[i] - posCenter).Norm();

		directionVec[i] = Rotate(directionVec[i], rotationDegree);
		vPoints[i] = posCenter + (directionVec[i] * distance[i]);
	}
}

RECT GetRotatedBody(const Vector2 vPoints[4])
{
	POINT points[4] = { vPoints[0], vPoints[1], vPoints[2], vPoints[3]};

	RECT rectBody = { 0, };
	rectBody.left = points[0].x;
	rectBody.right = points[0].x;
	rectBody.top = points[0].y;
	rectBody.bottom = points[0].y;

	for (int i = 1; i < 4; ++i)
	{
		if (rectBody.left > points[i].x)
		{
			rectBody.left = points[i].x;
		}
		else if (rectBody.right < points[i].x)
		{
			rectBody.right = points[i].x;
		}
		if (rectBody.top > points[i].y)
		{
			rectBody.top = points[i].y;
		}
		else if (rectBody.bottom < points[i].y)
		{
			rectBody.bottom = points[i].y;
		}
	}

	SetRectByWindow(rectBody);

	return rectBody;
}

void PaintHitbox(HDC hdc, RECT rectBody)
{
	FrameRect(hdc, &rectBody, (HBRUSH)GetStockObject(BLACK_BRUSH));
}

void SetRectByWindow(RECT& rect)
{
	if (rect.left > rect.right)
	{
		const LONG temp = rect.left;
		rect.left = rect.right;
		rect.right = temp;
	}
	if (rect.top > rect.bottom)
	{
		const LONG temp = rect.top;
		rect.top = rect.bottom;
		rect.bottom = temp;
	}

	if (rect.left < 0)
	{
		rect.left = 0;
	}
	if (rect.right > WINDOWSIZE_X)
	{
		rect.right = WINDOWSIZE_X;
	}
	if (rect.top < 0)
	{
		rect.top = 0;
	}
	if (rect.bottom > WINDOWSIZE_Y)
	{
		rect.bottom = WINDOWSIZE_Y;
	}
}

void CheckOverflowAdd(BYTE& lhs, const BYTE& rhs)
{
	if (lhs > UCHAR_MAX - rhs)
	{
		lhs = UCHAR_MAX;
	}
	else
	{
		lhs += rhs;
	}
}
void CheckOverflowSub(BYTE& lhs, const BYTE& rhs)
{
	if (lhs < rhs)
	{
		lhs = 0;
	}
	else
	{
		lhs -= rhs;
	}
}

// Project the vertices of each polygon onto a axis
void ComputeProjections(const std::vector<Vector2>& bounds_a, const std::vector<Vector2>& bounds_b, const Vector2& axisNormalized, std::vector<double>& projections_a, std::vector<double>& projections_b) {
	projections_a.clear();
	projections_b.clear();

	for (size_t i = 0; i < bounds_a.size(); i++) {
		const double projection_a = Vector2::Dot(axisNormalized, bounds_a[i]);
		const double projection_b = Vector2::Dot(axisNormalized, bounds_b[i]);
		projections_a.push_back(projection_a);
		projections_b.push_back(projection_b);
	}
}
bool IsOverlapping(const std::vector<double>& projections_a, const std::vector<double>& projections_b) {
	const double maxProjection_a = *std::max_element(projections_a.begin(), projections_a.end());
	const double minProjection_a = *std::min_element(projections_a.begin(), projections_a.end());
	const double maxProjection_b = *std::max_element(projections_b.begin(), projections_b.end());
	const double minProjection_b = *std::min_element(projections_b.begin(), projections_b.end());

	// does not necessarily mean the polygons are intersecting yet
	return !(maxProjection_a < minProjection_b or maxProjection_b < minProjection_a);
}

bool SATIntersect(const FRECT& rectSrc, const Vector2 vSrc[4])
{
	constexpr int vertexCount = 4;
	Vector2 vLT = { rectSrc.left, rectSrc.top };
	Vector2 vRT = { rectSrc.right, rectSrc.top };
	Vector2 vLB = { rectSrc.left, rectSrc.bottom };
	Vector2 vRB = { rectSrc.right, rectSrc.bottom };
	std::vector<Vector2> bounds_a;
	bounds_a.emplace_back(vLT);
	bounds_a.emplace_back(vRT);
	bounds_a.emplace_back(vLB);
	bounds_a.emplace_back(vRB);

	std::vector<Vector2> bounds_b;
	for (int i = 0; i < vertexCount; ++i)
	{
		bounds_b.emplace_back(vSrc[i]);
	}

	std::vector<double> proj_a;
	std::vector<double> proj_b;
	proj_a.reserve(bounds_a.size());
	proj_b.reserve(bounds_b.size());

	for (size_t i = 0; i < bounds_a.size(); i++) {
		const Vector2 crntPoint = bounds_a[i];
		const Vector2 nextPoint = bounds_a[(i + 1) % bounds_a.size()];
		const Vector2 axisNormalized = Vector2::Get_Normalized_Proj_Axis(crntPoint, nextPoint);
		ComputeProjections(bounds_a, bounds_b, axisNormalized, proj_a, proj_b);

		if (IsOverlapping(proj_a, proj_b) == false)
		{
			return false;
		}
	}

	for (size_t i = 0; i < bounds_b.size(); i++) {
		const Vector2 crntPoint = bounds_b[i];
		const Vector2 nextPoint = bounds_b[(i + 1) % bounds_b.size()];
		const Vector2 axisNormalized = Vector2::Get_Normalized_Proj_Axis(crntPoint, nextPoint);
		ComputeProjections(bounds_a, bounds_b, axisNormalized, proj_a, proj_b);

		if (IsOverlapping(proj_a, proj_b) == false)
		{
			return false;
		}
	}

	return true;
}

void ScaleRect(FRECT& rect, float scaleX, float scaleY)
{
	Vector2 size = { 0, };
	size.x = (rect.right - rect.left);
	size.y = (rect.bottom - rect.top);

	Vector2 posCenter = { 0, };
	posCenter.x = rect.left + (size.x / 2);
	posCenter.y = rect.top + (size.y / 2);

	size.x *= scaleX;
	size.y *= scaleY;

	rect.left = posCenter.x - (size.x / 2);
	rect.top = posCenter.y - (size.y / 2);
	rect.right = rect.left = size.x;
	rect.bottom = rect.top = size.y;
}
FRECT GetRect(const Vector2& posCenter, float radius)
{
	FRECT rect = { 0, };
	rect.left = posCenter.x - (radius / 2);
	rect.top = posCenter.y - (radius / 2);
	rect.right = rect.left + radius;
	rect.bottom = rect.top + radius;

	return rect;
}

bool IntersectRect2(const RECT& rect1, const RECT& rect2)
{
	RECT notuse = { 0, };
	return (bool)IntersectRect(&notuse, &rect1, &rect2);
}

float CalculateDamage(float damage, Type destType, Type srcType)
{
	switch (destType)
	{
	case Type::Elec:
		switch (srcType)
		{
		case Type::Elec:
			damage /= 1.35f;
			break;
		case Type::Water:
			damage /= 1.25f;
			break;
		case Type::Fire:
			damage *= 1.15f;
			break;
		case Type::Dark:
			break;
		default:
			assert(0);
			break;
		}
		break;
	case Type::Fire:
		switch (srcType)
		{
		case Type::Fire:
			damage /= 1.35f;
			break;
		case Type::Elec:
			damage /= 1.25f;
			break;
		case Type::Water:
			damage *= 1.15f;
			break;
		case Type::Dark:
			break;
		default:
			assert(0);
			break;
		}
		break;
	case Type::Water:
		switch (srcType)
		{
		case Type::Water:
			damage /= 1.35f;
			break;
		case Type::Fire:
			damage /= 1.25f;
			break;
		case Type::Elec:
			damage *= 1.15f;
			break;
		case Type::Dark:
			break;
		default:
			assert(0);
			break;
		}
		break;
	case Type::Dark:
		break;
	default:
		assert(0);
		break;
	}

	return damage;
}