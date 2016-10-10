////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"

// Function:		Application Class Constructor
// What it does:	Initialises all members to their default values/variables to set up the application class
// Takes in:		Nothing
// Returns:			Nothing

ApplicationClass::ApplicationClass()
{
	// Set all variables to default values to set up the class
	m_Input = 0;
	m_Direct3D = 0;
	m_Camera = 0;
	m_MapCamera = 0;
	m_Timer = 0;
	m_Position = 0;
	m_Fps = 0;
	m_Cpu = 0;
	m_FontShader = 0;
	m_Text = 0;
	m_TextureShader = 0;
	m_Light = 0;
	m_LightShader = 0;
	m_Wireframe = 0;
	m_MapRenderTexture = 0;
	m_MapWindow = 0;
	m_DistortionAmount = 0.001f;
	m_DistortionOn = true;
	m_CollisionOn = true;
	m_InstructionsOn = false;
	m_ButtonTimer = 0;
}


// Placeholder constructor

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


// Placeholder destructor

ApplicationClass::~ApplicationClass()
{
}


// Function:		Initialize Function
// What it does:	Initializes all objects within application class such as the window and shaders/cameras/meshes
// Takes in:		HINSTANCE - Instance object for the window, HWND - handle for the window, int - screen width dimensions, int - screen height dimensions
// Returns:			True if all objects have initialised correctly, false if an object is not initialised correctly so system class can 
//					shut it down and show error 

