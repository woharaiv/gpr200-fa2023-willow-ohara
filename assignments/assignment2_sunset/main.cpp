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
	{-0.8, -0.7,  0.0,  0.0,  0.0}, //Bottom Left
	{ 0.8, -0.7,  0.0,  1.0,  0.0}, //Bottom Right
	{ 0.8,  0.7,  0.0,  1.0,  1.0}, //Top Right
	{-0.8,  0.7,  0.0,  0.0,  1.0}   //Top Left
};

unsigned int indicies[6] = {
	0, 1, 2,
	2, 3, 0
};

float triangleColor[3] = { 1.0f, 0.5f, 0.0f };
float triangleBrightness = 1.0f;
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

	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Set uniforms
		shader.setVec3("_Color", triangleColor[0], triangleColor[1], triangleColor[2]);
		shader.setFloat("_Brightness", triangleBrightness);
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
			ImGui::Checkbox("Wireframe View", &drawWireframe);
			ImGui::ColorEdit3("Color", triangleColor);
			ImGui::SliderFloat("Brightness", &triangleBrightness, 0.0f, 1.0f);
			if (ImGui::BeginMenu("Specs"))
			{
				ImGui::Text("Resolution: (%f) x (%f)", resolution[0], resolution[1]);
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

