#include "dungeonclass.h"


// Function:		Dungeon class constructor
// What it does:	Initialises all variables and objects to their defaults to set up the class
// Takes in:		Nothing
// Returns:			Nothing

DungeonClass::DungeonClass()
{
	// Set all variables to default values to set up the class
	m_Device = 0;
	m_DeviceContext = 0;
	m_EntrancePiece = 0;
	m_RoomChance = 0;
	m_DoorDirection = 0;
	m_DoorPosition = D3DXVECTOR2(0.0f, 0.0f);
	m_DungeonDepth = 0;
	m_WidthHeight = D3DXVECTOR2(0.0f, 0.0f);
	m_NewCenter = D3DXVECTOR2(0.0f, 0.0f);
	m_PlayerPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_NextPlayerPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_PlayerLastPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_LookAt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_PlayerCanMove = true;
	m_CorridorChanceAftRoom = 0;
	m_CorridorChanceAftCorridor = 0;
	m_RoomChanceAftRoom = 0;
	m_RoomChanceAftCorridor = 0;
	m_RoomsLimit = 4.0f;
	m_PlayerMesh = 0;
	DungeonFailed = false;

	// Calculate the Grid offset
	// The Grid offset is used so that the dungeon can begin at the position (0, 0, 0) 
	// Array element IDs are positive only, therefore adding half of the grid amount will ensure the position relates to a positive grid element ID
	m_GridOffsetX = DUNGEON_GRID_X / 2;
	m_GridOffsetZ = DUNGEON_GRID_Y / 2;

	// Each cell in the grid corresponds to 0.1 on the map
	// Initialise all base grid values to 0
	for (int i = 0; i < DUNGEON_GRID_X; i ++)
	{
		for (int j = 0; j < DUNGEON_GRID_Y; j ++)
		{
			DungeonGrid[i][j] = COLLISION;
		}
	}
}


// Placeholder destructor for the dungeon class
DungeonClass::~DungeonClass()
{}


// Function:		Initialise function
// What it does:	Initialises the dungeon - Initialises first dungeon piece and ceiling piece, the player mesh, and begins calling
//					PlaceNextPiece four times with four directions of doors so that the dungeon will spread from the entrance piece
//					And will continue until the dungeon is fully created
// Takes in:		ID3D11Device pointer - Pointer to the DirectX device, ID3D11DeviceContext - Pointer to the DirectX device context
// Returns:			True if dungeon has been initialised successfully, false if not

bool DungeonClass::Initialise(ID3D11Device* Device, ID3D11DeviceContext* DeviceContext, HWND *hwnd)
{
	bool result;
	m_Device = Device;
	m_DeviceContext = DeviceContext;
	m_WindowHandle = hwnd;

	// Seed the rand generator with the current time
	srand(time(NULL));

	// Create the player mesh
	m_PlayerMesh = new MeshClass;
	if (!m_PlayerMesh)
	{
		return false;
	}

	// Initialise the player mesh
	result = m_PlayerMesh->Initialize(m_Device, m_DeviceContext, L"data/player_sprite.png", D3DXVECTOR3(0.0f, 0.0f, 0.0f), 2.0f, 2.0f, 1.0f);
	if (!result)
	{
		return false;
	}

	// Create the entrance piece (first piece of the dungeon)
	m_EntrancePiece = new DungeonPieceClass(DungeonPieceClass::ENTRANCE, 0);
	if (!m_EntrancePiece)
	{
		return false;
	}

	// Initialise the entrance piece
	result = m_EntrancePiece->Initialise(m_Device, m_DeviceContext, D3DXVECTOR2(0.0f, 0.0f), 2.0f, 2.0f, 4);
	if (!result)
	{
		return false;
	}

	// Create the first ceiling piece of the dungeon
	DungeonPieceClass *CeilingPiece = new DungeonPieceClass(DungeonPieceClass::CEILING, 0);
	if (!CeilingPiece)
	{
		return false;
	}

	// Initialise the ceiling piece
	result = CeilingPiece->Initialise(m_Device, m_DeviceContext, D3DXVECTOR2(0.0f, 0.0f), 2.0f, 2.0f, 4);
	if (!result)
	{
		return false;
	}

	// Fill the grid with the entrance piece at first
	CheckGrid(2.0f, 2.0f, D3DXVECTOR2(0.0f, 0.0f));
	// push_back entrance piece and ceiling piece as first dungeon pieces in their list
	m_DungeonPieces.push_back(m_EntrancePiece);
	m_CeilingPieces.push_back(CeilingPiece);

	// Call PlaceNextPiece for each room available in the entrance using 0 as the first depth for each one
	// This will then recursively call the same function for the room created, for the amount of times their are rooms available
	// from that new piece.
	result = PlaceNextPiece(0, m_EntrancePiece, RIGHT);
	if (!result)
	{
		return false;
	}

	result = PlaceNextPiece(0, m_EntrancePiece, UP);
	if (!result)
	{
		return false;
	}

	result = PlaceNextPiece(0, m_EntrancePiece, LEFT);
	if (!result)
	{
		return false;
	}

	result = PlaceNextPiece(0, m_EntrancePiece, DOWN);
	if (!result)
	{
		return false;
	}

	// Return true if everything is successful
	return true;
}


