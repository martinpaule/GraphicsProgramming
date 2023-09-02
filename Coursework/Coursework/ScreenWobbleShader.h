#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class ScreenWobbleShader : public BaseShader
{
private:
	struct ScreenSizeBufferType
	{
		float screenSizeX;
		float time;
		float wobbleStrength;
		float wobbleSpeed;
	};

public:

	ScreenWobbleShader(ID3D11Device* device, HWND hwnd,XMINT2 screenSize);
	~ScreenWobbleShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture,float dt);
	void haveDrink();
	void soberUp() {
		wobbleStrength = 0.0f;
		wobbleSpeed = 0.0f;
	}
private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* screenSizeBuffer;

	XMINT2 screenSizeXY;
	float elapsedTime = 0.0f;
	float wobbleStrength = 0.0f;
	float wobbleSpeed = 0.0f;
};