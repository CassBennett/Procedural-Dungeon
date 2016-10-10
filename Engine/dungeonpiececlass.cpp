#include "dungeonpiececlass.h"


// Function:		Dungeon Piece Constructor
// What it does:	Sets type of piece to the passed in type and the door direction to the passed in direction
//					Initialises all other members and objects to their default values to set up for the class	
// Takes in:		m_PieceType - the type of dungeon piece this instance is, int - Defined int for the direction of the room it is 
//					being placed in.
// Returns:			Nothing

DungeonPieceClass::DungeonPieceClass(m_PieceType Type, int Direction)
{
	// Set the type of the piece to the  type passed in and the door direction
	m_TypeOfPiece = Type;
	m_DoorDirection = Direction;

	// Initialise all variables to their default values/settings
	m_ExitsAmount = 0;
	m_Width = 0.0f;
	m_HalfWidth = 0.0f;
	m_Height = 0.0f;
	m_HalfHeight = 0.0f;
	m_Center = D3DXVECTOR2(0.0f, 0.0f);
	m_LowerSizeLimit = D3DXVECTOR2(0.0f, 0.0f);
	m_UpperSizeLimit = D3DXVECTOR2(0.0f, 0.0f);
	m_Device = 0;
	m_DeviceContext = 0;
	m_VertexBuffer = 0;
	m_IndexBuffer = 0;
	m_VertexCount = 0;
	m_IndexCount = 0;
	m_MapTexture = 0;
	m_DungeonTexture = 0;
	m_TextureScale = D3DXVECTOR2(1.0f, 1.0f);
	m_Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	WallsGenerated = false;
	m_LAvailable = false;
	m_RAvailable = false;
	m_UAvailable = false;
	m_DAvailable = false;
	m_LDoor = false;
	m_RDoor = false;
	m_UDoor = false;
	m_DDoor = false;

	// Sets the door leading to the parent room to true, so that in MakeRoom function
	// it doesn't try to create a door here because it's already available
	if (Direction == RIGHT)
	{
		m_LAvailable = true;
	}
	if (Direction == LEFT)
	{
		m_RAvailable = true;
	}
	if (Direction == UP)
	{
		m_DAvailable = true;
	}
	if (Direction == DOWN)
	{
		m_UAvailable = true;
	}

	// Set the size limit of the room based on the type of room it is so this info can be used to 
	// get a random height/width in GetWidthAndHeight function
	SetSizeLimits();
}


// Placeholder destructor
DungeonPieceClass::~DungeonPieceClass()
{
}


// Function:		Initialise function
// What it does:	Initialises the variables of the dungeon piece to the values passed in, and creates rooms based on the type of piece
//					the dungeon piece was set to in the constructor, using the room limit passed into this function
//					Calls the appropriate function for creating the room, and initialises for rendering
// Takes in:		ID3D11Device - pointer to DirectX device, ID3D11DeviceContext - Pointer to DirectX device context,
//					D3DXVECTOR2 - center of this piece, float - Width of this piece, float - Height of this piece
//					int - The door limit for the amount of rooms that can be created leading from this room
// Returns:			True if all initialisation has happened successfully, false if not

bool DungeonPieceClass::Initialise(ID3D11Device* Device, ID3D11DeviceContext* DeviceContext, D3DXVECTOR2 Center, float Width, float Height, int RoomLimit)
{
	bool result;		// For error checking
	
	// Assign values passed in to variables so they can be used throughout the 
	m_Center = Center;
	m_Width = Width;
	m_HalfWidth = (m_Width / 2);
	m_Height = Height;
	m_HalfHeight = (m_Height / 2);
	// Takes away 1 because when passed in RoomLimit = 4, will try and create a room for the previous room where it branched from and fail
	m_RoomsLimit = RoomLimit - 1;

	// So these can be used in the mesh initialisation function without having to pass the device and context through
	m_Device = Device;
	if (!m_Device)
	{
		return false;
	}

	m_DeviceContext = DeviceContext;
	if (!m_DeviceContext)
	{
		return false;
	}

	// If the type of room is a normal room, find a random number between 1 and the room limit 
	// to decide the amount of rooms this room can have
	if (m_TypeOfPiece == ROOM)
	{
		if (m_RoomsLimit != 0)	// If there are exits to create
		{
			// Use rand to find a random number between 1 and the room limit passed into this function
			int NumberOfDoors = rand() % m_RoomsLimit + 1;
			// Create the room using this amount of doors to randomly find which direction they will be in
			result = MakeRoom(NumberOfDoors);
		}
		else					// If there are no exits to create
		{
			// Create the room but don't generate any exits
			result = MakeRoom(m_RoomsLimit);
		}
	}

	// If the type of piece is a door, make a normal door sprite based on which door it is
	if (m_TypeOfPiece == DOOR)
	{
		result = MakeDoor();
	}

	// If the type of room is the entrance, use the normal MakeRoom function but use the room limit 4
	// so that it creates four doors from the entrance
	if (m_TypeOfPiece == ENTRANCE)
	{
		result = MakeRoom(4);
	}

	// If the type of room is a vertical corridor, create a vertical corridor 
	if (m_TypeOfPiece == VERTICAL_CORRIDOR)
	{
		result = MakeVerticalCorridor();
	}

	// If the type of room is a horizontal corridor, create a horizontal corridor
	if (m_TypeOfPiece == HORIZONTAL_CORRIDOR)
	{
		result = MakeHorizontalCorridor();
	}

	// If the type of room is a ceiling, create the ceiling sprite (drawn anti clockwise so that it shows above the player)
	if (m_TypeOfPiece == CEILING)
	{
		result = MakeCeiling();
	}

	// If the room creation fails, return false as this is an error
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffer that hold the geometry for the dungeon piece
	result = InitializeBuffers();
	if (!result)
	{
		return false;
	}

	// Return true if there have been no initialisation errors
	return true;
}


