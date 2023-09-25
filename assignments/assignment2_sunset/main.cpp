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
void renderUI();

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

//Sky Colors (Ref constants exist to add base color on color pickers)
float horizonTopColorDay[3] = { 0.5f, 0.5f, 1.0f };
const float HTCD_REF[3] = { 0.5f, 0.5f, 1.0f };
float horizonBotColorDay[3] = { 1.0f, 0.5f, 0.0f }; 
const float HBCD_REF[3] = { 1.0f, 0.5f, 0.0f };
float horizonTopColorNight[3] = { 0.125f, 0.125f, 0.25f }; 
const float HTCN_REF[3] = { 0.125f, 0.125f, 0.25f };
float horizonBotColorNight[3] = { 0.25f, 0.125f, 0.0f };
const float HBCN_REF[3] = { 0.25f, 0.125f, 0.0f };

//Sun Colors
float sunColorDay[4] = { 1.0f, 1.0f, 0.0f, 0.8f };
const float SCD_REF[4] = { 1.0f, 1.0f, 0.0f, 0.8f };
float sunColorNight[4] = { 0.25f, 0.25f, 0.0f, 0.8f };
const float SCN_REF[4] = { 0.25f, 0.25f, 0.0f, 0.8f };

//Sun Path Details
float sunOrbitPos[2] = { 0.25f, 0.0f };
float sunRadius = 0.5f;
float sunOrbitRadius = 2.0f;
float sunSpeed = 1.0f;

