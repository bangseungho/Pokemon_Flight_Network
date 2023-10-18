#pragma once

class Battle
{
public:
	CImage _water;
	CImage _fire;
	CImage _electronic;
	CImage _dark;
	RECT _rectImage = { 0, };
	RECT _rectDraw = { 0, };
	RECT _rectDraw2 = { 0, };
	int _mapSpeed = 0;
	int _shakeCount = 0;
	int _shakeWindow = 0;
	bool _gameFinish = false;

	bool isShake = false;
	bool isShakeLeft = true;

public:
	Battle();
	void Init();
	void Paint(HDC hdc, const RECT& rectWindow, StageElement _select_index);
	inline constexpr void ShakeMap(int shakeCount = 5)
	{
		_shakeCount = shakeCount * 10;
		isShake = true;
	}
};