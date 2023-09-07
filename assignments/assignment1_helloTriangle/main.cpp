#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>

unsigned int createVAO(float* vertexData, int numVertices);
unsigned int createShader(GLenum shaderType, const char* sourceCode);
unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);


const char* vertexShaderSource = R"(
	#version 450
	layout(location = 0) in vec3 vPos;
	layout(location = 1) in vec4 vColor;
	out vec4 Color;
	uniform float _Time;
	void main() 
	{
		Color = vColor;
		vec3 offset = vec3(0, sin(vPos.x + _Time), 0) * 0.5;
		gl_Position = vec4(vPos + offset, 1.0);
	}
)";

const char* fragmentShaderSource = R"(
	#version 450
	out vec4 FragColor;
	in vec4 Color;
	uniform float _Time;
	void main()
	{
		FragColor = Color * abs(sin(uTime));
	}
)";

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

float verticies[21] = {
	 //x   //y   //z   //r   //g   //b   //a
	-0.5, -0.5,  0.0,  1.0,  0.0,  0.0,  1.0,  //Left
	 0.5, -0.5,  0.0,  0.0,  1.0,  0.0,  1.0,  //Right
	 0.0,  0.5,  0.0,  0.0,  0.0,  1.0,  1.0   //Center
};

unsigned int vao, shaderProgram;

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

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	vao = createVAO(&verticies[0], 3);

	shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		float time = (float)glfwGetTime();
		int timeLoaction = glGetUniformLocation(shaderProgram, "_Time");
		glUniform1f(timeLoaction, time);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

unsigned int createVAO(float* vertexData, int numVertices)
{
	unsigned int vbo, vaoRef;
	//Define vbo as an array buffer, a special kind of buffer designed to supply vertex data
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//Allocates space for vertex data and sends vertex data to the GPU using the array buffer. GL_STATIC_DRAW declares that this data is static rather than dynamic.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	//Right now, our buffer is just filled with a random pile of bits that OpenGL doesn't know what to do with. The VAO will translate it into something it can work with.
	glGenVertexArrays(1, &vaoRef);
	glBindVertexArray(vaoRef);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//Defining the vertex attributes and identifies it as index [0]. There are [numVerticies] components of type [float], the values [are not] normalized (because that only matters for integers).
	//There's a stride of [numVerticies * size of float] bytes between each vertex, and this attribute is offset from the start of the vertex by [0].
		//Position
	glVertexAttribPointer(0, numVertices, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void*)0);
	glEnableVertexAttribArray(0);
		//Color
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void*)(sizeof(float)*numVertices));
	glEnableVertexAttribArray(1);

	return vaoRef;
}

unsigned int createShader(GLenum shaderType, const char* sourceCode)
{
	unsigned int shaderRef = NULL;
	glCreateShader(shaderType);
	glShaderSource(shaderRef, 1, &sourceCode, NULL);
	glCompileShader(shaderRef);
	int success;
	glGetShaderiv(shaderRef, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shaderRef, 512, NULL, infoLog);
		printf("Failed to compile shader: %s", infoLog);
	}
	return shaderRef;
}

unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
{
	unsigned int vertexShader, fragmentShader, shaderProgramRef;
	vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
	fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	//A program contains all the shader stages that happen in a draw call.
	shaderProgramRef = glCreateProgram();
	glAttachShader(shaderProgramRef, vertexShader);
	glAttachShader(shaderProgramRef, fragmentShader);
	//Link the stages
	glLinkProgram(shaderProgramRef);

	int success;
	glGetProgramiv(shaderProgramRef, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(shaderProgramRef, 512, NULL, infoLog);
		printf("Failed to link shader program: %s", infoLog);
		return 0;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgramRef;
}

