////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"


// Function:		Input Class Constructor
// What it does:	Initialises the current state of the Keyboard and Mouse to 0
// Takes in:		Nothing
// Returns:			Nothing

InputClass::InputClass()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
}


// Placeholder constructor and destructor
InputClass::InputClass(const InputClass& other)
{}
InputClass::~InputClass()
{}


// Function:		Initialise Input function
// What it does:	Initialises the input interfaces for the application and acquires the keyboard and mouse to read the input
// Takes in:		HINSTANCE - Instance object for the window, HWND - handle for the window, int - screen width dimensions, int - screen height dimensions
// Returns:			bool - True if the Input objects have been initialised correctly, false if not so that an error is shown

bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	// For error detection
	HRESULT result;

	// Store the screen size which will be used for positioning the mouse cursor.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Initialize the location of the mouse on the screen.
	m_mouseX = 0;
	m_mouseY = 0;

	// Initialize the main direct input interface.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Initialize the direct input interface for the keyboard.
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the keyboard to not share with other programs.
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}

	// Now acquire the keyboard.
	result = m_keyboard->Acquire();
	if(FAILED(result))
	{
		return false;
	}

	// Initialize the direct input interface for the mouse.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Set the data format for the mouse using the pre-defined mouse data format.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the mouse to share with other programs.
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}

	// Acquire the mouse.
	result = m_mouse->Acquire();
	if(FAILED(result))
	{
		return false;
	}

	return true;
}


// Function:		Shutdown function
// What it does:	Releases all input objects when the application ends to clean resources
// Takes in:		Nothing
// Returns:			Nothing

void InputClass::Shutdown()
{
	// Release the mouse.
	if(m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// Release the keyboard.
	if(m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// Release the main interface to direct input.
	if(m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}


// Function:		Frame  function
// What it does:	Is called each frame from the Application Class Frame function in order to detect any changes in the state of the keyboard or mouse and update them 
// Takes in:		Nothing
// Returns:			bool - True if the Input objects have been read correctly in this frame, false if not in order to show an error

bool InputClass::Frame()
{
	bool result;


	// Read the current state of the keyboard.
	result = ReadKeyboard();
	if(!result)
	{
		return false;
	}

	// Read the current state of the mouse.
	result = ReadMouse();
	if(!result)
	{
		return false;
	}

	// Process the changes in the mouse and keyboard.
	ProcessInput();

	return true;
}


// Function:		Read Keyboard function
// What it does:	Detects the current Keyboard state
// Takes in:		Nothing
// Returns:			bool - True if Keyboard has been read correctly, else false in order to show an error

bool InputClass::ReadKeyboard()
{
	HRESULT result;


	// Read the keyboard device.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if(FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}
		
	return true;
}


// Function:		Read Mouse function
// What it does:	Detects the current Mouse state
// Takes in:		Nothing
// Returns:			bool - True if Mouse has been read correctly, else false in order to show an error

bool InputClass::ReadMouse()
{
	HRESULT result;


	// Read the mouse device.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if(FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}


// Function:		Process Input function
// What it does:	Updates the location of the Mouse in relation to the screen
// Takes in:		Nothing
// Returns:			Nothing

void InputClass::ProcessInput()
{
	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// Ensure the mouse location doesn't exceed the screen width or height.
	if(m_mouseX < 0)  { m_mouseX = 0; }
	if(m_mouseY < 0)  { m_mouseY = 0; }
	
	if(m_mouseX > m_screenWidth)  { m_mouseX = m_screenWidth; }
	if(m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }
	
	return;
}


// Function:		Get Mouse Location function
// What it does:	Sets the values of the objects passed in to the current Mouse position, by accessing the address of the parameters objects
// Takes in:		Nothing
// Returns:			Nothing

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
	return;
}



////////////////////////////////////////////////////////////////////////////////////////////////
// The following is a list of functions used to check a specific state of the keyboard or mouse.
////////////////////////////////////////////////////////////////////////////////////////////////


bool InputClass::IsEscapePressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if(m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsSpacePressed() // Not used
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_SPACE] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsLeftPressed() // Rotate camera left
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_LEFT] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputClass::IsRightPressed() // Rotate camera right
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_RIGHT] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputClass::IsUpPressed() // Move camera forward
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_UP] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputClass::IsDownPressed() // Move camera backward
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_DOWN] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputClass::IsAPressed() // Move camera upward
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_A] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputClass::IsZPressed() // Move camera downward
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (m_keyboardState[DIK_Z] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputClass::IsPgUpPressed() // Rotate camera upward
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (m_keyboardState[DIK_PGUP] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputClass::IsPgDownPressed() // Rotate camera downward
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (m_keyboardState[DIK_PGDN] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputClass::IsWPressed() // Toggle Wireframe
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_W] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputClass::IsDPressed() // Toggle Distortion
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_D] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputClass::IsFPressed() // Decrease Distortion
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_F] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputClass::IsGPressed() // Increase Distortion
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_G] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputClass::IsCPressed() // Toggle Collision
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_C] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputClass::IsIPressed() // Toggle Instructions
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_I] & 0x80)
	{
		return true;
	}

	return false;
}