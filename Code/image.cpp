#include "stdafx.h"
#include "image.h"
#include "interface.h"

extern GameData gameData;

void Image::Load(const WCHAR* fileName, const POINT& imgSize, BYTE alpha)
{
	Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(fileName, false);
	bitmap->GetHBITMAP(NULL, &hBitmap);
	delete bitmap;

	if (hBitmap == NULL)
	{
		assert(0);
	}

	bFunction.BlendOp = AC_SRC_OVER;
	bFunction.BlendFlags = 0;
	bFunction.SourceConstantAlpha = alpha;
	bFunction.AlphaFormat = AC_SRC_ALPHA;

	this->rectImage = { 0, 0, imgSize.x, imgSize.y };
}
void Image::Paint(const HDC&hdc, const RECT& rectDraw, const RECT& rectImage) const
{
	const HDC memDC = CreateCompatibleDC(hdc);

	SelectObject(memDC, hBitmap);

	AlphaBlend(hdc, rectDraw.left, rectDraw.top, (rectDraw.right - rectDraw.left), (rectDraw.bottom - rectDraw.top),
		memDC, rectImage.left, rectImage.top, (rectImage.right - rectImage.left), (rectImage.bottom - rectImage.top), bFunction);
	
	DeleteDC(memDC);

	if (gameData.isShowDrawBox == true)
	{
		PaintHitbox(hdc, rectDraw);
	}
}
void Image::PaintRotation(const HDC& hdc, Vector2 vPoints[4], const RECT* rectImage) const
{
	if (rectImage == nullptr)
	{
		rectImage = &this->rectImage;
	}

	// Init memDC
	static HBITMAP hBitmap_Back = CreateCompatibleBitmap(hdc, WINDOWSIZE_X, WINDOWSIZE_Y);
	HDC memDC_Back = CreateCompatibleDC(hdc);
	HDC memDC_Object = CreateCompatibleDC(hdc);
	SelectObject(memDC_Back, hBitmap_Back);
	SelectObject(memDC_Object, hBitmap);

	// Get rotated rectangle
	POINT points[4] = { vPoints[0], vPoints[1], vPoints[2], vPoints[3] };
	RECT rectDraw = GetRotatedBody(vPoints);
	const int wDest = rectDraw.right - rectDraw.left;
	const int hDest = rectDraw.bottom - rectDraw.top;

	// erase a painted on memDC
	FillRect(memDC_Back, &rectDraw, (HBRUSH)GetStockObject(BLACK_BRUSH));

	// paint by rotated points on memDC
	const int imgWidth = rectImage->right - rectImage->left;
	const int imgHeight = rectImage->bottom - rectImage->top;
	PlgBlt(memDC_Back, points, memDC_Object, rectImage->left, rectImage->top, imgWidth, imgHeight, NULL, 0, 0);

	// paint on hdc with alphablend
	AlphaBlend(hdc, rectDraw.left, rectDraw.top, wDest, hDest,
		memDC_Back, rectDraw.left, rectDraw.top, wDest, hDest, bFunction);

	DeleteDC(memDC_Back);
	DeleteDC(memDC_Object);

	if (gameData.isShowHitbox == true)
	{
		std::swap(points[0], points[1]);
		SelectObject(hdc, (HBRUSH)GetStockObject(NULL_BRUSH));
		Polygon(hdc, points, 4);
		PaintHitbox(hdc, rectDraw);
	}
}
void Image::SetAlpha(BYTE alpha)
{
	bFunction.SourceConstantAlpha = alpha;
}



void ObjectImage::Load(const WCHAR* fileName, const POINT& imgSize, const POINT& bodyDrawPoint, const POINT& bodySize)
{
	Image::Load(fileName, imgSize);
	
	this->bodyDrawPoint = bodyDrawPoint;
	drawSize.x = rectImage.right - rectImage.left;
	drawSize.y = rectImage.bottom - rectImage.top;

	if (bodySize.x == 0)
	{
		this->bodySize = imgSize;
	}
	else
	{
		this->bodySize = bodySize;
	}
}

void ObjectImage::Paint(const HDC& hdc, const RECT& rectBody, const RECT* rectImage) const
{
	if (rectImage == nullptr)
	{
		rectImage = &this->rectImage;
	}

	RECT rectDraw = { 0, };
	rectDraw.left = rectBody.left - bodyDrawPoint.x;
	rectDraw.top = rectBody.top - bodyDrawPoint.y;
	rectDraw.right = rectDraw.left + drawSize.x;
	rectDraw.bottom = rectDraw.top + drawSize.y;

	Image::Paint(hdc, rectDraw, *rectImage);

	if (gameData.isShowHitbox == true)
	{
		PaintHitbox(hdc, rectBody);
	}
}
void ObjectImage::Paint(const RECT& rectDest, const HDC& hdc) const
{
	Image::Paint(hdc, rectDest, this->rectImage);
}

