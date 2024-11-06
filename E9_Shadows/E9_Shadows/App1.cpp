// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Create Mesh object and shader object
	mesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	model = new AModel(renderer->getDevice(), "res/teapot.obj");
	textureMgr->loadTexture(L"brick", L"res/brick1.dds");

	cubeMesh = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext());
	sphereMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());

	// initial shaderss
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);

	// Variables for defining shadow map
	int shadowmapWidth = 4096;//resolution
	int shadowmapHeight = 4096;
	int sceneWidth = 100;
	int sceneHeight = 100;

	// This is your shadow map
	shadowMap = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
	shadowMap2 = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);

	// Configure directional light
	light = new Light();
	light->setAmbientColour(0.8f, 0.8f, 0.8f, 1.0f);
	light->setDiffuseColour(0.0f, 0.0f, 1.0f, 1.0f);//blue
	light->setDirection(1.0f, -0.7f, -0.7f);
	light->setPosition(-10.f, 10.f, -10.f);
	light->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);

	light2 = new Light();
	light2->setDiffuseColour(1.0f, 0.0f, 0.0f, 1.0f);//red
	light2->setDirection(-1.0f, -0.7f, -0.7f);
	light2->setPosition(10.f, 10.f, -10.f);
	light2->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);

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

	// Perform depth pass
	depthPass();
	
	// Render scene
	finalPass();

	return true;
}

void App1::depthPass()
{
	// Set the render target to be the render to texture.
	shadowMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// get the world, view, and projection matrices from the camera and d3d objects.
	light->generateViewMatrix();
	XMMATRIX lightViewMatrix = light->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	// Render floor
	mesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(0.f, 7.f, 5.f);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	XMMATRIX rotationMatrix = XMMatrixRotationY(rotation);
	worldMatrix = XMMatrixMultiply(worldMatrix, rotationMatrix);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	// Render model
	model->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), model->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-10.f, 7.f, 5.f);
	sphereMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(10.f, 7.f, 5.f);
	cubeMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());

	shadowMap2->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	light2->generateViewMatrix();
	XMMATRIX lightViewMatrix2 = light2->getViewMatrix();
	XMMATRIX lightProjectionMatrix2 = light2->getOrthoMatrix();

	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	mesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix2, lightProjectionMatrix2);
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(0.f, 7.f, 0.f);
	worldMatrix = XMMatrixMultiply(worldMatrix, rotationMatrix);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	model->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix2, lightProjectionMatrix2);
	depthShader->render(renderer->getDeviceContext(), model->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-10.f, 7.f, 5.f);
	sphereMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix2, lightProjectionMatrix2);
	depthShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(10.f, 7.f, 5.f);
	cubeMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix2, lightProjectionMatrix2);
	depthShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

