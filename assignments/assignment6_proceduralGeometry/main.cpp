#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/texture.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <ew/camera.h>
#include <ew/cameraController.h>
#include <willowLib/procGen.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

float prevTime;

struct AppSettings {
	const char* shadingModeNames[6] = { "Solid Color","Normals","UVs","Texture","Lit","Texture Lit"};
	int shadingModeIndex = 5;

	ew::Vec3 bgColor = ew::Vec3(0.1f);
	ew::Vec3 shapeColor = ew::Vec3(1.0f);
	bool wireframe = false;
	bool drawAsPoints = false;
	bool backFaceCulling = false;

	//Euler angles (degrees)
	ew::Vec3 lightRotation = ew::Vec3(0, 0, 0);
}appSettings;

ew::Camera camera;
ew::CameraController cameraController;

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Camera", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Depth testing - required for depth sorting!
	glEnable(GL_DEPTH_TEST);
	glPointSize(3.0f);
	glPolygonMode(GL_FRONT_AND_BACK, appSettings.wireframe ? GL_LINE : GL_FILL);

	ew::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg",GL_REPEAT,GL_LINEAR);

	//Create cube
	willowLib::Cube testCube;
	//Create plane
	willowLib::Plane testPlane;
	//Create Cylinder
	willowLib::Cylinder testCyl;
	//Create Sphere
	willowLib::Sphere testSphere;
	//Create Torus
	willowLib::Torus testTorus;

	//Initialize transforms
	testCube.transform.position = ew::Vec3(-2, 0, 0);
	testPlane.transform.position = ew::Vec3(2, 0, 0);
	testCyl.transform.position = ew::Vec3(4, 0, 0);
	testSphere.transform.position = ew::Vec3(-4, 1, 0);
	testTorus.transform.position = ew::Vec3(0, 0, 0);
	resetCamera(camera,cameraController);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		cameraController.Move(window, &camera, deltaTime);

		//Render
		glClearColor(appSettings.bgColor.x, appSettings.bgColor.y, appSettings.bgColor.z,1.0f);

		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		shader.use();
		glBindTexture(GL_TEXTURE_2D, brickTexture);
		shader.setInt("_Texture", 0);
		shader.setInt("_Mode", appSettings.shadingModeIndex);
		shader.setVec3("_Color", appSettings.shapeColor);
		shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

		//Euler angels to forward vector
		ew::Vec3 lightRot = appSettings.lightRotation * ew::DEG2RAD;
		ew::Vec3 lightF = ew::Vec3(sinf(lightRot.y) * cosf(lightRot.x), sinf(lightRot.x), -cosf(lightRot.y) * cosf(lightRot.x));
		shader.setVec3("_LightDir", lightF);

		//Draw cube
		shader.setMat4("_Model", testCube.transform.getModelMatrix());
		testCube.getMesh().draw((ew::DrawMode)appSettings.drawAsPoints);

		//Draw plane
		shader.setMat4("_Model", testPlane.transform.getModelMatrix());
		testPlane.getMesh().draw((ew::DrawMode)appSettings.drawAsPoints);

		//Draw cylinder
		shader.setMat4("_Model", testCyl.transform.getModelMatrix());
		testCyl.getMesh().draw((ew::DrawMode)appSettings.drawAsPoints);

		//Draw sphere
		shader.setMat4("_Model", testSphere.transform.getModelMatrix());
		testSphere.getMesh().draw((ew::DrawMode)appSettings.drawAsPoints);

		//Draw torus
		shader.setMat4("_Model", testTorus.transform.getModelMatrix());
		testTorus.getMesh().draw((ew::DrawMode)appSettings.drawAsPoints);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Camera")) {
				ImGui::DragFloat3("Position", &camera.position.x, 0.1f);
				ImGui::DragFloat3("Target", &camera.target.x, 0.1f);
				ImGui::Checkbox("Orthographic", &camera.orthographic);
				if (camera.orthographic) {
					ImGui::DragFloat("Ortho Height", &camera.orthoHeight, 0.1f);
				}
				else {
					ImGui::SliderFloat("FOV", &camera.fov, 0.0f, 180.0f);
				}
				ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Far Plane", &camera.farPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Move Speed", &cameraController.moveSpeed, 0.1f);
				ImGui::DragFloat("Sprint Speed", &cameraController.sprintMoveSpeed, 0.1f);
				if (ImGui::Button("Reset")) {
					resetCamera(camera, cameraController);
				}
			}
			if (ImGui::CollapsingHeader("Rendering Settings"))
			{
				ImGui::ColorEdit3("BG color", &appSettings.bgColor.x);
				ImGui::ColorEdit3("Shape color", &appSettings.shapeColor.x);
				ImGui::Combo("Shading mode", &appSettings.shadingModeIndex, appSettings.shadingModeNames, IM_ARRAYSIZE(appSettings.shadingModeNames));
				if (appSettings.shadingModeIndex > 3) {
					ImGui::DragFloat3("Light Rotation", &appSettings.lightRotation.x, 1.0f);
				}
				ImGui::Checkbox("Draw as points", &appSettings.drawAsPoints);
				if (ImGui::Checkbox("Wireframe", &appSettings.wireframe)) {
					glPolygonMode(GL_FRONT_AND_BACK, appSettings.wireframe ? GL_LINE : GL_FILL);
				}
				if (ImGui::Checkbox("Back-face culling", &appSettings.backFaceCulling)) {
					if (appSettings.backFaceCulling)
						glEnable(GL_CULL_FACE);
					else
						glDisable(GL_CULL_FACE);
				}
			}
			if (ImGui::CollapsingHeader("Edit Cube"))
			{
				ImGui::PushID(0);
				ImGui::DragFloat3("Position", &testCube.transform.position.x, 0.1f);
				ImGui::DragFloat("Scale", &testCube.size, 0.05);
				ImGui::PopID();
			}
			if (ImGui::CollapsingHeader("Edit Plane"))
			{
				ImGui::PushID(1);
				ImGui::DragFloat3("Position", &testPlane.transform.position.x, 0.1f);
				ImGui::DragFloat("Scale", &testPlane.size, 0.05);
				ImGui::DragInt("Subdivisions", &testPlane.subdivisions, 1, 3, 128);
				ImGui::PopID();
			}
			if (ImGui::CollapsingHeader("Edit Cylinder"))
			{
				ImGui::PushID(2);
				ImGui::DragFloat3("Position", &testCyl.transform.position.x, 0.1f);
				ImGui::DragFloat("Height", &testCyl.height, 0.05);
				ImGui::DragFloat("Radius", &testCyl.radius, 0.05);
				ImGui::DragInt("Subdivisions", &testCyl.subdivisions, 1, 3, 128);
				ImGui::PopID();
			}
			if (ImGui::CollapsingHeader("Edit Sphere"))
			{
				ImGui::PushID(3);
				ImGui::DragFloat3("Position", &testSphere.transform.position.x, 0.1f);
				ImGui::DragFloat("Scale", &testSphere.radius, 0.05);
				ImGui::DragInt("Subdivisions", &testSphere.subdivisions, 1, 3, 128);
				ImGui::PopID();
			}
			if (ImGui::CollapsingHeader("Edit Torus"))
			{
				ImGui::PushID(4);
				ImGui::DragFloat3("Position", &testTorus.transform.position.x, 0.1f);
				ImGui::DragFloat("Radius", &testTorus.radius, 0.05);
				ImGui::DragFloat("Thickness", &testTorus.thickness, 0.05);
				ImGui::DragInt("Subdivisions", &testTorus.subdivisions, 1, 3, 128);
				ImGui::PopID();
			}
			ImGui::End();
			
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
	camera.position = ew::Vec3(0, 0, 3);
	camera.target = ew::Vec3(0);
	camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
	camera.fov = 60.0f;
	camera.orthoHeight = 6.0f;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.0f;
	camera.orthographic = false;

	cameraController.yaw = 0.0f;
	cameraController.pitch = 0.0f;
}


