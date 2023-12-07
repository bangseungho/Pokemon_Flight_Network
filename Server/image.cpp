#include "..\Utils.h"
#include "image.h"

// 오브젝트 이미지 로드 함수
void ObjectImage::Load(const WCHAR* fileName, const POINT& imgSize, const POINT& bodyDrawPoint, const POINT& bodySize)
{
	this->rectImage = { 0, 0, imgSize.x, imgSize.y };

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

// 이미지 스케일 함수
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

// 이펙트 이미지 로드 함수
void EffectImage::Load(const WCHAR* fileName, const POINT& imgSize, int maxFrame, BYTE alpha)
{
	this->rectImage = { 0, 0, imgSize.x, imgSize.y };

	// 스프라이트 애니메이션을 위해서 값을 증가시킨다.
	++rectImage.left;
	++rectImage.top;
	++rectImage.right;
	++rectImage.bottom;
	this->maxFrame = maxFrame;
	this->drawSize = imgSize;
}

// 이펙트 이미지 스케일 함수
void EffectImage::ScaleImage(float scaleX, float scaleY)
{
	drawSize.x = (LONG)((float)drawSize.x * scaleX);
	drawSize.y = (LONG)((float)drawSize.y * scaleY);
}

// GUI 이미지 로드 함수
void GUIImage::Load(const WCHAR* fileName, const POINT& imgSize, BYTE alpha)
{
	this->rectImage = { 0, 0, imgSize.x, imgSize.y };
}

// 스프라이트 애니메이션을 위해서 프레임수만큼 렉트 이미지에 더해준다.
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