// Light shader.h
// Basic single light shader setup
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class tessWaveShader : public BaseShader
{
private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* HSBuffer;
	ID3D11Buffer* ManipulationBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;

	float waveTime, waveHoldTime, waveAmplitude, maxWaveAmplitude, waveFrequency, waveSpeed;
	int waveRiseFallSwitch = 1;

	struct ManipulationBufferType
	{
		float time;
		float amplitude;
		float frequency;
		float speed;
	};
	struct HSBufferType
	{
		XMFLOAT3 camera_position;
		float tess_factor;
	};
	struct LightBufferType
	{
		XMFLOAT4 diffuse;
		XMFLOAT3 direction;
		float normalDisp;
	};
public:
	
	
	tessWaveShader(ID3D11Device* device, HWND hwnd);
	~tessWaveShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, XMFLOAT3 camera_pos, float tessFactor, float deltaTime, Light* light, bool normalDisp);
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);
	float getWaveAplit() { return waveAmplitude; }
	float getWaveTime() { return waveTime; }
	float getWaveHoldTime() { return waveHoldTime; }
			
};			
			