// Tessellation mesh.
// Builds a simple triangle mesh for tessellation demonstration
// Overrides sendData() function for different primitive topology
#include "TessPlane.h"

// initialise buffers and load texture.
TessPlane::TessPlane(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMINT2 dimentions_,float tileSize_)
{
    
    dimentions = dimentions_;
    tileSize = tileSize_;
    
    initBuffers(device);

}

// Release resources.
TessPlane::~TessPlane()
{
    // Run parent deconstructor
    BaseMesh::~BaseMesh();
}

// Build triangle (with texture coordinates and normals).
void TessPlane::initBuffers(ID3D11Device* device)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;

    vertexCount = (dimentions.x + 1) * (dimentions.y + 1);
    indexCount = dimentions.x * dimentions.y * 4;

    vertices = new VertexType[vertexCount];
    indices = new unsigned long[indexCount];

    //create the vertex array
    float texStepX, texStepY;
    texStepX = 1.0f / dimentions.x;
    texStepY = 1.0f / dimentions.y;

    int vertIdx = 0;
    for (int z = 0; z < dimentions.y + 1; z++) {
        for (int x = 0; x < dimentions.x + 1; x++) {
            // Load the vertex array with data.
            vertices[vertIdx].position = XMFLOAT3(x* tileSize, 0, z* tileSize);  // Top.
            vertices[vertIdx].texture = XMFLOAT2(x * texStepX, z * texStepY);         // XMFLOAT2((float)x_ / (float)resolution, (float)z_ / (float)resolution);
            vertices[vertIdx].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
            vertIdx++;
        }
    }

    int indcsIdx = 0;
    for (int z = 0; z < dimentions.y; z++) {
        for (int x = 0; x < dimentions.x; x++) {
            indices[indcsIdx] = (z) * (dimentions.x + 1) + (x + 1);
            indcsIdx++;
            indices[indcsIdx] = (z + 1) * (dimentions.x + 1) + (x + 1);
            indcsIdx++;
            indices[indcsIdx] = (z + 1) * (dimentions.x + 1) + (x);
            indcsIdx++;
            indices[indcsIdx] = (z) * (dimentions.x + 1) + (x);
            indcsIdx++;
        }
    }



    // Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;
    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;
    // Now create the vertex buffer.
    device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

    // Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;
    // Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;
    // Create the index buffer.
    device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

    // Release the arrays now that the vertex and index buffers have been created and loaded.
    delete[] vertices;
    vertices = 0;
    delete[] indices;
    indices = 0;
}

// Override sendData() to change topology type. Control point patch list is required for tessellation.
void TessPlane::sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top)
{
    unsigned int stride;
    unsigned int offset;

    stride = sizeof(VertexType);
    offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    // Set the type of primitive that should be rendered from this vertex buffer, in this case control patch for tessellation.
    deviceContext->IASetPrimitiveTopology(top);
}

