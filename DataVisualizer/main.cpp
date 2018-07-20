
#pragma once

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <string.h>

using namespace std;

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

//include camera
#include "Camera.h"

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

struct Point {
	long double time;
	long double x;
	long double y;
	long double z;


	Point* next;

	Point(long double t, long double X, long double Y, long double Z)
	{
		time = t;
		x = X;
		y = Y;
		z = Z;
		next = nullptr;
	}
};

Point* head;
Point* currentPoint;


#include <streambuf>
#include <cerrno>
#include <iomanip>      // std::setprecision


std::string get_file_contents(const char *filename)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in)
	{
		return(std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()));
	}
	throw(errno);
}
void removeCharsFromString(string &str, char* charsToRemove) {
	for (unsigned int i = 0; i < strlen(charsToRemove); ++i) {
		str.erase(remove(str.begin(), str.end(), charsToRemove[i]), str.end());
	}
}

double grabDouble(string &from)
{
	size_t indexOfNextWS = from.find(' ');
	string token = from.substr(0, indexOfNextWS);

	std::string::size_type sz;
	long double grabbedDoub = std::stod(token, &sz);
	if (from.length() > indexOfNextWS + 1)
		from.erase(0, indexOfNextWS + 1);
	return grabbedDoub;
}
// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

int main(void)
{
	string input = get_file_contents("input.json");

	size_t pos = 0;
	removeCharsFromString(input, "[],");
	long double t = grabDouble(input);
	long double x = grabDouble(input);
	long double y = grabDouble(input);
	long double z = grabDouble(input);
	head = new Point(t, x, y, z);
	currentPoint = head;


	size_t listLength = 1;


	cout << std::setprecision(20);
	do
	{
		t = grabDouble(input);
		x = grabDouble(input);
		y = grabDouble(input);
		z = grabDouble(input);
		currentPoint->next = new Point(t, x, y, z);
		currentPoint = currentPoint->next;
		listLength++;
	} while (pos = input.find(' ') != std::string::npos);

	currentPoint = head;
//	cout << "time: " << currentPoint->time << " x: " << currentPoint->x << " y: " << currentPoint->y << " z: " << currentPoint->z << '\n';
	while (currentPoint->next != nullptr)
	{
		currentPoint = currentPoint->next;
//		cout << "time: " << currentPoint->time << " x: " << currentPoint->x << " y: " << currentPoint->y << " z: " << currentPoint->z << '\n';
	}

//	cout << input << '\n';

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tutorial 04 - Colored Cube", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");


											   // Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
											   // A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
