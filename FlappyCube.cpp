// C++
#include <algorithm>
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <ctime>
#include <map>

// OpenGL
#define GLEW_STATIC
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <GLM/vec3.hpp>
#include <GLM/geometric.hpp>
#include <SOIL/SOIL.h>

// Custom
#include "Shader.hpp"
#include "Window.hpp"
#include "Collider.hpp"
#include "Texture.hpp"

// Defines
#define MAJOR 3
#define MINOR 3

// Global var
static bool gone = false;
static glm::vec3 temp_maxY(0), velocity(0), velocityUp(0, 0.05f, 0);
static glm::vec3 cube_pos(0);
static glm::vec3 acceleration_velocity(0, 0.0005f, 0);
static glm::vec3 acceleration(0, 4.05f, 0);
static double acceleration_time = 0.00001f;
static double stime = 0.01f;
static bool clicked = false;

// Main vars
static double sin_limit = 0.8, temp_sin;
static const int count_tubes = 4;
static const double start_from = 300, off_between = 230, right_edge = off_between * (count_tubes - 1.0f), speed = 1;
static const double point_of_score = 30 + speed;
static std::vector<std::pair<GLfloat, GLfloat>> offsets;

// Collliders
static std::vector<Collider> colliders;
static Collider cubeCol(30, 250, 35, 35);
Collider tubeCol(30, 250, 35, 35);

// Game vars
static double ofY = 70, ofYd = 100, height = 10000;
static unsigned bestcore = 0, score = 0, score_step = 20;
static bool scored_up = true;

// Function prototypes
void initGLFW();
void start();
void restart();

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// -------------------------------------------------------------
// Shaders

static const std::string cube_frag = R"(
#version 330

in vec2 txtrCoord;

out vec4 color;

uniform sampler2D txtr;

void main()
{
	color = texture(txtr, txtrCoord);
}
)";
static const std::string cube_vs = R"(
#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textureCoord;

out vec2 txtrCoord;

uniform vec3 newPos;

void main()
{
	gl_Position = vec4(position + newPos, 1.0f);
	
	txtrCoord = textureCoord;
}
)";
static const std::string tube_frag = R"(
#version 330

in vec2 txtrCoord;

out vec4 color;

uniform sampler2D txtr;

void main()
{
	color = texture(txtr, txtrCoord);
}
)";
static const std::string tubeDOWN_vs = R"(
#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 textureCoord;

out vec2 txtrCoord;

uniform vec2 offset;

void main()
{
	vec2 oldPos = vec2(pos.x + offset.x, pos.y + offset.y);
	gl_Position = vec4(mat2(1, 0, 0, -1) * oldPos, pos.z, 1.0f);

	txtrCoord = textureCoord;
}
)";
static const std::string tubeUP_vs = R"(
#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 textureCoord;

out vec2 txtrCoord;

uniform vec2 offset;

void main()
{
	gl_Position = vec4(pos.x + offset.x, pos.y + offset.y, pos.z, 1.0f);

	txtrCoord = textureCoord;
}
)";

static const std::string skybox_frag = R"(
#version 330

in vec2 txtrCoord;

out vec4 color;

uniform sampler2D txtr;

void main()
{
	color = texture(txtr, txtrCoord);
}
)";
static const std::string skybox_vs = R"(
#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textureCoord;

out vec2 txtrCoord;

void main()
{
	gl_Position = vec4(position, 1.0f);
	
	txtrCoord = textureCoord;
}
)";

// ~Shaders
// -------------------------------------------------------------



int main()
{
	std::cout << "Game: Flappy Cube\n\tAuthor: Sushchenko Dmytro\n\tVersion: 2.0\n\tStart of creation: 14.04.2021\n\tEnd of creation: 14.04.2021\n";
	initGLFW();
	start();

	std::cout << "\nThe program has closed successfully!\n";
	return 0;
}

// Funtion releases

void initGLFW()
{
	// Init GLFW
	glfwInit();
	// Setting GLFW
	// Set minimum required version for OpenGL. 
	// Major
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, MAJOR);
	// Minor
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MINOR);
	// Install profile for context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Turn off resizeble
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
}

