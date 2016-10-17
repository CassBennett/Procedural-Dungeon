////////////////////////////////////////////////////////////////////////////////
// Filename: dungeonpiececlass.h
////////////////////////////////////////////////////////////////////////////////

#ifndef DUNGEON_PIECE_CLASS_H
#define DUNGEON_PIECE_CLASS_H

#include <d3d11.h>
#include <d3dx10math.h>
#include "textureclass.h"
#include "meshclass.h"

// For rand
#include <stdlib.h>
#include <time.h>

// For list
#include <list>
using std::list;

// Next Door Direction Definitions
#define LEFT 1
#define UP 2
#define RIGHT 3
#define DOWN 4

#define NORTH_DOOR 1
#define EAST_DOOR 2
#define SOUTH_DOOR 3
#define WEST_DOOR 4

#define CURRENT_WALLS 4

#define HORIZONTAL_DOOR 1
#define VERTICAL_DOOR 2

// Stone Walls texture - http://www.sketchuptexture.com/p/stone-walls-and-paving-stone.html
// Wooden Floors texture - http://www.sketchuptexture.com/p/wood-wood-floors-bambu-thatch-cork.html

////////////////////////////////////////////////////////////////////////////////
// Class name: DungeonPieceClass
// Handles creation of randomised rooms and their floors/walls/ceilings as meshes
// Adapted from the MeshClass given by Paul Robertson in previous module: Graphics Programming with Shaders (Abertay University)
// Adapted for my dungeon application and for the dungeon pieces
////////////////////////////////////////////////////////////////////////////////

class DungeonPieceClass
{
private:
	// A struct to hold all information about a Vertex to pass into a Vertex Shader
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

public:
	// These are the types of rooms that can be created
	enum m_PieceType{ INIT, DOOR, ENTRANCE, ROOM, HORIZONTAL_CORRIDOR, VERTICAL_CORRIDOR, CEILING };

	// Function:		Dungeon Piece Constructor
	// What it does:	Sets type of piece to the passed in type and the door direction to the passed in direction
	//					Initialises all other members and objects to their default values to set up for the class	
	// Takes in:		m_PieceType - the type of dungeon piece this instance is, int - Defined int for the direction of the room it is 
	//					being placed in.
	// Returns:			Nothing
	DungeonPieceClass(m_PieceType Type, int Direction);


	// Placeholder destructor
	~DungeonPieceClass();


	// Function:		Initialise function
	// What it does:	Initialises the variables of the dungeon piece to the values passed in, and creates rooms based on the type of piece
	//					the dungeon piece was set to in the constructor, using the room limit passed into this function
	//					Calls the appropriate function for creating the room, and initialises for rendering
	// Takes in:		ID3D11Device - pointer to DirectX device, ID3D11DeviceContext - Pointer to DirectX device context,
	//					D3DXVECTOR2 - center of this piece, float - Width of this piece, float - Height of this piece
	//					int - The door limit for the amount of rooms that can be created leading from this room
	// Returns:			True if all initialisation has happened successfully, false if not
	bool Initialise(ID3D11Device*, ID3D11DeviceContext*, D3DXVECTOR2 Center, float Width, float Height, int RoomLimit);


	// Function:		Get Width and Height function
	// What it does:	Finds a random height/width between the smallest size limit and largest size limit for this room
	//					So that all rooms are different and unique
	// Takes in:		m_PieceType - type of the new room
	// Returns:			D3DXVECTOR2 - new randomised width and height vector
	D3DXVECTOR2 GetWidthAndHeight(m_PieceType NewPieceType);


	// Function:		Get Piece Type function
	// What it does:	Returns the type of piece this dungeon piece is
	// Takes in:		Nothing
	// Returns:			m_PieceType - type of piece
	m_PieceType GetPieceType();


	// Function:		Get Door Position function
	// What it does:	Returns the center of the door mesh based on the direction the door is created in
	// Takes in:		int - Door direction to tell the function where the door should be placed, DungeonPieceClass - Pointer to the dungeon piece
	// Returns:			D3DXVECTOR2 - The center of the door mesh
	D3DXVECTOR2 GetDoorPosition(int DoorDirection, DungeonPieceClass *ThisPiece);


	// Function:		Build Walls function
	// What it does:	Creates the meshes used for the walls, setting up two seperate walls with a space between
	//					for a wall with a door in the middle, and a single wall if a door is not present in that wall
	//					This function is called after all of the dungeon pieces have been created in the Application Class
	//					to make sure the walls are only created after all rooms and doors have been set 	
	// Takes in:		Nothing
	// Returns:			Nothing
	void BuildWalls();


	// Function:		Get Map Texture function
	// What it does:	Returns the map texture needed for rendering
	// Takes in:		Nothing
	// Returns:			ID3D11ShaderResourceView - pointer to map texture
	ID3D11ShaderResourceView* GetMapTexture();


	// Function:		Get Dungeon Texture function
	// What it does:	Returns the dungeon texture needed for rendering		
	// Takes in:		Nothing
	// Returns:			ID3D11ShaderResourceView - pointer to dungeon texture
	ID3D11ShaderResourceView* GetDungeonTexture();


