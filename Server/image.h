#pragma once

static const COLORREF transRGB = RGB(145, 91, 145);
const HBRUSH transBrush = CreateSolidBrush(transRGB);

class Image abstract {
private:
	HBITMAP hBitmap{};

protected:
	RECT rectImage = { 0, };
	POINT drawSize = { 0, };
	float scaleX = 1;
	float scaleY = 1;

public:
	inline RECT GetRectImage() const
	{
		return rectImage;
	}
	inline POINT GetDrawSize() const
	{
		return drawSize;
	}
	inline void GetScale(float& scaleX, float& scaleY) const
	{
		scaleX = this->scaleX;
		scaleY = this->scaleY;
	}
};

class ObjectImage : public Image{
private:
	POINT bodyDrawPoint = { 0, };
	POINT bodySize = { 0, };
	bool isScaled = false;

public:
	void Load(const WCHAR* fileName, const POINT& imgSize, const POINT& bodyDrawPoint = { 0, 0 }, const POINT& bodySize = { 0, 0 });
	void ScaleImage(float scaleX, float scaleY);
	
	inline POINT GetBodyDrawPoint() const
	{
		return bodyDrawPoint;
	}
	inline POINT GetBodySize() const
	{
		return bodySize;
	}
};

class EffectImage : public Image {
private:
	int maxFrame = 0;
public:
	void Load(const WCHAR* fileName, const POINT& imgSize, int maxFrame = 0, BYTE alpha = 0xff);
	void ScaleImage(float scaleX, float scaleY);

	inline int GetMaxFrame() const
	{
		return maxFrame;
	}
};


class GUIImage : public Image {
public:
	void Load(const WCHAR* fileName, const POINT& imgSize, BYTE alpha = 0xff);
};


class ISprite abstract {
protected:
	int frame = 0;
	RECT GetRectImage(const Image& image, int frame, int spriteRow = 0) const;
};

class IAnimatable abstract : public ISprite {
private:
	Action action = Action::Idle;
public:
	bool isRevFrame = false;

	inline Action GetAction() const
	{
		return action;
	}
	inline void SetAction(Action action, int frame)
	{
		this->frame = frame;
		this->action = action;
		this->isRevFrame = false;
	}


};