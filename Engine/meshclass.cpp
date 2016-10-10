////////////////////////////////////////////////////////////////////////////////
// Filename: meshclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "meshclass.h"


// Function:		Mesh Class constructor
// What it does:	Initialises all objects and variables to default values
// Takes in:		Nothing
// Returns:			Nothing

MeshClass::MeshClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
	m_Width = 0.0f;
	m_HalfWidth = 0.0f;
	m_Height = 0.0f;
	m_HalfHeight = 0.0f;
	m_Depth = 0.0f;
	m_HalfDepth = 0.0f;

	// Initialise vectors
	m_BottomLeft = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_TopLeft = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_TopRight = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_BottomRight = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_StartPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_EndPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_MeshAlongXAxis = false;
	m_MeshAlongZAxis = false;

	// Default Normal
	m_Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
}


// Placeholder constructor

MeshClass::MeshClass(const MeshClass& other)
{
}


// Placeholder destructor

MeshClass::~MeshClass()
{
}


// Function:		Initialise function
// What it does:	Initialises the mesh and its center, width, height data as well as the vertices needed to draw the geometry
//					Also loads the texture and initialises buffers
// Takes in:		ID3D11Device - pointer to device, ID3D11DeviceContext - pointer to device context, WCHAR - Texture file name
//					D3DXVECTOR3 - center vector of the mesh, float - width of mesh, float - height of mesh, float - depth of mesh
// Returns:			True if initialised properly, false if not

bool MeshClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* device_context, WCHAR* textureFilename, D3DXVECTOR3 Center, float Width, float Height, float Depth)
{
	bool result;		// For error checking

	// Get the mesh variables passed in
	m_Center = Center;
	m_Width = Width;
	m_Height = Height;
	m_Depth = Depth;
	m_HalfWidth = m_Width / 2;
	m_HalfHeight = m_Height / 2;
	m_HalfDepth = m_Depth / 2;
	m_Depth = Depth;

	// Set up the vertex positions based on the center, width and height and depth
	m_BottomLeft = D3DXVECTOR3(m_Center.x - m_HalfWidth, m_Depth, m_Center.y - m_HalfHeight);
	m_TopLeft = D3DXVECTOR3(m_Center.x - m_HalfWidth, m_Depth, m_Center.y + m_HalfHeight);
	m_TopRight = D3DXVECTOR3(m_Center.x + m_HalfWidth, m_Depth, m_Center.y + m_HalfHeight);
	m_BottomRight = D3DXVECTOR3(m_Center.x + m_HalfWidth, m_Depth, m_Center.y - m_HalfHeight);
	// Set texture scale to non repeat
	m_TextureScale = D3DXVECTOR2(1.0f, 1.0f);

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(device, device_context, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}


// Function:		Initialise Function
// What it does:	Differs from the other Initialise because it takes in the vertex points instead of setting it up in the function
//					Also takes in the normal and the texture scaling. This initialise is mostly used for the walls of the dungeon
// Takes in:		ID3D11Device - pointer to device, ID3D11DeviceContext - pointer to device context, WCHAR - Texture file name
//					D3DXVECTOR3 - bottom left vertex position, D3DXVECTOR3 top left vertex position, D3DXVECTOR3 top right vertex position,
//					D3DXVECTOR3 - bottom right vertex position, D3DXVECTOR3 - normal vector for this mesh, D3DXVECTOR3 - texture scale for this mesh
// Returns:			True if initialised properly, false if not

bool MeshClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* device_context, WCHAR* textureFilename, D3DXVECTOR3 BottomLeft, D3DXVECTOR3 TopLeft, D3DXVECTOR3 TopRight, 
							D3DXVECTOR3 BottomRight, D3DXVECTOR3 Normal, D3DXVECTOR2 TextureScale)
{
	bool result;		// For error checking

	// Get the vertex positions, normal and texture scale passed in
	m_BottomLeft = BottomLeft;
	m_TopLeft = TopLeft;
	m_TopRight = TopRight;
	m_BottomRight = BottomRight;
	m_Normal = Normal;
	m_TextureScale = TextureScale;

	// Check if the mesh is drawn along the x or z axis
	// This is used for setting the wall's position in the dungeon grid
	float x_difference = abs(m_BottomRight.x - m_BottomLeft.x);
	float z_difference = abs(m_BottomRight.z - m_BottomLeft.z);
	float y_difference = abs(m_TopLeft.y - m_BottomLeft.y);

	// If x difference is positive, the mesh is drawn along the x axis
	if (x_difference > 0)
	{
		m_Width = x_difference;
		m_MeshAlongXAxis = true;

		// If facing south
		if (m_BottomRight.x > m_BottomLeft.x)
		{
			m_Center.x = m_BottomLeft.x + (0.5 * x_difference);
			m_StartPos = m_BottomLeft;
			m_EndPos = m_BottomRight;
		}
		// if facing north
		else
		{
			m_Center.x = m_BottomRight.x + (0.5 * x_difference);
			m_StartPos = m_BottomRight;
			m_EndPos = m_BottomLeft;
		}
	}

	// Else if z difference is positive, the mesh is drawn along the z axis
	else if (z_difference > 0)
	{
		m_Width = z_difference;
		m_MeshAlongZAxis = true;

		// If facing west
		if (m_BottomLeft.z > m_BottomRight.z)
		{
			m_Center.z = m_BottomRight.z + (0.5 * z_difference);
			m_StartPos = m_BottomRight;
			m_EndPos = m_BottomLeft;
		}
		// If facing east
		else
		{
			m_Center.z = m_BottomLeft.z + (0.5 * z_difference);
			m_StartPos = m_BottomLeft;
			m_EndPos = m_BottomRight;
		}
	}

	// The height is the y difference
	m_Height = y_difference; 
	m_Center.y = m_BottomLeft.y + (0.5 * y_difference);

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(device, device_context, textureFilename);
	if (!result)
	{
		return false;
	}

	// If initialisation is successful, return true
	return true;
}