// Function:		Place Next Piece function
// What it does:	Loops through the number of attempts in order to find a suitable room leading off from the previous room, and initialises
//					the room and calls PlaceNextPiece again for the available rooms leading from this new room.
//					This is the main recursive function used for generating the dungeon as it calls itself based on what 
//					rooms are available in the room it creates.
// Takes in:		int - Depth value of the dungeon so that the dungeon can check how much it has generated,
//					DungeonPieceClass - pointer to the current piece that is creating a new room off it
//					int - DoorNumber value which uses defines of LEFT, RIGHT, UP, and DOWN to state the room side the next room is created from
// Returns:			Nothing

bool DungeonClass::PlaceNextPiece(int Depth, DungeonPieceClass *Piece, int DoorNumber)
{
	// Loop through this function in the amount of piece attempts to try and place a new room
	// If no rooms can be placed, skip this room
	for (int i = 0; i < PIECE_ATTEMPTS; i++)
	{
		// Create a new dungeon piece and a ceiling piece
		DungeonPieceClass* NewPiece;
		DungeonPieceClass* NewCeilingPiece;
		// Get the depth value for this call
		m_DungeonDepth = Depth;

		// Set the chances of rooms based on the current depth
		SetChances(Depth);

		// Calculate the random next piece that will be placed at the door that is passed into this function
		DungeonPieceClass::m_PieceType NewPieceType = CalculateNextPiece(Piece->GetPieceType(), DoorNumber);
		
		// Create the new dungeon piece
		NewPiece = new DungeonPieceClass(NewPieceType, DoorNumber);
		// Create the new ceiling piece
		NewCeilingPiece = new DungeonPieceClass(DungeonPieceClass::CEILING, 0);
		// Get the new randomised width and height depending on the type of room that has been chosen
		m_WidthHeight = NewPiece->GetWidthAndHeight(NewPieceType);
		// Get the new center of the room based on the direction this room is being created in, and the new width and height
		m_NewCenter = GetNextCenter(DoorNumber, Piece, m_WidthHeight.x, m_WidthHeight.y);

		// Check the grid to see if this dungeon piece can be placed, or if there is already a piece in that area
		// If the function returns true, then create this piece. Otherwise, delete the objects and try again
		if (CheckGrid(m_WidthHeight.x, m_WidthHeight.y, m_NewCenter))
		{
			// Initialise the new dungeon and ceiling pieces using the information already calculated for the room
			NewPiece->Initialise(m_Device, m_DeviceContext, m_NewCenter, m_WidthHeight.x, m_WidthHeight.y, m_RoomsLimit);
			m_DungeonPieces.push_back(NewPiece);
			NewCeilingPiece->Initialise(m_Device, m_DeviceContext, m_NewCenter, m_WidthHeight.x, m_WidthHeight.y, 0);
			m_CeilingPieces.push_back(NewCeilingPiece);

			// Update the room so that it knows which wall already has a door and which door direction is the previous room
			// so that it doesn't try to create a new room in that place
			UpdateRoom(NewPiece, DoorNumber);
		
			// Update the previous room so that it will draw a wall with a door between it and the new room
			if (DoorNumber == LEFT)
			{
				Piece->m_LDoor = true;
			}
			if (DoorNumber == RIGHT)
			{
				Piece->m_RDoor = true;
			}
			if (DoorNumber == UP)
			{
				Piece->m_UDoor = true;
			}
			if (DoorNumber == DOWN)
			{
				Piece->m_DDoor = true;
			}

			// Create the new door sprite
			DungeonPieceClass* NewDoor;

			// If the room is at the left or right of a room, set it to a vertical door and set the width and height
			if ((DoorNumber == LEFT) || (DoorNumber == RIGHT))
			{
				m_WidthHeight = D3DXVECTOR2(0.55f, 1.1f);
				m_DoorDirection = VERTICAL_DOOR;
			}
			// If the room is at the top or bottom of a room, set it to the horizontal door and set the width and height
			else if ((DoorNumber == UP) || (DoorNumber == DOWN))
			{
				m_WidthHeight = D3DXVECTOR2(1.1f, 0.55f);
				m_DoorDirection = HORIZONTAL_DOOR;
			}

			// Create the new door piece and get the new door position based on the direction this room is created in
			NewDoor = new DungeonPieceClass(DungeonPieceClass::DOOR, m_DoorDirection);
			m_DoorPosition = NewDoor->GetDoorPosition(DoorNumber, NewPiece); 

			// Initialise the new door with the position and set width and height
			NewDoor->Initialise(m_Device, m_DeviceContext, m_DoorPosition, m_WidthHeight.x, m_WidthHeight.y, 0);

			// Add this door to the list of doors to be rendered
			m_DoorPieces.push_back(NewDoor);

			// If this piece isn't at the dungeon depth limit, create new rooms for each of the available doors in this room
			if (Depth < DUNGEON_DEPTH)
			{
				// The depth passed in is the current depth of this call plus one, this is so that it will call these functions 
				// recursively until the generation reaches its maximum depth

				// Check if this room has a door available, and create the new room
				if (NewPiece->m_LAvailable)
				{
					PlaceNextPiece(Depth + 1, NewPiece, LEFT);
				}
				if (NewPiece->m_RAvailable)
				{
					PlaceNextPiece(Depth + 1, NewPiece, RIGHT);
				}
				if (NewPiece->m_DAvailable)
				{
					PlaceNextPiece(Depth + 1, NewPiece, DOWN);
				}
				if (NewPiece->m_UAvailable)
				{
					PlaceNextPiece(Depth + 1, NewPiece, UP);
				}
			}

			// Break out the for loop because a room has been successfully placed
			break;
		}
		else
		{
			if (DungeonFailed)
			{
				return false;
			}
			else
			{
				// Delete the new dungeon piece created and delete the new ceiling piece
				delete NewPiece;
				NewPiece = 0;
				delete NewCeilingPiece;
				NewCeilingPiece = 0;
				// It will then either try again if it is below the number of room attempts or it will leave this room
			}
		}

	}

	return true;
}


