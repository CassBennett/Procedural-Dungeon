////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_


///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define DIRECTINPUT_VERSION 0x0800


/////////////
// LINKING //
/////////////
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


//////////////
// INCLUDES //
//////////////
#include <dinput.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: InputClass
////////////////////////////////////////////////////////////////////////////////
class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	void GetMouseLocation(int&, int&);

	bool IsSpacePressed();			// Not used
	bool IsEscapePressed();			// Close application
	bool IsLeftPressed();			// Rotate camera left
	bool IsRightPressed();			// Rotate camera right
	bool IsUpPressed();				// Move camera forward
	bool IsDownPressed();			// Move camera backward
	bool IsAPressed();				// Move camera upward
	bool IsZPressed();				// Move camera downward
	bool IsPgUpPressed();			// Rotate camera upward
	bool IsPgDownPressed();			// Rotate camera downward
	bool IsWPressed();				// Toggle Wireframe
	bool IsDPressed();				// Toggle Distortion
	bool IsFPressed();				// Decrease Distortion
	bool IsGPressed();				// Increase Distortion
	bool IsCPressed();				// Toggle Collision
	bool IsIPressed();				// Toggle Instructions
private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
};

#endif