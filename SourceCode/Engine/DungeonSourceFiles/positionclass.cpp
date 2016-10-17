////////////////////////////////////////////////////////////////////////////////
// Filename: positionclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "positionclass.h"


// Function:		Position Class Constructor
// What it does:	Initialises all variables to their default values
// Takes in:		Nothing
// Returns:			Nothing

PositionClass::PositionClass()
{
	m_position.x = 0.0f;
	m_position.y = 0.0f;
	m_position.z = 0.0f;
	
	m_rotation.x = 0.0f;
	m_rotation.y = 0.0f;
	m_rotation.z = 0.0f;

	m_frameTime = 0.0f;

	m_forwardSpeed   = 0.0f;
	m_backwardSpeed  = 0.0f;
	m_upwardSpeed    = 0.0f;
	m_downwardSpeed  = 0.0f;
	m_leftTurnSpeed  = 0.0f;
	m_rightTurnSpeed = 0.0f;
	m_lookUpSpeed    = 0.0f;
	m_lookDownSpeed  = 0.0f;
}

// Placeholder constructor and destructor functions
PositionClass::PositionClass(const PositionClass& other)
{}
PositionClass::~PositionClass()
{}


// Function:		Set Position Function
// What it does:	Adjusts the current position values to the values passed into the function to update the player information
// Takes in:		float - X Position, float - Y Position, Z - Position
// Returns:			Nothing

void PositionClass::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
	return;
}


// Function:		Set Rotation Function
// What it does:	Adjusts the current rotation values to the values passed into the function to update the player information	
// Takes in:		float - X Position, float - Y Position, float - Z Position
// Returns:			Nothing
void PositionClass::SetRotation(float x, float y, float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
	return;
}


// Function:		Get Position Function
// What it does:	Sets the value of the arguments passed into the function to the current position stored in this class, through using the address of the arguments
// Takes in:		float& - X Position, float& - Y Position, float& - Z Position
// Returns:			Nothing

void PositionClass::GetPosition(float& x, float& y, float& z)
{
	x = m_position.x;
	y = m_position.y;
	z = m_position.z;
	return;
}


// Function:		Get Rotation Function
// What it does:	Sets the value of the arguments passed into the function to the current rotation stored in this class, through using the address of the arguments
// Takes in:		float& - X Rotation, float& - Y Rotation, float& - Z Rotation
// Returns:			Nothing

void PositionClass::GetRotation(float& x, float& y, float& z)
{
	x = m_rotation.x;
	y = m_rotation.y;
	z = m_rotation.z;
	return;
}


// Function:		Set Frame Time Function
// What it does:	Stores the current frame time of the application to be used to calculate movement/rotation in relation to the frame speed
// Takes in:		float - Current Frame Time
// Returns:			Nothing

void PositionClass::SetFrameTime(float time)
{
	m_frameTime = time;
	return;
}


// Function:		Move Forward Function
// What it does:	Updates the forward speed movement based upon the frame time, and updates the current position accordingly by moving along the xz plane
// Takes in:		bool - Represents whether the key used to move forward is pressed in this frame (Up Arrow Key)
// Returns:			Nothing

void PositionClass::MoveForward(bool keydown)
{
	float radians;


	// Update the forward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		// Accellerate the forward speed
		m_forwardSpeed += m_frameTime * 0.001f;

		if(m_forwardSpeed > (m_frameTime * 0.03f)) // At Max Speed
		{
			m_forwardSpeed = m_frameTime * 0.03f;
		}
	}
	else
	{
		// Deccellerate the forward speed
		m_forwardSpeed -= m_frameTime * 0.0007f;

		if(m_forwardSpeed < 0.0f) // At Min Speed
		{
			m_forwardSpeed = 0.0f;
		}
	}

	// Convert degrees to radians.
	radians = m_rotation.y * 0.0174532925f;

	// Update the position.
	m_position.x += sinf(radians) * m_forwardSpeed; // X Plane
	m_position.z += cosf(radians) * m_forwardSpeed; // Z Plane

	return;
}


// Function:		Move Backward Function
// What it does:	Updates the backward speed movement based upon the frame time, and updates the current position accordingly by moving along the xz plane
// Takes in:		bool - Represents whether the key used to move backward is pressed in this frame (Down Arrow Key)
// Returns:			Nothing

void PositionClass::MoveBackward(bool keydown)
{
	float radians;


	// Update the backward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		// Accellerate the backward speed
		m_backwardSpeed += m_frameTime * 0.001f;

		if(m_backwardSpeed > (m_frameTime * 0.03f)) // At Max Speed
		{
			m_backwardSpeed = m_frameTime * 0.03f;
		}
	}
	else
	{
		// Deccellerate the backward speed
		m_backwardSpeed -= m_frameTime * 0.0007f;
		
		if(m_backwardSpeed < 0.0f) // At Min Speed
		{
			m_backwardSpeed = 0.0f;
		}
	}

	// Convert degrees to radians.
	radians = m_rotation.y * 0.0174532925f;

	// Update the position.
	m_position.x -= sinf(radians) * m_backwardSpeed;
	m_position.z -= cosf(radians) * m_backwardSpeed;

	return;
}


// Function:		Move Upward Function
// What it does:	Updates the Upward speed movement based upon the frame time, and updates the current Y position accordingly
// Takes in:		bool - Represents whether the key used to move upward is pressed in this frame (A Key)
// Returns:			Nothing