inline double map(double value, std::pair<double, double> from, std::pair<double, double> to)
{
	return ((value - from.first) * (to.second - to.first)) / (from.second - from.first) + to.first;
}

inline double RandomFloat(double a, double b)
{
	double random = ((double)rand()) / (double)RAND_MAX;
	double diff = b - a;
	double r = random * diff;
	return std::sin(a + r);
}

inline void msleep(double seconds, Window& window)
{
	clock_t time_end = clock() + (seconds * 1000) * CLOCKS_PER_SEC / 1000;
	while (clock() < time_end)
	{
		glfwPollEvents();
		if (glfwWindowShouldClose(window.getWindow())) break;
	}
}

void start()
{
	Window window("Flappy Cube", 350, 500);
	window.setKeyCallBack(key_callback);

	// Shaders
	Shader tubeUP(tubeUP_vs, tube_frag);
	Shader tubeDOWN(tubeDOWN_vs, tube_frag);
	Shader cube(cube_vs, cube_frag);
	Shader skyBoxSh(skybox_vs, skybox_frag);

	// SkyBox
	GLfloat skyBox[] = {
		-1, 1, 0, 0, 0,
		 1, 1, 0, 1, 0,
		 1, -1, 0, 1, 1,
		-1, -1, 0, 0, 1
	};

	GLuint skyBox_indexes[] = {
		0, 1, 2,
		0, 2, 3
	};

	// Vertices
	GLfloat vertices[] = {
		map(115, {0, 300}, {-1, 1}), map(1000, {0, 500}, {-1, 1}), 0, 1.0f, 0.5f,
		map(115, {0, 300}, {-1, 1}), map(350, {0, 500}, {-1, 1}), 0, 1.0f, 0.0f,
		map(165, {0, 300}, {-1, 1}), map(350, {0, 500}, {-1, 1}), 0, 0.0f, 0.0f,
		map(165, {0, 300}, {-1, 1}), map(1000, {0, 500}, {-1, 1}), 0, 0.0f, 0.5f,

		map(105, {0, 300}, {-1, 1}), map(350, {0, 500}, {-1, 1}), 0, 1.0f, 1.0f,
		map(175, {0, 300}, {-1, 1}), map(350, {0, 500}, {-1, 1}), 0, 1.0f, 0.0f,
		map(175, {0, 300}, {-1, 1}), map(320, {0, 500}, {-1, 1}), 0, 0.0f, 0.0f,
		map(105, {0, 300}, {-1, 1}), map(320, {0, 500}, {-1, 1}), 0, 0.0f, 1.0f,

		map(30, {0, 300}, {-1, 1}), map(250, {0, 500}, {-1, 1}), 0, 1.0f, 1.0f,
		map(30, {0, 300}, {-1, 1}), map(285, {0, 500}, {-1, 1}), 0, 1.0f, 0.0f,
		map(60, {0, 300}, {-1, 1}), map(285, {0, 500}, {-1, 1}), 0, 0.0f, 0.0f,
		map(60, {0, 300}, {-1, 1}), map(250, {0, 500}, {-1, 1}), 0, 0.0f, 1.0f
	};

	//		Tube
	GLuint indexesTubeVertices[] = {
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7
	};
	//		Cube
	GLuint indexesCube[] = {
		0, 1, 2,
		0, 2, 3
	};

	// Load textures
	Texture tubeTxtr("src/textures/wall.png", SOIL_LOAD_RGBA), birdeTxtr("src/textures/bird.jpg"), skybox("src/textures/sky_box.png", SOIL_LOAD_RGBA);

	// Vertex Buffer Objects and Vertex Array Objects
	GLuint VBO, VAO, IBO, VBOc, VAOc, IBOc, VBOSkyBox, VAOSkyBox, IBOSkyBox;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &IBO);

	glGenBuffers(1, &VBOc);
	glGenVertexArrays(1, &VAOc);
	glGenBuffers(1, &IBOc);

	glGenBuffers(1, &VBOSkyBox);
	glGenVertexArrays(1, &VAOSkyBox);
	glGenBuffers(1, &IBOSkyBox);

	// Setting VAO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexesTubeVertices), indexesTubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	glBindVertexArray(VAOc);
	glBindBuffer(GL_ARRAY_BUFFER, VBOc);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOc);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexesCube), indexesCube, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)(40 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)(43 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	glBindVertexArray(VAOSkyBox);
	glBindBuffer(GL_ARRAY_BUFFER, VBOSkyBox);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyBox), skyBox, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOSkyBox);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyBox_indexes), skyBox_indexes, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	// Getting uniform vars
	GLuint loc_offsetUP = glGetUniformLocation(tubeUP.getID(), "offset");
	GLuint loc_offsetDOWN = glGetUniformLocation(tubeDOWN.getID(), "offset");
	GLuint loc_color = glGetUniformLocation(tubeUP.getID(), "newColor");
	GLuint loc_newPos = glGetUniformLocation(cube.getID(), "newPos");
	GLuint loc_tubeTextrUp = glGetUniformLocation(tubeUP.getID(), "txtr");
	GLuint loc_tubeTextrDown = glGetUniformLocation(tubeDOWN.getID(), "txtr");
	GLuint loc_birdTextr = glGetUniformLocation(cube.getID(), "txtr");
	GLuint loc_skyBoxTextr = glGetUniformLocation(skyBoxSh.getID(), "txtr");

	// Setup random
	srand(static_cast <unsigned> (time(0)));

	// Init tubes
	double temp_y = 0;
	offsets.push_back({ start_from, 0 });
	colliders.push_back({ start_from - 38.0f, 320, 63, 30 });
	colliders.push_back({ start_from - 38.0f, 152, 63, 30 });
	colliders.push_back({ start_from - 33.0f, 320, 5, height });
	colliders.push_back({ start_from - 33.0f, 152 - height, 5, height });
	for (int i = 1; i < count_tubes; ++i)
	{
		temp_sin = RandomFloat(0, 500);
		temp_y = std::abs(temp_sin) > sin_limit ? sin_limit : temp_sin;
		offsets.push_back({ start_from + off_between * i, temp_y });
		colliders.push_back({ start_from + off_between * i - 38.0f, map(temp_y, {-1, 1}, {0, 500}) + ofY, 63, 30 });
		colliders.push_back({ start_from + off_between * i - 38.0f, map(temp_y, {-1, 1}, {0, 500}) - ofYd, 63, 30 });
		colliders.push_back({ start_from + off_between * i - 33.0f, map(temp_y, {-1, 1}, {0, 500}) + ofY, 5, height });
		colliders.push_back({ start_from + off_between * i - 33.0f, map(temp_y, {-1, 1}, {0, 500}) - ofYd - height, 5, height });
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	int count_i = 0;
	while (!glfwWindowShouldClose(window.getWindow()))
	{
		// Background
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// SkyBoxs
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, skybox.getID());
		glUniform1i(loc_skyBoxTextr, 0);

		skyBoxSh.Use();
		glBindVertexArray(VAOSkyBox);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Tubes
		count_i = 0;
		for (auto& i : offsets)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tubeTxtr.getID());
			glUniform1i(loc_tubeTextrUp, 0);

			tubeUP.Use();
			glBindVertexArray(VAO);
			glUniform2f(loc_offsetUP, map(i.first, { 0, 300 }, { -1, 1 }), i.second);
			glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tubeTxtr.getID());
			glUniform1i(loc_tubeTextrDown, 0);

			tubeDOWN.Use();
			glBindVertexArray(VAO);
			glUniform2f(loc_offsetDOWN, map(i.first, { 0, 300 }, { -1, 1 }), -i.second);
			glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			if (gone)
			{
				if (std::abs(point_of_score - i.first) < speed) 
				{
					++score;
					std::cout << "Score: " << score << '\n';
				}

				i.first -= speed;

				if (i.first < -off_between)
				{
					i.first = right_edge;
					temp_sin = RandomFloat(-1, 1);
					i.second = std::abs(temp_sin) > sin_limit ? sin_limit : temp_sin;
					colliders[count_i].setX(right_edge - 38.0f);
					colliders[count_i].setY(map(i.second, { -1, 1 }, { 0, 500 }) + ofY);
					colliders[count_i + 1].setX(right_edge - 38.0f);
					colliders[count_i + 1].setY(map(i.second, { -1, 1 }, { 0, 500 }) - ofYd);
					colliders[count_i + 2].setX(right_edge - 33.0f);
					colliders[count_i + 2].setY(map(i.second, { -1, 1 }, { 0, 500 }) + ofY);
					colliders[count_i + 3].setX(right_edge - 33.0f);
					colliders[count_i + 3].setY(map(i.second, { -1, 1 }, { 0, 500 }) - ofYd - height);
				}
			}
			count_i += 4;
		}

		// Cube
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, birdeTxtr.getID());
		glUniform1i(loc_birdTextr, 0);

		cube.Use();
		glBindVertexArray(VAOc);
		glUniform3f(loc_newPos, cube_pos.x, cube_pos.y, cube_pos.z);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Modifications
		if (gone)
		{
			if (score != 0 && score % score_step == 0 && scored_up)
			{
				stime -= acceleration_time;
				scored_up = false;
			}
			else if (score % score_step != 0)
				scored_up = true;

			if (clicked && !(glm::length(temp_maxY - cube_pos) <= 0.00001f))
				cube_pos += velocityUp;
			else
			{
				cube_pos += (-velocity);
				velocity += acceleration_velocity;
				temp_maxY = glm::vec3(0);
				clicked = false;
			}

			cubeCol.setY(map(cube_pos.y, { -1, 1 }, { 0, 500 }));

			for (auto& i : colliders)
			{
				i.setX(i.getX() - speed);
				if (i.intersect(cubeCol)) 
				{
					gone = false;
					break;
				}
			}

			if (cube_pos.y < -1.0f) 
				gone = false;
		}

		// Swap buffers
		glfwSwapBuffers(window.getWindow());

		// Get input
		// Sleep thread
		msleep(stime, window);
	}
}

