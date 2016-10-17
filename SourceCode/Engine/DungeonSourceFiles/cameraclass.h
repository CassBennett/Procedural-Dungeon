////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3dx10math.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class CameraClass
{
// Public functions
public:

	// Function:		Camera Class Constructor
	// What it does:	Initialises all camera members to their default values/variables to set up the camera position, rotation, and lookat vectors
	// Takes in:		Nothing
	// Returns:			Nothing
	CameraClass();


	// Placeholder destructor function
	~CameraClass();


	// Function:		Set Camera Position Function
	// What it does:	Adjusts the cameras current position to the new position in Application Class as it changes
	// Takes in:		float - Camera X Position, float - Camera Y Position, float - Camera Z Position
	// Returns:			Nothing
	void SetPosition(float x, float y, float z);


	// Function:		Set Camera Rotation Function
	// What it does:	Adjusts the cameras current rotation settings to the values given to the function
	// Takes in:		float - Camera X Rotation (Pitch), float - Camera Y Rotation (Yaw), float - Camera Z Rotation (Roll)
	// Returns:			Nothing
	void SetRotation(float x, float y, float z);


	// Function:		Get Camera Position Function
	// What it does:	Returns the values of the private Position vector
	// Takes in:		Nothing
	// Returns:			D3DXVECTOR3 - The Vector3 holding the position values of the camera
	D3DXVECTOR3 GetPosition();


	// Function:		Get Camera Rotation Function
	// What it does:	Returns the values of the private Rotation vector
	// Takes in:		Nothing
	// Returns:			D3DXVECTOR3 - The Vector3 holding the rotation values of the camera
	D3DXVECTOR3 GetRotation();


	// Function:		Get Camera LookAt Function
	// What it does:	Returns the values of the private LookAt vector which holds the view direction of the camera
	// Takes in:		Nothing
	// Returns:			D3DXVECTOR3 - The Vector3 holding the LookAt values of the camera
	D3DXVECTOR3 GetLookat();


	// Function:		Render Camera Function 
	// What it does:	Calculates the rotation of the camera for rendering based on the rotation values, and creates the View Matrix from the updated information
	// Takes in:		Nothing
	// Returns:			Nothing
	void Render();


	// Function:		Get View Matrix Function 
	// What it does:	Sets the matrix passed into the function to the View Matrix stored in this class
	// Takes in:		D3DXMATRIX& - The address of the View Matrix to be changed, so that it may be set in this function
	// Returns:			Nothing
	void GetViewMatrix(D3DXMATRIX& viewMatrix);


	// Function:		Get Base View Matrix Function
	// What it does:	Sets the base view matrix passed into the function to the view matrix used for 2D rendering
	// Takes in:		D3DXMATRIX& - The address of the View Matrix to be changed, so that it may be set in this function
	// Returns:			Nothing
	void GetBaseViewMatrix(D3DXMATRIX& baseMatrix);


// Private Member Variables
private:
	
	// Camera transform objects
	D3DXVECTOR3 m_Position;
	D3DXVECTOR3 m_Rotation;
	D3DXVECTOR3 m_LookAt;
	D3DXMATRIX m_ViewMatrix;
	
};

#endif