// Function:		Calculate Next Piece function
// What it does:	Creates a new piece type and uses a random number between 1 and 100 to decide the next room to place. 
//					The next room to place uses chances based on the previous type of room, so that the dungeon will be varied
//					and original every time. Uses the direction of the room to decide on the new piece.
// Takes in:		DungeonPieceClass::m_PieceType - ParentPiece which is the dungeon piece that is creating the new room, 
//					int - value for the door number that the room is following from
// Returns:			DungeonPieceClass::m_PieceType - Type of piece generated by dungeon so it can be used to initialise the new dungeon piece

DungeonPieceClass::m_PieceType DungeonClass::CalculateNextPiece(DungeonPieceClass::m_PieceType ParentPieceType, int DoorNo)
{
	// Check all chances and return the type of room it is

	// Create a new room piece type & for now set the new room type to the INIT value
	DungeonPieceClass::m_PieceType New_Room;
	New_Room = DungeonPieceClass::INIT;
	int RoomNumber = DoorNo;

	// Get the chance value using rand between 1 and 100
	// This is the value used in the following checks that decides what the next room should be
	m_RoomChance = 0;
	m_RoomChance = rand() % (100 - 1) + 1;

	// If the parent room is a Horizontal Corridor 
	if (ParentPieceType == DungeonPieceClass::HORIZONTAL_CORRIDOR)
	{
		// Use Corridor chance after corridor. This is usually a low number that gets lower as the depth of the dungeon increases
		// to make it less likely that a corridor is chosen after a corridor, because it makes more sense to have a room after it 
		// however there might still be times when corridors follow corridors
		if (m_RoomChance <= 0 + m_CorridorChanceAftCorridor)
		{
			New_Room = DungeonPieceClass::HORIZONTAL_CORRIDOR;
		}
		// Use room chance after corridor. This is usually a high number that gets higher as the depth of the dungeon increases
		// to make it more likely that a room is chosen after a corridor because it makes more sense to have a room after a corridor
		if (m_RoomChance > 100 - m_RoomChanceAftCorridor)
		{
			New_Room = DungeonPieceClass::ROOM;
		}
	}

	// If the parent of the room is a normal type room/entrace and the door is at the right or left side of the room
	if ((ParentPieceType == DungeonPieceClass::ENTRANCE) || (ParentPieceType == DungeonPieceClass::ROOM))
	{
		if ((RoomNumber == RIGHT) || (RoomNumber == LEFT))
		{
			// Use room chance after room for this check. This is usually a number a little less than 50% of 100 so that it is slightly less likely for a room
			// to follow a room, and a little more likely that a corridor will follow
			if (m_RoomChance <= 0 + m_RoomChanceAftRoom)
			{
				New_Room = DungeonPieceClass::ROOM;
			}
			// Use corridor chance after room for this check. This is usually a number a little over than 50% of 100 so that it is slightly more likely that a 
			// corridor will follow after a room.
			if (m_RoomChance > 100 - m_CorridorChanceAftRoom)
			{
				New_Room = DungeonPieceClass::HORIZONTAL_CORRIDOR;
			}
		}
	}

	// If the parent of the room is a normal type room/entrace and the door is at the top or bottom side of the room
	if ((ParentPieceType == DungeonPieceClass::ENTRANCE) || (ParentPieceType == DungeonPieceClass::ROOM))
	{
		if ((RoomNumber == UP) || (RoomNumber == DOWN))
		if (m_RoomChance <= m_CorridorChanceAftRoom)
		{
			New_Room = DungeonPieceClass::VERTICAL_CORRIDOR;
		}
		if (m_RoomChance > 100 - m_RoomChanceAftRoom)
		{
			New_Room = DungeonPieceClass::ROOM;
		}
	}

	// Vertical Corridors
	if (ParentPieceType == DungeonPieceClass::VERTICAL_CORRIDOR)
	{
		if (m_RoomChance <= m_CorridorChanceAftCorridor)
		{
			New_Room = DungeonPieceClass::VERTICAL_CORRIDOR;
		}
		if (m_RoomChance > 100 - m_RoomChanceAftCorridor)
		{
			New_Room = DungeonPieceClass::ROOM;
		}
	}

	// Return the new room type so that the dungeon can then create a room and check if it will fit in the dungeon
	return New_Room;
}


