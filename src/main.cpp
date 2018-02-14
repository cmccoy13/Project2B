/*
ZJ Wood CPE 471 Lab 3 base code
*/

#include <iostream>
#include <glad/glad.h>
#include <cstdlib>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "vector"

#include "WindowManager.h"
#include "Shape.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;
shared_ptr<Shape> shape;


double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime = glfwGetTime();
	double difference = actualtime - lasttime;
	lasttime = actualtime;
	return difference;
}
class camera
{
public:
	glm::vec3 pos, rot;
	int w, a, s, d;
	camera()
	{
		w = a = s = d = 0;
		pos = rot = glm::vec3(0, 0, 0);
	}
	glm::mat4 process(double ftime)
	{
		float speed = 0;
		if (w == 1)
		{
			speed = 1 * ftime;
		}
		else if (s == 1)
		{
			speed = -1 * ftime;
		}
		float yangle = 0;
		if (a == 1)
			yangle = -1 * ftime;
		else if (d == 1)
			yangle = 1 * ftime;
		rot.y += yangle;
		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		glm::vec4 dir = glm::vec4(0, 0, speed, 1);
		dir = dir*R;
		pos += glm::vec3(dir.x, dir.y, dir.z);
		glm::mat4 T = glm::translate(glm::mat4(1), pos);
		return R*T;
	}
};

camera mycam;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;
	std::shared_ptr<Program> prog2;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;
	GLuint BGVertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;
	GLuint IndexBufferID;
	GLuint BackgroundBufferID;
	GLuint BGIndexBufferID;
	GLuint VertexBufferColor;
	double posX = 0;
	double posY = 0;
	float rad = 1.0;
	float numVert = 160;
	float sc = 1.0;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	float getParametricX(float angle, float radius)
	{
		return radius*cos(angle);
	}

	float getParametricY(float angle, float radius)
	{
		return radius*sin(angle);
	}

	std::vector<float> circleVertices(float numVertices, float radius)
	{
		std::vector<float> vertices;

		for (int i = 0; i <= numVertices / 2; i++)
		{
			float angle = (3.14159 * 2) / (numVertices / 2);

			for (float j = 0.0; j < 2.0; j++)
			{
				vertices.push_back(getParametricX(i*angle, radius));
				vertices.push_back(getParametricY(i*angle, radius));
				vertices.push_back(j);
			}
		}
		return vertices;
	}

	std::vector<int> circleIndices(float numVertices)
	{
		std::vector<int> indices;

		for (int i = 0; i < numVertices-2; i++)
		{
			indices.push_back(i);
			indices.push_back(i+1);
			indices.push_back(i + 2);
		}

		indices.push_back(numVertices-2);
		indices.push_back(numVertices-1);
		indices.push_back(0);

		indices.push_back(numVertices-1);
		indices.push_back(0);
		indices.push_back(1);

		return indices;
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		
		float newPt[2];
		if (action == GLFW_PRESS)
		{
			/*glfwGetCursorPos(window, &posX, &posY);
			std::cout << "Pos X " << posX <<  " Pos Y " << posY << std::endl;

			//change this to be the points converted to WORLD
			//THIS IS BROKEN< YOU GET TO FIX IT - yay!
			newPt[0] = 0;
			newPt[1] = 0;

			std::cout << "converted:" << newPt[0] << " " << newPt[1] << std::endl;
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
			//update the vertex array with the updated points
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*6, sizeof(float)*2, newPt);
			glBindBuffer(GL_ARRAY_BUFFER, 0);*/
		}
	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{
		//SPHERE
		string resourceDirectory = "../resources";
		shape = make_shared<Shape>();
		shape->loadMesh(resourceDirectory + "/sphere.obj");
		shape->resize();
		shape->init();
		//END SPHERE

		//CYLINDER
		std::vector<float> vertices = circleVertices(numVert, rad);
		std::vector<int> indices = circleIndices(numVert);

		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		glGenBuffers(1, &VertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);

		glGenBuffers(1, &IndexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindVertexArray(0);
		//END SUN
	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.5, 0.5, 0.9, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addAttribute("vertPos");
		//prog2->addAttribute("vertNor");

		prog2 = std::make_shared<Program>();
		prog2->setVerbose(true);
		prog2->setShaderNames(resourceDirectory + "/shader_vertexSphere.glsl", resourceDirectory + "/shader_fragmentSphere.glsl");
		prog2->init();
		prog2->addUniform("P");
		prog2->addUniform("V");
		prog2->addUniform("M");
		prog2->addUniform("Center");
		//prog2->addUniform("campos");
		prog2->addAttribute("vertPos");
		prog2->addAttribute("vertNor");
	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{
		double frametime = get_last_elapsed_time();

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		static float sc = 1;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now
		glm::mat4 V, M, PP;
		V = glm::mat4(1);
		M = glm::mat4(1);
		// Apply orthographic projection.
		PP = glm::ortho(-1 * aspect, 1 * aspect, -1.0f, 1.0f, -2.0f, 100.0f);		
		if (width < height)
		{
		PP = glm::ortho(-1.0f, 1.0f, -1.0f / aspect,  1.0f / aspect, -2.0f, 100.0f);
		}

		static float w = 0.5;
		
		//CYLINDER
		glm::mat4 Rotate = glm::rotate(glm::mat4(1.0f), w, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.4f));
		glm::mat4 Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-0.1f, 0.0f, 0.0f));

		M = Trans*Rotate*Scale;

		prog->bind();

		//send the matrices to the shaders
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &PP[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindVertexArray(VertexArrayID);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID);
		glDrawElements(GL_TRIANGLES, numVert * 3, GL_UNSIGNED_INT, (void *)0);

		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, 0.0f, 0.0f));
		M = Trans * Rotate * Scale;
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, numVert * 3, GL_UNSIGNED_INT, (void*)0); //Right H

		//shape->draw(prog);

		prog->unbind();
		//END CYLINDER

		
		//SPHERE
		//w = 0.0;
		//w += 1.0 * frametime;//rotation angle
		float trans = 0;// sin(t) * 2;
		glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 1.0f, 0.0f));
		float angle = -3.1415926 / 2.0;
		glm::mat4 RotateX = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		
		M = TransZ * S;

		// Draw the box using GLSL.
		prog2->bind();

		V = mycam.process(frametime);
		//send the matrices to the shaders
		glUniformMatrix4fv(prog2->getUniform("P"), 1, GL_FALSE, &PP[0][0]);
		glUniformMatrix4fv(prog2->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog2->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform4f(prog2->getUniform("Center"), width / 2, height / 2, 0, 0);
		//glUniform3fv(prog->getUniform("campos"), 1, &mycam.pos[0]);

		shape->draw(prog2);

		prog2->unbind();	
		//END SPHERE
		
	}
	float p2wX(float xp)
	{
		return xp / 320 - 1;
	}

	float p2wY(float yp)
	{
		return -(yp / 240 - 1);
	}

};
//******************************************************************************************
int main(int argc, char **argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom();

	// Loop until the user closes the window.
	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
