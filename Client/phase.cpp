#include "stdafx.h"
#include "phase.h"
#include "scene.h"
#include "stage.h"
#include "interface.h"
#include "Network.h"

extern SceneManager* sceneManager;
extern GameData gameData;
extern Stage stage;

// 페이즈 화면에서 필요한 페이즈들 생성과 필요한 이미지 모두 로드
PhaseManager::PhaseManager()
{
	_water = new Phase();
	_fire = new Phase();
	_elec = new Phase();
	_dark = new Phase();

	_water->_background.Load(L"images\\stage\\Water_stage.bmp");
	_fire->_background.Load(L"images\\stage\\Fire_stage.bmp");
	_elec->_background.Load(L"images\\stage\\Electronic_stage.bmp");
	_dark->_background.Load(L"images\\stage\\Dark_stage.bmp");

	_water->_boss.Load(L"images\\stage\\kyogred.png");
	_water->_melee.Load(L"images\\stage\\wingull.png");
	_water->_range.Load(L"images\\stage\\seadra.png");
	_water->_range_mask.Load(L"images\\stage\\seadra_mask.png");
	_water->_boss_mask.Load(L"images\\stage\\kyogre_mask.png");

	_fire->_boss.Load(L"images\\stage\\salamence.png");
	_fire->_melee.Load(L"images\\stage\\ledyba.png");
	_fire->_range.Load(L"images\\stage\\latias.png");
	_fire->_range_mask.Load(L"images\\stage\\latias_mask.png");
	_fire->_boss_mask.Load(L"images\\stage\\salamence_mask.png");

	_elec->_boss.Load(L"images\\stage\\ho-oh.png");
	_elec->_melee.Load(L"images\\stage\\beedrill.png");
	_elec->_range.Load(L"images\\stage\\zapados.png");
	_elec->_range_mask.Load(L"images\\stage\\zapados_mask.png");
	_elec->_boss_mask.Load(L"images\\stage\\ho-oh_mask.png");

	_dark->_boss.Load(L"images\\stage\\Weezing.png");
	_dark->_melee.Load(L"images\\stage\\crobat.png");
	_dark->_range.Load(L"images\\stage\\aerodactyl.png");
	_dark->_range_mask.Load(L"images\\stage\\aerodactyl_mask.png");
	_dark->_boss_mask.Load(L"images\\stage\\Weezing_mask.png");
}

void PhaseManager::Init()
{
	rectImage = { 0, 0, 150, 150 };
	boss_rectImage = { 0, 0, 250, 250 };

	rectDraw_melee = { 270, 490, 420, 640 };
	rectDraw_range = { 50, 320, 200, 470 };
	rectDraw_boss = { 180, 20, 430, 270 };
}

void PhaseManager::Move(float elapsedTime)
{
	if (sceneManager->IsLoading())
		return;

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		if (MY_INDEX == MP_INDEX) {
			PhaseData sendData = { MY_INDEX, VK_RETURN };
			GET_SINGLE(Network)->SendDataAndType<PhaseData>(sendData);
		}
	}

	auto& recvData = MEMBER_MAP(MP_INDEX).mPhaseData;
	if (recvData.InputKey == VK_RETURN) {
		sceneManager->StartLoading(sceneManager->GetHwnd());
		Sleep(10); // Release 모드에서 배틀이 끝난 후 자동으로 배틀이 또 다시 시작되는 버그 발견. sleep으로 해결
	}

	recvData.InputKey = 0;
}

