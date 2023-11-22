#pragma once
#include <array>
#include <memory>

enum MI_Menu { start = 0, producer, finish };

class IntroGameObject
{
public:
	virtual void Init();
	virtual void Init(const wchar_t* imgfile, Vector2 pos);
	virtual void Update(float elapsedTime);
	virtual void Paint(HDC hdc);

protected:
	CImage		mCImage;
	Vector2		mPos;
	POINT		mSize = { 0, };
	FRECT		mRectDraw = { 0, };
	RECT		mRectImage = { 0, };
};

class Cloud : public IntroGameObject
{
public:
	virtual void Update(float elapsedTime) override;

public:
	void SetMove(Vector2 move) { mMove = move; }
	void SetRectWindow(const RECT& window) { mRectWindow = window; }

private:
	Vector2 mMove;
	RECT	mRectWindow;
};

class Logo : public IntroGameObject
{
public:
	virtual void Init() override;
	virtual void Update(float elapsedTime) override;
	virtual void Paint(HDC hdc) override;

private:
	float		mLogoMovingCnt = 0;
	Vector2		mPokemonStrPos = {};
	Vector2		mFlightStrPos = {};
};

class Menu : public IntroGameObject
{
public:
	virtual void Init() override;
	virtual void Update(float elapsedTime) override;
	virtual void Paint(HDC hdc) override;

public: 
	int GetTwinkleCnt() const { return (int)mTwinkleCnt; }

private:
	CImage		mGlowingBlack;
	int			mFingerCount = 0;
	bool		mProducer = false;
	float		mTwinkleCnt = 0;
};

class Intro
{
public:
	virtual void Init();
	virtual void Update(float elapsedTime);
	virtual void Paint(HDC hdc);

public:
	std::shared_ptr<IntroGameObject> GetMenu() { return mMenu; }

private:
	std::shared_ptr<IntroGameObject>				mBackground;
	std::shared_ptr<IntroGameObject>				mMenu;
	std::shared_ptr<IntroGameObject>				mLogo;
	std::vector<std::shared_ptr<IntroGameObject>>	mClouds;
};