// Function:		Get Piece Type function
// What it does:	Returns the type of piece this dungeon piece is
// Takes in:		Nothing
// Returns:			m_PieceType - type of piece

DungeonPieceClass::m_PieceType DungeonPieceClass::GetPieceType()
{
	// Return this piece's type
	return m_TypeOfPiece;
}


// Function:		Make Room function 
// What it does:	Loops through the number of rooms passed in, to decide the directions available for
//					the new rooms randomly. Checks to see if a door isn't already in this place, then makes 
//					a door in that place up until the room limit count. Also loads in the textures for the rooms and 
//					sets the texture scale for normal rooms.
// Takes in:		int - Number of rooms limit so that room only creates this amount of doors
// Returns:			True if function has completed properly, false if not

bool DungeonPieceClass::MakeRoom(int NumOfRooms)
{
	bool result;		// For error checking

	int RoomCount = 0;

	// While the exits created have not reached the number of exits to create
	while (RoomCount != NumOfRooms)
	{
		// For the direction of the room, find a random number between 1 and 4 (which corresponds to directions defined in the .h)
		int direction = rand() % 4 + 1;

		// If the random direction is north
		if (direction == NORTH_DOOR)
		{
			// If there currently isn't a north room
			if (!m_UAvailable)
			{
				// Set this to true so that a room can be created in the north direction
				m_UAvailable = true;
				// Add to the room count so it knows an exit is created
				RoomCount ++;
			}
		}

		// If the random direction is south
		if (direction == SOUTH_DOOR)
		{
			// If there isn't currently a south room 
			if (!m_DAvailable)
			{
				// Set this to true so that a room can be created in the south direction
				m_DAvailable = true;
				// Add to the room count so it knows an exit is created
				RoomCount ++;
			}
		}

		// If the random direction is east
		if (direction == EAST_DOOR)
		{
			// If there isn't currently an east room
			if (!m_RAvailable)
			{
				// Set this to true so that a room can be created in the east direction
				m_RAvailable = true;
				// Add to the room count so it knows an exit is created
				RoomCount ++;
			}
		}

		// If the random direction is west
		if (direction == WEST_DOOR)
		{
			if (!m_LAvailable)
			{
				// Set this to true so that a room can be created in the west direction
				m_LAvailable = true;
				// Add to the room count so it knows an exit is created
				RoomCount ++;
			}
		}
	}

	// When placing a new piece in the dungeon, it checks to see which door directions are available, so the doors
	// that are set above will be used to create a new room 

	// Load the texture for the room piece
	if (m_TypeOfPiece == ROOM)
	{
		// Loads in the texture used for the map and for the actual dungeon
		result = LoadTextures(L"data/room.png", L"data/wooden_floors.png");
	}
	else if (m_TypeOfPiece == ENTRANCE)
	{
		result = LoadTextures(L"data/entrance.png", L"data/wooden_floors.png");
	}
	if (!result)
	{
		return false;
	}

	// Set the texture scale for the floor of the rooms so that the texture repeats twice in both u and v directions
	m_TextureScale = D3DXVECTOR2(1.0f, 1.0f);

	// Return true if there were no errors
	return true;		
}


// Function:		Make Door function
// What it does:	Loads in the textures used for the door depending on if the door is horizontal or vertical			
// Takes in:		Nothing
// Returns:			True if textures loaded properly, false if not

