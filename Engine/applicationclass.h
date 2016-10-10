////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


///////////////////////
// CLASS INCLUDES //
///////////////////////
#include "inputclass.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "timerclass.h"
#include "positionclass.h"
#include "fpsclass.h"
#include "cpuclass.h"
#include "fontshaderclass.h"
#include "textclass.h"
#include "lightclass.h"
#include "meshclass.h"
#include "dungeonclass.h"
#include "dungeonpiececlass.h"
#include "targetwindowclass.h"
#include "rendertextureclass.h"
#include "textureshaderclass.h"
#include "distortionshaderclass.h"
#include "lightshaderclass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class ApplicationClass
{

// Public functions
public:

	// Function:		Application Class Constructor
	// What it does:	Initialises all members to their default values/variables to set up the application class
	// Takes in:		Nothing
	// Returns:			Nothing
	ApplicationClass();

	// Placeholder functions for Constructor and Destructor
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	// Function:		Initialize Function
	// What it does:	Initializes all objects within application class such as the window and shaders/cameras/meshes
	// Takes in:		HINSTANCE - Instance object for the window, HWND - handle for the window, int - screen width dimensions, int - screen height dimensions
	// Returns:			True if all objects have initialised correctly, false if an object is not initialised correctly so system class can 
	//					shut it down and show error 
	bool Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);

	// Function:		Shutdown function
	// What it does:	Releases all objects when the application ends and deletes them, cleans resources
	// Takes in:		Nothing
	// Returns:			Nothing
	void Shutdown();

	// Function:		Frame function
	// What it does:	This is the applications Update function, and is called every frame from the System class. It calls the updates for the 
	//					objects and calls the RenderGraphics function every frame so screen and application settings are updated every frame
	// Takes in:		Nothing
	// Returns:			True if all update functions have been carried out successfully, false if not to close the application
	bool Frame();

