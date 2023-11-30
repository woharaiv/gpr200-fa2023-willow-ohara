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

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

float prevTime;
ew::Vec3 bgColor = ew::Vec3(0.1f);

ew::Camera camera;
ew::CameraController cameraController;

struct Light {
	ew::Vec3 color = { 1, 1, 1 };
	ew::Transform transform;
};

const int MAX_LIGHTS = 4;
int numLights = MAX_LIGHTS;

Light lights[4];

struct Material {
	float ambientK = 0.1f;
	float diffuseK = 0.25f;
	float specular = 1.0f;
	float shininess = 128;
};

Material mat;

bool blinnPhong = true;

const int NUM_SHELLS = 16;

struct HairProps {
	float baseThreshold = 1.0f;
	float thresholdDecay = 0.05f;
	float shellSpacing = 0.01f;
};

HairProps grass;


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

	//Global settings
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	
	ew::Shader unlitShader("assets/unlit.vert", "assets/unlit.frag");
	
	unsigned int randomMap = ew::loadTexture("assets/random.jpg", GL_REPEAT, GL_NEAREST);
	unsigned int perlinMap = ew::loadTexture("assets/perlin.jpg", GL_REPEAT, GL_LINEAR);
	unsigned int green = ew::loadTexture("assets/green.jpg", GL_REPEAT, GL_LINEAR);

	ew::Shader hairShader("assets/hairRender.vert", "assets/hairRender.frag");

	//Create meshes
	ew::Mesh mossBallMesh(ew::createSphere(0.5f, 64));
	ew::Mesh grassyPlaneMesh(ew::createPlane(5, 5, 8));
	
	//Initialize transforms
	ew::Transform mossBallTransform[NUM_SHELLS];
	ew::Transform grassyPlaneTransform[NUM_SHELLS];
	for (int i = 0; i < NUM_SHELLS; i++)
	{
		mossBallTransform[i].position = ew::Vec3(0, 0, 0);
		grassyPlaneTransform[i].position = ew::Vec3(0, -1.5f, 0);
	}
	
	resetCamera(camera,cameraController);

	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		//Update camera
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
		cameraController.Move(window, &camera, deltaTime);

		//RENDER
		glClearColor(bgColor.x, bgColor.y,bgColor.z,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				
		//Hair rendering
		hairShader.use();
		//Universal (for now) hair renderer values
		hairShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
		hairShader.setFloat("_ShellSpacing", grass.shellSpacing);
		hairShader.setFloat("_BaseColorThreshold", grass.baseThreshold);
		hairShader.setFloat("_ColorThresholdDecay", grass.thresholdDecay);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, green);
		hairShader.setInt("_Texture", 0);
		glActiveTexture(GL_TEXTURE1);

		//MossyBall
		//Set values used by all shells
		glBindTexture(GL_TEXTURE_2D, perlinMap);
		hairShader.setInt("_HairMap", 1);
		//Loop for each shell
		for (int i = 0; i < NUM_SHELLS; i++)
		{
			hairShader.setInt("_ShellNumber", i);
			hairShader.setMat4("_Model", mossBallTransform[i].getModelMatrix());
			mossBallMesh.draw();
		}
		//GrassyPlane
		//Set values used by all shells
		glBindTexture(GL_TEXTURE_2D, randomMap);
		hairShader.setInt("_HairMap", 1);
		//Loop for each shell
		for (int i = 0; i < NUM_SHELLS; i++)
		{
			hairShader.setInt("_ShellNumber", i);
			hairShader.setMat4("_Model", grassyPlaneTransform[i].getModelMatrix());
			grassyPlaneMesh.draw();
		}



		unlitShader.use();
		unlitShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
		//
		//render using unlit shader here
		//



		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Hair Rendering"))
			{
				ImGui::SliderFloat("Base Threshold", &grass.baseThreshold, 0, 1);
				ImGui::SliderFloat("Threshold Decay", &grass.thresholdDecay, 0, 1);
				ImGui::SliderFloat("Shell Spacing", &grass.shellSpacing, 0, 0.1f);
			}
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
			ImGui::ColorEdit3("BG color", &bgColor.x);
			ImGui::SliderInt("Number of lights", &numLights, 0, MAX_LIGHTS);
			for (int i = 0; i < numLights; i++)
			{
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader("Light"))
				{
					ImGui::DragFloat3("Position", &lights[i].transform.position.x, 0.1f);
					ImGui::ColorEdit3("Color", &lights[i].color.x);
				}
				ImGui::PopID();
			}

			ImGui::SliderFloat("Ambient K", &mat.ambientK, 0.0f, 1.0f);
			ImGui::SliderFloat("Diffuse K", &mat.diffuseK, 0.0f, 1.0f);
			ImGui::SliderFloat("Shininess", &mat.shininess, 2.0f, 1024.0f);
			ImGui::SliderFloat("Specular", &mat.specular, 0.0f, 1.0f);
			ImGui::Checkbox("Using Blinn-Phong?", &blinnPhong);

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
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
	camera.position = ew::Vec3(0, 0, 5);
	camera.target = ew::Vec3(0);
	camera.fov = 60.0f;
	camera.orthoHeight = 6.0f;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.0f;
	camera.orthographic = false;

	cameraController.yaw = 0.0f;
	cameraController.pitch = 0.0f;
}


