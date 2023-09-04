// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	srand(time(NULL));
	
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	//load the textures
	textureMgr->loadTexture(L"heliTexture", L"res/models/72-helecopter/ec135.bmp");
	textureMgr->loadTexture(L"bottleTexture", L"res/models/Bottle_of_Rum/14056_Diffuse.jpg");
	textureMgr->loadTexture(L"statue1Texture", L"res/models/Statue1/statue.jpg");
	textureMgr->loadTexture(L"statue2Texture", L"res/models/Statue2/Statue.jpg");
	textureMgr->loadTexture(L"sandBottom", L"res/Heightmap/Sand_Bottom/sand_dunes.jpg");
	textureMgr->loadTexture(L"consumeText", L"res/images/consumeText.png");
	textureMgr->loadTexture(L"fish1", L"res/images/fish1.png");
	textureMgr->loadTexture(L"fish2", L"res/images/fish2.png");
	textureMgr->loadTexture(L"fish3", L"res/images/fish3.png");
	textureMgr->loadTexture(L"fish4", L"res/images/fish4.png");
	
	//initialize shaders
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	horizontalBlurShader = new HorizontalBlurShader(renderer->getDevice(), hwnd);
	verticalBlurShader = new VerticalBlurShader(renderer->getDevice(), hwnd);
	depthDarkeningShader = new DepthDarkeningShader(renderer->getDevice(), hwnd);
	motionBlurShader = new MotionBlurShader(renderer->getDevice(), hwnd,XMINT2(screenWidth/2,screenHeight/2));
	screenWobbleShader = new ScreenWobbleShader(renderer->getDevice(), hwnd,XMINT2(screenWidth,screenHeight));
	tsWaveShader = new tessWaveShader(renderer->getDevice(), hwnd);
	tsSandBottomShader = new SandBottomShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);


	// Create tesselated planes
	tessWaterSurface = new TessPlane(renderer->getDevice(), renderer->getDeviceContext(),XMINT2(40,40),2.5f);
	sandBottom = new TessPlane(renderer->getDevice(), renderer->getDeviceContext(),XMINT2(40,40),2.5f);

	//normal meshes
	shadowPlane = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());

	//initialize PP vars
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);	// Full screen size
	RTT1 = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	RTT2 = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	
	//initialize models
	helicopterM = new AModel(renderer->getDevice(), "res/models/72-helecopter/chopper.obj");
	rumBottle = new AModel(renderer->getDevice(), "res/models/Bottle_of_Rum/14056_Bottle_of_Rum_v1_L2.obj");
	statue1 = new AModel(renderer->getDevice(), "res/models/Statue1/12328_Statue_v1_L2.obj");
	statue2 = new AModel(renderer->getDevice(), "res/models/Statue2/12338_Statue_v1_L3.obj");
	

	consumeText = new QuadMesh(renderer->getDevice(),renderer->getDeviceContext());

	//initialize other variables
	AppScreenHeight = screenHeight;
	AppScreenWidth = screenWidth;
	camera->setRotation(0, 90, 0);
	rumPosition = XMFLOAT3(18.5f, -29.3f, 39.0f);

	// Variables for defining shadow map
	int shadowmapWidth = 1024;
	int shadowmapHeight = 1024;
	int sceneWidth = 100;
	int sceneHeight = 100;

	shadowMap_ = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);

	// Configure directional light
	light = new Light();
	//42, -25,65
	light->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	light->setDiffuseColour(0.8f, 0.8f, 0.8f, 1.0f);
	light->setDirection(0.0f, -0.5f, 0.5f);
	light->setPosition(42.f, 0.f, 40.f);
	light->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);

	lightOrtoView = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 2, screenHeight / 2, -screenWidth / 2, screenHeight / 2);	// Full screen size


	//set up fishes
	for (int i = 0; i < 100; i++) {
		fishies[i].setup();
	}


}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	
}


bool App1::frame()
{
	bool result;


	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{

	//update scene variables
	deltaTime_ = timer->getTime();
	heliCircleMove += deltaTime_/2;
	if (heliCircleMove > 360) {
		heliCircleMove = 0.0f;
	}


	


	//calculate shadow map
	depthPass(light, shadowMap_);
	
	//if any PP is switched on, set RTT as the RenderTarget
	if (shouldMotionBlur || shouldDiveFilter || shouldDrunk) {
		RTT1->setRenderTarget(renderer->getDeviceContext());
		RTT1->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 0.0f, 1.0f, 1.0f);
	}

	//begin rendering scene components
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	modelPass();

	shadowPass();


	waveShaderPass();

	textRender();

	renderFishes();

	//PP passes
	if (shouldMotionBlur) {
		motionBlur();
	}
	if (shouldDiveFilter) {
		diveFilter();
	}
	if (shouldDrunk) {
		drunkShader();
	}

	if (shouldMotionBlur || shouldDiveFilter || shouldDrunk) {
		renderPP();
	}

	gui();

	renderer->endScene();
	
	return true;
}