// 페이즈 클리어 횟수에 따라서 페이즈에 표시해야 할 이미지들 렌더링
void PhaseManager::Paint(HDC hdc, const RECT& rectWindow, StageElement _select_index)
{
	switch (_select_index)
	{
	case StageElement::Water:
		_water->_background.Draw(hdc, rectWindow, rectWindow);

		_water->_melee.Draw(hdc, rectDraw_melee, rectImage);

		if (phaseWater == 0)
		{
			_water->_range_mask.Draw(hdc, rectDraw_range, rectImage);
			_water->_boss_mask.Draw(hdc, 160, -50, 320, 320, 0, 0, 320, 320);
		}
		else if (phaseWater > 0)
		{
			_water->_range.Draw(hdc, rectDraw_range, rectImage);
			if (phaseWater > 1)
			{
				_water->_boss.TransparentBlt(hdc, 160, -50, 320, 320, 0, 0, 320, 320, RGB(0, 0, 0));
			}
			else
			{
				_water->_boss_mask.Draw(hdc, 160, -50, 320, 320, 0, 0, 320, 320);
			}
		}
		break;
	case StageElement::Fire:
		_fire->_background.Draw(hdc, rectWindow, rectWindow);

		_fire->_melee.Draw(hdc, rectDraw_melee, rectImage);

		if (phaseFire == 0)
		{
			_fire->_range_mask.Draw(hdc, rectDraw_range, rectImage);
			_fire->_boss_mask.Draw(hdc, rectDraw_boss, boss_rectImage);
		}
		else if (phaseFire > 0)
		{
			_fire->_range.Draw(hdc, rectDraw_range, rectImage);
			if (phaseFire > 1)
			{
				_fire->_boss.Draw(hdc, rectDraw_boss, boss_rectImage);
			}
			else
			{
				_fire->_boss_mask.Draw(hdc, rectDraw_boss, boss_rectImage);
			}
		}
		break;
	case StageElement::Elec:
		_elec->_background.Draw(hdc, rectWindow, rectWindow);

		_elec->_melee.Draw(hdc, rectDraw_melee, rectImage);

		if (phaseElec == 0)
		{
			_elec->_range_mask.Draw(hdc, rectDraw_range, rectImage);
			_elec->_boss_mask.Draw(hdc, rectDraw_boss, boss_rectImage);
		}
		else if (phaseElec > 0)
		{
			_elec->_range.TransparentBlt(hdc, rectDraw_range, rectImage, RGB(0, 0, 0));
			if (phaseElec > 1)
			{
				_elec->_boss.Draw(hdc, rectDraw_boss, boss_rectImage);
			}
			else
			{
				_elec->_boss_mask.Draw(hdc, rectDraw_boss, boss_rectImage);
			}
		}
		break;
	case StageElement::Dark:
		_dark->_background.Draw(hdc, rectWindow, rectWindow);

		_dark->_melee.Draw(hdc, rectDraw_melee, rectImage);

		if (phaseDark == 0)
		{
			_dark->_range_mask.Draw(hdc, rectDraw_range, rectImage);
			_dark->_boss_mask.Draw(hdc, rectDraw_boss, boss_rectImage);
		}
		else if (phaseDark > 0)
		{
			_dark->_range.Draw(hdc, rectDraw_range, rectImage);
			if (phaseDark > 1)
			{
				_dark->_boss.Draw(hdc, rectDraw_boss, boss_rectImage);
			}
			else
			{
				_dark->_boss_mask.Draw(hdc, rectDraw_boss, boss_rectImage);
			}
		}
	}

	HFONT hFont = CreateFont(40, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("ChubbyChoo-SemiBold"));
	HFONT hFont2 = CreateFont(70, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("ChubbyChoo-SemiBold"));
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(255, 255, 255));
	TextOut(hdc, 290, 620, L"STAGE 1", 7);
	TextOut(hdc, 60, 460, L"STAGE 2", 7);

	HFONT oldFont2 = (HFONT)SelectObject(hdc, hFont2);
	TextOut(hdc, 240, 250, L"BOSS", 4);

	SelectObject(hdc, oldFont);
	SelectObject(hdc, oldFont2);
	DeleteObject(hFont);
	DeleteObject(hFont2);
}

// 페이즈를 클리어하였을 경우 현재 페이즈 값 증가 1스테이지당 총 3페이즈까지 존재
void PhaseManager::ClearPhase()
{
	int* crntPhase = nullptr;
	switch (stage.GetStage())
	{
	case StageElement::Water:
		crntPhase = &phaseWater;
		break;
	case StageElement::Fire:
		crntPhase = &phaseFire;
		break;
	case StageElement::Elec:
		crntPhase = &phaseElec;
		break;
	case StageElement::Dark:
		crntPhase = &phaseDark;
		break;
	default:
		assert(0);
		return;
	}

	if (*crntPhase >= 3)
	{
		return;
	}
	else if (++(*crntPhase) >= 3)
	{
		*crntPhase = 3;
		stage.ClearStage();
	}

	gameData.ClearRecord = phaseWater + phaseFire + phaseElec + phaseDark;
//#ifdef _DEBUG
//	std::cout << static_cast<uint32>(gameData.ClearRecord) << std::endl;
//#endif 
}

int PhaseManager::GetPhase()
{
	switch (stage.GetStage())
	{
	case StageElement::Water:
		return phaseWater;
	case StageElement::Fire:
		return phaseFire;
	case StageElement::Elec:
		return phaseElec;
	case StageElement::Dark:
		return phaseDark;
	default:
		assert(0);
		return 0;
	}
}