	// Function:		Render function
	// What it does:	Puts the vertex and index buffers in the graphics pipeline by calling RenderBuffers
	// Takes in:		Nothing
	// Returns:			Nothing
	void Render();


	// Function:		Get Index Count function
	// What it does:	Returns index count needed for rendering the dungeon pieces in RenderScene in application class
	// Takes in:		Nothing
	// Returns:			int - Index count
	int GetIndexCount();


	// Function:		Shutdown function
	// What it does:	Calls ReleaseTexture to release textures and ShutdownBuffers for when application ends		
	// Takes in:		Nothing
	// Returns:			Nothing
	void Shutdown();

private:
	// Function:		Get Rooms Available function
	// What it does:	Counts up the 'Available' bools to check how many rooms are currently available from this room
	// Takes in:		Nothing
	// Returns:			int - number of rooms available
	int GetRoomsAvailable();


	// Function:		Make Door function
	// What it does:	Loads in the textures used for the door depending on if the door is horizontal or vertical			
	// Takes in:		Nothing
	// Returns:			True if textures loaded properly, false if not
	bool MakeDoor();


	// Function:		Make Room function 
	// What it does:	Loops through the number of rooms passed in, to decide the directions available for
	//					the new rooms randomly. Checks to see if a door isn't already in this place, then makes 
	//					a door in that place up until the room limit count. Also loads in the textures for the rooms and 
	//					sets the texture scale for normal rooms.
	// Takes in:		int - Number of rooms limit so that room only creates this amount of doors
	// Returns:			True if function has completed properly, false if not
	bool MakeRoom(int NumOfRooms);


	// Function:		Make Vertical Corridor
	// What it does:	Sets up and down doors available to true, because it is a vertical corridor
	//					and loads the texture
	// Takes in:		Nothing
	// Returns:			True if textures loaded properly, false if not
	bool MakeVerticalCorridor();


	// Function:		Make Horizontal Corridor
	// What it does:	Sets left and right doors available to true, because it is a horizontal corridor
	//					and loads the texture
	// Takes in:		Nothing
	// Returns:			True if textures loaded properly, false if not
	bool MakeHorizontalCorridor();


	// Function:		Make Ceiling function
	// What it does:	Loads in the textures for the ceiling piece and sets the texture scale
	// Takes in:		Nothing
	// Returns:			True if textures loaded properly, false if not
	bool MakeCeiling();


	// Function:		Set Size Limits function 
	// What it does:	Sets the size limits for this room based on the type it is 
	//					Therefore each room can use these limits to find a random value between them and still hold their shape
	// Takes in:		Nothing
	// Returns:			Nothing
	void SetSizeLimits();


	// Function:		Load Textures function
	// What it does:	Takes in two file names and sets the first as the map texture, and second as the dungeon texture
	//					Then initialises these textures
	// Takes in:		WCHAR - Pointer to map texture file name, WCHAR - Pointer to dungeon texture file name
	// Returns:			True if textures initialised successfully, false if not
	bool LoadTextures(WCHAR*, WCHAR*);


	// Function:		Initialise Buffers function 
	// What it does:	Sets up the vertex and index buffer based on the type of piece it is
	//					Sets the positions of the vertices and the texture scales and normals to set up the dungeon piece geometry 
	// Takes in:		Nothing
	// Returns:			Returns true if successful, false if not
	bool InitializeBuffers();


	// Function:		Shutdown Buffers function
	// What it does:	Releases the buffer objects
	// Takes in:		Nothing
	// Returns:			Nothing
	void ShutdownBuffers();


	// Function:		Render Buffers fucntion
	// What it does:	Sets index and vertex buffers to active and the type of topology the geometry should be drawn as
	// Takes in:		Nothing
	// Returns:			Nothing
	void RenderBuffers();


	// Function:		Release Texture function
	// What it does:	Releases texture objects when called
	// Takes in:		Nothing
	// Returns:			Nothing
	void ReleaseTexture();


public:
	// Dungeon Piece Variables
	float m_HalfWidth;
	float m_HalfHeight;
	D3DXVECTOR2 m_Center;

	// Bools specifying if doors are available or not
	bool m_LAvailable, m_RAvailable, m_UAvailable, m_DAvailable;
	bool m_LDoor, m_RDoor, m_UDoor, m_DDoor;
	int m_DoorDirection;

	// Wall variables
	list<MeshClass*> Walls;
	bool WallsGenerated;	

private:
	// Dungeon Piece variables
	int m_ExitsAmount;
	float m_Width;
	float m_Height;
	int m_RoomsLimit;
	D3DXVECTOR2 m_LowerSizeLimit;
	D3DXVECTOR2 m_UpperSizeLimit;
	m_PieceType m_TypeOfPiece;

	// Rendering Variables
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_DeviceContext;
	D3DXVECTOR2 m_TextureScale;
	D3DXVECTOR3 m_Normal;
	ID3D11Buffer *m_VertexBuffer, *m_IndexBuffer;
	int m_VertexCount, m_IndexCount;
	TextureClass* m_MapTexture;
	TextureClass* m_DungeonTexture;
};


#endif
