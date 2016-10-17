////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cameraclass.h"



// Function:		Camera Class Constructor
// What it does:	Initialises all camera members to their default values/variables to set up the camera position, rotation, and lookat vectors
// Takes in:		Nothing
// Returns:			Nothing

CameraClass::CameraClass()
{
	m_Position.x = 0.0f;
	m_Position.y = 0.0f;
	m_Position.z = 0.0f;

	m_Rotation.x = 0.0f;
	m_Rotation.y= 0.0f;
	m_Rotation.z = 0.0f;

	m_LookAt.x = 0.0f;
	m_LookAt.y = 0.0f;
	m_LookAt.z = 1.0f;
}


// Placeholder destructor function

CameraClass::~CameraClass()
{
}



// Function:		Set Camera Position Function
// What it does:	Adjusts the cameras current position to the new position in Application Class as it changes
// Takes in:		float - Camera X Position, float - Camera Y Position, float - Camera Z Position
// Returns:			Nothing

void CameraClass::SetPosition(float x, float y, float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
	return;
}


// Function:		Set Camera Rotation Function
// What it does:	Adjusts the cameras current rotation settings to the values given to the function
// Takes in:		float - Camera X Rotation (Pitch), float - Camera Y Rotation (Yaw), float - Camera Z Rotation (Roll)
// Returns:			Nothing

void CameraClass::SetRotation(float x, float y, float z)
{
	m_Rotation.x = x;
	m_Rotation.y = y;
	m_Rotation.z = z;
	return;
}


// Function:		Get Camera Position Function
// What it does:	Returns the values of the private Position vector
// Takes in:		Nothing
// Returns:			D3DXVECTOR3 - The Vector3 holding the position values of the camera

D3DXVECTOR3 CameraClass::GetPosition()
{
	return m_Position;
}


// Function:		Get Camera Rotation Function
// What it does:	Returns the values of the private Rotation vector
// Takes in:		Nothing
// Returns:			D3DXVECTOR3 - The Vector3 holding the rotation values of the camera

D3DXVECTOR3 CameraClass::GetRotation()
{
	return m_Rotation;
}


// Function:		Get Camera LookAt Function
// What it does:	Returns the values of the private LookAt vector which holds the view direction of the camera
// Takes in:		Nothing
// Returns:			D3DXVECTOR3 - The Vector3 holding the LookAt values of the camera

D3DXVECTOR3 CameraClass::GetLookat()
{
	return m_LookAt;
}



// Function:		Render Camera Function 
// What it does:	Calculates the rotation of the camera for rendering based on the rotation values, and creates the View Matrix from the updated information
// Takes in:		Nothing
// Returns:			Nothing

void CameraClass::Render()
{
	// Temporary variables for calculations
	D3DXVECTOR3 up, position;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	position.x = m_Position.x;
	position.y = m_Position.y;
	position.z = m_Position.z;

	// Setup where the camera is looking by default.
	m_LookAt.x = 0.0f;
	m_LookAt.y = 0.0f;
	m_LookAt.z = 1.0f;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_Rotation.x * 0.0174532925f;
	yaw = m_Rotation.y * 0.0174532925f;
	roll = m_Rotation.z * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	D3DXVec3TransformCoord(&m_LookAt, &m_LookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	m_LookAt = position + m_LookAt;

	// Finally create the view matrix from the three updated vectors.
	D3DXMatrixLookAtLH(&m_ViewMatrix, &position, &m_LookAt, &up);

	return;
}


// Function:		Get View Matrix Function 
// What it does:	Sets the matrix passed into the function to the View Matrix stored in this class
// Takes in:		D3DXMATRIX& - The address of the View Matrix to be changed, so that it may be set in this function
// Returns:			Nothing

void CameraClass::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	// Sets the matrix found at this address to the current view matrix
	viewMatrix = m_ViewMatrix;
	return;
}


// Function:		Get Base View Matrix Function
// What it does:	Sets the base view matrix passed into the function to the view matrix used for 2D rendering
// Takes in:		D3DXMATRIX& - The address of the View Matrix to be changed, so that it may be set in this function
// Returns:			Nothing

void CameraClass::GetBaseViewMatrix(D3DXMATRIX& baseMatrix)
{
	// Temp variables for calculating
	D3DXVECTOR3 up, position, lookAt;

	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	position.x = 0.0;
	position.y = 0.0;
	position.z = -10.0;

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 0.0f;

	// Finally create the view matrix from the three updated vectors.
	D3DXMatrixLookAtLH(&baseMatrix, &position, &lookAt, &up);

	return;
}