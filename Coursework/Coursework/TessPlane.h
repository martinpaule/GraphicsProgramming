// Tessellation Mesh
// A simple triangle mesh used for tessellation demonstration.
// Different output type, instead of Triangle list, a 3 control point patch list.
// The tessellator uses these points to generate new geometry but sub-division.
// Due to change in geometry type the sendData() function has been overridden.

#ifndef _TESSPLANE_H_
#define _TESSPLANE_H_

#include "BaseMesh.h"

using namespace DirectX;

class TessPlane : public BaseMesh
{

public:
	TessPlane(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMINT2 dimentions_, float tileSize_);
	~TessPlane();

	void sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST) override;

protected:
	void initBuffers(ID3D11Device* device);
	XMINT2 dimentions;
	float tileSize;
};

#endif