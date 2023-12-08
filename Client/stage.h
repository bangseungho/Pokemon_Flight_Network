#pragma once

class Menu;

class Stage
{
private:
	class Target
	{
	public:
		FRECT _rectDraw = { 0, };
		RECT _rectImage = { 0, };
		CImage _img;
		CImage _select_img;
		FRECT _cam = { 0, };
		bool _select = false;
		StageElement _select_index;
	};

	StageElement _clearStage = StageElement::Water;

	RECT _rectDraw = { 0, };
	RECT _rectImage = { 0, };
	FRECT rectStage[5] = { 0, };
	CImage _water;
	CImage _fire;
	CImage _electronic;
	CImage _dark;
	CImage _dialog_bar;
	CImage mGlowingBlack;
	Target* target;
	FRECT mRectTarget;
	FRECT mRectBack;
	bool _dialogflag = false; // 해금되지 않은 스테이지를 선택한 경우 true
	bool _select_pokemon = false; // 포켓몬 선택창이 열린 경우 true
	bool _ready_Air_pokemon = false;
	bool _ready_Land_pokemon = false;
	bool _enter_select = false;

	CImage _select_Pikachu;
	CImage _select_Charmander;
	CImage _select_Squirtle;
	CImage _select_Zapados;
	CImage _select_Moltres;
	CImage _select_Articuno;

	CImage _ready_Pikachu[2];
	CImage _ready_Charmander[2];
	CImage _ready_Squirtle[2];

	CImage _ready_Zapados[2];
	CImage _ready_Moltres[2];
	CImage _ready_Articuno[2];

	POINT _fingerPos = { 0, };

	float mTwinkleCnt = 0;
	float moveX = 300;
	int _play_Air_pokemon = 0;
	int _play_Land_pokemon = 0;
	int mFingerCount;

	Type airPokemon = Type::Empty;
	Type landPokemon = Type::Empty;

public:
	bool _allHide = false;
	float _select_pokemon_move = 0;

	Stage();
	void Init();
	void SelectPokemonInit();
	void Paint(HDC hdc, const RECT& rectWindow);
	void Move(float elapsedTime);
	void fingerController(float elpasedTime);
	inline constexpr StageElement GetStage()
	{
		return target->_select_index;
	}
	inline constexpr Type GetAirPokemon()
	{
		return airPokemon;
	}
	inline constexpr Type GetLandPokemon()
	{
		return landPokemon;
	}
	void ClearStage();
};