// Function:		Get Next Center function
// What it does:	Uses the direction the room is created in and the width/height of the new piece
//					to find the center of the room leading off of the previous one
// Takes in:		int - Direction of the room it is being created in, DungeonPieceClass - pointer to the piece that is being placed
//					float - width value of the piece being created, float - height value of the piece being created
// Returns:			D3DXVECTOR2 - The new center of the next dungeon piece so that it can be placed next to the previous piece in the correct position

D3DXVECTOR2 DungeonClass::GetNextCenter(int Direction, DungeonPieceClass* ThisPiece, float Width, float Height)
{
	// Half Width and height of This Piece
	float Half_Height = ThisPiece->m_HalfHeight;
	float Half_Width = ThisPiece->m_HalfWidth;
	D3DXVECTOR2 Center = ThisPiece->m_Center;

	// Half Width and Height of second piece
	float New_Half_Height = (Height / 2.0);
	float New_Half_Width = (Width / 2.0);
	D3DXVECTOR2 NewCenter = D3DXVECTOR2(0.0f, 0.0f);

	// Check the direction that this room will be placed in after the last room, and get the new center position
	if (Direction == LEFT)
	{
		// Takes away the new half width from the left side of the parent dungeon piece, with same y value to get new center
		NewCenter = D3DXVECTOR2(Center.x - Half_Width - New_Half_Width, Center.y); 
	}
	if (Direction == RIGHT)
	{
		// Adds new half width to the right side of the parent dungeon piece, with same y value to get new center
		NewCenter = D3DXVECTOR2(Center.x + Half_Width + New_Half_Width, Center.y); 
	}
	if (Direction == UP)
	{
		// Keeps same x value, but adds new half height to the top side of the parent dungeon piece to get new center
		NewCenter = D3DXVECTOR2(Center.x, Center.y + Half_Height + New_Half_Height); 
	}
	if (Direction == DOWN)
	{
		// Keeps same x value, but takes away new half height from the bottom of the parent dungeon piece to get new center
		NewCenter = D3DXVECTOR2(Center.x, Center.y - Half_Height - New_Half_Height); 
	}

	// Return this new center so the piece can be placed
	return NewCenter;
}