bool DungeonPieceClass::MakeDoor()
{
	bool result;		// For error checking

	// Load the textures for each type of door
	if (m_DoorDirection == HORIZONTAL_DOOR)
	{
		result = LoadTextures(L"data/horizontal_door.png", L"data/wooden_floors.png");
	}
	if (m_DoorDirection == VERTICAL_DOOR)
	{
		result = LoadTextures(L"data/vertical_door.png", L"data/wooden_floors.png");
	}

	// If texture loading wasn't successful, return false
	if (!result)
	{
		return false;
	}

	// Set the texture scale so that texture doesn't repeat
	m_TextureScale = D3DXVECTOR2(1.0f, 1.0f);

	// Return true if there were no errors
	return true;
}


// Function:		Make Horizontal Corridor
// What it does:	Sets left and right doors available to true, because it is a horizontal corridor
//					and loads the texture
// Takes in:		Nothing
// Returns:			True if textures loaded properly, false if not
	
bool DungeonPieceClass::MakeHorizontalCorridor()
{
	bool result;		// For error checking

	// Make left and right doors available here because it's a horizontal corridor
	m_LAvailable = true;
	m_RAvailable = true;

	// Load the texture for this corridor
	result = LoadTextures(L"data/room.png", L"data/wooden_floors.png");
	
	// If texture loading wasn't successful, return false
	if (!result)
	{
		return false;
	}

	// Set the texture scale so that texture only repeats in the x axis 
	m_TextureScale = D3DXVECTOR2(2.0f, 1.0f);

	return true;		// No errors
}


// Function:		Make Vertical Corridor
// What it does:	Sets up and down doors available to true, because it is a vertical corridor
//					and loads the texture
// Takes in:		Nothing
// Returns:			True if textures loaded properly, false if not

bool DungeonPieceClass::MakeVerticalCorridor()
{
	bool result;		// For error checking

	// Make up and down doors available here because it's a vertical corridor
	m_UAvailable = true;
	m_DAvailable = true;

	// Load the texture for this corridor
	result = LoadTextures(L"data/room.png", L"data/wooden_floors.png");
	
	// If texture loading wasn't successful, return false
	if(!result)
	{
		return false;
	}

	// Set the texture scale so that texture only repeats in the y axis 
	m_TextureScale = D3DXVECTOR2(1.0f, 2.0f);

	return true;		// No errors
}


bool DungeonPieceClass::MakeCeiling()
{
	bool result;		// For error checking

	// Load the texture for the ceiling piece
	result = LoadTextures(L"data/room.png", L"data/wooden_floors.png");

	// If texture loading wasn't successful, return false
	if (!result)
	{
		return false;
	}

	// Set the texture scale so that it repeats in both the x and y axis twice
	m_TextureScale = D3DXVECTOR2(2.0f, 2.0f);

	return true;		// No errors
}


// Function:		Build Walls function
// What it does:	Creates the meshes used for the walls, setting up two seperate walls with a space between
//					for a wall with a door in the middle, and a single wall if a door is not present in that wall
//					This function is called after all of the dungeon pieces have been created in the Application Class
//					to make sure the walls are only created after all rooms and doors have been set 	
// Takes in:		Nothing
// Returns:			Nothing

