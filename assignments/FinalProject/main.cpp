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

#include <assimp/Importer.hpp>
#include "celLib/model.h"

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

const int MAX_SHELLS = 256;
int numShells = 16;

struct HairProps {
	float thresholdDecay = 0.05f;
	float strandSlope = 0.1f;
	float shellSpacing = 0.01f;
	float hairHeight = 0.2f;
	float attenuation = 1.4f;
};

HairProps grass;
const std::string& filepath = "assets/models/Shadow/Shadow.obj";


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
	unsigned int perlinMap = ew::loadTexture("assets/perlin.jpg", GL_REPEAT, GL_NEAREST);
	unsigned int green = ew::loadTexture("assets/green.jpg", GL_REPEAT, GL_LINEAR);
	unsigned int colorGrid = ew::loadTexture("assets/color_grid.jpg", GL_CLAMP_TO_EDGE, GL_NEAREST);
	unsigned int smile = ew::loadTexture("assets/smiley.jpg", GL_CLAMP_TO_EDGE, GL_NEAREST);
	unsigned int smileMap = ew::loadTexture("assets/smiley_hair.jpg", GL_CLAMP_TO_EDGE, GL_NEAREST);

	ew::Shader hairShader("assets/hairRender.vert", "assets/hairRender.frag");

	//Create meshes
	ew::Mesh mossBallMesh(ew::createSphere(0.5f, 64));
	ew::Mesh grassyPlaneMesh(ew::createPlane(5, 5, 8));
	
	//Initialize transforms
	ew::Transform mossBallTransform[MAX_SHELLS];
	ew::Transform grassyPlaneTransform[MAX_SHELLS];
	for (int i = 0; i < MAX_SHELLS; i++)
	{
		mossBallTransform[i].position = ew::Vec3(0, 0, 0);
		grassyPlaneTransform[i].position = ew::Vec3(0, -1.5f, 0);
	}
	
	resetCamera(camera,cameraController);

	celLib::Model model(filepath);
	
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
		hairShader.setFloat("_ShellSpacing", grass.hairHeight/numShells);
		hairShader.setInt("_ShellsRendering", numShells);
		hairShader.setFloat("_ColorThresholdDecay", pow(numShells, -1));
		hairShader.setFloat("_HairCutoffSlope", grass.strandSlope);
		hairShader.setFloat("_Attenuation", log(grass.attenuation)/log(numShells));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, green);
		hairShader.setInt("_Texture", 0);
		
		//MossyBall
		//Set values used by all shells
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, smile);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, smileMap);
		hairShader.setInt("_HairMap", 1);
		//Loop for each shell
		for (int i = 0; i < numShells; i++)
		{
			hairShader.setInt("_ShellNumber", i);
			hairShader.setMat4("_Model", mossBallTransform[i].getModelMatrix());
			mossBallMesh.draw();
		}
		//GrassyPlane
		//Set values used by all shells
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, green);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, randomMap);
		hairShader.setInt("_HairMap", 1);
		//Loop for each shell
		for (int i = 0; i < numShells; i++)
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
		//unlitShader.setMat4("_Model", );



		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Hair Rendering"))
			{
				ImGui::SliderInt("Shells", &numShells, 1, MAX_SHELLS);
				ImGui::SliderFloat("Strand Slope", &grass.strandSlope, 0, 1);
				ImGui::SliderFloat("Shell Spacing", &grass.shellSpacing, 0, 0.1f);
				ImGui::SliderFloat("Grass Height", &grass.hairHeight, 0, 2);
				ImGui::SliderFloat("Attenuation", &grass.attenuation, -1, 2);
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