// Function:		Update Room function
// What it does:	Updates the bools of the room created so that knows not to create a new room in the door it has just been created from
//					Also update so that the room knows it already has a door at the start so it will draw a wall to accomate this
// Takes in:		DungeonPieceClass - pointer to the new dungeon piece, int - Direction value for the direction this room will be placed in
// Returns:			Nothing

void DungeonClass::UpdateRoom(DungeonPieceClass* NewPiece, int Direction)
{
	// If the room has been made to the left of the parent piece
	if (Direction == LEFT)
	{	
		// The right door of this room isn't available to create a new room because this is the parent room
		NewPiece->m_RAvailable = false;
		// There is a right door in this room so update so room knows to build a wall with a door here
		NewPiece->m_RDoor = true;
	}

	// If the room has been made to the left of the parent piece
	if (Direction == RIGHT)
	{
		// The left door of this room isn't available to create a new room because this is the parent room
		NewPiece->m_LAvailable = false;
		// There is a left door in this room so update so room knows to build a wall with a door here
		NewPiece->m_LDoor = true;
	}

	// If the room has been made to the left of the parent piece
	if (Direction == UP)
	{
		// The bottom door of this room isn't available to create a new room because this is the parent room
		NewPiece->m_DAvailable = false;
		// There is a bottom door in this room so update so the room knows to build a wall with a door here
		NewPiece->m_DDoor = true;
	}

	// If the room has been made to the left of the parent piece
	if (Direction == DOWN)
	{
		// The top door of this room isn't available to create a new room because this is the parent room
		NewPiece->m_UAvailable = false;
		// There is a top door in this room so update so the room knows to build a wall with a door here
		NewPiece->m_UDoor = true;
	}
}


// Function:		Shutdown function
// What it does:	Releases all objects when the application ends and deletes them, cleans resources
// Takes in:		Nothing
// Returns:			Nothing
void DungeonClass::Shutdown()
{
	// Release dungeon pieces by iterating through the list and deleting each
	for (auto thisObject = m_DungeonPieces.begin(); thisObject != m_DungeonPieces.end(); ++thisObject)
	{
		DungeonPieceClass* Piece = *thisObject;
		Piece->Shutdown();
		delete Piece;
	}

	if (!m_DungeonPieces.empty())
	{
		m_DungeonPieces.clear();
	}

	// Release ceiling pieces by iterating through the list and deleting each
	for (auto thisObject = m_CeilingPieces.begin(); thisObject != m_CeilingPieces.end(); ++thisObject)
	{
		DungeonPieceClass* CeilingPiece = *thisObject;
		CeilingPiece->Shutdown();
		delete CeilingPiece;
	}

	if (!m_CeilingPieces.empty())
	{
		m_CeilingPieces.clear();
	}

	// Release door pieces by iterating through the list and deleting each
	for (auto thisObject = m_DoorPieces.begin(); thisObject != m_DoorPieces.end(); ++thisObject)
	{
		DungeonPieceClass* DoorPiece = *thisObject;
		DoorPiece->Shutdown();
		delete DoorPiece;
	}

	if (!m_DoorPieces.empty())
	{
		m_DoorPieces.clear();
	}

	// Release player mesh object
	if (m_PlayerMesh)
	{
		delete m_PlayerMesh;
		m_PlayerMesh = 0;
	}
}


// Function:		Check Grid function
// What it does:	It uses the new center, width and height of the new piece and checks to see if there is room available for it on the dungeon grid
//					The value on the grid if not available will be 1, and value will be 0 if available
//					If it doesn't return false, will fill in the grid for the new room 
// Takes in:		float - Width value of new piece, float - Height value of new piece, D3DXVECTOR2 - Vector of center of new piece
// Returns:			True if the dungeon piece using this width, height and center can be placed in the dungeon grid, false if not