void DungeonPieceClass::BuildWalls()
{
	// Set the texture scales for walls without doors, and walls with doors
	D3DXVECTOR2 DoorTextureScale = D3DXVECTOR2(1.0f, 2.0f);
	D3DXVECTOR2 WallTextureScale = D3DXVECTOR2(2.0f, 2.0f);
	
	D3DXVECTOR3 Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// Wall Initialisations

	// North Wall
	Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);		// This is the normal for north walls
	// If the north wall has a door, create two walls with a space between them for the door and add them to the wall list
	if (m_UDoor)
	{
		MeshClass *NorthLeftWall = new MeshClass();
		NorthLeftWall->Initialize(m_Device, m_DeviceContext, L"data/stone_walls.png",	D3DXVECTOR3(m_Center.x - m_HalfWidth, 0.0f, m_Center.y + m_HalfHeight),	// Bottom Left
																						D3DXVECTOR3(m_Center.x - m_HalfWidth, 1.5f, m_Center.y + m_HalfHeight), // Top Left
																						D3DXVECTOR3(m_Center.x - 0.3f, 1.5f, m_Center.y + m_HalfHeight),		// Top Right
																						D3DXVECTOR3(m_Center.x - 0.3f, 0.0f, m_Center.y + m_HalfHeight),		// Bottom Right
																						Normal, DoorTextureScale);
		Walls.push_back(NorthLeftWall);


		MeshClass *NorthRightWall = new MeshClass();
		NorthRightWall->Initialize(m_Device, m_DeviceContext, L"data/stone_walls.png",	D3DXVECTOR3(m_Center.x + 0.3f, 0.0f, m_Center.y + m_HalfHeight),		// Bottom Left
																						D3DXVECTOR3(m_Center.x + 0.3f, 1.5f, m_Center.y + m_HalfHeight),		// Top Left
																						D3DXVECTOR3(m_Center.x + m_HalfWidth, 1.5f, m_Center.y + m_HalfHeight),	// Top Right
																						D3DXVECTOR3(m_Center.x + m_HalfWidth, 0.0f, m_Center.y + m_HalfHeight),	// Bottom Right
																						Normal, DoorTextureScale);
		Walls.push_back(NorthRightWall);
	}
	// Else create a single wall the width of the room in this position and add it to the walls list
	else
	{
		MeshClass *NorthWall = new MeshClass();
		NorthWall->Initialize(m_Device, m_DeviceContext, L"data/stone_walls.png",	D3DXVECTOR3(m_Center.x - m_HalfWidth, 0.0f, m_Center.y + m_HalfHeight),		// Bottom Left
																					D3DXVECTOR3(m_Center.x - m_HalfWidth, 1.5f, m_Center.y + m_HalfHeight),		// Top Left
																					D3DXVECTOR3(m_Center.x + m_HalfWidth, 1.5f, m_Center.y + m_HalfHeight),		// Top Right
																					D3DXVECTOR3(m_Center.x + m_HalfWidth, 0.0f, m_Center.y + m_HalfHeight),		// Bottom Right
																					Normal, WallTextureScale);
		Walls.push_back(NorthWall);
	}

	// East wall
	Normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);		// This is the normal for east walls
	// If the east wall has a door, create two walls with a space between them for the door and add them to the wall list
	if (m_RDoor)
	{
		MeshClass *EastLeftWall = new MeshClass();

		EastLeftWall->Initialize(m_Device, m_DeviceContext, L"data/stone_walls.png",	D3DXVECTOR3(m_Center.x + m_HalfWidth, 0.0f, m_Center.y + m_HalfHeight),	// Bottom Left
																						D3DXVECTOR3(m_Center.x + m_HalfWidth, 1.5f, m_Center.y + m_HalfHeight),	// Top Left
																						D3DXVECTOR3(m_Center.x + m_HalfWidth, 1.5f, m_Center.y + 0.3f),			// Top Right
																						D3DXVECTOR3(m_Center.x + m_HalfWidth, 0.0f, m_Center.y + 0.3f),			// Bottom Right
																						Normal, DoorTextureScale);	
		Walls.push_back(EastLeftWall);

		MeshClass *EastRightWall = new MeshClass();

		EastRightWall->Initialize(m_Device, m_DeviceContext, L"data/stone_walls.png",	D3DXVECTOR3(m_Center.x + m_HalfWidth, 0.0f, m_Center.y - 0.3f),			// Bottom Left
																						D3DXVECTOR3(m_Center.x + m_HalfWidth, 1.5f, m_Center.y - 0.3f),			// Top Left
																						D3DXVECTOR3(m_Center.x + m_HalfWidth, 1.5f, m_Center.y - m_HalfHeight),	// Top Right
																						D3DXVECTOR3(m_Center.x + m_HalfWidth, 0.0f, m_Center.y - m_HalfHeight),	// Bottom Right
																						Normal, DoorTextureScale);
		Walls.push_back(EastRightWall);
	}
	// Else create a single wall the height of the room in this position and add it to the walls list
	else
	{
		MeshClass *EastWall = new MeshClass();

		EastWall->Initialize(m_Device, m_DeviceContext, L"data/stone_walls.png",	D3DXVECTOR3(m_Center.x + m_HalfWidth, 0.0f, m_Center.y + m_HalfHeight),		// Bottom Left
																					D3DXVECTOR3(m_Center.x + m_HalfWidth, 1.5f, m_Center.y + m_HalfHeight),		// Top Left
																					D3DXVECTOR3(m_Center.x + m_HalfWidth, 1.5f, m_Center.y - m_HalfHeight),		// Top Right
																					D3DXVECTOR3(m_Center.x + m_HalfWidth, 0.0f, m_Center.y - m_HalfHeight),		// Bottom Right
																					Normal, WallTextureScale);
		Walls.push_back(EastWall);
	}

	// South wall
	Normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);			// This is the normal for south walls
	// If the south wall has a door, create two walls with a space between them for the door and add them to the wall list
	if (m_DDoor)
	{
		MeshClass *SouthLeftWall = new MeshClass();
		SouthLeftWall->Initialize(m_Device, m_DeviceContext, L"data/stone_walls.png",	D3DXVECTOR3(m_Center.x + m_HalfWidth, 0.0f, m_Center.y - m_HalfHeight),	// Bottom Left
																						D3DXVECTOR3(m_Center.x + m_HalfWidth, 1.5f, m_Center.y - m_HalfHeight),	// Top Left
																						D3DXVECTOR3(m_Center.x + 0.3f, 1.5f,  m_Center.y - m_HalfHeight),		// Top Right
																						D3DXVECTOR3(m_Center.x + 0.3f, 0.0f, m_Center.y - m_HalfHeight),		// Bottom Right
																						Normal, DoorTextureScale);	

		Walls.push_back(SouthLeftWall);

		MeshClass *SouthRightWall = new MeshClass();
		SouthRightWall->Initialize(m_Device, m_DeviceContext, L"data/stone_walls.png",	D3DXVECTOR3(m_Center.x - 0.3f, 0.0f, m_Center.y - m_HalfHeight),		// Bottom Left
																						D3DXVECTOR3(m_Center.x - 0.3f, 1.5f, m_Center.y - m_HalfHeight),		// Top Left
																						D3DXVECTOR3(m_Center.x - m_HalfWidth, 1.5f, m_Center.y - m_HalfHeight),	// Top Right
																						D3DXVECTOR3(m_Center.x - m_HalfWidth, 0.0f, m_Center.y - m_HalfHeight),	// Bottom Right
																						Normal, DoorTextureScale);

		Walls.push_back(SouthRightWall);
	}
	// Else create a single wall the width of the room in this position and add it to the walls list
	else
	{
		MeshClass *SouthWall = new MeshClass();

		SouthWall->Initialize(m_Device, m_DeviceContext, L"data/stone_walls.png",	D3DXVECTOR3(m_Center.x + m_HalfWidth, 0.0f, m_Center.y - m_HalfHeight),		// Bottom Left
																					D3DXVECTOR3(m_Center.x + m_HalfWidth, 1.5f, m_Center.y - m_HalfHeight),		// Top Left
																					D3DXVECTOR3(m_Center.x - m_HalfWidth, 1.5f, m_Center.y - m_HalfHeight),		// Top Right
																					D3DXVECTOR3(m_Center.x - m_HalfWidth, 0.0f, m_Center.y - m_HalfHeight),		// Bottom Right
																					Normal, WallTextureScale);

		Walls.push_back(SouthWall);
	}

	// West wall
	Normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);			// This is the normal for west walls
	// If the west wall has a door, create two walls with a space between them for the door and add them to the wall list
	if (m_LDoor)
	{
		MeshClass *WestLeftWall = new MeshClass();
		WestLeftWall->Initialize(m_Device, m_DeviceContext, L"data/stone_walls.png",	D3DXVECTOR3(m_Center.x - m_HalfWidth, 0.0f, m_Center.y - m_HalfHeight),	// Bottom Left
																						D3DXVECTOR3(m_Center.x - m_HalfWidth, 1.5f, m_Center.y - m_HalfHeight),	// Top Left
																						D3DXVECTOR3(m_Center.x - m_HalfWidth, 1.5f, m_Center.y - 0.3f),			// Top Right
																						D3DXVECTOR3(m_Center.x - m_HalfWidth, 0.0f, m_Center.y - 0.3f),			// Bottom Right
																						Normal, DoorTextureScale);

		Walls.push_back(WestLeftWall);

		MeshClass *WestRightWall = new MeshClass();
		WestRightWall->Initialize(m_Device, m_DeviceContext, L"data/stone_walls.png",	D3DXVECTOR3(m_Center.x - m_HalfWidth, 0.0f, m_Center.y + 0.3f),			// Bottom Left
																				D3DXVECTOR3(m_Center.x - m_HalfWidth, 1.5f, m_Center.y + 0.3f),					// Top Left
																				D3DXVECTOR3(m_Center.x - m_HalfWidth, 1.5f, m_Center.y + m_HalfHeight),			// Top Right
																				D3DXVECTOR3(m_Center.x - m_HalfWidth, 0.0f, m_Center.y + m_HalfHeight),			// Bottom Right
																				Normal, DoorTextureScale);

		Walls.push_back(WestRightWall);
	}
	// Else create a single wall the height of the room in this position and add it to the walls list
	else
	{
		MeshClass *WestWall = new MeshClass();

		WestWall->Initialize(m_Device, m_DeviceContext, L"data/stone_walls.png",	D3DXVECTOR3(m_Center.x - m_HalfWidth, 0.0f, m_Center.y - m_HalfHeight),		// Bottom Left
																					D3DXVECTOR3(m_Center.x - m_HalfWidth, 1.5f, m_Center.y - m_HalfHeight),		// Top Left
																					D3DXVECTOR3(m_Center.x - m_HalfWidth, 1.5f, m_Center.y + m_HalfHeight),		// Top Right
																					D3DXVECTOR3(m_Center.x - m_HalfWidth, 0.0f, m_Center.y + m_HalfHeight),		// Bottom Right
																					Normal, WallTextureScale);

		Walls.push_back(WestWall);
	}

	// Set walls generated to true for this dungeon piece, so that the scene can now begin rendering them
	WallsGenerated = true;
}


