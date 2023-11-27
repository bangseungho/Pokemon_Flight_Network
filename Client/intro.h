#pragma once

enum MI_Menu { start = 0, producer, finish };

class Cloud : public MyGameObject
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

class Logo : public MyGameObject
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

class Menu : public MyGameObject
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
	std::shared_ptr<MyGameObject> GetMenu() { return mMenu; }

private:
	std::shared_ptr<MyGameObject>				mBackground;
	std::shared_ptr<MyGameObject>				mMenu;
	std::shared_ptr<MyGameObject>				mLogo;
	std::vector<std::shared_ptr<MyGameObject>>	mClouds;
};