// Private functions
private:

	// Function:		Handle Input function
	// What it does:	Checks the current input of the program and calls the functions to apply the input based on the keys pressed and the state of the application
	//					It is called every frame from the Frame() update function in this class.
	// Takes in:		float - Current time to set the frame time
	// Returns:			True if input updates have been successful, false if they aren't
	bool HandleInput(float);
	
	// Function:		Render Graphics function
	// What it does:	This is called every frame in the Frame update function in this class. It renders the geometry and textures to 
	//					the screen, calling RenderScene rendering all objects and also renders the text to the screen
	// Takes in:		Nothing
	// Returns:			True if all renders have been successful, false if not so that the application will close and show an error
	bool RenderGraphics();

	// Function:		Render To Texture function
	// What it does:	Uses the camera passed in to render the entire current scene to a texture object which can then be used with the 
	//					distortion shader for the post processing effect or just for the map texture in the corner of the screen
	// Takes in:		RenderTextureClass pointer - to the texture this window that will be used to show this texture, 
	//					CameraClass pointer - to the camera that is being used as the view for this texture, so that separate views can be rendered
	//											which is important to distinguish for the applications main camera view and the map camera view
	// Returns:			True if it has successfully rendered to the texture, false if not 
	bool RenderToTexture(RenderTextureClass* WindowTexture, CameraClass* CurrentCamera);

	// Function:		Render Scene function
	// What it does:	Uses the camera passed into the function to render the scene as it is according to that camera
	//					Calls RenderDungeon and RenderMap to render all objects in the scene
	// Takes in:		CameraClass pointer - To the camera used to render the scene so it can render the scene in different views
	// Returns:			True if it has successfully rendered, false if not
	bool RenderScene(CameraClass* currentCamera);

	// Function:		Toggle Wireframe function
	// What it does:	Based on the current state of the m_Wireframe bool, sets the bool to true false turning wireframe on/off
	//					This is called after checking the state of the W key and passing in the keyDown bool to this function
	//					If the keyDown bool is true it will apply the changes
	// Takes in:		bool - keydown variable which is true if the key has been pressed, used to check if changes should be applied
	// Returns:			Nothing
	void ToggleWireframe(bool keydown);

	// Function:		Toggle Collision function
	// What it does:	Based on the current state of the m_CollisionOn bool, sets the collision checks on/off so user can collide with walls or not
	//					The bool is checked when user is trying to move forward and backward, to limit movement when collision is on
	//					This is called after checking the state of the C key and passing in the keyDown bool to this function
	//					If the keyDown bool is true it will apply the changes
	// Takes in:		bool - keydown variable which is true if the key has been pressed, used to check if changes should be applied
	// Returns:			Nothing	
	void ToggleCollision(bool keyDown);

	// Function:		Toggle Instructions function
	// What it does:	Based on the current state of the m_InstructionsOn bool, sets the bool to true/false turning the instructions on/off
	//					The bool is checked in the RenderGraphics function to check if the instructions text should be rendered that frame or not.
	//					This is called after checking the state of the I key and passing in the keyDown bool to this function
	//					If the keyDown bool is true it will apply the changes
	// Takes in:		bool - keydown variable which is true if the key has been pressed, used to check if changes should be applied
	// Returns:			Nothing		
	void ToggleInstructions(bool keyDown);

	// Function:		Toggle Distortion function
	// What it does:	Based on the current state of the m_DistortionOn bool, sets the bool to true/false turning the distortion settings on/off
	//					The bool is checked in the RenderGraphics function to see if the distortion shader should be rendered using the distortion's render texture
	//					This is called after checking the state of the D key and passing in the keyDown bool to this function
	//					If the keyDown bool is true it will apply the changes
	// Takes in:		bool - keydown variable which is true if the key has been pressed, used to check if changes should be applied
	// Returns:			Nothing			
	void ToggleDistortion(bool keydown);

	// Function:		Change Distortion function 
	// What it does:	Changes the value of the distortion (m_DistortionAmount) passed into the distortion shader so that the intensity of the distortion
	//					is changed during run time. 
	//					This is called when checking the state of the F key which passes in a negative value to decrease the distortion, and 
	//					when checking the state of the G passes in a positive value to increase the distortion
	// Takes in:		float - value of the change to be applied to the distortion amount (positive = increasing, negative = decreasing)
	//					bool - keydown variable which is true if the key has been pressed, used to check if changes should be applied
	// Returns:			Nothing
	void ChangeDistortion(float change, bool keyDown);

	// Function:		Render Dungeon function
	// What it does:	Uses the camera passed into RenderScene to render the floor, ceiling, and walls of the dungeon 
	//					Loops through these meshes and renders them in position using translation and scaling
	// Takes in:		D3DXMATRIX - worldMatrix, set in RenderScene. D3DXMATRIX - viewMatrix, set in RenderScene and specific to the camera passed in
	//					D3DXMATRIX - projectionMatrix, set in RenderScene and specific to the camera that was passed in
	//					CameraClass pointer - To the camera used to render the scene so it can be passed into the light shader
	// Returns:			True if it has successfully rendered, false if not
	bool RenderDungeon(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, CameraClass* currentCamera);

	// Function:		Render Map function
	// What it does:	Uses the camera passed into RenderScene to render the map, the doors and the player mesh object
	//					Loops through the dungeon floor pieces and the dungeon door pieces to render them as normal for the map
	//					Renders the player sprite according to the position and rotation of the main player camera
	// Takes in:		D3DXMATRIX - worldMatrix, set in RenderScene. D3DXMATRIX - viewMatrix, set in RenderScene and specific to the camera passed in
	//					D3DXMATRIX - projectionMatrix, set in RenderScene and specific to the camera that was passed in
	// Returns:			True if it has successfully rendered, false if not
	bool RenderMap(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix);

// Private Member Variables
private:
	// General objects
	InputClass* m_Input;
	D3DClass* m_Direct3D;
	FpsClass* m_Fps;
	CpuClass* m_Cpu;
	TimerClass* m_Timer;
	TextClass* m_Text;
	LightClass* m_Light;

	// Camera and position objects
	CameraClass* m_Camera;					// Movement camera through the dungeon
	CameraClass* m_MapCamera;				// Second camera to focus on the map 
	PositionClass* m_Position;

	// Shader objects
	TextureShaderClass* m_TextureShader;
	FontShaderClass* m_FontShader;
	LightShaderClass* m_LightShader;
	DistortionShaderClass *m_DistortionShader;

	// Dungeon objects
	DungeonClass* m_Dungeon;

	// Map objects
	RenderTextureClass* m_MapRenderTexture;
	TargetWindowClass* m_MapWindow;

	// Post processing objects
	RenderTextureClass* m_DistortionTexture;
	TargetWindowClass* m_PostProcessingWindow;
	
	// Input variables
	bool m_Wireframe;
	bool m_CollisionOn;
	bool m_InstructionsOn;
	float m_DistortionAmount;
	bool m_DistortionOn;
	int m_ButtonTimer;
};

#endif