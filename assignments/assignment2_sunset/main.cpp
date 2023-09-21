#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <willowLib/shader.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

willowLib::Vertex vertices[4] = {
	 //x     y     z     u     v
	{-1.0, -1.0,  0.0,  0.0,  0.0}, //Bottom Left
	{ 1.0, -1.0,  0.0,  1.0,  0.0}, //Bottom Right
	{ 1.0,  1.0,  0.0,  1.0,  1.0}, //Top Right
	{-1.0,  1.0,  0.0,  0.0,  1.0}   //Top Left
};

unsigned int indicies[6] = {
	0, 1, 2,
	2, 3, 0
};

float horizonTopColorDay[3] = { 1.0f, 0.5f, 0.0f };
float horizonBotColorDay[3] = { 0.5f, 0.5f, 1.0f };
float horizonTopColorNight[3] = { 0.25f, 0.125f, 0.0f };
float horizonBotColorNight[3] = { 0.125f, 0.125f, 0.25f };

float sunSpeed = 1.0f;

float buildingColorDay[3] = { 0.2, 0.2, 0.2 };
float buildingColorNight[3] = { 0.1, 0.1, 0.1 };
float buildings[10] = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
bool showImGUIDemoWindow = false;
bool drawWireframe = false;
float resolution[2] = { SCREEN_WIDTH, SCREEN_HEIGHT};

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
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

	willowLib::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	shader.use();
	
	unsigned int vao = willowLib::createVAO(vertices, 4, indicies, 6);
	glBindVertexArray(vao);

	srand(time(NULL));
	for (int i = 0; i < sizeof(buildings)/sizeof(float); i++)
	{
		buildings[i] = ew::RandomRange(0, 1);
	}
	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Set uniforms
		shader.setVec3("_ColorTopRise", horizonTopColorDay[0], horizonTopColorDay[1], horizonTopColorDay[2]);
		shader.setVec3("_ColorBotRise", horizonBotColorDay[0], horizonBotColorDay[1], horizonBotColorDay[2]);
		shader.setVec3("_ColorTopSet", horizonTopColorNight[0], horizonTopColorNight[1], horizonTopColorNight[2]);
		shader.setVec3("_ColorBotSet", horizonBotColorNight[0], horizonBotColorNight[1], horizonBotColorNight[2]);
		shader.setVec3("_BuildingColorRise", buildingColorDay[0], buildingColorDay[1], buildingColorDay[2]);
		shader.setVec3("_BuildingColorSet", buildingColorNight[0], buildingColorNight[1], buildingColorNight[2]);
		shader.setFloatArray("_BuildingHeight", buildings, 10);
		shader.setFloat("_Speed", sunSpeed);
		shader.setVec2("_Resolution", resolution[0], resolution[1]);
		shader.setFloat("_Time", glfwGetTime());
		if (drawWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			ImGui::Checkbox("Show Demo Window", &showImGUIDemoWindow);
			if (ImGui::CollapsingHeader("Sky Settings"))
			{
				ImGui::ColorEdit3("Upper Color (Day)", horizonTopColorDay);
				ImGui::ColorEdit3("Lower Color (Day)", horizonBotColorDay);
				ImGui::ColorEdit3("Upper Color (Night)", horizonTopColorNight);
				ImGui::ColorEdit3("Lower Color (Night)", horizonBotColorNight);
				ImGui::SliderFloat("Speed", &sunSpeed, 0.0f, 25.0f);
			}
			if (ImGui::CollapsingHeader("Building Settings"))
			{
				ImGui::ColorEdit3("Building Color (Day)", buildingColorDay);
				ImGui::ColorEdit3("Building Color (Night)", buildingColorNight);
				ImGui::PushID("Building Heights");
				for (int i = 0; i < sizeof(buildings) / sizeof(float); i++)
				{
					if (i > 0) ImGui::SameLine();
					ImGui::PushID(i);
					ImGui::VSliderFloat("##v", ImVec2(18, 160), &buildings[i], 0.0f, 1.0f, "");
					if (ImGui::IsItemActive() || ImGui::IsItemHovered())
						ImGui::SetTooltip("%.3f", buildings[i]);
					ImGui::PopID();
				}
				ImGui::PopID();
			}
			if (ImGui::CollapsingHeader("Specs"))
			{
				ImGui::Text("Resolution: (%i) x (%i)", (int)resolution[0], (int)resolution[1]);
				ImGui::Text("Time: (%f)", glfwGetTime());
			}

			ImGui::End();
			if (showImGUIDemoWindow) {
				ImGui::ShowDemoWindow(&showImGUIDemoWindow);
			}

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
	resolution[0] = width;
	resolution[1] = height;
}

