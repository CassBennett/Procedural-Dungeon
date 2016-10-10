////////////////////////////////////////////////////////////////////////////////
// Filename: dungeonclass.h
// Dungeon Class
// Handles all processing for procedurally generating the dungeon, holding all dungeon pieces and the calculations for choosing the dungeon pieces
////////////////////////////////////////////////////////////////////////////////

#ifndef DUNGEON_CLASS_H
#define DUNGEON_CLASS_H

// For rand
#include <stdlib.h>
#include <time.h>

// For list
#include <list>
using std::list;

#include "dungeonpiececlass.h"
#include "meshclass.h"
#include "d3dclass.h"

// The size of the map available for the dungeon
#define DUNGEON_GRID_X 1000
#define DUNGEON_GRID_Y 1000

// Specify the depth of the dungeon
#define DUNGEON_DEPTH 5
// Specify the amount of attempts the generation should try when picking a room
#define PIECE_ATTEMPTS 100

// The scale of the dungeon once it is generated (used for rendering)
#define DUNGEON_SCALE 20

// Define values for checking the positions in front and behind the player in the map
#define FORWARD 1
#define BACKWARD -1

// Define values for checking the grid
#define DUNGEON_ROOM 1
#define COLLISION 0

// Arrow texture from - http://icons.mysitemyway.com/legacy-icon-tags/triangle/page/3/

////////////////////////////////////////////////////////////////////////////////
// Class name: DungeonClass
// Handles the creation of the procedural dungeon, holding lists for the dungeon pieces and ceilings
////////////////////////////////////////////////////////////////////////////////
class DungeonClass
{
public:
	// Function:		Dungeon class constructor
	// What it does:	Initialises all variables and objects to their defaults to set up the class
	// Takes in:		Nothing
	// Returns:			Nothing
	DungeonClass();

	// Placeholder destructor for the dungeon class
	~DungeonClass();

	// Function:		Shutdown function
	// What it does:	Releases all objects when the application ends and deletes them, cleans resources
	// Takes in:		Nothing
	// Returns:			Nothing
	void Shutdown();

	// Function:		Initialise function
	// What it does:	Initialises the dungeon - Initialises first dungeon piece and ceiling piece, the player mesh, and begins calling
	//					PlaceNextPiece four times with four directions of doors so that the dungeon will spread from the entrance piece
	//					And will continue until the dungeon is fully created
	// Takes in:		ID3D11Device pointer - Pointer to the DirectX device, ID3D11DeviceContext - Pointer to the DirectX device context
	// Returns:			True if dungeon has been initialised successfully, false if not
	bool Initialise(ID3D11Device*, ID3D11DeviceContext*);

	// Function:		Fill Grid With Wall function
	// What it does:	This is called for each wall belonging to a dungeon piece in ApplicationClass when the walls have been created so that 
	//					the walls are present in the dungeon grid and can be collided with. Uses the start and end positions of the wall to fill 
	//					in the information in the grid
	// Takes in:		MeshClass - Pointer to the wall mesh that is passed in so that it can be checked
	// Returns:			Nothing
	void FillGridWithWall(MeshClass *Wall);

	// Function:		Set Player Position function
	// What it does:	Takes in the position of the camera and uses matrix multiplication to convert it to the
	//					value it would be in the dungeon grid. This is because the dungeon has several transforms when rendered, therefore
	//					Need to transform them back to normal to get them on the original grid
	// Takes in:		D3DXVECTOR3 - vector for the position of the player (the camera)
	// Returns:			Nothing
	void SetPlayerPosition(D3DXVECTOR3 position);

	// Function:		Can Move Player function
	// What it does:	Called in the HandleInput function in ApplicationClass when moving forwards or backwards with collision on.
	//					Checks the direction the player wants to move in and uses the lookat vector to check the next position the player will be in
	//					if they do move in this direction. It then checks if this position is in a free space (within a room) in the dungeon grid or if
	//					it is a wall or outside the dungeon.
	// Takes in:		D3DXVECTOR3 - lookat vector, int - direction value to specify the direction the player wants to move in
	// Returns:			True if the player can move in this direction and isn't colliding, false if it will collide
	bool CanMovePlayer(D3DXVECTOR3 lookat, int direction);

private:
	// Function:		Place Next Piece function
	// What it does:	Loops through the number of attempts in order to find a suitable room leading off from the previous room, and initialises
	//					the room and calls PlaceNextPiece again for the available rooms leading from this new room.
	//					This is the main recursive function used for generating the dungeon as it calls itself based on what 
	//					rooms are available in the room it creates.
	// Takes in:		int - Depth value of the dungeon so that the dungeon can check how much it has generated,
	//					DungeonPieceClass - pointer to the current piece that is creating a new room off it
	//					int - DoorNumber value which uses defines of LEFT, RIGHT, UP, and DOWN to state the room side the next room is created from
	// Returns:			Nothing
	void PlaceNextPiece(int Depth, DungeonPieceClass* Piece, int DoorNumber);