void App1::modelPass() {

	//declare world, view and projection matrices
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	//HELICOPTER-------------------------------------------------------
	//move translate
	XMMATRIX heliTranslateM = XMMatrixTranslation(50.0f, 0.0f, 50.0f);
	XMMATRIX heliRotateM = XMMatrixRotationRollPitchYaw(0, heliCircleMove, 0);
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationRollPitchYaw(0, 90, 0));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(2.0f, 2.0f, 2.0f));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(30.0f, 30.0f, 30.0f));
	worldMatrix = XMMatrixMultiply(worldMatrix, heliRotateM);
	worldMatrix = XMMatrixMultiply(worldMatrix, heliTranslateM);

	// Send geometry data, set shader parameters, render object with shader
	helicopterM->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"heliTexture"),shouldNormDisp);
	textureShader->render(renderer->getDeviceContext(), helicopterM->getIndexCount());

	//RUM BOTTLE---------------------------------------------------------------------------
	worldMatrix = renderer->getWorldMatrix();

	//apply matrix manipulations
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationRollPitchYaw(45, 0, 90));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(0.2f,0.2f,0.2f));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(rumPosition.x,rumPosition.y,rumPosition.z));

	// Send geometry data, set shader parameters, render object with shader
	rumBottle->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"bottleTexture"), shouldNormDisp);
	textureShader->render(renderer->getDeviceContext(), rumBottle->getIndexCount());


	
}


//depth function for the shadows
void App1::depthPass(Light* light, ShadowMap* shadowMap)
{
	// Set the render target to be the render to texture.
	shadowMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// get the world, view, and projection matrices from the camera and d3d objects.
	light->generateViewMatrix();
	XMMATRIX lightViewMatrix = light->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	//STATUE 1---------------------------------------------------------------------------
	worldMatrix = renderer->getWorldMatrix();
	
	//apply matrix manipulations
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationRollPitchYaw(1.6, 0, 0.5));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(0.05f, 0.05f, 0.05f));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(40.0f, -30.0f, 70.0f));
	
	// Send geometry data, set shader parameters, render object with shader
	statue1->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), statue1->getIndexCount());
	
	
	//STATUE 2------------------------------------------------------------------------------
	worldMatrix = renderer->getWorldMatrix();
	
	//apply matrix manipulations
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationRollPitchYaw(1.6, 0, 0));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(0.05f, 0.05f, 0.05f));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(46.0f, -30.0f, 67.0f));
	
	// Send geometry data, set shader parameters, render object with shader
	statue2->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), statue2->getIndexCount());


	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}


//shadows on selected geometry
void App1::shadowPass()
{


	//declare world, view and projection matrices
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	//render the sand 30 units down
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(0.0f, -30.0f, 0.0f));

	// Send geometry data, set shader parameters, render object with shader
	sandBottom->sendData(renderer->getDeviceContext());
	tsSandBottomShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, XMFLOAT3(camera->getPosition().x, 30.0f + camera->getPosition().y, camera->getPosition().z), 20, textureMgr->getTexture(L"sandBottom"), light, shadowMap_->getDepthMapSRV(), shouldNormDisp);
	tsSandBottomShader->render(renderer->getDeviceContext(), sandBottom->getIndexCount());

	//STATUE 1---------------------------------------------------------------------------
	worldMatrix = renderer->getWorldMatrix();

	//apply matrix manipulations
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationRollPitchYaw(1.6, 0, 0.5));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(0.05f, 0.05f, 0.05f));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(40.0f, -30.0f, 70.0f));


	statue1->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"statue1Texture"), shadowMap_->getDepthMapSRV(), light,shouldNormDisp);
	shadowShader->render(renderer->getDeviceContext(), statue1->getIndexCount());

	//STATUE 2------------------------------------------------------------------------------
	worldMatrix = renderer->getWorldMatrix();

	//apply matrix manipulations
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationRollPitchYaw(1.6, 0, 0));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(0.05f, 0.05f, 0.05f));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(46.0f, -30.0f, 67.0f));


	statue2->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"statue2Texture"), shadowMap_->getDepthMapSRV(), light, shouldNormDisp);
	shadowShader->render(renderer->getDeviceContext(), statue2->getIndexCount());


	
}

