#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class MotionBlurShader : public BaseShader
{
private:
	struct ScreenSizeBufferType
	{
		float screenWidth;
		XMFLOAT2 blurDirection;
		float padding;
	};

public:

	MotionBlurShader(ID3D11Device* device, HWND hwnd, XMINT2 screenMidPos);
	~MotionBlurShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, float width,int mouseX,int mouseY, bool isLooking);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* screenSizeBuffer;

	int midX, midY;
	float timePassed = 0.0f;
};