	// Function:		Calculate Next Piece function
	// What it does:	Creates a new piece type and uses a random number between 1 and 100 to decide the next room to place. 
	//					The next room to place uses chances based on the previous type of room, so that the dungeon will be varied
	//					and original every time. Uses the direction of the room to decide on the new piece.
	// Takes in:		DungeonPieceClass::m_PieceType - ParentPiece which is the dungeon piece that is creating the new room, 
	//					int - value for the door number that the room is following from
	// Returns:			DungeonPieceClass::m_PieceType - Type of piece generated by dungeon so it can be used to initialise the new dungeon piece
	DungeonPieceClass::m_PieceType CalculateNextPiece(DungeonPieceClass::m_PieceType ParentPiece, int DoorNo);

	// Function:		Get Next Center function
	// What it does:	Uses the direction the room is created in and the width/height of the new piece
	//					to find the center of the room leading off of the previous one
	// Takes in:		int - Direction of the room it is being created in, DungeonPieceClass - pointer to the piece that is being placed
	//					float - width value of the piece being created, float - height value of the piece being created
	// Returns:			D3DXVECTOR2 - The new center of the next dungeon piece so that it can be placed next to the previous piece in the correct position
	D3DXVECTOR2 GetNextCenter(int Direction, DungeonPieceClass* ThisPiece, float Width, float Height); 

	// Function:		Update Room function
	// What it does:	Updates the bools of the room created so that knows not to create a new room in the door it has just been created from
	//					Also update so that the room knows it already has a door at the start so it will draw a wall to accomate this
	// Takes in:		DungeonPieceClass - pointer to the new dungeon piece, int - Direction value for the direction this room will be placed in
	// Returns:			Nothing
	void UpdateRoom(DungeonPieceClass* NewPiece, int Direction);

	// Function:		Check Grid function
	// What it does:	It uses the new center, width and height of the new piece and checks to see if there is room available for it on the dungeon grid
	//					The value on the grid if not available will be 1, and value will be 0 if available
	//					If it doesn't return false, will fill in the grid for the new room 
	// Takes in:		float - Width value of new piece, float - Height value of new piece, D3DXVECTOR2 - Vector of center of new piece
	// Returns:			True if the dungeon piece using this width, height and center can be placed in the dungeon grid, false if not
	bool CheckGrid(float Width, float Height, D3DXVECTOR2 Center);

	// Function:		Set Chances function
	// What it does:	Changes the chances available for the dungeon as the depth of the dungeon increases
	//					It is called in PlaceNextPiece and changes the chance values based on the depth, to make it more likely or less likely
	//					That certain types of rooms will be created closer to the end of the dungeon
	// Takes in:		int - Depth value
	// Returns:			Nothing
	void SetChances(int depth);

// Public member variables
public:
	// Dungeon piece lists
	list<DungeonPieceClass*> m_DungeonPieces;	
	list<DungeonPieceClass*> m_CeilingPieces;
	list<DungeonPieceClass*> m_DoorPieces;

	// Public player variables
	MeshClass *m_PlayerMesh;
	D3DXVECTOR3 m_PlayerPosition;
	D3DXVECTOR3 m_PlayerLastPosition;

// Private member variables
private:
	// Direct X objects
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_DeviceContext;

	// Dungeon variables
	float DungeonGrid[DUNGEON_GRID_X][DUNGEON_GRID_Y];
	DungeonPieceClass* m_EntrancePiece;
	int m_DungeonDepth; 
	int m_CorridorChanceAftRoom;
	int m_CorridorChanceAftCorridor;
	int m_RoomChanceAftRoom;
	int m_RoomChanceAftCorridor;
	int m_RoomsLimit;

	// Room variables
	int m_RoomChance;
	int m_DoorDirection;	
	D3DXVECTOR2 m_DoorPosition;
	D3DXVECTOR2 m_WidthHeight;
	D3DXVECTOR2	m_NewCenter;

	// Player variables
	D3DXVECTOR3 m_NextPlayerPosition;
	D3DXVECTOR3 m_LookAt;
	D3DXVECTOR3 m_Position;
	bool m_PlayerCanMove;
};


#endif