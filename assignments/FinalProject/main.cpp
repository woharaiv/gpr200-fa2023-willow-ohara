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

struct HairProps {
	float thresholdDecay = 0.05f;
	float strandSlope = 0.45f;
	float shellSpacing = 0.01f;
	float hairHeight = 0.5f;
	float attenuation = 3.0f;
	int numShells = 64;
};

HairProps grass;
HairProps hair;
HairProps pack;

const std::string& filepath_shadow = "assets/models/Shadow/Shadow.obj";
const std::string& filepath_backpack = "assets/models/backpack/backpack.obj";

struct ModelSettings 
{
	ew::Vec3 position = ew::Vec3(2.0f, -1.0f, -0.5f);
	ew::Vec3 rotation = ew::Vec3(0.0f);
	//ew::Vec3(0.03f, 0.03f, 0.03f);
	ew::Vec3 scale = ew::Vec3(1.0f);
};

ModelSettings m_settings;
ModelSettings hairyModelSettings;

void drawHairMesh(ew::Shader& hairShader, ew::Camera& camera, ew::Mat4 modelMat4, HairProps properties, ew::Mesh& mesh, unsigned int texture, unsigned int hairMap);
void drawHairModel(ew::Shader& hairShader, ew::Camera& camera, ModelSettings modelSettings, HairProps properties, celLib::Model& model, unsigned int texture, unsigned int hairMap);

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

	//Setting structs
	hair.strandSlope = 1;
	hair.attenuation = 2;

	ew::Shader unlitShader("assets/unlit.vert", "assets/unlit.frag");
	
	unsigned int randomMap = ew::loadTexture("assets/random.jpg", GL_REPEAT, GL_NEAREST);
	unsigned int perlinMap = ew::loadTexture("assets/perlin.jpg", GL_REPEAT, GL_NEAREST);
	unsigned int green = ew::loadTexture("assets/green.jpg", GL_REPEAT, GL_LINEAR);
	unsigned int colorGrid = ew::loadTexture("assets/color_grid.jpg", GL_CLAMP_TO_EDGE, GL_NEAREST);
	unsigned int smile = ew::loadTexture("assets/smiley.jpg", GL_CLAMP_TO_EDGE, GL_NEAREST);
	unsigned int smileMap = ew::loadTexture("assets/smiley_hair.jpg", GL_CLAMP_TO_EDGE, GL_NEAREST);
	
	ew::Shader hairShader("assets/hairRender.vert", "assets/hairRender.frag");

	//Create meshes
	ew::Mesh smileyBallMesh(ew::createSphere(0.5f, 64));
	ew::Mesh grassyPlaneMesh(ew::createPlane(5, 5, 8));
	
	//Initialize transforms
	ew::Transform smileyBallTransform;
	ew::Transform grassyPlaneTransform;
	smileyBallTransform.position = ew::Vec3(0, 0, 0);
	grassyPlaneTransform.position = ew::Vec3(0, -1.5f, 0);
	
	
	resetCamera(camera,cameraController);

	celLib::Model testModel(filepath_backpack);
	//unsigned int modelTexture = ew::loadTexture("assets/models/Shadow/texture000.png", GL_REPEAT, GL_LINEAR);
	
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
		//Smiley face
		drawHairMesh(hairShader, camera, smileyBallTransform.getModelMatrix(), hair, smileyBallMesh, smile, smileMap);
		//Grassy Plane
		drawHairMesh(hairShader, camera, grassyPlaneTransform.getModelMatrix(), grass, grassyPlaneMesh, green, randomMap);
		//Hairy Backpack
		drawHairModel(hairShader, camera, hairyModelSettings, pack, testModel, green, randomMap);
		
		unlitShader.use();
		unlitShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
		//
		//render using unlit shader here
		//
		//unlitShader.setMat4("_Model", );
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D,modelTexture);
		/*ew::Mat4 tModel_T = ew::Translate(m_settings.position);
		ew::Mat4 tModel_R = ew::RotateX(m_settings.rotation.x) * ew::RotateY(m_settings.rotation.y) * ew::RotateZ(m_settings.rotation.z);
		ew::Mat4 tModel_S = ew::Scale(m_settings.scale);
		ew::Mat4 tModel = tModel_T * tModel_R * tModel_S;
		unlitShader.setMat4("_Model", tModel);
		testModel.Draw();*/



		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Grass"))
			{
				ImGui::PushID("Grass");
				ImGui::SliderInt("Shells", &grass.numShells, 1, MAX_SHELLS);
				ImGui::SliderFloat("Strand Slope", &grass.strandSlope, 0, 1);
				ImGui::SliderFloat("Shell Spacing", &grass.shellSpacing, 0, 0.1f);
				ImGui::SliderFloat("Grass Height", &grass.hairHeight, 0, 2);
				ImGui::DragFloat("Attenuation", &grass.attenuation, 0.01, 0);
				ImGui::PopID();
			}
			if (ImGui::CollapsingHeader("Hair"))
			{
				ImGui::PushID("Hair");
				ImGui::SliderInt("Shells", &hair.numShells, 1, MAX_SHELLS);
				ImGui::SliderFloat("Strand Slope", &hair.strandSlope, 0, 1);
				ImGui::SliderFloat("Shell Spacing", &hair.shellSpacing, 0, 0.1f);
				ImGui::SliderFloat("Hair Height", &hair.hairHeight, 0, 2);
				ImGui::DragFloat("Attenuation", &hair.attenuation, 0.01, 0);
				ImGui::PopID();
			}
			if (ImGui::CollapsingHeader("Backpack"))
			{
				ImGui::PushID("Pack");
				ImGui::SliderInt("Shells", &pack.numShells, 1, MAX_SHELLS);
				ImGui::SliderFloat("Strand Slope", &pack.strandSlope, 0, 1);
				ImGui::SliderFloat("Shell Spacing", &pack.shellSpacing, 0, 0.1f);
				ImGui::SliderFloat("Hair Length", &pack.hairHeight, 0, 2);
				ImGui::DragFloat("Attenuation", &pack.attenuation, 0.01, 0);
				ImGui::PopID();
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
			/*if (ImGui::CollapsingHeader("Model"))
			{
				ImGui::DragFloat3("Position", &m_settings.position.x, 0.05f );
				ImGui::DragFloat3("Rotation", &m_settings.rotation.x, 0.05f );
				ImGui::DragFloat3("Scale", &m_settings.scale.x, 0.01f);
			}*/
			if (ImGui::CollapsingHeader("Hairy Model"))
			{
				ImGui::PushID("Hairy");
				ImGui::DragFloat3("Position", &hairyModelSettings.position.x, 0.05f);
				ImGui::DragFloat3("Rotation", &hairyModelSettings.rotation.x, 0.05f);
				ImGui::DragFloat3("Scale", &hairyModelSettings.scale.x, 0.01f);
				ImGui::PopID();
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

void drawHairMesh(ew::Shader& hairShader, ew::Camera& camera, ew::Mat4 modelMat4, HairProps properties, ew::Mesh& mesh, unsigned int texture, unsigned int hairMap)
{
	hairShader.use();
	//Universal (for now) hair renderer values
	hairShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
	hairShader.setFloat("_ShellSpacing", properties.hairHeight / properties.numShells);
	hairShader.setInt("_ShellsRendering", properties.numShells);
	hairShader.setFloat("_ColorThresholdDecay", pow(properties.numShells, -1));
	hairShader.setFloat("_HairCutoffSlope", properties.strandSlope);
	hairShader.setFloat("_Attenuation", log(properties.attenuation) / log(properties.numShells));
	hairShader.setInt("_Texture", 0);
	hairShader.setInt("_HairMap", 1);
	hairShader.setVec3("_Scale", ew::Vec3(1.0));

	//SmileyBall
	//Set values used by all shells
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, hairMap);
	//Loop for each shell
	for (int i = 0; i < properties.numShells; i++)
	{
		hairShader.setInt("_ShellNumber", i);
		hairShader.setMat4("_Model", modelMat4);
		mesh.draw();
	}
}

void drawHairModel(ew::Shader& hairShader, ew::Camera& camera, ModelSettings modelSettings, HairProps properties, celLib::Model& model, unsigned int texture, unsigned int hairMap)
{
	hairShader.use();
	//Universal (for now) hair renderer values
	hairShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
	hairShader.setFloat("_ShellSpacing", properties.hairHeight / properties.numShells);
	hairShader.setInt("_ShellsRendering", properties.numShells);
	hairShader.setFloat("_ColorThresholdDecay", pow(properties.numShells, -1));
	hairShader.setFloat("_HairCutoffSlope", properties.strandSlope);
	hairShader.setFloat("_Attenuation", log(properties.attenuation) / log(properties.numShells));
	hairShader.setInt("_Texture", 0);
	hairShader.setInt("_HairMap", 1);
	hairShader.setVec3("_Scale", modelSettings.scale);

	//SmileyBall
	ew::Mat4 model_T = ew::Translate(modelSettings.position);
	ew::Mat4 model_R = ew::RotateX(modelSettings.rotation.x) * ew::RotateY(modelSettings.rotation.y) * ew::RotateZ(modelSettings.rotation.z);
	ew::Mat4 model_S = ew::Scale(modelSettings.scale);
	ew::Mat4 model_mat4 = model_T * model_R * model_S;
	//Set values used by all shells
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, hairMap);
	//Loop for each shell
	for (int i = 0; i < properties.numShells; i++)
	{
		hairShader.setInt("_ShellNumber", i);
		hairShader.setMat4("_Model", model_mat4);
		model.Draw();
	}
}


