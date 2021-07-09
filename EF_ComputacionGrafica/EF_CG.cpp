#include <iostream>

#include <files.hpp>
#include <model.hpp>
#include <cam.hpp>
#include <figures3.h>

#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>

const u32 FSIZE = sizeof(f32);
const u32 ISIZE = sizeof(i32);
const u32 SCR_WIDTH = 1280;
const u32 SCR_HEIGHT = 720;
const f32  ASPECT = (f32)SCR_WIDTH / (f32)SCR_HEIGHT;

glm::vec3 lightColor = glm::vec3(0.99f, 0.99f, 0.99f);
glm::vec3 playerPos;
std::vector<glm::vec3> bullets;

Cam* cam;
bool firstMouse = true;
f32 lastx = SCR_WIDTH / 2.0f;
f32 lasty = SCR_HEIGHT / 2.0f;

f32 deltaTime = 0.0f;
f32 lastFrame = 0.0f;

/**
 * keyboard input processing
 **/
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cam->processKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cam->processKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cam->processKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cam->processKeyboard(RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		bullets.push_back(glm::vec3(playerPos.x, playerPos.y + 1.0f, playerPos.z));
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		playerPos.x += 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		playerPos.x -= 0.01f;
	}
}

void mouse_callback(GLFWwindow* window, f64 xpos, f64 ypos) {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		cam->movePov(xpos, ypos);
	}
	else {
		cam->stopPov();
	}
}

void scroll_callback(GLFWwindow* window, f64 xoffset, f64 yoffset) {
	cam->processScroll((f32)yoffset);
}