// Function:		Set Size Limits function 
// What it does:	Sets the size limits for this room based on the type it is 
//					Therefore each room can use these limits to find a random value between them and still hold their shape
// Takes in:		Nothing
// Returns:			Nothing

void DungeonPieceClass::SetSizeLimits()
{
	// X is width, Y is height
	// All are multiplied by 100 so rand can find an random integer between the two values and then / 100 to get back to the real dimensions
	
	if (m_TypeOfPiece == ROOM)
	{
		m_LowerSizeLimit = D3DXVECTOR2(15.0f, 15.0f);		// 1.5, 1.5
		m_UpperSizeLimit = D3DXVECTOR2(40.0f, 40.0f);		// 4.0, 4.0
	}
	if (m_TypeOfPiece == VERTICAL_CORRIDOR)
	{
		m_LowerSizeLimit = D3DXVECTOR2(10.0f, 20.0f);		// 1.0, 2.0
		m_UpperSizeLimit = D3DXVECTOR2(10.0f, 50.0f);		// 1.0, 5.0
	}
	if (m_TypeOfPiece == HORIZONTAL_CORRIDOR)
	{
		m_LowerSizeLimit = D3DXVECTOR2(20.0f, 10.0f);		// 2.0, 1.0
		m_UpperSizeLimit = D3DXVECTOR2(50.0f, 10.0f);		// 5.0, 1.0
	}
}


