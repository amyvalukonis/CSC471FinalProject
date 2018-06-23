/**
 * Base code
 * Draws two meshes and one ground plane, one mesh has textures, as well
 * as ground plane.
 * Must be fixed to load in mesh with multiple shapes (dummy.obj)
 */

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "WindowManager.h"
#include "GLTextureWriter.h"
#include <stb/stb_image.h>
#include <math.h> 

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;
	std::shared_ptr<Program> prog2; 
	std::shared_ptr<Program> texProg;
	std::shared_ptr<Program> texProg2;
	std::shared_ptr<Program> gaussProg; 
	std::shared_ptr<Program> dogProg; 

	std::vector<shared_ptr<Shape>> AllShapes;
	shared_ptr<Shape> world;
	shared_ptr<Shape> Nef;
	shared_ptr<Shape> shape;
	shared_ptr<Shape> dog; 

	//ground plane info
	GLuint GrndBuffObj, GrndNorBuffObj, GrndTexBuffObj, GIndxBuffObj;
	int gGiboLen;

	GLuint VertexArrayID;
	GLuint VertexBufferID;

	GLuint frameBuffer; 
	GLuint depthTexture;

	//three different textures
	shared_ptr<Texture> texture0;
 	shared_ptr<Texture> texture1;
 	shared_ptr<Texture> texture2;

	int gMat = 0;

	vec3 gTrans = vec3(0);
	float gScale = 1.0;
	vec3 gDTrans = vec3(0);
	float gDScale = 1.0;
	vec3 dummyTrans = vec3(0);
	float dummyScale = 1.0;
	float cTheta = 0;

	bool FirstTime = true; 
	bool mouseDown = false;
	bool Moving = false;
	bool horizontal = true; 
	
	float h = 0;
	float vertical = 0;

	float dogH = 0; 
	float dogV = 0;  
	bool moveX = false; 
	bool moveY = false; 
	bool onX = false;
	bool onY = true; 
	float changeX = 0; 
	float changeY = 0; 

	float phi, theta, rotate = 5;
	double prevX=0, prevY=0;

	float time = 0; 
	float texTime = 0;
	float blendFactor = 0.5; 
	float skyboxRotationSpeed = 1; 
	float skyboxRotation = 0; 

	bool scene = true; 
	int white = 0; 

	float points [108]= {
	  -10.0f,  10.0f, -10.0f,
	  -10.0f, -10.0f, -10.0f,
	   10.0f, -10.0f, -10.0f,
	   10.0f, -10.0f, -10.0f,
	   10.0f,  10.0f, -10.0f,
	  -10.0f,  10.0f, -10.0f,
	  
	  -10.0f, -10.0f,  10.0f,
	  -10.0f, -10.0f, -10.0f,
	  -10.0f,  10.0f, -10.0f,
	  -10.0f,  10.0f, -10.0f,
	  -10.0f,  10.0f,  10.0f,
	  -10.0f, -10.0f,  10.0f,
	  
	   10.0f, -10.0f, -10.0f,
	   10.0f, -10.0f,  10.0f,
	   10.0f,  10.0f,  10.0f,
	   10.0f,  10.0f,  10.0f,
	   10.0f,  10.0f, -10.0f,
	   10.0f, -10.0f, -10.0f,
	   
	  -10.0f, -10.0f,  10.0f,
	  -10.0f,  10.0f,  10.0f,
	   10.0f,  10.0f,  10.0f,
	   10.0f,  10.0f,  10.0f,
	   10.0f, -10.0f,  10.0f,
	  -10.0f, -10.0f,  10.0f,
	  
	  -10.0f,  10.0f, -10.0f,
	   10.0f,  10.0f, -10.0f,
	   10.0f,  10.0f,  10.0f,
	   10.0f,  10.0f,  10.0f,
	  -10.0f,  10.0f,  10.0f,
	  -10.0f,  10.0f, -10.0f,
	  
	  -10.0f, -10.0f, -10.0f,
	  -10.0f, -10.0f,  10.0f,
	   10.0f, -10.0f, -10.0f,
	   10.0f, -10.0f, -10.0f,
	  -10.0f, -10.0f,  10.0f,
	   10.0f, -10.0f,  10.0f
	};

	GLuint vbo;
	GLuint vao;
	GLuint tex_cube; 
	GLuint tex_cube2; 

	GLuint quad_VertexArrayID; 
	GLuint quad_vertexbuffer;

	GLuint frameBuf; 
	GLuint colorBuf[2]; 
	GLuint depthBuf; 

	GLuint pingpongFBO[2];
	GLuint pingpongBuffer[2];

	GLuint dogTex;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		cout << "onX is " << onX << endl; 
		cout << "onY is " << onY << endl; 
		cout << "changeY is" << changeY << endl; 
		cout << "changeX is " << changeX << endl;
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		else if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			h+=0.5; 
		}
		else if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			h-=0.5; 
		}
		else if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			vertical+=0.5; 
		}
		else if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			vertical-=0.5; 
		}
		else if (key == GLFW_KEY_K && action == GLFW_PRESS)
		{
			dogV+=0.5; 
		}
		else if (key == GLFW_KEY_I && action == GLFW_PRESS)
		{
			dogV-=0.5; 
		}
		else if (key == GLFW_KEY_L && action == GLFW_PRESS)
		{
			dogH+=0.5; 
			if(onY == true) {
				changeX = 90; 
				onX = true; 
				onY = false; 
			}
			else {
				changeX = 0; 
			}
		}
		else if (key == GLFW_KEY_J && action == GLFW_PRESS)
		{
			dogH-=0.5; 
			if(onY == true) { 
				changeX = -90; 
				onX = true; 
				onY = false; 
			}
			else {
				changeX = 0; 
			}
		}
		else if (key == GLFW_KEY_M && action == GLFW_PRESS)
		{
			scene = !scene; 
		}
	}

	void scrollCallback(GLFWwindow* window, double deltaX, double deltaY)
	{
		cTheta += (float) deltaX;
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			mouseDown = true;
			glfwGetCursorPos(window, &posX, &posY);
			cout << "Pos X " << posX << " Pos Y " << posY << endl;
			Moving = true;
		}

		if (action == GLFW_RELEASE)
		{
			Moving = false;
			mouseDown = false;
		}
	}

	void computeAngles(double posX, double posY)
	{
    	theta -= (posX - prevX) * 0.005;
    	phi += (posY - prevY) * 0.005;
	}

	void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
	{
    	if (Moving)
    	{
    		computeAngles(xpos, ypos);
    	}
    	prevX = xpos;
    	prevY = ypos; 
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	// Code to load in the three textures
	void initTex(const std::string& resourceDirectory)
	{
		texture0 = make_shared<Texture>();
		texture0->setFilename(resourceDirectory + "crate.jpg");
		texture0->init();
		texture0->setUnit(0);
		texture0->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	} 

	GLuint loadTexture(string File, GLuint TextureID) {

		glGenTextures(1, &TextureID);
		glBindTexture(GL_TEXTURE_2D, TextureID);
			
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		//CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		//CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		int width, height, channels;
		const string FileName = "../resources/"  + File;
		unsigned char * data = stbi_load(FileName.c_str(), &width, &height, &channels, 0);
		if (data)
		{
			CHECKED_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
			CHECKED_GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
			stbi_image_free(data);
		}
		else
		{
			printf("Failed to load image from file '%s', reason: %s\n", FileName.c_str(), stbi_failure_reason());
			exit(1);
		}
		return TextureID;
	}


	//code to set up the two shaders - a diffuse shader and texture mapping
	void init(const std::string& resourceDirectory)
	{
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		GLSL::checkVersion();

		cTheta = 0;
		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(
			resourceDirectory + "/simple_vert.glsl",
			resourceDirectory + "/simple_frag.glsl");
		if (! prog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("MatAmb");
		prog->addUniform("MatDif");
		prog->addUniform("fogColor");
		//prog->addUniform("blendFactor");
		prog->addUniform("skybox");
		prog->addUniform("skybox2");
		prog->addUniform("scene");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");

		texProg = make_shared<Program>();
		texProg->setVerbose(true);
		texProg->setShaderNames(
			resourceDirectory + "/tex_vert.glsl",
			resourceDirectory + "/tex_frag0.glsl");
		if (! texProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
 		texProg->addUniform("P");
		texProg->addUniform("V");
		texProg->addUniform("M");
		texProg->addAttribute("vertPos");
		texProg->addAttribute("vertNor");
		texProg->addUniform("Texture0");

		dogTex = loadTexture("world.jpg", dogTex);
		dogProg = make_shared<Program>();
		dogProg->setVerbose(true);
		dogProg->setShaderNames(
			resourceDirectory + "/dog_vert.glsl",
			resourceDirectory + "/dog_frag.glsl");
		if (! dogProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		dogProg->addUniform("P");
		dogProg->addUniform("V");
		dogProg->addUniform("M");
		dogProg->addAttribute("vertPos");
		dogProg->addAttribute("vertNor");
		dogProg->addAttribute("vertTex");
		dogProg->addUniform("Texture0");

		// Initialize the GLSL program.

		//initTex(resourceDirectory); 
		prog2 = make_shared<Program>();
		prog2->setVerbose(true);
		prog2->setShaderNames(
			resourceDirectory + "/sphere_vert.glsl",
			resourceDirectory + "/sphere_frag.glsl");
		if (! prog2->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog2->addUniform("P");
		prog2->addUniform("V");
		prog2->addUniform("M");
		prog2->addUniform("MatAmb");
		prog2->addUniform("MatDif");
		prog2->addAttribute("vertPos");
		prog2->addAttribute("vertNor");
		prog2->addUniform("Texture0");
		prog2->addUniform("white"); 
		prog2->addUniform("uTime");

		glGenFramebuffers(1,&frameBuf);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuf);
		//createFBO(frameBuf);

		glGenTextures(2,colorBuf); 

		for (unsigned int i = 0; i < 2; i++)
		{
		    glBindTexture(GL_TEXTURE_2D, colorBuf[i]);
		    glTexImage2D(
		        GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL
		    );
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		    glFramebufferTexture2D(
		        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuf[i], 0
		    );
		} 

		GLenum DrawBuffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
		glDrawBuffers(2, DrawBuffers);

		texProg2 = make_shared<Program>();
		texProg2->setVerbose(true);
		texProg2->setShaderNames(
			resourceDirectory + "/pass_vert.glsl",
			resourceDirectory + "/tex_fragH.glsl");
		if (! texProg2->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		texProg2->addUniform("texBuf");
		texProg2->addAttribute("vertPos");
		texProg2->addUniform("dir");


		glGenFramebuffers(2, pingpongFBO);
		glGenTextures(2, pingpongBuffer);

		for (unsigned int i = 0; i < 2; i++)
		{
		    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		    glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
		    glTexImage2D(
		        GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL
		    );
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		    glFramebufferTexture2D(
		        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0
		    );
		}

		gaussProg = make_shared<Program>();
		gaussProg->setVerbose(true);
		gaussProg->setShaderNames(
			resourceDirectory + "/gaus_vert.glsl",
			resourceDirectory + "/gaus_frag.glsl");
		if (! gaussProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		gaussProg->addUniform("texBuf");
		gaussProg->addAttribute("vertPos");
		gaussProg->addUniform("dir");
		gaussProg->addUniform("horizontal");
	 }

	/**** geometry set up for a quad *****/
	void initQuad()
	{
		//now set up a simple quad for rendering FBO
		glGenVertexArrays(1, &quad_VertexArrayID);
		glBindVertexArray(quad_VertexArrayID);

		static const GLfloat g_quad_vertex_buffer_data[] =
		{
			-1.0f, -1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,
		};

		glGenBuffers(1, &quad_vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
	}

	/*void createFBO(GLuint& fb)
	{
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);

		glBindFramebuffer(GL_FRAMEBUFFER, fb);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			cout << "Error setting up frame buffer - exiting" << endl;
			exit(0);
		}
	}*/ 

	// To complete image processing on the specificed texture
	// Right now just draws large quad to the screen that is texture mapped
	// with the prior scene image - next lab we will process
	void ProcessImage(GLuint inTex)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, inTex);

		gaussProg->bind();
		glUniform1i(gaussProg->getUniform("texBuf"), 0);
		glUniform2f(gaussProg->getUniform("dir"), -1, 0);
		glUniform1i(gaussProg->getUniform("horizontal"),horizontal); 
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);
		gaussProg->unbind();
	}

	void initGeom(const std::string& resourceDirectory)
	{
		vector<tinyobj::shape_t> TOshapes;
		vector<tinyobj::material_t> objMaterials;

		string errStr;
		bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
						(resourceDirectory + "/dummy.obj").c_str());

		if (!rc)
		{
			cerr << errStr << endl;
		}
		else
		{
			// some data to keep track of where our mesh is in space
			shared_ptr<Shape> temp;
			vec3 Gmin, Gmax;
			Gmin = vec3(std::numeric_limits<float>::max());
			Gmax = vec3(-std::numeric_limits<float>::max());
			for (size_t i = 0; i < TOshapes.size(); i++)
			{
				temp = make_shared<Shape>(); 
				temp->createShape(TOshapes[i]); 
				temp->measure();
				temp->init(); 
				Gmin = vec3(std::min(temp->min.x,Gmin.x), std::min(temp->min.y,Gmin.y), std::min(temp->min.z,Gmin.z));
				Gmax = vec3(std::max(temp->max.x,Gmax.x), std::max(temp->max.y,Gmax.y), std::max(temp->max.z,Gmax.z)); 
				AllShapes.push_back(temp);
			}

			dummyTrans = vec3(Gmin + 0.5f*(Gmax - Gmin));
			if (Gmax.x > Gmax.y && Gmax.x > Gmax.z)
			{
				dummyScale = 2.f / (Gmax.x-Gmin.x);
			}
			else if (Gmax.y > Gmax.x && Gmax.y > Gmax.z)
			{
				dummyScale = 2.f / (Gmax.y-Gmin.y);
			}
			else
			{
				dummyScale = 2.f / (Gmax.z-Gmin.z);
			}
		}

		// now read in the sphere for the world
		rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
						(resourceDirectory + "/sphere.obj").c_str());

		world =  make_shared<Shape>();
		world->createShape(TOshapes[0]);
		world->measure();
		world->init();

		// compute its transforms based on measuring it
		gDTrans = world->min + 0.5f*(world->max - world->min);
		if (world->max.x >world->max.y && world->max.x > world->max.z)
		{
			gDScale = 2.f / (world->max.x-world->min.x);
		}
		else if (world->max.y > world->max.x && world->max.y > world->max.z)
		{
			gDScale = 2.f / (world->max.y-world->min.y);
		}
		else
		{
			gDScale = 2.f / (world->max.z-world->min.z);
		}

		// now read in the Nefertiti model
		rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
						(resourceDirectory + "/Nefertiti-10K.obj").c_str());

		Nef = make_shared<Shape>();
		Nef->createShape(TOshapes[0]);
		Nef->measure();
		Nef->init();

		// compute its transforms based on measuring it
		gTrans = Nef->min + 0.5f * (Nef->max - Nef->min);
		if (Nef->max.x > Nef->max.y && Nef->max.x > Nef->max.z)
		{
			gScale = 2.f / (Nef->max.x - Nef->min.x);
		}
		else if (Nef->max.y > Nef->max.x && Nef->max.y > Nef->max.z)
		{
			gScale = 2.f / (Nef->max.y - Nef->min.y);
		}
		else
		{
			gScale = 2.f / (Nef->max.z - Nef->min.z);
		}

		shape = make_shared<Shape>();
		shape->loadMesh("../resources/sphere.obj");
		shape->measure();
		shape->init();

		dog = make_shared<Shape>();
		dog->loadMesh("../resources/dog.obj");
		dog->measure();
		dog->init();

		initQuad();
		//glGenTextures(1, &tex_cube);
		//glGenTextures(1, &tex_cube2);
		//bindTextures();
		glActiveTexture(GL_TEXTURE0);
	  	glGenTextures(1, &tex_cube);
		glActiveTexture(GL_TEXTURE1);
	  	glGenTextures(1, &tex_cube2);
		create_cube_map("../resources/moonwaw_ft.tga", "../resources/moonwaw_bk.tga", "../resources/moonwaw_up.tga", "../resources/moonwaw_dn.tga", "../resources/moonwaw_lf.tga", "../resources/moonwaw_rt.tga", &tex_cube);
		create_cube_map("../resources/morningdew_ft.tga", "../resources/morningdew_bk.tga", "../resources/morningdew_up.tga", "../resources/morningdew_dn.tga", "../resources/morningdew_lf.tga", "../resources/morningdew_rt.tga", &tex_cube2);

	}

	/*void bindTextures(){
		texTime += glfwGetTime() * 1000;
		texTime = (int) texTime % 24000;
		int texture1;
		int texture2;
		float blendFactor;		
		if(texTime >= 0 && texTime < 5000){
			texture1 = tex_cube;
			texture2 = tex_cube;
			blendFactor = (time - 0)/(5000 - 0);
		}else if(texTime >= 5000 && texTime < 8000){
			texture1 = tex_cube;
			texture2 = tex_cube2;
			blendFactor = (time - 5000)/(8000 - 5000);
		}else if(texTime >= 8000 && texTime < 21000){
			texture1 = tex_cube2;
			texture2 = tex_cube2;
			blendFactor = (texTime - 8000)/(21000 - 8000);
		}else{
			texture1 = tex_cube2;
			texture2 = tex_cube;
			blendFactor = (texTime - 21000)/(24000 - 21000);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture2);
	}*/ 

	/*void bindTextures() {
	  glActiveTexture(GL_TEXTURE0);
	  glGenTextures(1, &tex_cube);
	  glBindTexture(GL_TEXTURE_CUBE_MAP, tex_cube);
	 // glActiveTexture(GL_TEXTURE1);
	  //glGenTextures(1, &tex_cube2);
	  //glBindTexture(GL_TEXTURE_CUBE_MAP, tex_cube2);
	}*/ 

	void create_cube_map(
	  const char* front,
	  const char* back,
	  const char* top,
	  const char* bottom,
	  const char* left,
	  const char* right,
	  GLuint* tex_cube) {

	 // glActiveTexture(GL_TEXTURE0);
	  //glGenTextures(1, tex_cube);
	  
	  // load each image and copy into a side of the cube-map texture
	  load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, back);
	  load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, front);
	  load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, top);
	  load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, bottom);
	  load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, left);
	  load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_X, right);
	  // format cube map texture
	  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	bool load_cube_map_side(
	  GLuint texture, GLenum side_target, const char* file_name) {
	  glBindTexture(GL_TEXTURE_CUBE_MAP, texture); 
	  int x, y, n;
	  int force_channels = 4;
	  unsigned char*  image_data = stbi_load(
	    file_name, &x, &y, &n, force_channels);
	  if (!image_data) {
	    fprintf(stderr, "ERROR: could not load %s\n", file_name);
	    return false;
	  }
	  // non-power-of-2 dimensions check
	  if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
	    fprintf(stderr,
	    	"WARNING: image %s is not power-of-2 dimensions\n",
	    	file_name);
	  }
	  
	  // copy image data into 'target' side of cube map
	  glTexImage2D(
	    side_target,
	    0,
	    GL_RGBA,
	    x,
	    y,
	    0,
	    GL_RGBA,
	    GL_UNSIGNED_BYTE,
	    image_data);
	  free(image_data);
	  return true;
	}

	void render()
	{
    	double x, y, z;
    	x = cos(phi)*cos(theta);
    	y = sin(phi);
    	z = cos(phi)*sin(theta);

    	skyboxRotation += skyboxRotationSpeed * glfwGetTime(); 
    	skyboxRotation = (skyboxRotation * M_PI) / 180;

    	//generate vao's and vbo's for skybox
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(float), &points, GL_STATIC_DRAW);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		//frameBuf / colorBuffer (texture) objects that store full color and brightness 
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float aspect = width/(float)height;

		// Create the matrix stacks
		auto P = make_shared<MatrixStack>();
		auto V = make_shared<MatrixStack>();
		auto M = make_shared<MatrixStack>();

		glDepthMask(GL_FALSE);

		P->pushMatrix();
		P->perspective(45.0f, aspect, 0.01f, 100.0f);

		V->pushMatrix();
		V->translate(vec3(h,0,vertical));
		V->lookAt(vec3(0,0,0), vec3(x,y,z), vec3(0,1,0));

		//Draw skybox
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));

		M->pushMatrix();
			M->loadIdentity();
			M->rotate(skyboxRotation,vec3(0,1,0));
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE,value_ptr(M->topMatrix()) );
			glUniform3f(prog->getUniform("fogColor"), 0.6,0.6,0.6);
			//glUniform1f(prog->getUniform("blendFactor"), 0.5);
			glUniform1i(prog->getUniform("skybox"), 0);
			glUniform1i(prog->getUniform("skybox2"),1);
			glUniform1i(prog->getUniform("scene"),scene);
		M->popMatrix(); 

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex_cube);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);

		/*glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex_cube2);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);*/

		/*glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);*/ 

		prog->unbind();

		//draw the sphere aka planet
		prog2->bind(); 
		glUniformMatrix4fv(prog2->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
		glUniformMatrix4fv(prog2->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));

		M->pushMatrix();
			M->loadIdentity();
			M->rotate(skyboxRotation,vec3(0,1,0));
			M->pushMatrix(); 
			M->translate(vec3(20,6.5,0));
			M->scale(vec3(2,2,2));
			glUniform1i(prog2->getUniform("white"),0);
			glUniformMatrix4fv(prog2->getUniform("M"), 1, GL_FALSE,value_ptr(M->topMatrix()) );

			shape->draw(prog2);
			for (int i = 0; i < 1000; i++) {
			  M->pushMatrix();
			  M->translate(vec3(-10 + rand() % 1000, 7 + rand() % 100, -10 + rand() % 100));
			  M->scale(vec3(0.3,0.3,0.3));
			  glUniform1i(prog2->getUniform("white"),1);
			  glUniformMatrix4fv(prog2->getUniform("M"), 1, GL_FALSE,value_ptr(M->topMatrix()) ); 
			  shape->draw(prog2);
			  M->popMatrix();
			}
			M->popMatrix(); 

			M->pushMatrix();
			M->translate(vec3(10,5,7));
			M->scale(vec3(1,1,1));
			glUniform1i(prog2->getUniform("white"),0);
			glUniformMatrix4fv(prog2->getUniform("M"), 1, GL_FALSE,value_ptr(M->topMatrix()) );
			shape->draw(prog2);
			M->popMatrix(); 

			M->pushMatrix();
			M->translate(vec3(-10,5,10));
			M->scale(vec3(2,2,2));
			glUniform1i(prog2->getUniform("white"),0);
			glUniformMatrix4fv(prog2->getUniform("M"), 1, GL_FALSE,value_ptr(M->topMatrix()) );
			shape->draw(prog2);
			M->popMatrix(); 

			M->pushMatrix();
			M->translate(vec3(20,5,-20));
			M->scale(vec3(0.5,0.5,0.5));
			glUniform1i(prog2->getUniform("white"),0);
			glUniformMatrix4fv(prog2->getUniform("M"), 1, GL_FALSE,value_ptr(M->topMatrix()) );
			shape->draw(prog2);
			M->popMatrix(); 

			M->pushMatrix();
			M->translate(vec3(-30,5,10));
			M->scale(vec3(0.5,0.5,0.5));
			glUniform1i(prog2->getUniform("white"),0);
			glUniformMatrix4fv(prog2->getUniform("M"), 1, GL_FALSE,value_ptr(M->topMatrix()) );
			shape->draw(prog2);
			M->popMatrix(); 
			M->popMatrix(); 

			prog2->unbind(); 
			
			dogProg->bind();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, dogTex); 

			glUniformMatrix4fv(dogProg->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
			glUniformMatrix4fv(dogProg->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));

			M->pushMatrix();
				M->loadIdentity();
				M->translate(vec3(0,0,-3));
				M->translate(vec3(dogH, 0,dogV));
				//M->rotate(90,vec3(0,1,0));
				M->rotate(changeY,vec3(0,1,0));
				M->scale(vec3(0.2,0.2,0.2)); 
				glUniformMatrix4fv(dogProg->getUniform("M"), 1, GL_FALSE,value_ptr(M->topMatrix()) );
				glUniform1i(dogProg->getUniform("Texture0"),0);
				dog->draw(dogProg); 
				M->popMatrix(); 
			dogProg->unbind();

			//M->pushMatrix();
			//M->translate(vec3(10,10,10));
			//M->scale(vec3(0.2,0.2,0.2));
			//glUniformMatrix4fv(prog2->getUniform("M"), 1, GL_FALSE,value_ptr(M->topMatrix()) );
			//dog->draw(prog2);
			//M->popMatrix(); 

		//M->popMatrix();
		//prog2->unbind(); 

		//perform gaussian blur on scene 
		//bool first_iteration = true;
		//int amount = 10;
		//horizontal initially is true (0)
		//for (unsigned int i = 0; i < amount; i++)
		//{
	   	//	glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]); 
	    //	glBindTexture(
	      //  	GL_TEXTURE_2D, first_iteration ? colorBuf[1] : pingpongBuffer[!horizontal]
	    	//); 
	    	//ProcessImage(pingpongBuffer[i+1%2]);
	    	//horizontal = !horizontal;
	    //	if (first_iteration)
	      //  	first_iteration = false;
	//	}
	//	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	}

	void SetMaterial(int i)
	{
		switch (i)
		{
		case 0: //shiny blue plastic
			glUniform3f(prog->getUniform("MatAmb"), 0.02f, 0.04f, 0.2f);
			glUniform3f(prog->getUniform("MatDif"), 0.0f, 0.16f, 0.9f);
			break;
		case 1: // flat grey
			glUniform3f(prog->getUniform("MatAmb"), 0.13f, 0.13f, 0.14f);
			glUniform3f(prog->getUniform("MatDif"), 0.3f, 0.3f, 0.4f);
			break;
		case 2: //brass
			glUniform3f(prog->getUniform("MatAmb"), 0.3294f, 0.2235f, 0.02745f);
			glUniform3f(prog->getUniform("MatDif"), 0.7804f, 0.5686f, 0.11373f);
			break;
		case 3: //copper
			glUniform3f(prog->getUniform("MatAmb"), 0.1913f, 0.0735f, 0.0225f);
			glUniform3f(prog->getUniform("MatDif"), 0.7038f, 0.27048f, 0.0828f);
			break;
		}
	}

};

int main(int argc, char **argv)
{
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
			resourceDir = argv[1];
	}

	Application *application = new Application();

	WindowManager *windowManager = new WindowManager();
	windowManager->init(512, 512);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	application->init(resourceDir);
	application->initGeom(resourceDir);

	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
		application->render();

		glfwSwapBuffers(windowManager->getHandle());

		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
