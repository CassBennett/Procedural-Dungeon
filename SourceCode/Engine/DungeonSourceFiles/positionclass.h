////////////////////////////////////////////////////////////////////////////////
// Filename: positionclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _POSITIONCLASS_H_
#define _POSITIONCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <math.h>
#include "d3dclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: PositionClass
// Stores and updates information about the player's current transform in the scene
////////////////////////////////////////////////////////////////////////////////

class PositionClass
{
public:
	// Function:		Position Class Constructor
	// What it does:	Initialises all variables to their default values
	// Takes in:		Nothing
	// Returns:			Nothing
	PositionClass();


	// Placeholder constructor and destructor functions
	PositionClass(const PositionClass&);
	~PositionClass();


	// Function:		Set Position Function
	// What it does:	Adjusts the current position values to the values passed into the function to update the player information
	// Takes in:		float - X Position, float - Y Position, Z - Position
	// Returns:			Nothing
	void SetPosition(float x, float y, float z);


	// Function:		Set Rotation Function
	// What it does:	Adjusts the current rotation values to the values passed into the function to update the player information	
	// Takes in:		float - X Position, float - Y Position, float - Z Position
	// Returns:			Nothing
	void SetRotation(float x, float y, float z);


	// Function:		Get Position Function
	// What it does:	Sets the value of the arguments passed into the function to the current position stored in this class, through using the address of the arguments
	// Takes in:		float& - X Position, float& - Y Position, float& - Z Position
	// Returns:			Nothing
	void GetPosition(float& x, float& y, float& z);


	// Function:		Get Rotation Function
	// What it does:	Sets the value of the arguments passed into the function to the current rotation stored in this class, through using the address of the arguments
	// Takes in:		float& - X Rotation, float& - Y Rotation, float& - Z Rotation
	// Returns:			Nothing
	void GetRotation(float& x, float& y, float& z);


	// Function:		Set Frame Time Function
	// What it does:	Stores the current frame time of the application to be used to calculate movement/rotation in relation to the frame speed
	// Takes in:		float - Current Frame Time
	// Returns:			Nothing
	void SetFrameTime(float time);


	// Function:		Move Forward Function
	// What it does:	Updates the forward speed movement based upon the frame time, and updates the current position accordingly by moving along the xz plane
	// Takes in:		bool - Represents whether the key used to move forward is pressed in this frame (Up Arrow Key)
	// Returns:			Nothing
	void MoveForward(bool keydown);


	// Function:		Move Backward Function
	// What it does:	Updates the backward speed movement based upon the frame time, and updates the current position accordingly by moving along the xz plane
	// Takes in:		bool - Represents whether the key used to move backward is pressed in this frame (Down Arrow Key)
	// Returns:			Nothing
	void MoveBackward(bool keydown);
	
	
	// Function:		Move Upward Function
	// What it does:	Updates the Upward speed movement based upon the frame time, and updates the current Y position accordingly
	// Takes in:		bool - Represents whether the key used to move upward is pressed in this frame (A Key)
	// Returns:			Nothing
	void MoveUpward(bool keydown);


	// Function:		Move Downward Function
	// What it does:	Updates the downward speed movement based upon the frame time, and updates the current Y position accordingly
	// Takes in:		bool - Represents whether the key used to move downward is pressed in this frame (Z Key)
	// Returns:			Nothing
	void MoveDownward(bool);


	// Function:		Turn Left Function
	// What it does:	Updates the left rotation speed based upon the frame time, and updates the current rotation accordingly by rotating around the Y axis
	// Takes in:		bool - Represents whether the key used to turn left is pressed in this frame (Left Arrow Key)
	// Returns:			Nothing
	void TurnLeft(bool);


	// Function:		Turn Right Function
	// What it does:	Updates the right rotation speed based upon the frame time, and updates the current rotation accordingly by rotating around the Y axis
	// Takes in:		bool - Represents whether the key used to turn right is pressed in this frame (Right Arrow Key)
	// Returns:			Nothing
	void TurnRight(bool);


	// Function:		Look Up Function
	// What it does:	Updates the Look Up rotation speed based upon the frame time, and updates the rotation accordingly by rotating around the X axis
	// Takes in:		bool - Represents whether the key used to look up is pressed in this frame (Page Up Key)
	// Returns:			Nothing
	void LookUpward(bool);


	// Function:		Look Down Function
	// What it does:	Updates the Look Down rotation speed based upon the frame time, and updates the rotation accordingly by rotating around the X axis
	// Takes in:		bool - Represents whether the key used to look down is pressed in this frame (Page Down Key)
	// Returns:			Nothing
	void LookDownward(bool);

	// Public Member Variables
	// Forward movement speed and backward movement speed, used for collision detection
	float m_forwardSpeed, m_backwardSpeed;

// Private Member Variables
private:
	// Vectors holding information about the players position and rotation
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_rotation;

	// Current frame time
	float m_frameTime;

	// Speed Variables
	float m_upwardSpeed, m_downwardSpeed;
	float m_leftTurnSpeed, m_rightTurnSpeed;
	float m_lookUpSpeed, m_lookDownSpeed;
};

#endif