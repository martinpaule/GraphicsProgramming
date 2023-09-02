// tessellation shader.cpp
#include "tessWaveShader.h"


tessWaveShader::tessWaveShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"tessWave_vs.cso", L"tessWave_hs.cso", L"tessWave_ds.cso", L"tessWave_ps.cso");
}


tessWaveShader::~tessWaveShader()
{
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	if (layout)
	{
		layout->Release();
		layout = 0;
	}
	// Release the matrix constant buffer.
	if (ManipulationBuffer)
	{
		ManipulationBuffer->Release();
		ManipulationBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void tessWaveShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{

	srand(time(0));

	waveAmplitude = 0.0f;
	maxWaveAmplitude = 1.0f;
	waveFrequency = 0.8f;
	waveSpeed = 2.0f;
	waveHoldTime = 0.0f;
	waveTime = 0.0f;

	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC ManipulationBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Setup the description of the dynamic Manipulation constant buffer that is in the vertex shader.
	ManipulationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ManipulationBufferDesc.ByteWidth = sizeof(ManipulationBufferType);
	ManipulationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ManipulationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ManipulationBufferDesc.MiscFlags = 0;
	ManipulationBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&ManipulationBufferDesc, NULL, &ManipulationBuffer);

	// Setup light buffer
// Setup the description of the light dynamic constant buffer that is in the pixel shader.
// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	D3D11_BUFFER_DESC HSBufferDesc;
	HSBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	HSBufferDesc.ByteWidth = sizeof(HSBufferType);
	HSBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HSBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HSBufferDesc.MiscFlags = 0;
	HSBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&HSBufferDesc, NULL, &HSBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Setup light buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);


}

void tessWaveShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}


void tessWaveShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, XMFLOAT3 camera_pos, float tessFactor,float deltaTime, Light* light, bool normalDisp)
{

	waveTime += deltaTime;

	if (waveRiseFallSwitch == 0) {
		waveHoldTime += deltaTime;

		if (waveHoldTime > 1.5f) {
			waveHoldTime = 0.0f;
			if (waveAmplitude > maxWaveAmplitude) {
				waveRiseFallSwitch = -1;
			}
			else if (waveAmplitude < 0.0f) {
				waveRiseFallSwitch = 1;
			}
		}
	}
	else {
		waveAmplitude += deltaTime * waveRiseFallSwitch; //edit the wave (rise/fall)
	}

	if (waveAmplitude > maxWaveAmplitude) { //if wave reached max height
		if (waveRiseFallSwitch == 1) {
			waveRiseFallSwitch = 0;

		}

	}

	if (waveAmplitude < 0.0f) { //if wave fell fully

		if (waveRiseFallSwitch == -1) {  
			waveRiseFallSwitch = 0;

			maxWaveAmplitude = float(rand() % 100) / 100.0f + 0.5f; //random values for next waves
			waveFrequency = float(rand() % 50) / 100.0f + 0.5f;
			waveSpeed = float(rand() % 200) / 100.0f + 1.0f;
		}
	}

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	//send the vertex manipulation data to the domain shader
	ManipulationBufferType* manipPtr;
	deviceContext->Map(ManipulationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	manipPtr = (ManipulationBufferType*)mappedResource.pData;
	manipPtr->time = waveTime;
	manipPtr->amplitude = waveAmplitude;
	manipPtr->frequency = waveFrequency;
	manipPtr->speed = waveSpeed;
	deviceContext->Unmap(ManipulationBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &ManipulationBuffer);

	//Additional
	// Send light data to pixel shader
	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	//lightPtr->diffuse = light->getDiffuseColour();
	//lightPtr->direction = light->getDirection();
	lightPtr->diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	lightPtr->direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	lightPtr->normalDisp = normalDisp;
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(2, 1, &lightBuffer);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(HSBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	HSBufferType* tessBuffer_ = (HSBufferType*)mappedResource.pData;
	tessBuffer_->camera_position = camera_pos;
	tessBuffer_->tess_factor = tessFactor;

	// Set shader texture resource in the pixel shader.
	//deviceContext->PSSetShaderResources(0, 1, &texture);
	//deviceContext->DSSetShaderResources(1, 1, &texture);

	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->DSSetSamplers(1, 1, &sampleState);

	// Set shader texture resource in the pixel shader.
	//deviceContext->VSSetShaderResources(1, 1, &texture);

	deviceContext->VSSetSamplers(1, 1, &sampleState);

	deviceContext->Unmap(HSBuffer, 0);
	deviceContext->HSSetConstantBuffers(1, 1, &HSBuffer);
}


