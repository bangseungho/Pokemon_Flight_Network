#pragma once

class SceneManager {
private:
	Scene crntScene = Scene::Intro;
	Scene prevScene = Scene::Intro;
	HWND mHwnd;

	RECT rectWindow = { 0, };
	RECT rectDisplay = { 0, };

	bool isEnding = false;
	bool isLoading = false;

	void StartPaint(const HWND& hWnd, PAINTSTRUCT& ps, HDC& hdc, HDC& memDC, HBITMAP& hBitmap, RECT& rectWindow) const;
	void FinishPaint(const HWND& hWnd, PAINTSTRUCT& ps, HDC& hdc, HDC& memDC, HBITMAP& hBitmap, RECT& rectWindow) const;
	void DeleteScene(const HWND& hWnd);
	void LoadScene(const HWND& hWnd);

	unordered_map<uint8, class Player*> mMemberMap;

public:
	SceneManager();
	void Init(const HWND& hWnd);
	void Move();
	void Paint();
	void MoveScene(const HWND& hWnd, Scene scene);
	void StartLoading(const HWND& hWnd);
	void StopLoading(const HWND& hWnd);

	HWND GetHwnd() const { return mHwnd; }
	bool GetIsEnding() const { return isEnding; }
	unordered_map<uint8, class Player*>& GetMemberMap();

	inline bool constexpr IsLoading()
	{
		return isLoading;
	}


	inline constexpr RECT GetRectWindow()
	{
		return rectWindow;
	}
	inline constexpr RECT GetRectDisplay() const
	{
		return rectDisplay;
	}
	inline constexpr Scene GetScene()
	{
		return crntScene;
	}
	inline constexpr Scene GetPrevScene()
	{
		return prevScene;
	}
	inline constexpr void GameClear()
	{
		isEnding = true;
	}
};

class Loading
{
private:
	CImage mGlowingBlack;
	CImage _loading_pokemon;
	CImage _loading_bar[5];
	int _loading_bar_cnt = 0;

public:
	RECT _loding_pokemon_rectImage = { 0, };
	RECT _loding_pokemon_rectDraw = { 0, };
	int _loading_pokemon_cnt = 0;

	Loading();
	void Paint(HDC hdc, HWND hWnd, const RECT& rectWindow);
	void Load(const HWND& hWnd);
	inline constexpr bool IsLoaded()
	{
		return (_loading_bar_cnt == 4) ? true : false;
	}
	inline constexpr void ResetLoading()
	{
		_loading_pokemon_cnt = 0;
		_loading_bar_cnt = 0;
	}
};