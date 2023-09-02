#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class SandBottomShader : public BaseShader
{
private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* HSBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* normDisplBuffer;

	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;

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
		float padding;
	};
	struct normDispBufferType {
		float ShouldDisplayNormal;
		XMFLOAT3 padding;
	};
public:


	SandBottomShader(ID3D11Device* device, HWND hwnd);
	~SandBottomShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, XMFLOAT3 camera_pos, float tessFactor, ID3D11ShaderResourceView* texture, Light* light, ID3D11ShaderResourceView* depthMap, bool normalDisp);
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);


};
