#ifndef PLACE_H
#define PLACE_H

#include <glad/glad.h>
#include "model.h"
#include "texture.h"
#include "shader.h"
#include "camera.h"

class Place {
private:
	vec2 windowSize;
	// 房间
	Model* room;
	Texture* roomTexture;
	Shader* roomShader;

	// 太阳
	Model* sun;
	vec3 lightPos;							// 光源位置
	mat4 lightSpaceMatrix;					// 将顶点世界坐标转换为以光源为中心的坐标
	Shader* sunShader;

	Camera* camera;
	// 模型变换矩阵
	mat4 model;
	mat4 projection;
	mat4 view;
public:
	Place(vec2 windowSize, Camera* camera) {
		this->windowSize = windowSize;
		this->camera = camera;
		this->lightPos = vec3(0.0, 400.0, 150.0);
		mat4 lightProjection = ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 500.0f);
		mat4 lightView = lookAt(lightPos, vec3(0.0f), vec3(0.0, 1.0, 0.0));
		this->lightSpaceMatrix = lightProjection * lightView;
		LoadModel();
		LoadTexture();
		LoadShader();
	}
	// 更新变换矩阵
	void Update() {
		this->model = mat4(1.0);
		this->view = camera->GetViewMatrix();
		this->projection = perspective(radians(camera->GetZoom()), windowSize.x / windowSize.y, 0.1f, 500.0f);
	}
	// 渲染房间
	void RoomRender(Shader* shader, int depthMap = -1) {
		if (shader == NULL) {
			shader = roomShader;
			shader->Bind();
			shader->SetMat4("view", view);
			shader->SetMat4("projection", projection);
		}
		else {
			shader->Bind();
		}
		shader->SetMat4("model", model);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, roomTexture->GetId());
		if (depthMap != -1) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, depthMap);
		}
		glBindVertexArray(room->GetVAO());
		glDrawElements(GL_TRIANGLES, static_cast<GLuint>(room->GetIndices().size()), GL_UNSIGNED_INT, 0);
		shader->Unbind();
		glBindVertexArray(0);
	}
	// 渲染太阳
	void SunRender() {
		Shader* shader = sunShader;
		shader->Bind();
		shader->SetMat4("projection", projection);
		shader->SetMat4("model", model);
		shader->SetMat4("view", view);
		glBindVertexArray(sun->GetVAO());
		glDrawElements(GL_TRIANGLES, static_cast<GLuint>(sun->GetIndices().size()), GL_UNSIGNED_INT, 0);
		shader->Unbind();
		glBindVertexArray(0);
	}
private:
	// 加载模型
	void LoadModel() {
		room = new Model("res/model/room.obj");
		sun = new Model("res/model/sun.obj");
	}
	// 加载纹理
	void LoadTexture() {
		roomTexture = new Texture("res/texture/wall.jpg");
	}
	// 加载着色器
	void LoadShader() {
		roomShader = new Shader("res/shader/room.vert", "res/shader/room.frag");
		roomShader->Bind();
		roomShader->SetInt("diffuse", 0);
		roomShader->SetInt("shadowMap", 1);
		roomShader->SetVec3("lightPos", lightPos);
		roomShader->SetVec3("viewPos", camera->GetPosition());
		roomShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
		roomShader->Unbind();

		sunShader = new Shader("res/shader/sun.vert", "res/shader/sun.frag");
		sunShader->Bind();
		sunShader->Unbind();
	}
};

#endif // !PLACE_H