bool ApplicationClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	bool result;
	float cameraX, cameraY, cameraZ;
	float rotationX, rotationY, rotationZ;
	D3DXMATRIX baseViewMatrix;
	char videoCard[128];
	int videoMemory;
	
	// Create the input object.  The input object will be used to handle reading the keyboard and mouse input from the user.
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}

	// Initialize the input object.
	result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Create the Direct3D object.
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, false, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize DirectX 11.", L"Error", MB_OK);
		return false;
	}

	// Create the main camera object used for rendering the user's view in the dungeon
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// Initialize a base view matrix with the camera for 2D user interface rendering.
	m_Camera->SetPosition(0.0f, 0.0f, 0.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	// Set the initial position of the camera (center of dungeon)
	cameraX = 410.0f;
	cameraY = 20.0f;
	cameraZ = 410.0f;
	rotationX = 0.0f;
	rotationY = 0.0f; 
	rotationZ = 0.0f;

	m_Camera->SetPosition(cameraX, cameraY, cameraZ);
	m_Camera->SetRotation(rotationX, rotationY, rotationZ);

	// Create the map object used for rendering the map
	m_MapCamera = new CameraClass;
	if (!m_MapCamera)
	{
		return false;
	}

	// Initialize a base view matrix with the camera for 2D user interface rendering.
	m_MapCamera->SetPosition(0.0f, 0.0f, 0.0f);
	m_MapCamera->Render();
	m_MapCamera->GetViewMatrix(baseViewMatrix);

	// Set the initial position of the map camera (looking at the map's center)
	float mapCameraX = 20.0f;
	float mapCameraY = 55.0f;
	float mapCameraZ = 20.0f;
	float mapRotationX = 90.0f;
	float mapRotationY = 0.0f;
	float mapRotationZ = 0.0f;

	m_MapCamera->SetPosition(mapCameraX, mapCameraY, mapCameraZ);
	m_MapCamera->SetRotation(mapRotationX, mapRotationY, mapRotationZ);

	// Create the Dungeon object
	m_Dungeon = new DungeonClass;
	if (!m_Dungeon)
	{
		return false;
	}

	// Initialise the Dungeon object 
	result = m_Dungeon->Initialise(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the dungeon object.", L"Error", MB_OK);
		return false;
	}

	// Initialise the walls for the dungeon after the dungeon is initialised
	for (auto DungeonPiece = m_Dungeon->m_DungeonPieces.begin(); DungeonPiece != m_Dungeon->m_DungeonPieces.end(); ++ DungeonPiece)
	{
		// Call BuildWalls on each piece so that walls are placed around that dungeon piece after it has been created
		(*DungeonPiece)->BuildWalls();
		// For each wall created, then call FillGridWithWall passing in each wall of this dungeon piece so they are included in the collision grid
		for (auto Wall = (*DungeonPiece)->Walls.begin(); Wall != (*DungeonPiece)->Walls.end(); ++ Wall)
		{
			// Ensures that there is information on the collision grid for each wall so they can be collided with
			m_Dungeon->FillGridWithWall(*Wall);
		}
	}

	// Create the map target render object
	m_MapRenderTexture = new RenderTextureClass;
	if (!m_MapRenderTexture)
	{
		return false;
	}

	// Initialise the Map Render Texture 
	// Pass in the dimensions of what the texture should render, and the screen far and near values to render
	// This is the render target used for the map
	result = m_MapRenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, 100.0f, 10.0f);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Map Render Texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the map target window
	m_MapWindow = new TargetWindowClass;
	if (!m_MapWindow)
	{
		return false;
	}

	// Set the Map window width and height
	float RenderTextureWidth = 800;
	float RenderTextureHeight = 300;
	float MapWindowX = -570; 
	float MapWindowY = 260;

	// Initialise the Map render texture Window by passing in the screen width and height
	// Pass in the Render Texture Width and Height for the dimensions of the texture and the position of the window on the screen
	result = m_MapWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, RenderTextureWidth, RenderTextureHeight, MapWindowX, MapWindowY);	// Top left
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Map Render Texture Window object.", L"Error", MB_OK);
		return false;		
	}

	// Create the distortion shader object.
	m_DistortionShader = new DistortionShaderClass;
	if (!m_DistortionShader)
	{
		return false;
	}

	// Initialize the distortion shader object.
	result = m_DistortionShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the distortion shader object.", L"Error", MB_OK);
		return false;
	}

	// Set up the post processing render texture objects
	// Create the Distortion render texture object
	m_DistortionTexture = new RenderTextureClass;
	if (!m_DistortionTexture)
	{
		return false;
	}

	// Initialise the distortion render texture object
	result = m_DistortionTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, 100.0f, 10.0f);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the distortion texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the post processing distortion target window
	m_PostProcessingWindow = new TargetWindowClass;
	if (!m_PostProcessingWindow)
	{
		return false;
	}

	// Initialise the post processing window using screen width and height as the target texture to render, and screen width and height as the size of the 
	// window. Position at center 0, 0. 
	result = m_PostProcessingWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, screenWidth, screenHeight, 0, 0);	// Center
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the post processing window object.", L"Error", MB_OK);
		return false;		
	}

	// Create the light shader object
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
	{
		return false;
	}

	// Initialise the light shader object
	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;	
	}

	// Create the light object.
	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}

	// Initialize the light object values
	m_Light->SetDiffuseColor(10.0, 10.0, 10.0f, 1.0);
	m_Light->SetAmbientColor(0.3, 0.3, 0.3, 1.0);
	m_Light->SetSpecularColor(1.0, 1.0, 1.0, 0.0);
	m_Light->SetSpecularPower(10.0);
	m_Light->SetPointLightPos(cameraX, cameraY, cameraZ);
	m_Light->SetDirection(0.0, -1.0, 0.0);
	m_Light->SetConstantFactor(1.0f);
	m_Light->SetLinearFactor(0.125);
	m_Light->SetQuadraticFactor(0.0f);
	m_Light->SetDistance(40.0f);

	// Create the timer object.
	m_Timer = new TimerClass;
	if (!m_Timer)
	{
		return false;
	}

	// Initialize the timer object.
	result = m_Timer->Initialize();
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
		return false;
	}

	// Create the position object.
	m_Position = new PositionClass;
	if (!m_Position)
	{
		return false;
	}

	// Set the initial position of the viewer to the same as the initial camera position.
	m_Position->SetPosition(cameraX, cameraY, cameraZ);
	m_Position->SetRotation(rotationX, rotationY, rotationZ);

	// Create the color shader object.
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	// Initialize the color shader object.
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the fps object.
	m_Fps = new FpsClass;
	if (!m_Fps)
	{
		return false;
	}

	// Initialize the fps object.
	m_Fps->Initialize();

	// Create the cpu object.
	m_Cpu = new CpuClass;
	if (!m_Cpu)
	{
		return false;
	}

	// Initialize the cpu object.
	m_Cpu->Initialize();

	// Create the font shader object.
	m_FontShader = new FontShaderClass;
	if (!m_FontShader)
	{
		return false;
	}

	// Initialize the font shader object.
	result = m_FontShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the text object.
	m_Text = new TextClass;
	if (!m_Text)
	{
		return false;
	}

	// Initialize the text object.
	result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	// Retrieve the video card information.
	m_Direct3D->GetVideoCardInfo(videoCard, videoMemory);

	// Set the video card information in the text object.
	result = m_Text->SetVideoCardInfo(videoCard, videoMemory, m_Direct3D->GetDeviceContext());
	if (!result)
	{
		MessageBox(hwnd, L"Could not set video card info in the text object.", L"Error", MB_OK);
		return false;
	}

	// Return true if no errors
	return true;
}