bool DungeonClass::CheckGrid(float Width, float Height, D3DXVECTOR2 Center)
{
	// Multiply these values by 10 to get them into the grid values
	Width = Width * 10;
	Height = Height * 10;
	float Center_X = (Center.x) * 10;
	float Center_Y = (Center.y) * 10;

	// Get the grid values needed to check if this room can be placed in the dungeon grid
	float HalfWidthLeft = floor(0.5 * Width);
	float HalfWidthRight = (0.5 * Width);
	float HalfHeight = (0.5 * Height);
	int Start_x = (Center_X - HalfWidthLeft) + m_GridOffsetX;
	int Start_y = (Center_Y - HalfHeight) + m_GridOffsetZ;
	int End_x = (Center_X + HalfWidthRight) + m_GridOffsetX;
	int End_y = (Center_Y + HalfHeight) + m_GridOffsetZ;

	// If the starting position is outside of the dungeon grid return false straight away
	if (!InBounds(Start_x, Start_y))
	{
		DungeonFailed = true;
		return false;
	}

	// Check from the start x value of the room's dimensions to the end x value
	for (int i = Start_x + 1; i < End_x - 1; i++)
	{
		// Check from the start y value of the room's dimensions to the y value
		for (int j = Start_y + 1; j < End_y - 1; j++)
		{
			// If the position in the dungeon grid returns 1, this means there is something there and the room can't be placed,
			// therefore return false
			if (InBounds(i, j))
			{
				if (DungeonGrid[i][j] == DUNGEON_ROOM)
				{
					return false;
				}
			}
			else
			{
				// Dungeon grid too small for the dungeon to generate therefore need to exit program
				DungeonFailed = true;
				return false;
			}
		}
	}

	// If not, fill in data using these values so grid knows a new piece is there
	for (int i = Start_x; i <= End_x; i++)
	{
		for (int j = Start_y; j <= End_y; j++)
		{
			if (InBounds(i, j))
			{
				// Set the value in these positions within the dimensions of the new piece to show there is now something there
				DungeonGrid[i][j] = DUNGEON_ROOM;
			}
			else
			{
				// Dungeon grid too small for the dungeon to generate therefore need to exit program
				DungeonFailed = true;
				return false;
			}
		}
	}
	
	// Return that the piece has been placed successfully
	return true;
}


// Function:		Set Chances function
// What it does:	Changes the chances available for the dungeon as the depth of the dungeon increases
//					It is called in PlaceNextPiece and changes the chance values based on the depth, to make it more likely or less likely
//					That certain types of rooms will be created closer to the end of the dungeon
// Takes in:		int - Depth value
// Returns:			Nothing

void DungeonClass::SetChances(int depth)
{
	// If the depth reaches the max dungeon depth
	if (depth == DUNGEON_DEPTH)
	{
		// Reduce the chance for a corridor to 0, so that there aren't corridors at the end of the dungeon that don't lead to a room
		m_CorridorChanceAftRoom = 0.0f;
		m_CorridorChanceAftCorridor = 0.0f;
		// Increase the chance for a corridor to 100%, so that at the end of the dungeon only rooms will be created
		m_RoomChanceAftRoom = 100.0f;
		m_RoomChanceAftCorridor = 100.0f;
		// Room limit is set to 1 so that the room won't create a new room
		m_RoomsLimit = 1.0f;
	}
	else
	{
		// Set the chance for the corridors after a room to a number just above 50% to make it more likely a corridor 
		// is made after a room
		m_CorridorChanceAftRoom = 60;

		// Set the chance for a corridor after a corridor to 10T so that it is less likely to produce two corridors in a row
		m_CorridorChanceAftCorridor = 10;

		// Set the chance for a room after a room to a number just below 50% to make it less likely a room is made 
		// after a room
		m_RoomChanceAftRoom = 40;

		// Set the chance for a room after a corridor to 90% so that it is more likely to produce a room after a corridor is placed
		m_RoomChanceAftCorridor = 90;

		// Change the rooms limit to 3 so that less rooms are available to be created than at the start of the dungeon
		m_RoomsLimit = 4.0f;
	}
	// If near the start of the dungeon, set the rooms limit to 4 so that the rooms will have more doors available at the start so
	// dungeon will branch off nicely
	if ((depth == 0) || (depth == 1))
	{
		m_RoomsLimit = 4.0f;
	}
}


// Function:		Fill Grid With Wall function
// What it does:	This is called for each wall belonging to a dungeon piece in ApplicationClass when the walls have been created so that 
//					the walls are present in the dungeon grid and can be collided with. Uses the start and end positions of the wall to fill 
//					in the information in the grid
// Takes in:		MeshClass - Pointer to the wall mesh that is passed in so that it can be checked
// Returns:			Nothing

