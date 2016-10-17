#include "lightclass.h"


LightClass::LightClass()
{
}


LightClass::LightClass(const LightClass& other)
{
}


LightClass::~LightClass()
{
}


void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}


void LightClass::SetSpecularColor(float red, float green, float blue, float alpha)
{
	m_specularColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}


void LightClass::SetSpecularPower(float power)
{
	m_specularPower = power;
	return;
}


void LightClass::SetDirection(float x, float y, float z)
{
	m_direction = D3DXVECTOR3(x, y, z);
	return;
}


void LightClass::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void LightClass::SetPointLightPos(float x, float y, float z)
{
	m_position = D3DXVECTOR3(x, y, z);
	return;
}

void LightClass::SetConstantFactor(float factor)
{
	m_constantFactor = factor;
}

void LightClass::SetLinearFactor(float factor)
{
	m_linearFactor = factor;
}

void LightClass::SetQuadraticFactor(float factor)
{
	m_quadraticFactor = factor;
}

void LightClass::SetDistance(float distance)
{
	m_distance = distance;
}

D3DXVECTOR4 LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}


D3DXVECTOR4 LightClass::GetSpecularColor()
{
	return m_specularColor;
}


D3DXVECTOR4 LightClass::GetAmbientColor()
{
	return m_ambientColor;
}


float LightClass::GetSpecularPower()
{
	return m_specularPower;
}


D3DXVECTOR3 LightClass::GetDirection()
{
	return m_direction;
}

D3DXVECTOR3 LightClass::GetPointLightPos()
{
	return m_position;
}

float LightClass::GetConstantFactor()
{
	return m_constantFactor;
}

float LightClass::GetLinearFactor()
{
	return m_linearFactor;
}

float LightClass::GetQuadraticFactor()
{
	return m_quadraticFactor;
}

float LightClass::GetDistance()
{
	return m_distance;
}