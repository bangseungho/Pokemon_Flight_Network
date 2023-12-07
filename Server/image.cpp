#include "..\Utils.h"
#include "image.h"

// ������Ʈ �̹��� �ε� �Լ�
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

// �̹��� ������ �Լ�
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

// ����Ʈ �̹��� �ε� �Լ�
void EffectImage::Load(const WCHAR* fileName, const POINT& imgSize, int maxFrame, BYTE alpha)
{
	this->rectImage = { 0, 0, imgSize.x, imgSize.y };

	// ��������Ʈ �ִϸ��̼��� ���ؼ� ���� ������Ų��.
	++rectImage.left;
	++rectImage.top;
	++rectImage.right;
	++rectImage.bottom;
	this->maxFrame = maxFrame;
	this->drawSize = imgSize;
}

// ����Ʈ �̹��� ������ �Լ�
void EffectImage::ScaleImage(float scaleX, float scaleY)
{
	drawSize.x = (LONG)((float)drawSize.x * scaleX);
	drawSize.y = (LONG)((float)drawSize.y * scaleY);
}

// GUI �̹��� �ε� �Լ�
void GUIImage::Load(const WCHAR* fileName, const POINT& imgSize, BYTE alpha)
{
	this->rectImage = { 0, 0, imgSize.x, imgSize.y };
}

// ��������Ʈ �ִϸ��̼��� ���ؼ� �����Ӽ���ŭ ��Ʈ �̹����� �����ش�.
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