void App1::textRender() {

	//declare world, view and projection matrices
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	//get direction vector
	XMFLOAT2 faceDirV = XMFLOAT2(camera->getPosition().x - rumPosition.x, camera->getPosition().z - rumPosition.z);

	if (faceDirV.x * faceDirV.x + faceDirV.y * faceDirV.y < 100) {//if nearby

		float rotateAngle = atan(faceDirV.x / faceDirV.y);//calculate rotate angle
		if (faceDirV.y > 0) {
			rotateAngle += 3.14;//fix for flipped image
		}

		//apply matrix manipulations
		worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationRollPitchYaw(0, rotateAngle, 0));
		worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(2.0f, 2.0f, 2.0f));
		worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(rumPosition.x, rumPosition.y + 5.0f, rumPosition.z));

		// Send geometry data, set shader parameters, render object with shader
		consumeText->sendData(renderer->getDeviceContext());
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"consumeText"),0);
		textureShader->render(renderer->getDeviceContext(), consumeText->getIndexCount());


		if (input->isKeyDown('C')) {//have drink if X pressed
			screenWobbleShader->haveDrink();
			input->SetKeyUp('C');
			rumPosition.x = float(rand() % 900) / 10.0f + 5.0f; //set new bottle coordinates
			rumPosition.z = float(rand() % 900) / 10.0f + 5.0f;
			rumPosition.y = -29.0f;
		}
		
	}
	if (input->isKeyDown('X')) {//if X is pressed, reset wobbling values
		input->SetKeyUp('X');
		screenWobbleShader->soberUp();
	}
}


void App1::waveShaderPass() {

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Generate the view matrix based on the camera's position.
	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// Send geometry data, set shader parameters, render object with shader
	tessWaterSurface->sendData(renderer->getDeviceContext());
	tsWaveShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, camera->getPosition(), 20, deltaTime_, nullptr,shouldNormDisp);
	tsWaveShader->render(renderer->getDeviceContext(), tessWaterSurface->getIndexCount());


	
}

//Function that pushes RTT1 to RTT2 and then clears RTT1 (important to also delete the pointer reference!!!!!)
void App1::pushRTT1toRTT2() {

	RTT2->setRenderTarget(renderer->getDeviceContext());
	RTT2->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// RENDER THE RENDER TEXTURE SCENE
	// Requires 2D rendering and an ortho mesh.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	// Send geometry data, set shader parameters, render object with shader
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, RTT1->getShaderResourceView(),0);
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	delete(RTT1);
	RTT1 = new RenderTexture(renderer->getDevice(), AppScreenWidth, AppScreenHeight, SCREEN_NEAR, SCREEN_DEPTH);
}

//Function that simply draws PP to the back buffer
void App1::renderPP() {
	
	pushRTT1toRTT2();

	RTT1->setRenderTarget(renderer->getDeviceContext());
	RTT1->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f);
	renderer->setBackBufferRenderTarget();

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX baseViewMatrix = camera->getOrthoViewMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();

	// Send geometry data, set shader parameters, render object with shader
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, RTT2->getShaderResourceView(),0);
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);
}

void App1::motionBlur()
{
	pushRTT1toRTT2();

	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	RTT1->setRenderTarget(renderer->getDeviceContext());
	RTT1->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = renderer->getOrthoMatrix();

	// Send geometry data, set shader parameters, render object with shader
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	motionBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, RTT2->getShaderResourceView(), AppScreenWidth,input->getMouseX(), input->getMouseY(),input->isRightMouseDown());
	motionBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

}

void App1::drunkShader()
{
	pushRTT1toRTT2();

	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	RTT1->setRenderTarget(renderer->getDeviceContext());
	RTT1->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = renderer->getOrthoMatrix();

	// Send geometry data, set shader parameters, render object with shader
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	screenWobbleShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, RTT2->getShaderResourceView(),deltaTime_);
	screenWobbleShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);


}