//Building Details
float buildingColorDay[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
const float BCN_REF[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
float buildingColorNight[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
const float BCD_REF[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
float buildings[10] = { 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f};

bool showImGUIDemoWindow = false;
bool drawWireframe = false;
int resolution[2] = { SCREEN_WIDTH, SCREEN_HEIGHT};



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

	srand((unsigned int)time(NULL));
	for (int i = 0; i < sizeof(buildings)/sizeof(float); i++)
	{
		buildings[i] = ew::RandomRange(0, 1);
		if (buildings[i] > 0.5)
			buildings[i] = ew::RandomRange(0, 1);
	}

	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Set uniforms
		shader.setFloat("_Speed", sunSpeed);
		shader.setVec2("_Resolution", resolution[0], resolution[1]);
		shader.setFloat("_Time", (float)glfwGetTime());
		
		//Sky Color
		shader.setVec3("_ColorTopRise", horizonTopColorDay[0], horizonTopColorDay[1], horizonTopColorDay[2]);
		shader.setVec3("_ColorBotRise", horizonBotColorDay[0], horizonBotColorDay[1], horizonBotColorDay[2]);
		shader.setVec3("_ColorTopSet", horizonTopColorNight[0], horizonTopColorNight[1], horizonTopColorNight[2]);
		shader.setVec3("_ColorBotSet", horizonBotColorNight[0], horizonBotColorNight[1], horizonBotColorNight[2]);
		
		//Sun Color
		shader.setVec4("_SunColorRise", sunColorDay[0], sunColorDay[1], sunColorDay[2], sunColorDay[3]);
		shader.setVec4("_SunColorSet", sunColorNight[0], sunColorNight[1], sunColorNight[2], sunColorNight[3]);
		//Sun Orbit Details
		shader.setVec2("_SunOrbitPos", sunOrbitPos[0], sunOrbitPos[1]);
		shader.setFloat("_SunRadius", sunRadius);
		shader.setFloat("_SunOrbitRadius", sunOrbitRadius);
		
		//Building details
		shader.setVec4("_BuildingColorRise", buildingColorDay[0], buildingColorDay[1], buildingColorDay[2], buildingColorDay[3]);
		shader.setVec4("_BuildingColorSet", buildingColorNight[0], buildingColorNight[1], buildingColorNight[2], buildingColorNight[3]);
		shader.setFloatArray("_BuildingHeight", buildings, 10);
		
		if (drawWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		renderUI();

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

int colorPickerChoice = 0;

void renderUI()
{
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();
	float w = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.y) * 0.50f;
	ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_DisplayRGB;

	ImGui::Begin("Settings");
	//ImGui::Checkbox("Show Demo Window", &showImGUIDemoWindow);
	if (ImGui::CollapsingHeader("Sky Settings"))
	{
		ImGui::SliderFloat("Speed", &sunSpeed, 0.0f, 25.0f);
		ImGui::Text("Color to Edit");
		ImGui::Combo(" ", &colorPickerChoice, "Day Color\0Night Color\0Sun Settings\0All Sky Settings");
		switch (colorPickerChoice)
		{
		case 0:
			flags |= ImGuiColorEditFlags_NoAlpha;
			ImGui::SetNextItemWidth(w);
			ImGui::ColorPicker4("Upper Color (Day)", horizonTopColorDay, flags, HTCD_REF);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(w);
			ImGui::ColorPicker4("Lower Color (Day)", horizonBotColorDay, flags, HBCD_REF);
			break;
		case 1:
			flags |= ImGuiColorEditFlags_NoAlpha;
			ImGui::SetNextItemWidth(w);
			ImGui::ColorPicker4("Upper Color (Night)", horizonTopColorNight, flags, HTCN_REF);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(w);
			ImGui::ColorPicker4("Lower Color (Night)", horizonBotColorNight, flags, HTCD_REF);
			break;
		case 2:
			flags |= ImGuiColorEditFlags_AlphaBar;
			ImGui::SetNextItemWidth(w);
			ImGui::ColorPicker4("Sun Color (Day)", sunColorDay, flags, SCD_REF);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(w);
			ImGui::ColorPicker4("Sun Color (Night)", sunColorNight, flags, SCN_REF);
			ImGui::SliderFloat("Sun Radius", &sunRadius, 0.0f, 5.0f, "%.3f");
			ImGui::SliderFloat2("Sun Path Center", sunOrbitPos, -2, 2);
			break;
		case 3:
		default:
			flags |= ImGuiColorEditFlags_NoAlpha;
			ImGui::SetNextItemWidth(w);
			ImGui::ColorPicker4("Upper Color (Day)", horizonTopColorDay, flags, HTCD_REF);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(w);
			ImGui::ColorPicker4("Lower Color (Day)", horizonBotColorDay, flags, HBCD_REF);
			ImGui::SetNextItemWidth(w);
			ImGui::ColorPicker4("Upper Color (Night)", horizonTopColorNight, flags, HTCN_REF);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(w);
			ImGui::ColorPicker4("Lower Color (Night)", horizonBotColorNight, flags, HBCN_REF);
			flags |= ImGuiColorEditFlags_AlphaBar; //This doesn't work for some reason. Neither does passing "(flags | ImGuiColorEditFlags_AlphaBar)" into the flags argument. Not sure why.
			ImGui::SetNextItemWidth(w);
			ImGui::ColorPicker4("Sun Color (Day)", sunColorDay, flags, SCD_REF);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(w);
			ImGui::ColorPicker4("Sun Color (Night)", sunColorNight, flags, SCN_REF);
			ImGui::SliderFloat("Sun Radius", &sunRadius, 0.0f, 5.0f, "%.3f");
			ImGui::SliderFloat2("Sun Path Center", sunOrbitPos, -2, 2);
			break;
		}
	}
	if (ImGui::CollapsingHeader("Building Settings"))
	{
		flags |= ImGuiColorEditFlags_AlphaBar;
		ImGui::SetNextItemWidth(w);
		ImGui::ColorPicker4("Building Color (Day)", buildingColorDay, flags, buildingColorDay);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(w);
		ImGui::ColorPicker4("Building Color (Night)", buildingColorNight, flags, buildingColorNight);
		ImGui::Text("Building Heights");
		for (int i = 0; i < sizeof(buildings) / sizeof(float); i++)
		{
			if (i > 0) ImGui::SameLine();
			ImGui::PushID(i); //Needs id here or else it makes all the sliders the same value
			ImGui::VSliderFloat("##v", ImVec2(40, 160), &buildings[i], 0.0f, 1.0f, "");
			ImGui::PopID();
		}
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

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	resolution[0] = width;
	resolution[1] = height;
}