// Function:		Get Width and Height function
// What it does:	Finds a random height/width between the smallest size limit and largest size limit for this room
//					So that all rooms are different and unique
// Takes in:		m_PieceType - type of the new room
// Returns:			D3DXVECTOR2 - new randomised width and height vector

D3DXVECTOR2 DungeonPieceClass::GetWidthAndHeight(m_PieceType NewPieceType)
{
	D3DXVECTOR2 WidthHeight = D3DXVECTOR2(0.0f, 0.0f);

	// Set the rand values to the lower size limit at first
	float RandValueX = m_LowerSizeLimit.x;
	float RandValueY = m_UpperSizeLimit.y;

	// If the lower and upper limits don't match then find a random value between them
	if (m_UpperSizeLimit.x != m_LowerSizeLimit.x)
	{
		RandValueX = rand() % (int)(m_UpperSizeLimit.x - m_LowerSizeLimit.x) + m_LowerSizeLimit.x;
	}
	if (m_UpperSizeLimit.y != m_LowerSizeLimit.y)
	{
		RandValueY = rand() % (int)(m_UpperSizeLimit.y - m_LowerSizeLimit.y) + m_LowerSizeLimit.y;
	}

	// Set the width and height vector to these values
	WidthHeight.x = floor(RandValueX);
	WidthHeight.x = WidthHeight.x / int(10);
	WidthHeight.y = floor(RandValueY);
	WidthHeight.y = WidthHeight.y / int(10);

	// Return the new randomised width and height
	return WidthHeight;
}


// Function:		Get Rooms Available function
// What it does:	Counts up the 'Available' bools to check how many rooms are currently available from this room
// Takes in:		Nothing
// Returns:			int - number of rooms available

int DungeonPieceClass::GetRoomsAvailable()
{
	int RoomCount = 0;
	RoomCount = (int)m_LAvailable + (int)m_RAvailable + (int)m_UAvailable + (int)m_DAvailable;

	return RoomCount;
}


// Function:		Get Door Position function
// What it does:	Returns the center of the door mesh based on the direction the door is created in
// Takes in:		int - Door direction to tell the function where the door should be placed, DungeonPieceClass - Pointer to the dungeon piece
// Returns:			D3DXVECTOR2 - The center of the door mesh

