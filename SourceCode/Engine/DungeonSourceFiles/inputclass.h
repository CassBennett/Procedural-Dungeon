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
// Handles the creation of Input interface objects and detection of input information
// Adapted from the InputClass given by Paul Robertson in previous module: Graphics Programming with Shaders
////////////////////////////////////////////////////////////////////////////////

class InputClass
{
public:
	// Function:		Input Class Constructor
	// What it does:	Initialises the current state of the Keyboard and Mouse to 0
	// Takes in:		Nothing
	// Returns:			Nothing
	InputClass();


	// Placeholder constructor and destructor
	InputClass(const InputClass&);
	~InputClass();


	// Function:		Initialise Input function
	// What it does:	Initialises the input interfaces for the application and acquires the keyboard and mouse to read the input
	// Takes in:		HINSTANCE - Instance object for the window, HWND - handle for the window, int - screen width dimensions, int - screen height dimensions
	// Returns:			bool - True if the Input objects have been initialised correctly, false if not so that an error is shown
	bool Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);


	// Function:		Shutdown function
	// What it does:	Releases all input objects when the application ends to clean resources
	// Takes in:		Nothing
	// Returns:			Nothing
	void Shutdown();


	// Function:		Frame  function
	// What it does:	Is called each frame from the Application Class Frame function in order to detect any changes in the state of the keyboard or mouse and update them 
	// Takes in:		Nothing
	// Returns:			bool - True if the Input objects have been read correctly in this frame, false if not in order to show an error
	bool Frame();


	// Function:		Get Mouse Location function
	// What it does:	Sets the values of the objects passed in to the current Mouse position, by accessing the address of the parameters objects
	// Takes in:		Nothing
	// Returns:			Nothing
	void GetMouseLocation(int&, int&);


	// The following is a list of functions used to check a specific state of the keyboard or mouse.
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

	// Function:		Read Keyboard function
	// What it does:	Detects the current Keyboard state
	// Takes in:		Nothing
	// Returns:			bool - True if Keyboard has been read correctly, else false in order to show an error
	bool ReadKeyboard();


	// Function:		Read Mouse function
	// What it does:	Detects the current Mouse state
	// Takes in:		Nothing
	// Returns:			bool - True if Mouse has been read correctly, else false in order to show an error
	bool ReadMouse();


	// Function:		Process Input function
	// What it does:	Updates the location of the Mouse in relation to the screen
	// Takes in:		Nothing
	// Returns:			Nothing
	void ProcessInput();

private:
	// Input Device Objects
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	// State objects for the keyboard and mouse
	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	// Variables to hold information about the screen and mouse position
	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
};

#endif