void ObjectImage::ScaleImage(float scaleX, float scaleY)
{
	if (isScaled == true)
	{
		return;
	}
	isScaled = true;

	drawSize.x = (LONG)((float)drawSize.x * scaleX);
	drawSize.y = (LONG)((float)drawSize.y * scaleY);
	bodyDrawPoint.x = (LONG)((float)bodyDrawPoint.x * scaleX);
	bodyDrawPoint.y = (LONG)((float)bodyDrawPoint.y * scaleY);
	bodySize.x = (LONG)((float)bodySize.x * scaleX);
	bodySize.y = (LONG)((float)bodySize.y * scaleY);

	this->scaleX = scaleX;
	this->scaleY = scaleY;
}


void EffectImage::Load(const WCHAR* fileName, const POINT& imgSize, int maxFrame, BYTE alpha)
{
	Image::Load(fileName, imgSize, alpha);
	++rectImage.left;
	++rectImage.top;
	++rectImage.right;
	++rectImage.bottom;
	this->maxFrame = maxFrame;
	this->drawSize = imgSize;
}
void EffectImage::Paint(const HDC& hdc, const POINT& drawPoint, const RECT* rectImage) const
{
	if (rectImage == nullptr)
	{
		rectImage = &this->rectImage;
	}

	RECT rectDraw = { 0, };
	rectDraw.left = drawPoint.x - (drawSize.x / 2);
	rectDraw.top = drawPoint.y - (drawSize.y / 2);
	rectDraw.right = rectDraw.left + drawSize.x;
	rectDraw.bottom = rectDraw.top + drawSize.y;

	Image::Paint(hdc, rectDraw, *rectImage);
}
void EffectImage::Paint(const HDC& hdc, const RECT& rectDest, const RECT* rectImage) const
{
	if (rectImage == nullptr)
	{
		rectImage = &this->rectImage;
	}

	Image::Paint(hdc, rectDest, *rectImage);
}
void EffectImage::ScaleImage(float scaleX, float scaleY)
{
	drawSize.x = (LONG)((float)drawSize.x * scaleX);
	drawSize.y = (LONG)((float)drawSize.y * scaleY);
}






void GUIImage::Load(const WCHAR* fileName, const POINT& imgSize, BYTE alpha)
{
	Image::Load(fileName, imgSize, alpha);
}
void GUIImage::Paint(const HDC& hdc, const RECT& rectDest)
{
	Image::Paint(hdc, rectDest, rectImage);
}
void GUIImage::PaintBlack(const HDC& hdc, const RECT& rectDest)
{
	const HDC memDC = CreateCompatibleDC(hdc);
	const HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rectImage.right, rectImage.bottom);
	SelectObject(memDC, hBitmap);

	const RECT rectMask = { 0, 0, rectImage.right, rectImage.bottom };
	Image::Paint(memDC, rectMask, rectImage);
	SelectObject(hdc, GetStockObject(GRAY_BRUSH));
	StretchBlt(hdc, rectDest.left, rectDest.top, rectDest.right - rectDest.left, rectDest.bottom - rectDest.top, memDC, 0, 0, rectImage.right, rectImage.bottom, MERGECOPY);

	DeleteDC(memDC);
	DeleteObject(hBitmap);
}
void GUIImage::PaintGauge(const HDC& hdc, const RECT& rectDest, float current, float max)
{
	constexpr int gapY = 6;
	const int imgWidth = rectImage.right - rectImage.left;
	const int imgHeight = rectImage.bottom - rectImage.top;
	const int wDest = rectDest.right - rectDest.left;
	const int hDest = rectDest.bottom - rectDest.top;

	const HDC memDC = CreateCompatibleDC(hdc);
	const HBITMAP hBitmap = CreateCompatibleBitmap(hdc, imgWidth, imgHeight);
	SelectObject(memDC, hBitmap);

	FillRect(memDC, &rectImage, transBrush);
	Image::Paint(memDC, rectImage, rectImage);

	RECT rectErase = rectImage;
	float unitHeight = imgHeight / max;
	rectErase.bottom = rectErase.top + ((max - current) * unitHeight);

	FillRect(memDC, &rectErase, transBrush);

	TransparentBlt(hdc, rectDest.left, rectDest.top, wDest, hDest, memDC, rectImage.left, rectImage.top, imgWidth, imgHeight, transRGB);

	DeleteDC(memDC);
	DeleteObject(hBitmap);
}






RECT ISprite::GetRectImage(const Image& image, int frame, int spriteRow) const
{
	const POINT drawSize = image.GetDrawSize();
	RECT rectImage = image.GetRectImage();

	int width = (rectImage.right - rectImage.left) + 1;
	int height = (rectImage.bottom - rectImage.top) + 1;
	rectImage.left += (width * frame);
	rectImage.right += (width * frame);
	rectImage.top += (height * spriteRow);
	rectImage.bottom += (height * spriteRow);

	return rectImage;
}