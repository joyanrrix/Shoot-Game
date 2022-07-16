#ifndef WORLD_H
#define WORLD_H

#include <GLFW/glfw3.h>
#include "place.h"
#include "player.h"
#include "camera.h"
#include "ballmanager.h"

class World {
private:
	GLFWwindow* window;
	vec2 windowSize;

	Place* place;				// 场景
	Player* player;				// 玩家
	Camera* camera;				// 摄像机
	BallManager* ball;			// 小球

	// 阴影
	GLuint depthMap;
	GLuint depthMapFBO;
	Shader* simpleDepthShader;
	mat4 lightSpaceMatrix;
public:
	World(GLFWwindow* window, vec2 windowSize) {
		this->window = window;
		this->windowSize = windowSize;

		simpleDepthShader = new Shader("res/shader/shadow.vert", "res/shader/shadow.frag");

		vec3 lightPos(0.0, 400.0, 150.0);
		mat4 lightProjection = ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 500.0f);
		mat4 lightView = lookAt(lightPos, vec3(0.0f), vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

		camera = new Camera(window);
		place = new Place(windowSize, camera);
		player = new Player(windowSize, camera);
		ball = new BallManager(windowSize, camera);

		glGenFramebuffers(1, &depthMapFBO);
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	// 更新数据
	void Update(float deltaTime) {
		camera->Update(deltaTime);
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			ball->Update(camera->GetPosition(), camera->GetFront(), true);
			player->Update(deltaTime, true);
		}
		else {
			ball->Update(camera->GetPosition(), camera->GetFront(), false);
			player->Update(deltaTime, false);
		}
		place->Update();
	}
	// 渲染模型
	void Render() {
		RenderDepth();
		player->Render();
		place->RoomRender(NULL, depthMap);
		place->SunRender();
		ball->Render(NULL, depthMap);
	}

	GLuint GetScore() {
		return ball->GetScore();
	}
	// 判断游戏是否结束
	bool IsOver() {
		return ball->IsOver();
	}
	// 设置游戏模式
	void SetGameModel(GLuint num) {
		ball->SetGameModel(num);
	}
private:
	// 渲染深度图
	void RenderDepth() {
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		simpleDepthShader->Bind();
		simpleDepthShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, 1024, 1024);
		glClear(GL_DEPTH_BUFFER_BIT);
		place->RoomRender(simpleDepthShader);
		ball->Render(simpleDepthShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, windowSize.x, windowSize.y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
};

#endif
