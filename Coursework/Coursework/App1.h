// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"
#include "shader_h_cpp_files/TextureShader.h"
#include "shader_h_cpp_files/HorizontalBlurShader.h"
#include "shader_h_cpp_files/MotionBlurShader.h"
#include <chrono>
#include "TessPlane.h"
#include "tessWaveShader.h"
#include "SandBottomShader.h"
#include "DepthDarkeningShader.h"
#include "VerticalBlurShader.h"
#include "ScreenWobbleShader.h"
#include "Fish.h"
#include "DepthShader.h"
#include "ShadowShader.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();

	void modelPass();

	void waveShaderPass();

	void pushRTT1toRTT2();
	void motionBlur();
	void diveFilter();
	void drunkShader();

	void renderPP();
	
	void textRender();
	void renderFishes();

	void depthPass(Light* light, ShadowMap* shadowMap);
	void shadowPass();

	void gui();

private:
	//general shaders
	TextureShader* textureShader;
	tessWaveShader* tsWaveShader;
	SandBottomShader* tsSandBottomShader;
	ShadowShader* shadowShader;
	DepthShader* depthShader;

	//PP shaders
	ScreenWobbleShader* screenWobbleShader;
	HorizontalBlurShader* horizontalBlurShader;
	VerticalBlurShader* verticalBlurShader;
	DepthDarkeningShader* depthDarkeningShader;

	MotionBlurShader* motionBlurShader;

	QuadMesh* consumeText;

	//objects
	PlaneMesh* waterSurface;
	TessPlane* tessWaterSurface;
	TessPlane* sandBottom;

	//PP vars
	OrthoMesh* orthoMesh;
	RenderTexture* RTT1;
	RenderTexture* RTT2;

	//models
	AModel* helicopterM;
	AModel* rumBottle;
	AModel* statue1;
	AModel* statue2;

	//variables needed for shadows
	PlaneMesh* shadowPlane;
	OrthoMesh* lightOrtoView;
	Light* light;
	ShadowMap* shadowMap_;
	//ShadowMap* shadowMap2;

	//vars
	float heliCircleMove = 0.0f; //angle around Y how the heli circles
	XMFLOAT3 rumPosition;
	

	int AppScreenWidth, AppScreenHeight;

	bool shouldMotionBlur = false;
	bool shouldDiveFilter = false;
	bool shouldDrunk = false;
	bool shouldNormDisp = false;
	bool hasSnorkel = false;
	
	Fish fishies[100];
	int fishCount = 50;
	float deltaTime_;


};



#endif