// Function:		Shutdown function
// What it does:	Calls ReleaseTexture to release textures and ShutdownBuffers for when application ends		
// Takes in:		Nothing
// Returns:			Nothing

void MeshClass::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();
	
	// Release the vertex and index buffers.
	ShutdownBuffers();

	return;
}


// Function:		Render function
// What it does:	Puts the vertex and index buffers in the graphics pipeline by calling RenderBuffers
// Takes in:		ID3D11DeviceContext - pointer to the device context
// Returns:			Nothing

void MeshClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}


// Function:		Get Index Count function
// What it does:	Returns index count needed for rendering the mesh
// Takes in:		Nothing
// Returns:			int - Index count

int MeshClass::GetIndexCount()
{
	return m_indexCount;
}


// Function:		Get Texture function
// What it does:	Returns the first texture object of the mesh
// Takes in:		Nothing
// Returns:			ID3D11ShaderResourceView - pointer to the first texture object resource view

ID3D11ShaderResourceView* MeshClass::GetTexture()
{
	return m_Texture->GetTexture();
}


// Function:		Get Texture 2 function
// What it does:	Returns the second texture object of the mesh
// Takes in:		Nothing
// Returns:			ID3D11ShaderResourceView - pointer to the second texture object resource view

ID3D11ShaderResourceView* MeshClass::GetTexture2()
{
	return m_Texture->GetTexture();
}


// Function:		Initialise Buffers function 
// What it does:	Sets up the vertex and index buffer based on the type of piece it is
//					Sets the positions of the vertices and the texture co-ordinates and normals to set up the mesh geometry 
// Takes in:		Nothing
// Returns:			Returns true if successful, false if not

bool MeshClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set the number of vertices in the vertex array.
	m_vertexCount = 4;

	// Set the number of indices in the index array.
	m_indexCount = 6;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// Load the vertex array with data.
	// Use the vertex positions, normal and texture scaling set in the initialise functions
	vertices[0].position = m_BottomLeft;			// Bottom left.
	vertices[0].texture = D3DXVECTOR2(0.0f, m_TextureScale.y);
	vertices[0].normal = m_Normal;

	vertices[1].position = m_TopLeft;				// Top left.
	vertices[1].texture = D3DXVECTOR2(0.0f, 0.0f);
	vertices[1].normal = m_Normal;
	
	vertices[2].position = m_TopRight;				// Top right.
	vertices[2].texture = D3DXVECTOR2(m_TextureScale.x, 0.0f);
	vertices[2].normal = m_Normal;

	vertices[3].position = m_BottomRight;			// Bottom right.
	vertices[3].texture = D3DXVECTOR2(m_TextureScale.x, m_TextureScale.y);
	vertices[3].normal = m_Normal;

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top left.
	indices[2] = 2;  // Top right.

	indices[3] = 0;	// bottom left
	indices[4] = 2;	// top right
	indices[5] = 3;	// bottom right

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
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

void MeshClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


// Function:		Render Buffers fucntion
// What it does:	Sets index and vertex buffers to active and the type of topology the geometry should be drawn as
// Takes in:		Nothing
// Returns:			Nothing

void MeshClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


// Function:		Load Texture function
// What it does:	Takes in a file name and sets it to the texture object and initialises
// Takes in:		ID3D11Device - pointer to the directx device, WCHAR - Pointer to the texture filename
// Returns:			True if textures initialised successfully, false if not

bool MeshClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* device_context, WCHAR* filename)
{
	bool result;


	// Create the texture object.
	m_Texture = new TextureClass;
	if(!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	return true;
}


// Function:		Release Texture function
// What it does:	Releases texture objects when called
// Takes in:		Nothing
// Returns:			Nothing

void MeshClass::ReleaseTexture()
{
	// Release the texture object.
	if(m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

