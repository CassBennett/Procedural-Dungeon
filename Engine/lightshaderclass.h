#ifndef _LIGHTSHADERCLASS_H_
#define _LIGHTSHADERCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
#include "lightclass.h"
#include "cameraclass.h"
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: LightShaderClass
// Handles creation of the light pixel and vertex shaders giving light variables to the light shader for rendering
// Created for application in previous module with help from Paul Robertson: Graphics Programming with Shaders
// This is used to render the main parts of the dungeon
////////////////////////////////////////////////////////////////////////////////
class LightShaderClass
{
private:
		// MatrixBuffer struct that passes in World, View and Projection matrix into the vertex shader
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	// LightBuffer struct that passes in the light variables to the light pixel shader so that the shader changes 
	// the rendering in real time
	struct LightBufferType
	{
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR3 lightDirection;
		float padding;
		D3DXVECTOR4 position;
		float constantFactor;
		float linearFactor;
		float quadraticFactor;
		float distance;
		float specularPower;
		D3DXVECTOR3 padding2;
		D3DXVECTOR4 specularColor;
	};

	// CameraBuffer struct that passes in the camera variables to the light vertex shader so that the shader changes
	// the rendering due to the camera 
	 struct CameraBuffer
	 {
		 D3DXVECTOR3 cameraPosition;
		 float padding;
	 };

public:
	// Function:		Light Shader constructor
	// What it does:	Initialises all variables and objects to their defaults
	// Takes in:		Nothing
	// Returns:			Nothing
	LightShaderClass();

	// Placeholder constructor and destructor
	LightShaderClass(const LightShaderClass&);
	~LightShaderClass();

	// Function:		Initialise function
	// What it does:	Calls 'InitializeShader' passing in the device, window, and names of the light vertex and pixel shader
	// Takes in:		ID3D11Device - pointer to the directx device, HWND - window
	// Returns:			True if initialisation was successful, false if not
	bool Initialize(ID3D11Device*, HWND);

	// Function:		Shutdown function
	// What it does:	Shuts down all objects in the shader and the vertex and pixel shader
	// Takes in:		Nothing
	// Returns:			Nothing
	void Shutdown();

	// Function:		Render function
	// What it does:	This is called in the Application class to render objects using this shader. Takes in the objects and variables needed
	//					for the light shader and passes them into the SetShaderParameters function
	// Takes in:		ID3D11DeviceContext - pointer to the device context, int - index count of the geometry, D3DXMATRIX - World Matrix, D3DXMATRIX, View Matrix
	//					D3DXMATRIX - projection matrix, ID3D11ShaderResourceView - pointer to the texture, LightClass - pointer to the light used to render with,
	//					CameraClass - pointer to the camera used
	// Returns:			True if successful, false if not 
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, LightClass* light, CameraClass*);

private:
	// Function:		Initialize Shader function
	// What it does:	This function compiles the vertex and pixel shader code using the shader file names and create the shaders using the appropriate
	//					buffers. Also creates the vertex input layout for the information to be passed to the Vertex shader as input (position, texture co-ordinates,
	//					and normals). Sets up description of buffers and samplers and creates them. 
	// Takes in:		ID3D11Device - pointer to device, HWND - window, WCHAR* - pointer to the vertex shader filename, WCHAR* - pointer to the pixel shader filename
	// Returns:			True if successful, false if not
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);

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
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	// Function:		Set Shader Parameters function
	// What it does:	This function gives the objects to the buffers in the shaders so that the shader can read the information from
	//					the application and sets the constant buffers in their place in the shader. It also sets up the samplers and textures
	//					needed in the pixel shader. 
	// Takes in:		ID3D11DeviceContext - pointer to the device context, D3DXMATRIX - world matrix, D3DXMATRIX view matrix, D3DXMATRIX projection matrix
	//					ID3D11ShaderResourceView - pointer to the texture, LightClass - pointer to the light used to render with, CameraClass - pointer to the camera used
	// Returns:			True if successful, false if not
	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, LightClass* light, CameraClass*);
	
	// Function:		Render Shader function
	// What it does:	Sets the shaders and input layout in the device, and sets the samplers
	// Takes in:		ID3D11DeviceContext - pointer to the device context, int - index count
	// Returns:			Nothing
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;

	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_cameraBuffer;
};

#endif