D3DXVECTOR2 DungeonPieceClass::GetDoorPosition(int DoorDirection, DungeonPieceClass *ThisPiece)
{
	D3DXVECTOR2 center = D3DXVECTOR2(0.0f, 0.0f);
	
	// Uses the direction of the door to find where the center of the door mesh should be
	// By looking at the center, height and width of the dungeon piece
	if (DoorDirection == LEFT)
	{
		D3DXVECTOR2 RightSide = D3DXVECTOR2((ThisPiece->m_Center.x + ThisPiece->m_HalfWidth), ThisPiece->m_Center.y); 
		center = RightSide;
	}
	if (DoorDirection == RIGHT)
	{
		D3DXVECTOR2 LeftSide = D3DXVECTOR2((ThisPiece->m_Center.x - ThisPiece->m_HalfWidth), ThisPiece->m_Center.y);
		center = LeftSide;
	}
	if (DoorDirection == UP)
	{
		D3DXVECTOR2 DownSide = D3DXVECTOR2(ThisPiece->m_Center.x, (ThisPiece->m_Center.y - ThisPiece->m_HalfHeight));
		center = DownSide;
	}
	if (DoorDirection == DOWN)
	{
		D3DXVECTOR2 UpSide = D3DXVECTOR2(ThisPiece->m_Center.x, (ThisPiece->m_Center.y + ThisPiece->m_HalfHeight));
		center = UpSide;
	}

	// Return this center so the door can be positioned
	return center;
}



// Function:		Get Index Count function
// What it does:	Returns index count needed for rendering the dungeon pieces in RenderScene in application class
// Takes in:		Nothing
// Returns:			int - Index count

int DungeonPieceClass::GetIndexCount()
{
	return m_IndexCount;
}


// Function:		Get Map Texture function
// What it does:	Returns the map texture needed for rendering
// Takes in:		Nothing
// Returns:			ID3D11ShaderResourceView - pointer to map texture

ID3D11ShaderResourceView* DungeonPieceClass::GetMapTexture()
{
	return m_MapTexture->GetTexture();
}


// Function:		Get Dungeon Texture function
// What it does:	Returns the dungeon texture needed for rendering		
// Takes in:		Nothing
// Returns:			ID3D11ShaderResourceView - pointer to dungeon texture
	
ID3D11ShaderResourceView* DungeonPieceClass::GetDungeonTexture()
{
	return m_DungeonTexture->GetTexture();
}


// Function:		Render function
// What it does:	Puts the vertex and index buffers in the graphics pipeline by calling RenderBuffers
// Takes in:		Nothing
// Returns:			Nothing

void DungeonPieceClass::Render()
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers();

	return;
}


// Function:		Shutdown function
// What it does:	Calls ReleaseTexture to release textures and ShutdownBuffers for when application ends		
// Takes in:		Nothing
// Returns:			Nothing

void DungeonPieceClass::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();
	
	// Release the vertex and index buffers.
	ShutdownBuffers();

	return;
}


// Function:		Initialise Buffers function 
// What it does:	Sets up the vertex and index buffer based on the type of piece it is
//					Sets the positions of the vertices and the texture scales and normals to set up the dungeon piece geometry 
// Takes in:		Nothing
// Returns:			Returns true if successful, false if not

