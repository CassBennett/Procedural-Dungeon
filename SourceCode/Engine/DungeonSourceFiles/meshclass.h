////////////////////////////////////////////////////////////////////////////////
// Filename: meshclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MESHCLASS_H_
#define _MESHCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include "textureclass.h"
#include <stdlib.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: MeshClass
// Handles the creation of a square mesh object
// Adapted from the MeshClass given by Paul Robertson in previous module: Graphics Programming with Shaders
// Used to create meshes to be rendered in the scene
////////////////////////////////////////////////////////////////////////////////
class MeshClass
{
private:

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

public:
	// Function:		Mesh Class constructor
	// What it does:	Initialises all objects and variables to default values
	// Takes in:		Nothing
	// Returns:			Nothing
	MeshClass();

	// Placeholder constructor and destructor
	MeshClass(const MeshClass&);
	~MeshClass();

	// Function:		Initialise function
	// What it does:	Initialises the mesh and its center, width, height data as well as the vertices needed to draw the geometry
	//					Also loads the texture and initialises buffers
	// Takes in:		ID3D11Device - pointer to device, ID3D11DeviceContext - pointer to device context, WCHAR - Texture file name
	//					D3DXVECTOR3 - center vector of the mesh, float - width of mesh, float - height of mesh, float - depth of mesh
	// Returns:			True if initialised properly, false if not
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, WCHAR*, D3DXVECTOR3 Center, float Width, float Height, float Depth);

	// Function:		Initialise Function
	// What it does:	Differs from the other Initialise because it takes in the vertex points instead of setting it up in the function
	//					Also takes in the normal and the texture scaling. This initialise is mostly used for the walls of the dungeon
	// Takes in:		ID3D11Device - pointer to device, ID3D11DeviceContext - pointer to device context, WCHAR - Texture file name
	//					D3DXVECTOR3 - bottom left vertex position, D3DXVECTOR3 top left vertex position, D3DXVECTOR3 top right vertex position,
	//					D3DXVECTOR3 - bottom right vertex position, D3DXVECTOR3 - normal vector for this mesh, D3DXVECTOR3 - texture scale for this mesh
	// Returns:			True if initialised properly, false if not
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, WCHAR*, D3DXVECTOR3 BottomLeft, D3DXVECTOR3 TopLeft, D3DXVECTOR3 TopRight, 
						D3DXVECTOR3 BottomRight, D3DXVECTOR3 Normal, D3DXVECTOR2 TextureScale);

	// Function:		Shutdown function
	// What it does:	Calls ReleaseTexture to release textures and ShutdownBuffers for when application ends		
	// Takes in:		Nothing
	// Returns:			Nothing
	void Shutdown();

	// Function:		Render function
	// What it does:	Puts the vertex and index buffers in the graphics pipeline by calling RenderBuffers
	// Takes in:		ID3D11DeviceContext - pointer to the device context
	// Returns:			Nothing
	void Render(ID3D11DeviceContext*);

	// Function:		Get Index Count function
	// What it does:	Returns index count needed for rendering the mesh
	// Takes in:		Nothing
	// Returns:			int - Index count
	int GetIndexCount();

	// Function:		Get Texture function
	// What it does:	Returns the first texture object of the mesh
	// Takes in:		Nothing
	// Returns:			ID3D11ShaderResourceView - pointer to the first texture object resource view
	ID3D11ShaderResourceView* GetTexture();

	// Function:		Get Texture 2 function
	// What it does:	Returns the second texture object of the mesh
	// Takes in:		Nothing
	// Returns:			ID3D11ShaderResourceView - pointer to the second texture object resource view
	ID3D11ShaderResourceView* GetTexture2();

	// Function:		Load Texture function
	// What it does:	Takes in a file name and sets it to the texture object and initialises
	// Takes in:		ID3D11Device - pointer to the directx device, WCHAR - Pointer to the texture filename
	// Returns:			True if textures initialised successfully, false if not
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, WCHAR*);

	// Function:		Load Texture function
	// What it does:	Takes in two file names and sets the first as the first texture object, and second as the second texture object
	//					Then initialises these textures
	// Takes in:		ID3D11Device - pointer to the directx device, WCHAR - Pointer to first texture file name, WCHAR - Pointer to second texture file name
	// Returns:			True if textures initialised successfully, false if not
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, WCHAR*, WCHAR*);

	// Function:		Initialise Buffers function 
	// What it does:	Sets up the vertex and index buffer based on the type of piece it is
	//					Sets the positions of the vertices and the texture co-ordinates and normals to set up the mesh geometry 
	// Takes in:		Nothing
	// Returns:			Returns true if successful, false if not
	bool InitializeBuffers(ID3D11Device*);

	// Function:		Shutdown Buffers function
	// What it does:	Releases the buffer objects
	// Takes in:		Nothing
	// Returns:			Nothing
	void ShutdownBuffers();

	// Function:		Render Buffers fucntion
	// What it does:	Sets index and vertex buffers to active and the type of topology the geometry should be drawn as
	// Takes in:		Nothing
	// Returns:			Nothing
	void RenderBuffers(ID3D11DeviceContext*);

	// Function:		Release Texture function
	// What it does:	Releases texture objects when called
	// Takes in:		Nothing
	// Returns:			Nothing
	void ReleaseTexture();

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	bool m_twoTextures;
	TextureClass* m_Texture;
	TextureClass* m_Texture2;

	D3DXVECTOR3 m_StartPos;
	D3DXVECTOR3 m_EndPos;
	bool m_MeshAlongXAxis;
	bool m_MeshAlongZAxis;
	float m_Width;

private:
	float m_HalfWidth;
	float m_Height;
	float m_HalfHeight;
	float m_Depth;
	float m_HalfDepth;
	D3DXVECTOR3 m_BottomLeft;
	D3DXVECTOR3 m_TopLeft;
	D3DXVECTOR3 m_TopRight;
	D3DXVECTOR3 m_BottomRight;
	D3DXVECTOR3 m_Normal;
	D3DXVECTOR2 m_TextureScale;
	D3DXVECTOR3 m_Center;

	private:

};

#endif