i32 main() {
	GLFWwindow* window = glutilInit(3, 3, SCR_WIDTH, SCR_HEIGHT, "Maincra");
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	Files* files = new Files("bin", "resources/textures", "resources/objects");

	Shader* matShader = new Shader(files, "lightingmaps.vert", "lightingmaps.frag");
	Shader* shaderCubes = new Shader(files, "shaderC.vert", "shaderC.frag");
	Shader* shader = new Shader(files, "shader.vert", "shader.frag");

	Cube* cubex = new Cube();

	Model* ufo = new Model(files, "ufo/Low_poly_UFO.obj");

	srand(time(0));
	u32 rows = 3;
	u32 columns = 8;
	u32 row = 0;
	u32 index = 0;
	f32 width = 2.0f;

	std::vector<glm::vec3> positions(rows*columns);

	for (u32 i = 0; i < rows; ++i) {
		for (u32 j = 0; j < columns; ++j) {
			f32 x = j * 4;
			f32 z = 10.0f;
			f32 y = 10.0f*i;
			positions[index] = glm::vec3(x, y, z);
			index++;
		}
	}

	for (u32 i = 0; i < positions.size(); i++) {
		std::cout << positions[i].x << " " << positions[i].z << " " << positions[i].y << " " << std::endl;
	}

	//light position
	glm::vec3 lightPos = glm::vec3(positions[1].x, 20 + positions[1].y, positions[1].z);
	//camera position
	glm::vec3 initPos = glm::vec3(positions[2].x, -10.0f, -30.0f);

	playerPos = glm::vec3(positions[2].x, -10.0f, 10.0f);
	cam = new Cam(initPos);

	u32 vbo, vao, ebo, matVao;
	glGenVertexArrays(1, &vao);
	glGenVertexArrays(1, &matVao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, cubex->getVSize()*FSIZE,
		cubex->getVertices(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubex->getISize()*ISIZE,
		cubex->getIndices(), GL_STATIC_DRAW);

	// posiciones
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, cubex->len(), cubex->skip(0));
	glEnableVertexAttribArray(0);
	// normales
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, cubex->len(), cubex->skip(6));
	glEnableVertexAttribArray(2);
	// textures
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, cubex->len(), cubex->skip(9));
	glEnableVertexAttribArray(3);

	// matVAO
	glBindVertexArray(matVao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ARRAY_BUFFER, cubex->getVSize()*FSIZE,
		cubex->getVertices(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubex->getISize()*ISIZE,
		cubex->getIndices(), GL_STATIC_DRAW);

	// posiciones
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, cubex->len(), cubex->skip(0));
	glEnableVertexAttribArray(0);
	// normales
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, cubex->len(), cubex->skip(6));
	glEnableVertexAttribArray(1);
	// textures
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, cubex->len(), cubex->skip(9));
	glEnableVertexAttribArray(2);

	u32 player = shaderCubes->loadTexture("emerald.jpg");
	u32 bullet = matShader->loadTexture("gold.jpg");

	f32 move = 0.01f;
	u32 movetimer = 1000;
	u32 movecount = 0;

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		f32 currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glBindTexture(GL_TEXTURE_2D, player);

		processInput(window);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		shaderCubes->use();

		glm::mat4 proj = glm::perspective(cam->zoom, ASPECT, 0.1f, 100.0f);
		shaderCubes->setMat4("proj", proj);

		glm::mat4 view = glm::mat4(1.0f);
		shaderCubes->setMat4("view", cam->getViewM4());

		lightPos.x = float(30/2) * (cos(currentFrame) + sin(currentFrame));
		lightPos.z = float(30/2) * (cos(currentFrame) - sin(currentFrame));
		shaderCubes->setVec3("xyz", lightPos);
		shaderCubes->setVec3("xyzColor", lightColor);
		shaderCubes->setVec3("xyzView", cam->pos);

		glBindVertexArray(vao);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, playerPos);
		shaderCubes->setMat4("model", model);
		glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, 0);

		
		////////////////////////////////////////////// Objetos //////////////////////////////////////////////
		shader->use();

		shader->setVec3("xyz", lightPos);
		shader->setVec3("xyzColor", lightColor);
		shader->setVec3("xyzView", cam->pos);
		proj = glm::perspective(cam->zoom, ASPECT, 0.1f, 100.0f);
		shader->setMat4("proj", proj);
		shader->setMat4("view", cam->getViewM4());

		model = glm::mat4(1.0f);

		/////// UFOs
		for (u32 i = 0; i < positions.size(); i++) {
			model = glm::mat4(1.0f);
			positions[i].x += move;
			//if (i < 5 || i >= 10) { positions[i].x += move; }
			//else{ positions[i].x -= move; }
			glm::vec3 ufoCoord = glm::vec3(positions[i].x, positions[i].y, positions[i].z);
			model = translate(model, ufoCoord);
			model = glm::scale(model, glm::vec3(0.05f));
			shader->setMat4("model", model);
			ufo->Draw(shader);
		}

		// Balas con material de Oro

		glBindVertexArray(matVao);
		matShader->use();

		for (u32 i = 0; i < bullets.size(); i++) {

			matShader->setVec3("xyzLht.position", lightPos);
			matShader->setVec3("xyz", cam->pos);

			matShader->setVec3("xyzLht.ambient", 0.25f, 0.25f, 0.25f);
			matShader->setVec3("xyzLht.diffuse", 0.4f, 0.4f, 0.4f);
			matShader->setVec3("xyzLht.specular", 0.774f, 0.774f, 0.774f);

			matShader->setVec3("xyzMat.specular", 0.5f, 0.5f, 0.5f);
			matShader->setF32("xyzMat.shininess", 0.6f);
			matShader->setVec3("xyzColor", lightColor.x, lightColor.y, lightColor.z);

			matShader->setMat4("proj", proj);
			matShader->setMat4("view", cam->getViewM4());
			bullets[i].y += 0.01f;
			glm::vec3 matPos = glm::vec3(bullets[i]);
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, matPos);
			matShader->setMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, bullet);
			glDrawElements(GL_TRIANGLES, cubex->getISize(), GL_UNSIGNED_INT, 0);
		}

		// colisiones
		for (u32 i = 0; i < bullets.size(); i++) {
			for (u32 j = 0; j < positions.size(); j++) {
				if (((positions[j].x + 2.0f) >= bullets[i].x) && ((positions[j].x - width) <= bullets[i].x)
					&& (positions[j].y < bullets[i].y) && (positions[j].y + 2.0f >= bullets[i].y)) {
					positions.erase(positions.begin() + j);
					bullets.erase(bullets.begin() + i);
					break;
				}
			}
		}

		for (u32 i = 0; i < bullets.size(); i++) {
			if (bullets[i].y >= 25.0f) {
				bullets.erase(bullets.begin() + i);
			}
		}
		
		movecount++;
		if (movecount % 200 == 0){ bullets.push_back(glm::vec3(playerPos.x, playerPos.y + 1.0f, playerPos.z)); }
		if (movecount == movetimer) {
			movecount = 0; 
			move *= -1;
			for (u32 i = 0; i < positions.size(); i++) {
				positions[i].y -= 1.0f;
			}
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);

	delete shaderCubes;
	delete shader;
	delete cubex;
	delete cam;
	delete files;

	return 0;
}

/* vim: set tabstop=2:softtabstop=2:shiftwidth=2:noexpandtab */