bool DungeonPieceClass::InitializeBuffers()
{
	// Set up the vertex and index buffers/arrays
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set the number of vertices in the vertex array.
	m_VertexCount = 4;

	// Set the number of indices in the index array.
	m_IndexCount = 6;

	// Create the vertex array.
	vertices = new VertexType[m_VertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_IndexCount];
	if (!indices)
	{
		return false;
	}

	// Use the default y position of 0 unless the type of piece is a door (slightly higher than dungeon piece
	// so it draws in front of the map)
	float yPosition = 0.0f;
	if (m_TypeOfPiece == DOOR)
	{
		yPosition = 0.005;
	}

	// Ceiling pieces draw differently to all other pieces because they need to be drawn anti clockwise so player can view 
	// the mesh from beneath it
	if (m_TypeOfPiece == CEILING)
	{
		// Load the vertex array with data.
		// Ceiling normal is down the negative y axis
		// Uses the ceiling texture scale here for setting texture U V co-ordinates
		yPosition = 0;

		vertices[0].position = D3DXVECTOR3(m_Center.x - m_HalfWidth, yPosition, m_Center.y - m_HalfHeight);  // Bottom left.
		vertices[0].texture = D3DXVECTOR2(0.0f, m_TextureScale.y);
		vertices[0].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);

		vertices[1].position = D3DXVECTOR3(m_Center.x + m_HalfWidth, yPosition, m_Center.y - m_HalfHeight);  // Bottom right.
		vertices[1].texture = D3DXVECTOR2(m_TextureScale.x, m_TextureScale.y);
		vertices[1].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	
		vertices[2].position = D3DXVECTOR3(m_Center.x + m_HalfWidth, yPosition, m_Center.y + m_HalfHeight);  // Top right.
		vertices[2].texture = D3DXVECTOR2(m_TextureScale.x, 0.0f);
		vertices[2].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);

		vertices[3].position = D3DXVECTOR3(m_Center.x - m_HalfWidth, yPosition, m_Center.y + m_HalfHeight);  // Top left.
		vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);
		vertices[3].normal = D3DXVECTOR3(0.0f, -1.0, 0.0f);
	}
	else
	{
		// Load the vertex array with data.
		// Dungeon piece normal is up the positive y axis
		// Uses the texture scale passed in that is set by each object to change the U and V co-ordinates for repeating
		vertices[0].position = D3DXVECTOR3(m_Center.x - m_HalfWidth, yPosition, m_Center.y - m_HalfHeight);  // Bottom left.
		vertices[0].texture = D3DXVECTOR2(0.0f, m_TextureScale.y);
		vertices[0].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		vertices[1].position = D3DXVECTOR3(m_Center.x - m_HalfWidth, yPosition, m_Center.y + m_HalfHeight);  // Top left.
		vertices[1].texture = D3DXVECTOR2(0.0f, 0.0f);
		vertices[1].normal = D3DXVECTOR3(0.0f, 1.0, 0.0f);
	
		vertices[2].position = D3DXVECTOR3(m_Center.x + m_HalfWidth, yPosition, m_Center.y + m_HalfHeight);  // Top right.
		vertices[2].texture = D3DXVECTOR2(m_TextureScale.x, 0.0f);
		vertices[2].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		vertices[3].position = D3DXVECTOR3(m_Center.x + m_HalfWidth, yPosition, m_Center.y - m_HalfHeight);  // Bottom right.
		vertices[3].texture = D3DXVECTOR2(m_TextureScale.x, m_TextureScale.y);
		vertices[3].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	}

	// Load the index array with data.
	indices[0] = 0;			// Bottom left.
	indices[1] = 1;			// Top left.
	indices[2] = 2;			// Top right.

	indices[3] = 0;			// bottom left
	indices[4] = 2;			// top right
	indices[5] = 3;			// bottom right

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_VertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = m_Device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_IndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = m_Device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	// Return true if successful
	return true;
}


// Function:		Shutdown Buffers function
// What it does:	Releases the buffer objects
// Takes in:		Nothing
// Returns:			Nothing

void DungeonPieceClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_IndexBuffer)
	{
		m_IndexBuffer->Release();
		m_IndexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_VertexBuffer)
	{
		m_VertexBuffer->Release();
		m_VertexBuffer = 0;
	}

	return;
}


// Function:		Render Buffers fucntion
// What it does:	Sets index and vertex buffers to active and the type of topology the geometry should be drawn as
// Takes in:		Nothing
// Returns:			Nothing

void DungeonPieceClass::RenderBuffers()
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	m_DeviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	m_DeviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


// Function:		Load Textures function
// What it does:	Takes in two file names and sets the first as the map texture, and second as the dungeon texture
//					Then initialises these textures
// Takes in:		WCHAR - Pointer to map texture file name, WCHAR - Pointer to dungeon texture file name
// Returns:			True if textures initialised successfully, false if not

bool DungeonPieceClass::LoadTextures(WCHAR* MapFile, WCHAR* DungeonFile)
{
	bool result;

	// Each dungeon piece has two textures, one for rendering the piece to the map and one for rendering in the dungeon

	// Create the map texture object
	m_MapTexture = new TextureClass;
	if (!m_MapTexture)
	{
		return false;
	}

	// Initialize the map texture object.
	result = m_MapTexture->Initialize(m_Device, MapFile);
	if (!result)
	{
		return false;
	}

	// Create the dungeon texture object.
	m_DungeonTexture = new TextureClass;
	if (!m_DungeonTexture)
	{
		return false;
	}

	// Initialize the dungeon texture object.
	result = m_DungeonTexture->Initialize(m_Device, DungeonFile);
	if(!result)
	{
		return false;
	}

	// Return true if successful
	return true;
}


// Function:		Release Texture function
// What it does:	Releases texture objects when called
// Takes in:		Nothing
// Returns:			Nothing

void DungeonPieceClass::ReleaseTexture()
{
	// Release the texture object.
	if(m_DungeonTexture)
	{
		m_DungeonTexture->Shutdown();
		delete m_DungeonTexture;
		m_DungeonTexture = 0;
	}

	// Release the texture object.
	if(m_MapTexture)
	{
		m_MapTexture->Shutdown();
		delete m_MapTexture;
		m_MapTexture = 0;
	}

	return;
}




