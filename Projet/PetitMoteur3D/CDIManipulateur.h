#pragma once
#include <dinput.h>
class CDIManipulateur
{
public: 

	bool Init(HINSTANCE hInstance, HWND hWnd);
	void StatutClavier();
	bool ToucheAppuyee(UINT touche);
	void SaisirEtatSouris();

	const DIMOUSESTATE& EtatSouris() noexcept { return mouseState; }

	bool  isInit() noexcept
	{
		return bDejaInit; 
	}


	void Unacquire() noexcept
	{
		if (bDejaInit)
		{
			pSouris->Unacquire();
			pClavier->Unacquire();
		}
	};

	void Acquire(HWND hWnd) noexcept
	{
		if (bDejaInit)
		{
			pClavier->SetCooperativeLevel(hWnd,
				DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
			pSouris->Acquire();

			pSouris->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
			pClavier->Acquire();
		}
	};


	CDIManipulateur();
	~CDIManipulateur();
	int x, y;
	bool click = false;

private : 
	IDirectInput8* pDirectInput;
	IDirectInputDevice8* pClavier;
	IDirectInputDevice8* pSouris;
	IDirectInputDevice8* pJoystick;

	static bool bDejaInit;
	
	char tamponClavier[256];

protected: 
	DIMOUSESTATE mouseState;
};


