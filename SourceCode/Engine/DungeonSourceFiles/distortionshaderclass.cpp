////////////////////////////////////////////////////////////////////////////////
// Filename: distortionshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "distortionshaderclass.h"


// Function:		Distortion Shader constructor
// What it does:	Initialises all variables and objects to their defaults
// Takes in:		Nothing
// Returns:			Nothing

DistortionShaderClass::DistortionShaderClass()
{
	m_VertexShader = 0;
	m_PixelShader = 0;
	m_Layout = 0;
	m_MatrixBuffer = 0;
	m_SampleState = 0;
	m_DistortionBuffer = 0;
	m_ElapsedTime = 0;
	m_DistortionAmount = 0.001f;
	m_DistortionSeed = 0.0f;
	m_DistortionTimer = 0.0f;
}


// Placeholder constructor
DistortionShaderClass::DistortionShaderClass(const DistortionShaderClass& other)
{
}


// Placeholder destructor
DistortionShaderClass::~DistortionShaderClass()
{
}


// Function:		Initialise function
// What it does:	Calls 'InitializeShader' passing in the device, window, and names of the distortion vertex and pixel shader
// Takes in:		ID3D11Device - pointer to the directx device, HWND - window
// Returns:			True if initialisation was successful, false if not

bool DistortionShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;		// For error checking
	
	// Initialize the distortion vertex and pixel shader
	result = InitializeShader(device, hwnd, L"distortion.vs", L"distortion.ps");
	if (!result)
	{
		return false;
	}

	// Return true if initialisation was successful
	return true;
}


// Function:		Shutdown function
// What it does:	Shuts down all objects in the shader and the vertex and pixel shader
// Takes in:		Nothing
// Returns:			Nothing

void DistortionShaderClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}


// Function:		Render function
// What it does:	This is called in the Application class to render objects using this shader. Takes in the objects and variables needed
//					for the distortion shaders and passes them into the SetShaderParameters function
// Takes in:		ID3D11DeviceContext - pointer to the device context, int - index count of the geometry, D3DXMATRIX - World Matrix, D3DXMATRIX, View Matrix
//					D3DXMATRIX - projection matrix, ID3D11ShaderResourceView - pointer to the texture, float - current distortion amount (which can be changed 
//					by user in application class)
// Returns:			True if successful, false if not 

bool DistortionShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
				D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float DistortionAmount)
{
	bool result;		// For error checking

	// Set the distortion amount to the value passed in so that this can be set in the DistortionBuffer 
	m_DistortionAmount = DistortionAmount;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	// Return true if successful
	return true;
}


// Function:		Initialize Shader function
// What it does:	This function compiles the vertex and pixel shader code using the shader file names and create the shaders using the appropriate
//					buffers. Also creates the vertex input layout for the information to be passed to the Vertex shader as input (position, texture co-ordinates,
//					and normals). Sets up description of buffers and samplers and creates them. 
// Takes in:		ID3D11Device - pointer to device, HWND - window, WCHAR* - pointer to the vertex shader filename, WCHAR* - pointer to the pixel shader filename
// Returns:			True if successful, false if not

bool DistortionShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC distortionBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Compile the distortion vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "DistortionVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
				       &vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the distortion pixel shader code.
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "DistortionPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
				       &pixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was  nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader);
	if(FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShader);
	if(FAILED(result))
	{
		return false;
	}

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the geometry class used and the vertex shader so that 
	// the vertices, textures and normals can be rendered correctly

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), 
					   &m_Layout);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_MatrixBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the dynamic distortion constant buffer that is in the pixel shader
	distortionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	distortionBufferDesc.ByteWidth = sizeof(DistortionBufferType);
	distortionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	distortionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	distortionBufferDesc.MiscFlags = 0;
	distortionBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = device->CreateBuffer(&distortionBufferDesc, NULL, &m_DistortionBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_SampleState);
	if (FAILED(result))
	{
		return false;
	}
	
	// Return true if successful
	return true;
}


// Function:		Shutdown Shader function
// What it does:	Release objects/shaders
// Takes in:		Nothing
// Returns:			Nothing

void DistortionShaderClass::ShutdownShader()
{
	// Release the sampler state.
	if(m_SampleState)
	{
		m_SampleState->Release();
		m_SampleState = 0;
	}

	// Release the matrix constant buffer.
	if(m_MatrixBuffer)
	{
		m_MatrixBuffer->Release();
		m_MatrixBuffer = 0;
	}

	// Release the distortion constant buffer
	if (m_DistortionBuffer)
	{
		m_DistortionBuffer->Release();
		m_DistortionBuffer = 0;
	}

	// Release the layout.
	if(m_Layout)
	{
		m_Layout->Release();
		m_Layout = 0;
	}

	// Release the pixel shader.
	if(m_PixelShader)
	{
		m_PixelShader->Release();
		m_PixelShader = 0;
	}

	// Release the vertex shader.
	if(m_VertexShader)
	{
		m_VertexShader->Release();
		m_VertexShader = 0;
	}

	return;
}


// Function:		Output Shader Error Message
// What it does:	This is called if a shader fails to compile, and passes in the error message to show what has gone wrong
//					Writes the error message to the Shader-Error.txt file 
// Takes in:		ID3D10Blob - pointer to the error message buffer, HWND - window, WCHAR - pointer to the shader file name
// Returns:			Nothing

void DistortionShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for(i=0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}


// Function:		Set Shader Parameters function
// What it does:	This function gives the objects to the buffers in the shaders so that the shader can read the information from
//					the application and sets the constant buffers in their place in the shader. It also sets up the samplers and textures
//					needed in the pixel shader. 
// Takes in:		ID3D11DeviceContext - pointer to the device context, D3DXMATRIX - world matrix, D3DXMATRIX view matrix, D3DXMATRIX projection matrix
//					ID3D11ShaderResourceView - pointer to the texture
// Returns:			True if successful, false if not

bool DistortionShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
					     D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	DistortionBufferType* dataPtr2;
	unsigned int bufferNumber;

	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_MatrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_MatrixBuffer);

	// Lock the distortion constant buffer so it can be written to.
	result = deviceContext->Map(m_DistortionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (DistortionBufferType*)mappedResource.pData;

	// Use a counter as the elapsed time to pass into the distortion shader
	// So that the shader uses this value to distort the texture differently every frame
	m_ElapsedTime += 0.1f;

	// Copy the distortion variables into the constant buffer.
	dataPtr2->distortion_amount = m_DistortionAmount;
	dataPtr2->seed = 1;
	dataPtr2->timer = m_ElapsedTime;
	dataPtr2->padding = 0;
	
	// Unlock the constant buffer.
	deviceContext->Unmap(m_DistortionBuffer, 0);

	// Set the position of the distortion constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the distortion constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_DistortionBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}


// Function:		Render Shader function
// What it does:	Sets the shaders and input layout in the device, and sets the samplers
// Takes in:		ID3D11DeviceContext - pointer to the device context, int - index count
// Returns:			Nothing

void DistortionShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_Layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_VertexShader, NULL, 0);
	deviceContext->PSSetShader(m_PixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_SampleState);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}