void PositionClass::MoveUpward(bool keydown)
{
	// Update the upward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_upwardSpeed += m_frameTime * 0.003f;

		if(m_upwardSpeed > (m_frameTime * 0.03f))
		{
			m_upwardSpeed = m_frameTime * 0.03f;
		}
	}
	else
	{
		m_upwardSpeed -= m_frameTime * 0.002f;

		if(m_upwardSpeed < 0.0f)
		{
			m_upwardSpeed = 0.0f;
		}
	}

	// Update the height position.
	m_position.y += m_upwardSpeed;

	return;
}


// Function:		Move Downward Function
// What it does:	Updates the downward speed movement based upon the frame time, and updates the current Y position accordingly
// Takes in:		bool - Represents whether the key used to move downward is pressed in this frame (Z Key)
// Returns:			Nothing

void PositionClass::MoveDownward(bool keydown)
{
	// Update the downward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_downwardSpeed += m_frameTime * 0.003f;

		if(m_downwardSpeed > (m_frameTime * 0.03f))
		{
			m_downwardSpeed = m_frameTime * 0.03f;
		}
	}
	else
	{
		m_downwardSpeed -= m_frameTime * 0.002f;

		if(m_downwardSpeed < 0.0f)
		{
			m_downwardSpeed = 0.0f;
		}
	}

	// Update the height position.
	m_position.y -= m_downwardSpeed;

	return;
}


// Function:		Turn Left Function
// What it does:	Updates the left speed movement based upon the frame time, and updates the current rotation accordingly by rotating around the Y axis
// Takes in:		bool - Represents whether the key used to turn left is pressed in this frame (Left Arrow Key)
// Returns:			Nothing

void PositionClass::TurnLeft(bool keydown)
{
	// Update the left turn speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_leftTurnSpeed += m_frameTime * 0.01f;

		if(m_leftTurnSpeed > (m_frameTime * 0.15f))
		{
			m_leftTurnSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_leftTurnSpeed -= m_frameTime* 0.005f;

		if(m_leftTurnSpeed < 0.0f)
		{
			m_leftTurnSpeed = 0.0f;
		}
	}

	// Update the rotation.
	m_rotation.y -= m_leftTurnSpeed;

	// Keep the rotation in the 0 to 360 range.
	if(m_rotation.y < 0.0f)
	{
		m_rotation.y += 360.0f;
	}

	return;
}


// Function:		Turn Right Function
// What it does:	Updates the right speed movement based upon the frame time, and updates the current rotation accordingly by rotating around the Y axis
// Takes in:		bool - Represents whether the key used to turn right is pressed in this frame (Right Arrow Key)
// Returns:			Nothing

void PositionClass::TurnRight(bool keydown)
{
	// Update the right turn speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_rightTurnSpeed += m_frameTime * 0.01f;

		if(m_rightTurnSpeed > (m_frameTime * 0.15f))
		{
			m_rightTurnSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_rightTurnSpeed -= m_frameTime* 0.005f;

		if(m_rightTurnSpeed < 0.0f)
		{
			m_rightTurnSpeed = 0.0f;
		}
	}

	// Update the rotation.
	m_rotation.y += m_rightTurnSpeed;

	// Keep the rotation in the 0 to 360 range.
	if(m_rotation.y > 360.0f)
	{
		m_rotation.y -= 360.0f;
	}

	return;
}


// Function:		Look Up Function
// What it does:	Updates the Look Up rotation speed based upon the frame time, and updates the rotation accordingly by rotating around the X axis
// Takes in:		bool - Represents whether the key used to look up is pressed in this frame (Page Up Key)
// Returns:			Nothing

void PositionClass::LookUpward(bool keydown)
{
	// Update the upward rotation speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_lookUpSpeed += m_frameTime * 0.01f;

		if(m_lookUpSpeed > (m_frameTime * 0.15f))
		{
			m_lookUpSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_lookUpSpeed -= m_frameTime* 0.005f;

		if(m_lookUpSpeed < 0.0f)
		{
			m_lookUpSpeed = 0.0f;
		}
	}

	// Update the rotation.
	m_rotation.x -= m_lookUpSpeed;

	// Keep the rotation maximum 90 degrees.
	if(m_rotation.x > 90.0f)
	{
		m_rotation.x = 90.0f;
	}

	return;
}


// Function:		Look Down Function
// What it does:	Updates the Look Down rotation speed based upon the frame time, and updates the rotation accordingly by rotating around the X axis
// Takes in:		bool - Represents whether the key used to look down is pressed in this frame (Page Down Key)
// Returns:			Nothing

void PositionClass::LookDownward(bool keydown)
{
	// Update the downward rotation speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_lookDownSpeed += m_frameTime * 0.01f;

		if(m_lookDownSpeed > (m_frameTime * 0.15f))
		{
			m_lookDownSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_lookDownSpeed -= m_frameTime* 0.005f;

		if(m_lookDownSpeed < 0.0f)
		{
			m_lookDownSpeed = 0.0f;
		}
	}

	// Update the rotation.
	m_rotation.x += m_lookDownSpeed;

	// Keep the rotation maximum 90 degrees.
	if(m_rotation.x < -90.0f)
	{
		m_rotation.x = -90.0f;
	}

	return;
}