/*	static const GLfloat g_vertex_buffer_data[] = {
		-10.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};
	*/
	std::vector<float> g_vertex_buffer_data;
	std::vector<float> g_color_buffer_data;
	
	//I assumed MAX car speed is 280 km/hour, which is
	float const maxSpeed = 77.77778; // m/s
	
	float const minSpeed = 0;
	float const offsetX = 0.02f;
	float const offsetY = 0.04f;
	float prevX = 0;
	float prevY = 0;
	float prevZ = 0;
	//main location coord
	g_vertex_buffer_data.push_back(0);
	g_vertex_buffer_data.push_back(0);
	g_vertex_buffer_data.push_back(0);
	//support coord 1
	g_vertex_buffer_data.push_back(-offsetX);
	g_vertex_buffer_data.push_back(-offsetY);
	g_vertex_buffer_data.push_back(0);
	//support coord 2
	g_vertex_buffer_data.push_back(offsetX);
	g_vertex_buffer_data.push_back(-offsetY);
	g_vertex_buffer_data.push_back(0);

	g_color_buffer_data.push_back(0.5f);
	g_color_buffer_data.push_back(0.5f);
	g_color_buffer_data.push_back(0.5f);

	g_color_buffer_data.push_back(0.5f);
	g_color_buffer_data.push_back(0.5f);
	g_color_buffer_data.push_back(0.5f);

	g_color_buffer_data.push_back(0.5f);
	g_color_buffer_data.push_back(0.5f);
	g_color_buffer_data.push_back(0.5f);

	currentPoint = head->next;
	Point* prevPoint = head;
	while (currentPoint != nullptr) {
		prevX += (float)(currentPoint->x - prevPoint->x);
		prevY += (float)(currentPoint->y - prevPoint->y);
		prevZ += (float)(currentPoint->z - prevPoint->z);
		//main coord
		g_vertex_buffer_data.push_back(prevX); //subtract previous corresponding coordinate from each current and add to the array
		g_vertex_buffer_data.push_back(prevY);
		g_vertex_buffer_data.push_back(prevZ);
		//support coord 1
		g_vertex_buffer_data.push_back(prevX - offsetX);
		g_vertex_buffer_data.push_back(prevY - offsetY);
		g_vertex_buffer_data.push_back(prevZ);
		//support coord 2
		g_vertex_buffer_data.push_back(prevX + offsetX);
		g_vertex_buffer_data.push_back(prevY - offsetY);
		g_vertex_buffer_data.push_back(prevZ);
		
		g_color_buffer_data.push_back(0.5f);
		g_color_buffer_data.push_back(0.1f);
		g_color_buffer_data.push_back(0.4f);

		g_color_buffer_data.push_back(0.8f);
		g_color_buffer_data.push_back(0.2f);
		g_color_buffer_data.push_back(0.5f);

		g_color_buffer_data.push_back(0.9f);
		g_color_buffer_data.push_back(0.1f);
		g_color_buffer_data.push_back(0.3f);

		prevPoint = currentPoint;
		currentPoint = currentPoint->next; 
	}
	cout<< g_vertex_buffer_data.size();
	for (int i = 2; i < g_vertex_buffer_data.size(); i+=3)
	{
		cout << " X: " << g_vertex_buffer_data[i-2]<<" Y: "<< g_vertex_buffer_data[i-1]<< " Z: "<< g_vertex_buffer_data[i] << '\n';
	}
	/*
	// One color for each vertex. They were generated randomly.
	static const GLfloat g_color_buffer_data[] = {
		0.99f,  0.771f,  0.014f,
		0.99f,  0.99f,  0.436f,
		0.327f,  0.99f,  0.844f,
		0.822f,  0.99f,  0.201f,
		0.435f,  0.602f,  0.99,
		0.99,  0.99,  0.99f,
		0.597f,  0.770f,  0.761f,
		0.99,  0.436f,  0.730f,
		0.359f,  0.583f,  0.152f,
		0.483f,  0.596f,  0.789f,
		0.559f,  0.861f,  0.639f,
		0.195f,  0.548f,  0.859f,
		0.99f, 0.99f,  0.99f,
		0.99f, 0.99,  0.99f,
		0.99f,  0.99f,  0.99f,
		0.99f,  0.99f,  0.99f,
		0.99f,  0.99f,0.99f,
		0.99f, 0.99f,  0.99f,
		0.997f,  0.513f,  0.064f,
		0.945f,  0.719f,  0.592f,
		0.543f,  0.021f,  0.978f,
		0.279f,  0.317f,  0.505f,
		0.167f,  0.620f,  0.077f,
		0.347f,  0.857f,  0.137f,
		0.055f,  0.953f,  0.042f,
		0.714f,  0.505f,  0.345f,
		0.783f,  0.290f,  0.734f,
		0.99f,  0.99f,  0.99f,
		0.99f,  0.99f,  0.99f,
		0.99f,  0.99f,  0.99f,
		0.99f,  0.99f,  0.99f,
		0.99f,  0.99f,  0.99f,
		0.99f,  0.99f,  0.99f,
		0.673f,  0.211f,  0.457f,
		0.820f,  0.883f,  0.371f,
		0.982f,  0.099f,  0.879f
	};
	*/
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * g_vertex_buffer_data.size(), &g_vertex_buffer_data[0], GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * g_color_buffer_data.size(), &g_color_buffer_data[0], GL_STATIC_DRAW);


	do {
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		glm::mat4 Projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		glm::mat4 View = camera.GetViewMatrix();
		//// Model matrix : an identity matrix (model will be at the origin)
		glm::mat4 Model = glm::mat4(1.0f);
		
		glm::mat4 MVP = Projection * View * Model;
//		cam.ProcessKeyboard(Camera_Movement::FORWARD,deltaTime);
		//listen to the input

		//MOVE CAMERA
/*		
//		glfwSetCursorPosCallback(window, mouse_callback);
		
		processInput(window);

		//cam position
		glm::vec3 cameraPos = glm::vec3(4.0f, 3.0f, -3.0f);
		//cam orientation
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	//	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		
		
		//cam direction
		glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
		//right axis
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
		//up axis
		glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
		//rotate
		float radius = 10.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
*/
//		glm::mat4 View;
//		view = glm::lookAt(glm::vec3(camX, 3.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
		
//		View = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		//mod view proj
//		MVP = Projection * View * Model;

		
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, g_vertex_buffer_data.size()); // 12*3 indices starting at 0 -> 12 triangles
		
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}