
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
#include <streambuf>
#include <cerrno>
#include <iomanip>      // std::setprecision

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
#include "Parser.h"
#include "Shader.h"




Point* head;
Point* currentPoint;
vector <glm::vec3> cameraSpots; //store pointers to first points of path, to allow users to
size_t cameraSpotIndex = 0;
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
	//how else do I debug all these beatiful tiny fractions of space... 
	cout << std::setprecision(20);
	string folderPath = ("data/");
	head = parseFolderContent(folderPath);
	currentPoint = head;

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


	std::vector<float> g_vertex_buffer_data;
	std::vector<float> g_color_buffer_data;
	
	//I assumed MAX car speed is 60 km/hour, which is
	float const maxSpeed = 16.6667f ; // m/s 
	
	float const minSpeed = 0;
	
	glm::vec3 const minColor(0.329, 0.066, 0.450); //brown
	glm::vec3 const maxColor(1, 0.823, 0.101); //purple
	float const offsetX = 0.06f;
	float const offsetY = 0.12f;

//	float const offsetX = 1.0f;
//	float const offsetY = 2.0f;
	//current triangle
	//actual point
	glm::vec3 curMain = glm::vec3(0, 0, 0);
	//support points to make a triangle
	glm::vec3 curSup1 = glm::vec3(0, 0, 0);
	glm::vec3 curSup2 = glm::vec3(0, 0, 0);

	//previous triangle to draw polygons between
	glm::vec3 prevMain = glm::vec3(0, 0, 0);
	glm::vec3 prevSup1 = glm::vec3(0, 0, 0); //previous support point 1
	glm::vec3 prevSup2 = glm::vec3(0, 0, 0); //previous support point 2

	glm::vec3 velocity = glm::vec3(0, 0, 0);
	glm::vec3 rotation = glm::vec3(0, 0, 0);
	glm::vec3 unitRot = glm::vec3(0, 0, 0);
	

	velocity = glm::vec3(curMain.x - prevMain.x, curMain.y - prevMain.y, curMain.z - prevMain.z);
	rotation = glm::cross(velocity, glm::vec3(0, 1.0f, 0));
	unitRot = glm::normalize(rotation);

	//support coord 1
	curSup1.x = -unitRot.x * offsetX;
	curSup1.y = -offsetY;
	curSup1.z = -unitRot.z * offsetX;

	//support coord 2
	curSup2.x = unitRot.x * offsetX;
	curSup2.y = -offsetY;
	curSup2.z = unitRot.z * offsetX;

	//move cam to the 1st point

	currentPoint = head->next;
	Point* prevPoint = head;