void restart()
{
	bestcore = score > bestcore ? score : bestcore;
	score = 0;
	std::cout << "Best Score: " << bestcore << '\n';

	cubeCol = Collider(30, 250, 35, 35);

	temp_maxY = velocity = glm::vec3(0);
	velocityUp = glm::vec3(0, 0.05f, 0);
	cube_pos = glm::vec3(0);
	acceleration_velocity = glm::vec3(0, 0.0005f, 0);
	acceleration = glm::vec3(0, 0.2f, 0);
	acceleration_time = 0.00001f;
	stime = 0.01f;
	clicked = false;

	offsets.clear();
	colliders.clear();

	offsets.push_back({ start_from, 0 });
	colliders.push_back({ start_from - 38.0f, 320, 63, 30 });
	colliders.push_back({ start_from - 38.0f, 152, 63, 30 });
	colliders.push_back({ start_from - 33.0f, 320, 5, height });
	colliders.push_back({ start_from - 33.0f, 152 - height, 5, height });
	for (int i = 1; i < count_tubes; ++i)
	{
		temp_sin = RandomFloat(0, 500);
		temp_sin = std::abs(temp_sin) > sin_limit ? sin_limit : temp_sin;
		offsets.push_back({ start_from + off_between * i, temp_sin });
		colliders.push_back({ start_from + off_between * i - 38.0f, map(temp_sin, {-1, 1}, {0, 500}) + ofY, 63, 30 });
		colliders.push_back({ start_from + off_between * i - 38.0f, map(temp_sin, {-1, 1}, {0, 500}) - ofYd, 63, 30 });
		colliders.push_back({ start_from + off_between * i - 33.0f, map(temp_sin, {-1, 1}, {0, 500}) + ofY, 5, height });
		colliders.push_back({ start_from + off_between * i - 33.0f, map(temp_sin, {-1, 1}, {0, 500}) - ofYd - height, 5, height });
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When the user presses ESC, we set the WindowShouldClose property to true,
	// and the application will close after that
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		// Restart game
		if (!gone)
			restart();

		// Start game
			gone = true;

		// Change cube pos
		velocity = glm::vec3(0);
		clicked = true;
		temp_maxY = (cube_pos + acceleration);
	}
}