void DungeonClass::FillGridWithWall(MeshClass *Wall)
{
	float start = 0.0f;
	float end = 0.0f;

	// If this wall is placed along the X axis, use the x values to find the start and end positions in the grid
	if (Wall->m_MeshAlongXAxis)
	{
		// Multiply the positions by 10 to get them into the grid values (Because one cell in the grid represents 0.1)
		// Add the Grid offset in order to get positions into a positive value to correspond to that element in the grid array
		start = (Wall->m_StartPos.x * 10) + m_GridOffsetX;
		end = (Wall->m_EndPos.x * 10) + m_GridOffsetX;
		// Get the fixed z position of the wall so it can be used for the grid
		int z_pos = (Wall->m_StartPos.z * 10) + m_GridOffsetZ;
		// Fill the grid so that the dungeon knows there is a collision here
		for (int i = start; i <= end; i++)
		{
			DungeonGrid[i][z_pos] = COLLISION;	
		}
	}

	// Else if this wall is placed along the z axis, use the z values to find the start and end positions int he grid
	else if (Wall->m_MeshAlongZAxis)
	{
		// Multiply the positions by 10 to get them into the grid values
		start = (Wall->m_StartPos.z * 10) + m_GridOffsetZ;
		end = (Wall->m_EndPos.z * 10) + m_GridOffsetZ;
		// Get the fixed x position of the wall so it can be used for the grid
		int z_pos = (Wall->m_StartPos.z * 10) + m_GridOffsetZ;
		int x_pos = (Wall->m_StartPos.x * 10) + m_GridOffsetX;
		for (int i = start; i <= end; i++)
		{
			// Fill the grid so that the dungeon knows there is a collision here
			DungeonGrid[x_pos][i] = COLLISION;
		}
	}
}


// Function:		Set Player Position function
// What it does:	Takes in the position of the camera and uses matrix multiplication to convert it to the
//					value it would be in the dungeon grid. This is because the dungeon has several transforms when rendered, therefore
//					Need to transform them back to normal to get them on the original grid
// Takes in:		D3DXVECTOR3 - vector for the position of the player (the camera), 
// Returns:			Nothing

void DungeonClass::SetPlayerPosition(D3DXVECTOR3 position)
{
	D3DXMATRIX rotationMatrix, scalingMatrix, worldMatrix, translationMatrix;
	m_Position = position;

	// Create a matrix of the player's co-ordinates and undo the transformations applied to the dungeon so it can be used
	// in the grid

	D3DXMATRIX coordinates = D3DXMATRIX( m_Position.x,		m_Position.y,		m_Position.z,		1.0f,
											0.0f,				1.0f,				0.0f,				0.0f,
											0.0f,				0.0f,				1.0f,				0.0f,
											0.0f,				0.0f,				0.0f,				1.0f );

	// The dungeon is scaled up, therefore scale it down by the same amount
	float scale = 1.0/DUNGEON_SCALE;

	// Carry out the opposite matrix transformations to get the players position within the map/grid
	D3DXMatrixTranslation(&translationMatrix, -10.0f, -10.0f, -10.0f);
	D3DXMatrixMultiply(&coordinates, &coordinates, &translationMatrix);
	D3DXMatrixScaling(&scalingMatrix, scale, scale, scale);
	D3DXMatrixMultiply(&coordinates, &coordinates, &scalingMatrix);

	// Set the new player position to the co-ordinates taken after this transformation
	// (Leaves y as 0.1 because it only needs to be present above the map) 
	m_PlayerPosition = D3DXVECTOR3(coordinates._11, 0.1f, coordinates._13);
}


// Function:		Can Move Player function
// What it does:	Called in the HandleInput function in ApplicationClass when moving forwards or backwards with collision on.
//					Checks the direction the player wants to move in and uses the lookat vector to check the next position the player will be in
//					if they do move in this direction. It then checks if this position is in a free space (within a room) in the dungeon grid or if
//					it is a wall or outside the dungeon.
// Takes in:		D3DXVECTOR3 - lookat vector, int - direction value to specify the direction the player wants to move in
// Returns:			True if the player can move in this direction and isn't colliding, false if it will collide