//	cout << "x: " << head->x << " y: " << head->y << " z: " << head->z << '\n';
	while (currentPoint != NULL) {

		if (!currentPoint->newPath)
		{
			//main value of location
			curMain.x = prevMain.x + (float)(currentPoint->x - prevPoint->x);
			curMain.y = prevMain.y + (float)(currentPoint->y - prevPoint->y);
			curMain.z = prevMain.z + (float)(currentPoint->z - prevPoint->z);
		}
		else
		{
			//uncomment for loolz
//			curMain = glm::vec3(0,0,0);
		}
	
		//we want every triangle to be rotated in the direction of following one 
		velocity = glm::vec3(curMain.x - prevMain.x, curMain.y - prevMain.y, curMain.z - prevMain.z);
		rotation = glm::cross(velocity, glm::vec3(0, 1.0f, 0));
		unitRot = glm::normalize(rotation);

		//support coord 1
		curSup1.x = curMain.x - unitRot.x * offsetX;
		curSup1.y = curMain.y - offsetY;
		curSup1.z = curMain.z - unitRot.z * offsetX;

		//support coord 2
		curSup2.x = curMain.x + unitRot.x * offsetX;
		curSup2.y = curMain.y - offsetY;
		curSup2.z = curMain.z + unitRot.z * offsetX;

//		cout << "x: " << curMain.x << " y: " << curMain.y << " z: " << curMain.z << '\n';// << std::to_string(currentPoint);
	/*
		//main coord
		g_vertex_buffer_data.push_back(curMain.x); 
		g_vertex_buffer_data.push_back(curMain.y);
		g_vertex_buffer_data.push_back(curMain.z);

		//adding to support coord 1 vertex array
		g_vertex_buffer_data.push_back(curSup1.x);
		g_vertex_buffer_data.push_back(curSup1.y);
		g_vertex_buffer_data.push_back(curSup1.z);

		//adding support coord 2 to vertex array
		g_vertex_buffer_data.push_back(curSup2.x);
		g_vertex_buffer_data.push_back(curSup2.y);
		g_vertex_buffer_data.push_back(curSup2.z);
*/
		//don't render, if this is a new path
		if (!currentPoint->newPath)
		{

			//SIDE ONE
			g_vertex_buffer_data.push_back(curMain.x); //cur main
			g_vertex_buffer_data.push_back(curMain.y);
			g_vertex_buffer_data.push_back(curMain.z);

			g_vertex_buffer_data.push_back(prevMain.x); //prev main
			g_vertex_buffer_data.push_back(prevMain.y);
			g_vertex_buffer_data.push_back(prevMain.z);

			g_vertex_buffer_data.push_back(prevSup1.x); // prev sup 1
			g_vertex_buffer_data.push_back(prevSup1.y);
			g_vertex_buffer_data.push_back(prevSup1.z);

			g_vertex_buffer_data.push_back(curMain.x); // cur main
			g_vertex_buffer_data.push_back(curMain.y);
			g_vertex_buffer_data.push_back(curMain.z);

			g_vertex_buffer_data.push_back(prevSup1.x); // prev sup 1
			g_vertex_buffer_data.push_back(prevSup1.y);
			g_vertex_buffer_data.push_back(prevSup1.z);

			g_vertex_buffer_data.push_back(curSup1.x); // cur sup 1
			g_vertex_buffer_data.push_back(curSup1.y);
			g_vertex_buffer_data.push_back(curSup1.z);

			//SIDE TWO
			g_vertex_buffer_data.push_back(curMain.x); //cur main
			g_vertex_buffer_data.push_back(curMain.y);
			g_vertex_buffer_data.push_back(curMain.z);

			g_vertex_buffer_data.push_back(prevMain.x); //prev main
			g_vertex_buffer_data.push_back(prevMain.y);
			g_vertex_buffer_data.push_back(prevMain.z);

			g_vertex_buffer_data.push_back(prevSup2.x); // prev sup 2
			g_vertex_buffer_data.push_back(prevSup2.y);
			g_vertex_buffer_data.push_back(prevSup2.z);

			g_vertex_buffer_data.push_back(curMain.x); // cur main
			g_vertex_buffer_data.push_back(curMain.y);
			g_vertex_buffer_data.push_back(curMain.z);

			g_vertex_buffer_data.push_back(prevSup2.x); // prev sup 2
			g_vertex_buffer_data.push_back(prevSup2.y);
			g_vertex_buffer_data.push_back(prevSup2.z);

			g_vertex_buffer_data.push_back(curSup2.x); // cur sup 2
			g_vertex_buffer_data.push_back(curSup2.y);
			g_vertex_buffer_data.push_back(curSup2.z);

			//BOTTOM

			g_vertex_buffer_data.push_back(curSup1.x); //cur sup 1
			g_vertex_buffer_data.push_back(curSup1.y);
			g_vertex_buffer_data.push_back(curSup1.z);

			g_vertex_buffer_data.push_back(prevSup1.x); //prev sup 1
			g_vertex_buffer_data.push_back(prevSup1.y);
			g_vertex_buffer_data.push_back(prevSup1.z);

			g_vertex_buffer_data.push_back(prevSup2.x); // prev sup 2
			g_vertex_buffer_data.push_back(prevSup2.y);
			g_vertex_buffer_data.push_back(prevSup2.z);

			g_vertex_buffer_data.push_back(curSup1.x); // cur sup 1
			g_vertex_buffer_data.push_back(curSup1.y);
			g_vertex_buffer_data.push_back(curSup1.z);

			g_vertex_buffer_data.push_back(prevSup2.x); // prev sup 2
			g_vertex_buffer_data.push_back(prevSup2.y);
			g_vertex_buffer_data.push_back(prevSup2.z);

			g_vertex_buffer_data.push_back(curSup2.x); // cur sup 2
			g_vertex_buffer_data.push_back(curSup2.y);
			g_vertex_buffer_data.push_back(curSup2.z);
			
			//COLOR COLOR COLOR

			float velLerpVal = (glm::length(velocity) * 10 - minSpeed) / (maxSpeed - minSpeed); //~10Hz to m/s
			glm::vec3 curColor = ((maxColor - minColor) * velLerpVal) + minColor;

			for (int i = 0; i < 18; i++)
			{
				g_color_buffer_data.push_back(curColor.x);
				g_color_buffer_data.push_back(curColor.y);
				g_color_buffer_data.push_back(curColor.z);
			}
		}
		else
		{
			cameraSpots.push_back(glm::vec3(curMain.x, curMain.y, curMain.z));
		}
		prevPoint = currentPoint;
		currentPoint = currentPoint->next; 

		prevMain = curMain;
		prevSup1 = curSup1;
		prevSup2 = curSup2;

//		if (g_color_buffer_data.size() > 1000)
//			break;
	}

//	cout << "cam x " << camera.Position.x << " float y " << camera.Position.y << " float z " << camera.Position.z << '\n';

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * g_vertex_buffer_data.size(), &g_vertex_buffer_data[0], GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * g_color_buffer_data.size(), &g_color_buffer_data[0], GL_STATIC_DRAW);

	cout << " size " << g_vertex_buffer_data.size();
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

//used to ensure mouseclick happens only once
static int oldState = GLFW_RELEASE;
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	
	//ensure mouseclick happens only once
	int newState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (newState == GLFW_RELEASE && oldState == GLFW_PRESS) {
		cameraSpotIndex++;
		if (cameraSpotIndex > cameraSpots.size() - 1)
			cameraSpotIndex = 0;
		camera.Position = cameraSpots[cameraSpotIndex];
//		cout << "cam spot x: " << cameraSpots[cameraSpotIndex].x << '\n';
	} 
	oldState = newState;
	
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
//	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		
//	}

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