// Function:		Shutdown function
// What it does:	Releases all objects when the application ends and deletes them, cleans resources
// Takes in:		Nothing
// Returns:			Nothing

void ApplicationClass::Shutdown()
{
	// Release the camera object
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the map camera object
	if (m_MapCamera)
	{
		delete m_MapCamera;
		m_MapCamera = 0;
	}

	// Release the position object
	if (m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	// Release the texture shader
	if (m_TextureShader)
	{
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the light shader
	if (m_LightShader)
	{
		if (m_LightShader)
		{
			delete m_LightShader;
			m_LightShader = 0;
		}
	}

	// Release the distortion shader
	if (m_DistortionShader)
	{
		delete m_DistortionShader;
		m_DistortionShader = 0;
	}

	// Release the Dungeon object
	if (m_Dungeon)
	{
		m_Dungeon->Shutdown();
		delete m_Dungeon;
		m_Dungeon = 0;
	}

	// Release the map render texture 
	if (m_MapRenderTexture)
	{
		delete m_MapRenderTexture;
		m_MapRenderTexture = 0;
	}

	// Release Map Window Texture
	if (m_MapWindow)
	{
		delete m_MapWindow;
		m_MapWindow = 0;
	}

	// Release the distortion texture
	if (m_DistortionTexture)
	{
		delete m_DistortionTexture;
		m_DistortionTexture = 0;
	}

	// Release the post processing window
	if (m_PostProcessingWindow)
	{
		delete m_PostProcessingWindow;
		m_PostProcessingWindow = 0;
	}



	// Release the light object.
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the text object.
	if(m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	// Release the font shader object.
	if(m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	// Release the cpu object.
	if(m_Cpu)
	{
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	// Release the fps object.
	if(m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}

	// Release the position object.
	if(m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	// Release the timer object.
	if(m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the Direct3D object.
	if(m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	// Release the input object.
	if(m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	return;
}

// Function:		Frame function
// What it does:	This is the applications Update function, and is called every frame from the System class. It calls the updates for the 
//					objects and calls the RenderGraphics function every frame so screen and application settings are updated every frame
// Takes in:		Nothing
// Returns:			True if all update functions have been carried out successfully, false if not to close the application

bool ApplicationClass::Frame()
{
	bool result;

	// Update the player and light position every frame
	m_Dungeon->SetPlayerPosition(m_Camera->GetPosition());
	m_Light->SetPointLightPos(m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z);

	// Read the user input.
	result = m_Input->Frame();
	if (!result)
	{
		return false;
	}
	
	// Check if the user pressed escape and wants to exit the application.
	if (m_Input->IsEscapePressed() == true)
	{
		return false;
	}

	// If button timer >= 0, reduce button timer. Button Timer is only set to a value when a toggle button is pressed
	// So that there is a delay between the time between a button is pressed until it can be pressed again for easier toggle presses
	if (m_ButtonTimer >= 0)
	{
		m_ButtonTimer--;
	}

	// Update the system stats.
	m_Timer->Frame();
	m_Fps->Frame();
	m_Cpu->Frame();

	// Update the FPS value in the text object.
	result = m_Text->SetFps(m_Fps->GetFps(), m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}
	
	// Update the CPU usage value in the text object.
	result = m_Text->SetCpu(m_Cpu->GetCpuPercentage(), m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// Do the frame input processing.
	result = HandleInput(m_Timer->GetTime());
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = RenderGraphics();
	if (!result)
	{
		return false;
	}

	// Return true if no errors
	return result;
}


// Function:		Handle Input function
// What it does:	Checks the current input of the program and calls the functions to apply the input based on the keys pressed and the state of the application
//					It is called every frame from the Frame() update function in this class.
// Takes in:		float - Current time to set the frame time
// Returns:			True if input updates have been successful, false if they aren't

bool ApplicationClass::HandleInput(float frameTime)
{
	bool keyDown, result;
	float posX, posY, posZ, rotX, rotY, rotZ;

	// Set the frame time for calculating the updated position.
	m_Position->SetFrameTime(frameTime);

	// Turn the player camera left - LEFT KEY
	keyDown = m_Input->IsLeftPressed();
	m_Position->TurnLeft(keyDown);

	// Turn the player camera right - RIGHT KEY
	keyDown = m_Input->IsRightPressed();
	m_Position->TurnRight(keyDown);

	// Check if the player wants to move forward - UP KEY
	keyDown = m_Input->IsUpPressed();

	// If collision is currently on, only allow if it is possible for player to move in this direction by calling CanMovePlayer
	// Will not call MoveForward if player will collide with a wall or leave the dungeon space
	if ((m_CollisionOn) && (m_Dungeon->CanMovePlayer(m_Camera->GetLookat(), FORWARD)))
	{	
		m_Position->MoveForward(keyDown);
	}
	// If player has collided, set the speed to 0.0 so that it won't try and move at the same speed it stopped at when it isn't colliding after this
	if ((m_CollisionOn) && (!m_Dungeon->CanMovePlayer(m_Camera->GetLookat(), FORWARD)))
	{
		m_Position->m_forwardSpeed = 0.0f;
	}
	else if (!m_CollisionOn)			// Else if collision is not on, player can move anywhere on the map
	{
		m_Position->MoveForward(keyDown);
	}

	// Check if the player wants to move backward - DOWN KEY
	keyDown = m_Input->IsDownPressed(); 

	// If collision is currently on, only allow if it is possible for player to move in this direction by calling CanMovePlayer
	// Will not call MoveBackward if player will collide with a wall or leave dungeon space
	if ((m_CollisionOn) && (m_Dungeon->CanMovePlayer(m_Camera->GetLookat(), BACKWARD)))
	{
		m_Position->MoveBackward(keyDown);
	}
	// If player has collided, set the speed to 0.0 so that it won't try and move at the same speed it stopped at when it isn't colliding after this
	if ((m_CollisionOn) && (!m_Dungeon->CanMovePlayer(m_Camera->GetLookat(), BACKWARD)))
	{
		m_Position->m_backwardSpeed = 0.0f;
	}
	else if (!m_CollisionOn)				// Else if collision is not on, player can move anywhere on the map
	{
		m_Position->MoveBackward(keyDown);
	}

	// Move player camera upwards - A KEY
	keyDown = m_Input->IsAPressed();
	m_Position->MoveUpward(keyDown);

	// Move player camera downwards - Z KEY
	keyDown = m_Input->IsZPressed();
	m_Position->MoveDownward(keyDown);

	// Rotate player camera upwards - PGUP KEY
	keyDown = m_Input->IsPgUpPressed();
	m_Position->LookUpward(keyDown);

	// Rotate player camera downwards - PGDOWN KEY
	keyDown = m_Input->IsPgDownPressed();
	m_Position->LookDownward(keyDown);
	
	// Toggle wireframe for the application - W KEY
	keyDown = m_Input->IsWPressed();	
	ToggleWireframe(keyDown);

	// Toggle Distortion for the application - D KEY
	keyDown = m_Input->IsDPressed();		
	ToggleDistortion(keyDown);

	// Decrease Distortion for the application - F KEY
	keyDown = m_Input->IsFPressed();		
	ChangeDistortion(-0.001f, keyDown);

	// Increase Distortion for the application - G KEY
	keyDown = m_Input->IsGPressed();		
	ChangeDistortion(0.001f, keyDown);

	// Toggle Collision for the application - C KEY
	keyDown = m_Input->IsCPressed();		
	ToggleCollision(keyDown);
	
	// Toggle Instructions for the application - I KEY
	keyDown = m_Input->IsIPressed();		
	ToggleInstructions(keyDown);
	
	// Get the view point position/rotation.
	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);

	// Set the position of the main camera.
	m_Camera->SetPosition(posX, posY, posZ);

	// Update the position values in the text object.
	result = m_Text->SetCameraPosition(posX, posY, posZ, m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// Set the rotation of the main camera
	m_Camera->SetRotation(rotX, rotY, rotZ);

	// Update the rotation values in the text object.
	result = m_Text->SetCameraRotation(rotX, rotY, rotZ, m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// Update the look at values in the text object.
	D3DXVECTOR3 CamLookAt = m_Camera->GetLookat();
	result = m_Text->SetCameraLookat(CamLookAt.x, CamLookAt.y, CamLookAt.z, m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// Return true if there are no errors
	return true;
}


// Function:		Toggle Wireframe function
// What it does:	Based on the current state of the m_Wireframe bool, sets the bool to true false turning wireframe on/off
//					This is called after checking the state of the W key and passing in the keyDown bool to this function
//					If the keyDown bool is true it will apply the changes
// Takes in:		bool - keydown variable which is true if the key has been pressed, used to check if changes should be applied
// Returns:			Nothing

void ApplicationClass::ToggleWireframe(bool keyDown)
{
	// If button timer is ready and the key is down
	if ((m_ButtonTimer <= 0) && (keyDown))
	{
		// If wireframe is currently on
		if (m_Wireframe)
		{
			// Set the fill mode to Solid
			m_Direct3D->rasterDesc.FillMode = D3D11_FILL_SOLID;
			m_Direct3D->GetDevice()->CreateRasterizerState(&m_Direct3D->rasterDesc, &m_Direct3D->m_rasterState);
			m_Direct3D->GetDeviceContext()->RSSetState(m_Direct3D->m_rasterState);
			// Set wireframe to false
			m_Wireframe = false;
		}
		// If wireframe is currently off
		else if (!m_Wireframe)
		{
			// Set the fill mode to Wireframe
			m_Direct3D->rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
			m_Direct3D->GetDevice()->CreateRasterizerState(&m_Direct3D->rasterDesc, &m_Direct3D->m_rasterState);
			m_Direct3D->GetDeviceContext()->RSSetState(m_Direct3D->m_rasterState);
			// Set the wireframe to true
			m_Wireframe = true;
		}
		keyDown = false;
		// Increase button timer so there is a delay until the key can be pressed again
		m_ButtonTimer = 20;
	}
}


// Function:		Toggle Distortion function
// What it does:	Based on the current state of the m_DistortionOn bool, sets the bool to true/false turning the distortion settings on/off
//					The bool is checked in the RenderGraphics function to see if the distortion shader should be rendered using the distortion's render texture
//					This is called after checking the state of the D key and passing in the keyDown bool to this function
//					If the keyDown bool is true it will apply the changes
// Takes in:		bool - keydown variable which is true if the key has been pressed, used to check if changes should be applied
// Returns:			Nothing			

void ApplicationClass::ToggleDistortion(bool keyDown)
{
	// If button timer is ready and the key is down
	if ((m_ButtonTimer <= 0) && (keyDown))
	{
		// If the distortion is currently on
		if (m_DistortionOn)
		{
			// Turn the distortion off so the texture using the shader does not render
			m_DistortionOn = false;
		}
		// If the distortion is currently off
		else if (!m_DistortionOn)
		{
			// Turn the distortion on so the texture using the shader will render 
			m_DistortionOn = true;
		}
		// Increase button timer so there is a delay until the key can be pressed again
		m_ButtonTimer = 20;

	}
}


// Function:		Change Distortion function 
// What it does:	Changes the value of the distortion (m_DistortionAmount) passed into the distortion shader so that the intensity of the distortion
//					is changed during run time. 
//					This is called when checking the state of the F key which passes in a negative value to decrease the distortion, and 
//					when checking the state of the G passes in a positive value to increase the distortion
// Takes in:		float - value of the change to be applied to the distortion amount (positive = increasing, negative = decreasing)
//					bool - keydown variable which is true if the key has been pressed, used to check if changes should be applied
// Returns:			Nothing

void ApplicationClass::ChangeDistortion(float change, bool keyDown)
{
	// If the key is down, change the distortion amount by the value passed in
	if (keyDown)
	{
		// Change the distortion amount that is passed into the distortion shader
		m_DistortionAmount += change;
		// If distortion amount reaches upper 0.5 limit, set to 0.5
		if (m_DistortionAmount > 0.5)
		{
			m_DistortionAmount = 0.5;
		}
		// If distortion amount reaches lower 0.001 limit, set to 0.001
		if (m_DistortionAmount < 0.001)
		{
			m_DistortionAmount = 0.001;
		}
	}
}


// Function:		Toggle Collision function
// What it does:	Based on the current state of the m_CollisionOn bool, sets the collision checks on/off so user can collide with walls or not
//					The bool is checked when user is trying to move forward and backward, to limit movement when collision is on
//					This is called after checking the state of the C key and passing in the keyDown bool to this function
//					If the keyDown bool is true it will apply the changes
// Takes in:		bool - keydown variable which is true if the key has been pressed, used to check if changes should be applied
// Returns:			Nothing	

void ApplicationClass::ToggleCollision(bool keyDown)
{
	// If button timer is ready and the key is down
	if ((m_ButtonTimer <= 0) && (keyDown))
	{
		// If collision is currently on
		if (m_CollisionOn)
		{
			// Set collision to false
			m_CollisionOn = false;
		}
		// If collision is currently off
		else if (!m_CollisionOn)
		{
			// Sets the player back in the dungeon grid at the last place they were 
			D3DXVECTOR3 PlayerLastPos = m_Dungeon->m_PlayerLastPosition;
			m_Position->SetPosition(PlayerLastPos.x, PlayerLastPos.y, PlayerLastPos.z);
			// Set collision to true
			m_CollisionOn = true;
		}
		// Increase button timer so there is a delay until the key can be pressed again
		m_ButtonTimer = 20;
	}
}


// Function:		Toggle Instructions function
// What it does:	Based on the current state of the m_InstructionsOn bool, sets the bool to true/false turning the instructions on/off
//					The bool is checked in the RenderGraphics function to check if the instructions text should be rendered that frame or not.
//					This is called after checking the state of the I key and passing in the keyDown bool to this function
//					If the keyDown bool is true it will apply the changes
// Takes in:		bool - keydown variable which is true if the key has been pressed, used to check if changes should be applied
// Returns:			Nothing	

void ApplicationClass::ToggleInstructions(bool keyDown)
{
	// If button timer is ready and the key is down
	if ((m_ButtonTimer <= 0) && (keyDown))
	{
		// If instructions are currently on
		if (m_InstructionsOn)
		{
			// Set instructions to false
			m_InstructionsOn = false;
		}
		// If instructions are currently off
		else if (!m_InstructionsOn)
		{
			// Set instructions to true
			m_InstructionsOn = true;
		}
		// Increase button timer so there is a delay until the key can be pressed again
		m_ButtonTimer = 20;
	}
}


// Function:		Render Graphics function
// What it does:	This is called every frame in the Frame update function in this class. It renders the geometry and textures to 
//					the screen, calling RenderScene rendering all objects and also renders the text to the screen
// Takes in:		Nothing
// Returns:			True if all renders have been successful, false if not so that the application will close and show an error

bool ApplicationClass::RenderGraphics()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, baseViewMatrix;
	bool result;

	// Render scene to the map texture using the maps camera orientation/position
	result = RenderToTexture(m_MapRenderTexture, m_MapCamera);
	if(!result)
	{
		return false;
	}

	// Render scene to the distortion texture using the normal camera orientation/position
	result = RenderToTexture(m_DistortionTexture, m_Camera);
	if(!result)
	{
		return false;
	}

	// Clear the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Render the scene as normal to the back buffer.
	result = RenderScene(m_Camera);
	if(!result)
	{
		return false;
	}

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);			

	// If the distortion is turned on
	if (m_DistortionOn)
	{
		// Render the scene as a texture using the distortion shader, passing in the distortion amount set in this class
		m_PostProcessingWindow->Render(m_Direct3D->GetDeviceContext());
		m_DistortionShader->Render(m_Direct3D->GetDeviceContext(), m_PostProcessingWindow->GetIndexCount(), worldMatrix, baseViewMatrix, 
									orthoMatrix, m_DistortionTexture->GetShaderResourceView(), m_DistortionAmount);
	}

	D3DXMatrixTranslation(&worldMatrix, 0.0f, 2.0f, 1.0f);

	// Render the scene viewed from the map camera to the map render texture and render this window
	m_MapWindow->Render(m_Direct3D->GetDeviceContext());
	m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_MapWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix, m_MapRenderTexture->GetShaderResourceView());
	
	// Turn on the alpha blending before rendering the text.
	m_Direct3D->TurnOnAlphaBlending();

	// Render the text user interface elements.
	result = m_Text->Render(m_Direct3D->GetDeviceContext(), m_FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	// If the instructions are turned on, render the instructions at the upper right corner of the screen
	if (m_InstructionsOn)
	{
		result = m_Text->RenderInstructions(m_Direct3D->GetDeviceContext(), m_FontShader, worldMatrix, orthoMatrix);
		if(!result)
		{
			return false;
		}
	}

	// Turn off alpha blending after rendering the text.
	m_Direct3D->TurnOffAlphaBlending();

	m_Direct3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}


// Function:		Render To Texture function
// What it does:	Uses the camera passed in to render the entire current scene to a texture object which can then be used with the 
//					distortion shader for the post processing effect or just for the map texture in the corner of the screen
// Takes in:		RenderTextureClass pointer - to the texture this window that will be used to show this texture, 
//					CameraClass pointer - to the camera that is being used as the view for this texture, so that separate views can be rendered
//											which is important to distinguish for the applications main camera view and the map camera view
// Returns:			True if it has successfully rendered to the texture, false if not 

bool ApplicationClass::RenderToTexture(RenderTextureClass* WindowTexture, CameraClass* CurrentCamera)
{
	// changes render target
	bool result;

	// Set the render target to be the render to texture.
	WindowTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	WindowTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Render the scene now and it will draw to the render to texture instead of the back buffer.
	result = RenderScene(CurrentCamera);
	if (!result)
	{
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}


// Function:		Render Scene function
// What it does:	Uses the camera passed into the function to render the scene as it is according to that camera
//					Calls RenderDungeon and RenderMap to render all objects in the scene
// Takes in:		CameraClass pointer - To the camera used to render the scene so it can render the scene in different views
// Returns:			True if it has successfully rendered, false if not

bool ApplicationClass::RenderScene(CameraClass* currentCamera)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, scalingMatrix, rotationMatrix, translationMatrix;
	bool result;

	// Generate the view matrix based on the camera's position.
	currentCamera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	currentCamera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Render the map using the current camera's world matrix, view matrix and projection matrix
	RenderMap(worldMatrix, viewMatrix, projectionMatrix);

	// Render the dungeon using the current camera's world matrix, view matrix and projection matrix
	RenderDungeon(worldMatrix, viewMatrix, projectionMatrix, currentCamera);

	return true;
}


// Function:		Render Dungeon function
// What it does:	Uses the camera passed into RenderScene to render the floor, ceiling, and walls of the dungeon 
//					Loops through these meshes and renders them in position using translation and scaling
// Takes in:		D3DXMATRIX - worldMatrix, set in RenderScene. D3DXMATRIX - viewMatrix, set in RenderScene and specific to the camera passed in
//					D3DXMATRIX - projectionMatrix, set in RenderScene and specific to the camera that was passed in
//					CameraClass pointer - To the camera used to render the scene so it can be passed into the light shader
// Returns:			True if it has successfully rendered, false if no

bool ApplicationClass::RenderDungeon(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, CameraClass* currentCamera)
{
	bool result;
	D3DXMATRIX scalingMatrix, translationMatrix;

	// Scale and translate the floor dungeon pieces to their new position, scaled larger than the map pieces and placed in a separate area
	D3DXMatrixScaling(&scalingMatrix, DUNGEON_SCALE, DUNGEON_SCALE, DUNGEON_SCALE);
	D3DXMatrixTranslation(&translationMatrix, 10.0f, 10.0f, 10.0f);
	D3DXMatrixMultiply(&worldMatrix, &scalingMatrix, &translationMatrix);

	// Loop through all the dungeon pieces to render them in their new positions
	for (auto DungeonPiece = m_Dungeon->m_DungeonPieces.begin(); DungeonPiece != m_Dungeon->m_DungeonPieces.end(); ++ DungeonPiece)
	{
		// Push mesh data onto gfx hardware
		(*DungeonPiece)->Render();

		// Use the light shader to render the dungeon floor pieces, so that it is affected by the light shader
		result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), (*DungeonPiece)->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
			(*DungeonPiece)->GetDungeonTexture(), m_Light, currentCamera);
		if(!result)
		{
			return false;
		}

		// Loop through the walls belonging to this dungeon piece and render them to the scene
		for (auto Wall = (*DungeonPiece)->Walls.begin(); Wall != (*DungeonPiece)->Walls.end(); Wall++)
		{
			// Only render the walls after all have been fully created
			if ((*DungeonPiece)->WallsGenerated)
			{
				// Call render for the wall mesh
				(*Wall)->Render(m_Direct3D->GetDeviceContext());

				// Render the walls with the light shader so that it can be affected by the light that moves with the player
				result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), (*Wall)->GetIndexCount(), worldMatrix, viewMatrix, 
													projectionMatrix, (*Wall)->GetTexture(), m_Light, currentCamera);
				if (!result)
				{
					return false;
				}
			}
		}
	}

	// Scale and translate the ceiling dungeon pieces similar to the floor pieces, but with a higher y translate so it is above the floor like a ceiling
	m_Direct3D->GetWorldMatrix(worldMatrix);
	D3DXMatrixScaling(&scalingMatrix, DUNGEON_SCALE, DUNGEON_SCALE, DUNGEON_SCALE);
	D3DXMatrixTranslation(&translationMatrix, 10.0f, 40.0f, 10.0f);
	D3DXMatrixMultiply(&worldMatrix, &scalingMatrix, &translationMatrix);

	// Loop through all of the ceiling pieces and render them to the scene
	for (auto CeilingPiece = m_Dungeon->m_CeilingPieces.begin(); CeilingPiece != m_Dungeon->m_CeilingPieces.end(); ++ CeilingPiece)
	{
		// Push mesh data onto gfx hardware
		(*CeilingPiece)->Render();

		// Render the ceiling pieces using the light shader so that the render is affected by the light that moves with the player
		result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), (*CeilingPiece)->GetIndexCount(), worldMatrix, viewMatrix, 
															projectionMatrix, (*CeilingPiece)->GetDungeonTexture(), m_Light, currentCamera);
		if(!result)
		{
			return false;
		}
	}

	return true;
}


// Function:		Render Map function
// What it does:	Uses the camera passed into RenderScene to render the map, the doors and the player mesh object
//					Loops through the dungeon floor pieces and the dungeon door pieces to render them as normal for the map
//					Renders the player sprite according to the position and rotation of the main player camera
// Takes in:		D3DXMATRIX - worldMatrix, set in RenderScene. D3DXMATRIX - viewMatrix, set in RenderScene and specific to the camera passed in
//					D3DXMATRIX - projectionMatrix, set in RenderScene and specific to the camera that was passed in
// Returns:			True if it has successfully rendered, false if not

bool ApplicationClass::RenderMap(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	bool result;
	D3DXMATRIX scalingMatrix, translationMatrix, rotationMatrix;

	// Loop through all of the dungeon pieces and render them in position as usual
	for (auto DungeonPiece = m_Dungeon->m_DungeonPieces.begin(); DungeonPiece != m_Dungeon->m_DungeonPieces.end(); ++ DungeonPiece)
	{
		// Push mesh data onto gfx hardware
		(*DungeonPiece)->Render();

		// Render the Dungeon pieces using the texture shader, because only want to show the texture and don't want the map to be affected by the lighting
		result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), (*DungeonPiece)->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
				(*DungeonPiece)->GetMapTexture());
		if (!result)
		{
			return false;
		}
	}

	m_Direct3D->GetWorldMatrix(worldMatrix);

	// Loop through all of the door pieces for the map and render them to the map
	for (auto DoorPiece = m_Dungeon->m_DoorPieces.begin(); DoorPiece != m_Dungeon->m_DoorPieces.end(); ++ DoorPiece)
	{
		// Push mesh data onto gfx hardware
		(*DoorPiece)->Render();

		// Render the door pieces using the texture shader, because only want to show the texture and don't want the map to be affected by the lighting
		result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), (*DoorPiece)->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
			(*DoorPiece)->GetMapTexture());
		if (!result)
		{
			return false;
		}
	}

	// Render the player mesh to the screen in the position of the main camera and rotation
	m_Direct3D->GetWorldMatrix(worldMatrix);
	// Translate the player mesh according to the cameras current x and z positions in the dungeon
	D3DXMatrixTranslation(&translationMatrix, m_Dungeon->m_PlayerPosition.x,  m_Dungeon->m_PlayerPosition.y, m_Dungeon->m_PlayerPosition.z);
	// Get the camera rotation and convert the degrees to radians
	D3DXVECTOR3 CamRotation = m_Camera->GetRotation();
	float angle = CamRotation.y * (D3DX_PI/180);
	// Create the rotation matrix using this rotation to apply it to the player mesh
	D3DXMatrixRotationY(&rotationMatrix, angle);
	D3DXMatrixMultiply(&worldMatrix, &rotationMatrix, &translationMatrix);
	
	// Render the player mesh to the screen on the map
	m_Dungeon->m_PlayerMesh->Render(m_Direct3D->GetDeviceContext());

	// Render the player mesh using the texture shader, because only want to show the texture and don't want the map to be affected by the lighting
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Dungeon->m_PlayerMesh->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Dungeon->m_PlayerMesh->GetTexture());
	if (!result)
	{
		return false;
	}

	return true;
}
