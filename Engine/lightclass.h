#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_

#include <d3dx10math.h>

////////////////////////////////////////////////////////////////////////////////
// Class name: LightClass
// Handles the creation and settings of a light
// Created for application in previous module with help from Paul Robertson: Graphics Programming with Shaders
// Used with the light shader class to apply lighting through the shader
////////////////////////////////////////////////////////////////////////////////

class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetDiffuseColor(float, float, float, float);
	void SetAmbientColor(float, float, float, float);
	void SetSpecularColor(float, float, float, float);
	void SetPointLightPos(float, float, float);
	void SetDirection(float, float, float);
	void SetSpecularPower(float);
	void SetConstantFactor(float);
	void SetLinearFactor(float);
	void SetQuadraticFactor(float);
	void SetDistance(float);

	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR4 GetAmbientColor();
	D3DXVECTOR3 GetDirection();
	D3DXVECTOR4 GetSpecularColor();
	D3DXVECTOR3 GetPointLightPos();
	float GetSpecularPower();
	float GetConstantFactor();
	float GetLinearFactor();
	float GetQuadraticFactor();
	float GetDistance();

private:
	D3DXVECTOR4 m_diffuseColor;
	D3DXVECTOR4 m_ambientColor;
	D3DXVECTOR3 m_direction;
	D3DXVECTOR4 m_specularColor;
	D3DXVECTOR3 m_position;
	float m_specularPower;
	float m_constantFactor;
	float m_linearFactor;
	float m_quadraticFactor;
	float m_distance;
};

#endif