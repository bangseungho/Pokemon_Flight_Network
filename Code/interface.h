#pragma once
class Player;
class GUIImage;

typedef struct tagGameData {
	bool isBattleStart = false;
	Difficulty difficulty = Difficulty::Easy;
	StageElement stage = StageElement::Null;

	bool isShowHitbox = false;
	bool isShowDrawBox = false;
}GameData;

class GUIManager {
private:
	typedef struct HurtGUI {
		GUIImage* gui = nullptr;
		BYTE alpha = 0x00;

		void ReduceAlpha();
	}HurtGUI;

	const RECT* rectWindow = nullptr;
	float iconMoveMaxY = 0;
	float iconMoveMinY = 0;
	float iconMoveAmount = 0;
	bool isIconStop = false;
	bool isClearPhase = false;

	const BYTE hurtGUI_alpha = 0x90;

	RECT rectMain = { 0, };
	RECT rectSkill_Q = { 0, };
	RECT rectSkill_W = { 0, };
	RECT rectSkill_E = { 0, };
	RECT rectHP = { 0, };
	RECT rectMP = { 0, };
	RECT rectMoveBar = { 0, };
	RECT rectGaugeMoveBar = { 0, };
	FRECT rectPokemonIcon = { 0, };

	GUIImage* mainGUI = nullptr;
	GUIImage* gagueGUI_main = nullptr;
	GUIImage* gagueGUI_border = nullptr;
	GUIImage* gagueGUI_hp = nullptr;
	GUIImage* gagueGUI_mp = nullptr;

	GUIImage* icon_Q = nullptr;
	GUIImage* icon_W = nullptr;
	GUIImage* icon_E = nullptr;

	GUIImage* moveBarGUI = nullptr;
	GUIImage* gaugeMoveBarGUI = nullptr;
	GUIImage* icon_pokemon = nullptr;

	HurtGUI hurtGUI_Fire;
	HurtGUI hurtGUI_Water;
	HurtGUI hurtGUI_Elec;
	HurtGUI hurtGUI_Dark;
public:
	GUIManager(const RECT& rectWindow);
	void Paint(const HDC& hdc);
	void Update(const HWND& hWnd);
	RECT GetRectDisplay() const;
	void DisplayHurtFrame(Type type);
	
	inline constexpr bool IsFieldEnd()
	{
		return isIconStop;
	}
	inline constexpr void SkipField()
	{
		rectPokemonIcon.top = iconMoveMaxY;
	}
};

void CheckKeyDown(const HWND& hWnd, const WPARAM& wParam);
void CheckKeyUp(const HWND& hWnd, const WPARAM& wParam);