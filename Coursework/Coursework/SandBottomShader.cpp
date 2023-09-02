#include "SandBottomShader.h"


SandBottomShader::SandBottomShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"sand_bottom_vs.cso", L"sand_bottom_hs.cso", L"sand_bottom_ds.cso", L"sand_bottom_ps.cso");
}


SandBottomShader::~SandBottomShader()
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
	if (HSBuffer)
	{
		HSBuffer->Release();
		HSBuffer = 0;
	}
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}
	if (layout)
	{
		layout->Release();
		layout = 0;
	}


	//Release base shader components
	BaseShader::~BaseShader();
}

void SandBottomShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{

	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC normDispBufDesc;

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

	// Setup normal Display buffer
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	normDispBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	normDispBufDesc.ByteWidth = sizeof(LightBufferType);
	normDispBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	normDispBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	normDispBufDesc.MiscFlags = 0;
	normDispBufDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &normDisplBuffer);
}

void SandBottomShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}


void SandBottomShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, XMFLOAT3 camera_pos, float tessFactor, ID3D11ShaderResourceView* texture, Light* light, ID3D11ShaderResourceView* depthMap, bool normalDisp)
{

	

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);
	XMMATRIX tLightViewMatrix = XMMatrixTranspose(light->getViewMatrix());
	XMMATRIX tLightProjectionMatrix = XMMatrixTranspose(light->getOrthoMatrix());

	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	dataPtr->lightView = tLightViewMatrix;
	dataPtr->lightProjection = tLightProjectionMatrix;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	//Additional
	// Send light data to pixel shader
	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->diffuse = light->getDiffuseColour();
	lightPtr->direction = light->getDirection();
	lightPtr->padding = 0.0f;

	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);


	//Additional
	// inform about displaying normals
	normDispBufferType* normDptr;
	deviceContext->Map(normDisplBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	normDptr = (normDispBufferType*)mappedResource.pData;
	normDptr->ShouldDisplayNormal = normalDisp;
	normDptr->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);

	deviceContext->Unmap(normDisplBuffer, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &normDisplBuffer);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(HSBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	HSBufferType* tessBuffer_ = (HSBufferType*)mappedResource.pData;
	tessBuffer_->camera_position = camera_pos;
	tessBuffer_->tess_factor = tessFactor;

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->DSSetShaderResources(1, 1, &texture);

	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->DSSetSamplers(1, 1, &sampleState);

	// Set shader texture resource in the pixel shader.
	deviceContext->VSSetShaderResources(1, 1, &texture);

	deviceContext->VSSetSamplers(1, 1, &sampleState);

	deviceContext->Unmap(HSBuffer, 0);
	deviceContext->HSSetConstantBuffers(1, 1, &HSBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(1, 1, &depthMap);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);
}