void App1::diveFilter() {


	//HORIZONTAL BLUR -----------------------------------------------------
	pushRTT1toRTT2();

	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	//Blur Strength calculation
	int BS = 0;
	if (camera->getPosition().y < 0) {
		BS = abs(camera->getPosition().y) / 2;
	}

	RTT1->setRenderTarget(renderer->getDeviceContext());
	RTT1->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = renderer->getOrthoMatrix();

	// Send geometry data, set shader parameters, render object with shader
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	horizontalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, RTT2->getShaderResourceView(),XMFLOAT2(AppScreenWidth,AppScreenHeight),hasSnorkel,BS);
	horizontalBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	delete(RTT2);
	RTT2 = new RenderTexture(renderer->getDevice(), AppScreenWidth, AppScreenHeight, SCREEN_NEAR, SCREEN_DEPTH);


	//----------------------VERTICAL BLUR
	pushRTT1toRTT2();

	RTT1->setRenderTarget(renderer->getDeviceContext());
	RTT1->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f);
	// Send geometry data, set shader parameters, render object with shaderer->setBackBufferRenderTarget();

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = renderer->getOrthoMatrix();

	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	verticalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, RTT2->getShaderResourceView(), XMFLOAT2(AppScreenWidth, AppScreenHeight), hasSnorkel,BS);
	verticalBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	delete(RTT2);
	RTT2 = new RenderTexture(renderer->getDevice(), AppScreenWidth, AppScreenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	//--------------------------------DEPTH DARKENING SHADER

	pushRTT1toRTT2();

	RTT1->setRenderTarget(renderer->getDeviceContext());
	RTT1->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = renderer->getOrthoMatrix();

	// Send geometry data, set shader parameters, render object with shader
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	depthDarkeningShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, RTT2->getShaderResourceView(), camera->getPosition().y);
	depthDarkeningShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	
}

void App1::renderFishes() {
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// 1. Sort the fishes by depth (distance from the camera)
	std::sort(fishies, fishies + fishCount, [&viewMatrix](const Fish& a, const Fish& b) {
		XMVECTOR aPos = XMVectorSet(a.currentLocation.x, a.currentLocation.y, a.currentLocation.z, 1.0f);
		XMVECTOR bPos = XMVectorSet(b.currentLocation.x, b.currentLocation.y, b.currentLocation.z, 1.0f);
		aPos = XMVector3TransformCoord(aPos, viewMatrix);
		bPos = XMVector3TransformCoord(bPos, viewMatrix);
		return XMVectorGetZ(aPos) > XMVectorGetZ(bPos);
	});

	for (int i = 0; i < fishCount; i++) {
		//update fish movement and billboarding rotation
		fishies[i].update(camera->getPosition(), deltaTime_);

		//apply matrix manipulations (translation, billboard rotation)
		worldMatrix = renderer->getWorldMatrix();
		worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationRollPitchYaw(0, fishies[i].Yrotation, 0));
		worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(fishies[i].currentLocation.x, fishies[i].currentLocation.y, fishies[i].currentLocation.z));


		consumeText->sendData(renderer->getDeviceContext());


		//evenly distribute fish texture (tried writing this bit nicer but even if i save the name as wtring and then use .c_str() into wchar it lets me pass the argument but doesn't work, this was the best solution)
		if (fishies[i].mySprite == 0) {
			textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"fish1"),0);
		}
		else if (fishies[i].mySprite == 1) {
			textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"fish2"),0);
		}
		else if (fishies[i].mySprite == 2) {
			textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"fish3"),0);
		}
		else {
			textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"fish4"),0);
		}
		
		
		textureShader->render(renderer->getDeviceContext(), consumeText->getIndexCount());

	}

	

}

//IMGUI
void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);




	// Build UI
	if (ImGui::CollapsingHeader("General Information")) {
		ImGui::Text("FPS: %.2f", timer->getFPS());
		ImGui::Text("MousePos: X:%d  Y:%d", input->getMouseX(), input->getMouseY());
		ImGui::Text("Camera Position: X:%f  Y:%f  Z:%f", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
	}
	if (ImGui::CollapsingHeader("Controls")) {
		ImGui::Text("Movement: WASD QE (+Shift to 'sprint')");
		ImGui::Text("Camera: RMB + Mouse");
		ImGui::Text("Consume: C");
		ImGui::Text("Reset Drunk Wobble ('sober up'): X");
	}
	if (ImGui::CollapsingHeader("Post Processes")) {
		ImGui::Checkbox("Motion Blur", &shouldMotionBlur);
		ImGui::Checkbox("Drunk Filter", &shouldDrunk);
		ImGui::Checkbox("Dive Filter", &shouldDiveFilter);
		ImGui::Checkbox("Snorkel (for Dive Filter)", &hasSnorkel);
	}
	if (ImGui::CollapsingHeader("Other Settings")) {
		ImGui::SliderInt("fish count", &fishCount, 0, 100);
		ImGui::Checkbox("Normal texturing", &shouldNormDisp);
		ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	}

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