bool DungeonClass::CanMovePlayer(D3DXVECTOR3 lookat, int direction)
{
	m_LookAt = lookat;

	// If player is trying to move forward
	if (direction == FORWARD)
	{
		// Get the vector from the look at position to the players position
		m_NextPlayerPosition = m_LookAt - m_Position;
		// Normalise this vector
		D3DXVec3Normalize(&m_NextPlayerPosition, &m_NextPlayerPosition);
		// Set next player position as the original position plus half of the normalised vector from look at to position
		m_NextPlayerPosition = m_PlayerPosition + (0.1 * m_NextPlayerPosition);
	}
	else if (direction == BACKWARD)
	{
		// Get the vector from the look at position to the players position
		m_NextPlayerPosition = m_LookAt - m_Position;
		// Normalise this vector
		D3DXVec3Normalize(&m_NextPlayerPosition, &m_NextPlayerPosition);
		// Set next player position as the original position minus half of the normalised vector from look at to position
		m_NextPlayerPosition = m_PlayerPosition - (0.1 * m_NextPlayerPosition);
	}

	// Get the next player position into the grid position by multiplying by 10 and adding the grid offset
	int xpos = (m_NextPlayerPosition.x * 10) + m_GridOffsetX;
	int ypos = (m_NextPlayerPosition.z * 10) + m_GridOffsetZ;

	// Check this position in the grid. If this position is a collision, return false so that player doesn't move in that direction
	if (DungeonGrid[xpos][ypos] == COLLISION)
	{
		m_PlayerCanMove = false;
	}
	else
	{
		// Else set the players last position to its current position (this is so that player can return to the last position when turning
		// collision on/off) and set player can move to true so player moves in this direction
		m_PlayerLastPosition = m_Position;
		m_PlayerCanMove = true;
	}

	// Return the bool 
	return m_PlayerCanMove;
}



// Function:		In Bounds function
// What it does:	Checks that the given position is within the dungeon grid before trying to access this element of the grid array.
//					This is to protect against the application accessing illegal data
// Takes in:		int - X Position on the grid, int - Y Position on the grid
// Returns:			bool - True or false to allow the application to continue without errors

bool DungeonClass::InBounds(int x, int y)
{
	if ((x < 0) || (y < 0))
	{
		return false;
	}
	if ((x >= DUNGEON_GRID_X) || (y >= DUNGEON_GRID_Y))
	{
		return false;
	}

	return true;
}


// Function:		Get Dungeon Piece List function
// What it does:	Called in the Application Class so as to initialise the dungeons walls and doors for each dungeon piece, 
//					and also so that the app can render each Dungeon Piece
// Takes in:		Nothing
// Returns:			A pointer to the Dungeon Piece Class Pointer list so that the list may be iterated through 

list<DungeonPieceClass*> *DungeonClass::GetDungeonPieces()
{
	return &m_DungeonPieces;
}


// Function:		Get Ceiling Piece List function
// What it does:	Called in the Application Class so as to render the Ceiling pieces in the dungeon
// Takes in:		Nothing
// Returns:			A pointer to the Ceiling Piece Class Pointer list

list<DungeonPieceClass*> *DungeonClass::GetCeilingPieces()
{
	return &m_CeilingPieces;
}


// Function:		Get Door Piece List function
// What it does:	Called in the Application Class so as to render the Door pieces on the map
// Takes in:		Nothing
// Returns:			A pointer to the Door Piece Class Pointer list

list<DungeonPieceClass*> *DungeonClass::GetDoorPieces()
{
	return &m_DoorPieces;
}


// Function:		Get Player Mesh function
// What it does:	Called in the Application Class to render the player mesh arrow
// Takes in:		Nothing
// Returns:			A pointer to the Player Mesh object

MeshClass *DungeonClass::GetPlayerMesh()
{
	return m_PlayerMesh;
}


// Function:		Get Player Position function
// What it does:	Called in the Application class to position the player mesh on the map
// Takes in:		Nothing
// Returns:			The D3DXVECTOR3 Position of the player on the map

D3DXVECTOR3 DungeonClass::GetPlayerPosition()
{
	return m_PlayerPosition;
}



// Function:		Get Last Player Position Function
// What it does:	Called in the Application Class and used to detect collision between the player and walls
// Takes in:		Nothing
// Returns:			The previous D3DXVECTOR3 Position of the player 

D3DXVECTOR3 DungeonClass::GetPlayerLastPos()
{
	return m_PlayerLastPosition;
}