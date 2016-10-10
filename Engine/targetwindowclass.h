#ifndef _TARGETWINDOWCLASS_H_
#define _TARGETWINDOWCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>

////////////////////////////////////////////////////////////////////////////////
// Class name: TargetWindowClass
// Handles the creation of a target window object
// Adapted for application in previous module with help from Paul Robertson: Graphics Programming with Shaders
// Used to render a texture to a window
////////////////////////////////////////////////////////////////////////////////

class TargetWindowClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	TargetWindowClass();
	TargetWindowClass(const TargetWindowClass&);
	~TargetWindowClass();

	bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int windowWidth, int windowHeight, int centerX, int centerY);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device* device, int screenWidth, int screenHeight, int windowWidth, int windowHeight, int centerX, int centerY);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
};

#endif