//void App1::depthPass()
//{
//	// **First Depth Pass for Light 1**
//
//	// Bind the shadow map render target for light1
//	shadowMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());
//
//	// Generate the view matrix for light1
//	light->generateViewMatrix();
//	XMMATRIX lightViewMatrix = light->getViewMatrix();
//	XMMATRIX lightProjectionMatrix = light->getOrthoMatrix();
//
//	// Initialize the world matrix
//	XMMATRIX worldMatrix;
//	XMMATRIX rotationMatrix = XMMatrixRotationY(rotation);
//	XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
//
//	// **Render Floor**
//	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
//	mesh->sendData(renderer->getDeviceContext());
//	depthShader->setShaderParameters(
//		renderer->getDeviceContext(),
//		worldMatrix,
//		lightViewMatrix,
//		lightProjectionMatrix
//	);
//	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());
//
//	// **Render Model**
//	worldMatrix = renderer->getWorldMatrix();
//	worldMatrix = XMMatrixMultiply(scaleMatrix, worldMatrix);    // Scale
//	worldMatrix = XMMatrixMultiply(rotationMatrix, worldMatrix); // Rotate
//	worldMatrix = XMMatrixTranslation(0.f, 7.f, 5.f) * worldMatrix; // Translate
//	model->sendData(renderer->getDeviceContext());
//	depthShader->setShaderParameters(
//		renderer->getDeviceContext(),
//		worldMatrix,
//		lightViewMatrix,
//		lightProjectionMatrix
//	);
//	depthShader->render(renderer->getDeviceContext(), model->getIndexCount());
//
//	// **Render Sphere**
//	worldMatrix = renderer->getWorldMatrix();
//	worldMatrix = XMMatrixTranslation(-10.f, 7.f, 5.f);
//	sphereMesh->sendData(renderer->getDeviceContext());
//	depthShader->setShaderParameters(
//		renderer->getDeviceContext(),
//		worldMatrix,
//		lightViewMatrix,
//		lightProjectionMatrix
//	);
//	depthShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());
//
//	// **Render Cube**
//	worldMatrix = renderer->getWorldMatrix();
//	worldMatrix = XMMatrixTranslation(10.f, 7.f, 5.f);
//	cubeMesh->sendData(renderer->getDeviceContext());
//	depthShader->setShaderParameters(
//		renderer->getDeviceContext(),
//		worldMatrix,
//		lightViewMatrix,
//		lightProjectionMatrix
//	);
//	depthShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());
//
//	// **Second Depth Pass for Light 2**
//
//	// Bind the shadow map render target for light2
//	shadowMap2->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());
//
//	// Generate the view matrix for light2
//	light2->generateViewMatrix();
//	XMMATRIX lightViewMatrix2 = light2->getViewMatrix();
//	XMMATRIX lightProjectionMatrix2 = light2->getOrthoMatrix();
//
//	// Reuse rotation and scale matrices
//	// Note: Ensure 'rotation' variable is consistent between passes
//
//	// **Render Floor**
//	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
//	mesh->sendData(renderer->getDeviceContext());
//	depthShader->setShaderParameters(
//		renderer->getDeviceContext(),
//		worldMatrix,
//		lightViewMatrix2,
//		lightProjectionMatrix2
//	);
//	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());
//
//	// **Render Model**
//	worldMatrix = renderer->getWorldMatrix();
//	worldMatrix = XMMatrixMultiply(scaleMatrix, worldMatrix);    // Scale
//	worldMatrix = XMMatrixMultiply(rotationMatrix, worldMatrix); // Rotate
//	worldMatrix = XMMatrixTranslation(0.f, 7.f, 5.f) * worldMatrix; // Translate
//	model->sendData(renderer->getDeviceContext());
//	depthShader->setShaderParameters(
//		renderer->getDeviceContext(),
//		worldMatrix,
//		lightViewMatrix2,
//		lightProjectionMatrix2
//	);
//	depthShader->render(renderer->getDeviceContext(), model->getIndexCount());
//
//	// **Render Sphere**
//	worldMatrix = renderer->getWorldMatrix();
//	worldMatrix = XMMatrixTranslation(-10.f, 7.f, 5.f);
//	sphereMesh->sendData(renderer->getDeviceContext());
//	depthShader->setShaderParameters(
//		renderer->getDeviceContext(),
//		worldMatrix,
//		lightViewMatrix2,
//		lightProjectionMatrix2
//	);
//	depthShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());
//
//	// **Render Cube**
//	worldMatrix = renderer->getWorldMatrix();
//	worldMatrix = XMMatrixTranslation(10.f, 7.f, 5.f);
//	cubeMesh->sendData(renderer->getDeviceContext());
//	depthShader->setShaderParameters(
//		renderer->getDeviceContext(),
//		worldMatrix,
//		lightViewMatrix2,
//		lightProjectionMatrix2
//	);
//	depthShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());
//
//	// **Reset Render Target and Viewport**
//
//	// Set back buffer as render target and reset viewport
//	renderer->setBackBufferRenderTarget();
//	renderer->resetViewport();
//}

void App1::finalPass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	camera->update();
	rotation += 0.1;

	// get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	// Render floor
	mesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), shadowMap->getDepthMapSRV(), shadowMap2->getDepthMapSRV(), light, light2);
	shadowShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	// Render model
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(0.f, 7.f, 0.f);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	XMMATRIX rotationMatrix = XMMatrixRotationY(rotation);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	worldMatrix = XMMatrixMultiply(worldMatrix, rotationMatrix);
	model->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), shadowMap->getDepthMapSRV(), shadowMap2->getDepthMapSRV(), light, light2);
	shadowShader->render(renderer->getDeviceContext(), model->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-10.f, 7.f, 5.f);
	sphereMesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), shadowMap->getDepthMapSRV(), shadowMap2->getDepthMapSRV(), light, light2);
	shadowShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(10.f, 7.f, 5.f);
	cubeMesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), shadowMap->getDepthMapSRV(), shadowMap2->getDepthMapSRV(), light, light2);
	shadowShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());

	gui();
	renderer->endScene();
}



void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	// Controls for Light 1
	static float position[3] = { light->getPosition().x, light->getPosition().y, light->getPosition().z };
	ImGui::SliderFloat3("Light 1 Position", position, -50.f, 50.f);
	light->setPosition(position[0], position[1], position[2]);

	static float direction[3] = { light->getDirection().x, light->getDirection().y, light->getDirection().z };
	ImGui::SliderFloat3("Light 1 Direction", direction, -1.f, 1.f);
	light->setDirection(direction[0], direction[1], direction[2]);

	// Controls for Light 2
	static float position2[3] = { light2->getPosition().x, light2->getPosition().y, light2->getPosition().z };
	ImGui::SliderFloat3("Light 2 Position", position2, -50.f, 50.f);
	light2->setPosition(position2[0], position2[1], position2[2]);

	static float direction2[3] = { light2->getDirection().x, light2->getDirection().y, light2->getDirection().z };
	ImGui::SliderFloat3("Light 2 Direction", direction2, -1.f, 1.f);
	light2->setDirection(direction2[0], direction2[1], direction2[2]);
	

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

