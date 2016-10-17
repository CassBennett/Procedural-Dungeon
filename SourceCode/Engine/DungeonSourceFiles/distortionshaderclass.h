////////////////////////////////////////////////////////////////////////////////
// Filename: distortionshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _DISTORTIONSHADERCLASS_H_
#define _DISTORTIONSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
#include <time.h>
using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Class name: DistortionShaderClass
// Handles shader input for the distortion.vs and distortion.ps shaders
// Adapted from the LightShaderClass given by Paul Robertson in previous module: Graphics Programming with Shaders
// Adapted for my post processing distortion shader
////////////////////////////////////////////////////////////////////////////////

class DistortionShaderClass
{
private:
	// MatrixBuffer struct that passes in World, View and Projection matrix into the distortion vertex shader
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	// DistortionBuffer struct that passes in the distortion variables to the distortion pixel shader so that the shader changes 
	// the distortion in real time
	struct DistortionBufferType
	{
		float timer;
		float distortion_amount;
		float seed;
		float padding;
	};

public:
	// Function:		Distortion Shader constructor
	// What it does:	Initialises all variables and objects to their defaults
	// Takes in:		Nothing
	// Returns:			Nothing
	DistortionShaderClass();


	// Placeholder constructor and destructor
	DistortionShaderClass(const DistortionShaderClass&);
	~DistortionShaderClass();


	// Function:		Initialise function
	// What it does:	Calls 'InitializeShader' passing in the device, window, and names of the distortion vertex and pixel shader
	// Takes in:		ID3D11Device - pointer to the directx device, HWND - window
	// Returns:			True if initialisation was successful, false if not
	bool Initialize(ID3D11Device* device, HWND hwnd);


	// Function:		Shutdown function
	// What it does:	Shuts down all objects in the shader and the vertex and pixel shader
	// Takes in:		Nothing
	// Returns:			Nothing
	void Shutdown();


	// Function:		Render function
	// What it does:	This is called in the Application class to render objects using this shader. Takes in the objects and variables needed
	//					for the distortion shaders and passes them into the SetShaderParameters function
	// Takes in:		ID3D11DeviceContext - pointer to the device context, int - index count of the geometry, D3DXMATRIX - World Matrix, D3DXMATRIX, View Matrix
	//					D3DXMATRIX - projection matrix, ID3D11ShaderResourceView - pointer to the texture, float - current distortion amount (which can be changed 
	//					by user in application class)
	// Returns:			True if successful, false if not 
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float DistortionAmount);

private:
	// Function:		Initialize Shader function
	// What it does:	This function compiles the vertex and pixel shader code using the shader file names and create the shaders using the appropriate
	//					buffers. Also creates the vertex input layout for the information to be passed to the Vertex shader as input (position, texture co-ordinates,
	//					and normals). Sets up description of buffers and samplers and creates them. 
	// Takes in:		ID3D11Device - pointer to device, HWND - window, WCHAR* - pointer to the vertex shader filename, WCHAR* - pointer to the pixel shader filename
	// Returns:			True if successful, false if not
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);


	// Function:		Shutdown Shader function
	// What it does:	Release objects/shaders
	// Takes in:		Nothing
	// Returns:			Nothing
	void ShutdownShader();


	// Function:		Output Shader Error Message
	// What it does:	This is called if a shader fails to compile, and passes in the error message to show what has gone wrong
	//					Writes the error message to the Shader-Error.txt file 
	// Takes in:		ID3D10Blob - pointer to the error message buffer, HWND - window, WCHAR - pointer to the shader file name
	// Returns:			Nothing
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);


	// Function:		Set Shader Parameters function
	// What it does:	This function gives the objects to the buffers in the shaders so that the shader can read the information from
	//					the application and sets the constant buffers in their place in the shader. It also sets up the samplers and textures
	//					needed in the pixel shader. 
	// Takes in:		ID3D11DeviceContext - pointer to the device context, D3DXMATRIX - world matrix, D3DXMATRIX view matrix, D3DXMATRIX projection matrix
	//					ID3D11ShaderResourceView - pointer to the texture
	// Returns:			True if successful, false if not
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture);


	// Function:		Render Shader function
	// What it does:	Sets the shaders and input layout in the device, and sets the samplers
	// Takes in:		ID3D11DeviceContext - pointer to the device context, int - index count
	// Returns:			Nothing
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

private:
	// Shader objects
	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;
	ID3D11InputLayout* m_Layout;
	ID3D11Buffer* m_MatrixBuffer;
	ID3D11Buffer* m_DistortionBuffer;
	ID3D11SamplerState* m_SampleState;

	// Variables to pass into the DistortionBuffer
	time_t m_DistortionTimer;
	float m_ElapsedTime;
	float m_DistortionAmount;
	float m_DistortionSeed;
};

#endif