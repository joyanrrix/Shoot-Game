#ifndef PLAYER_H
#define PLAYER_H

#include <glad/glad.h>
#include "texture.h"
#include "shader.h"
#include "model.h"
#include "camera.h"

class Player {
private:
	vec2 windowSize;					// 窗口尺寸
	// 枪的相关数据
	Model* gun;
	vec3 gunPos;						// 枪的位置坐标
	Shader* gunShader;
	mat4 gunModel;						// 枪模型位置变换矩阵
	Texture* diffuseMap;				// 漫反射贴图
	Texture* specularMap;				// 镜面反射贴图
	float gunRecoil;					// 后座力
	// 准星
	Model* dot;							
	Shader* dotShader;
	mat4 dotModel;						// 准星模型位置变换矩阵
	// 摄像头
	Camera* camera;
	// 变换矩阵
	mat4 projection;
	mat4 view;
public:
	Player(vec2 windowSize, Camera* camera) {
		this->windowSize = windowSize;
		this->camera = camera;
		this->gunRecoil = 10.0f;
		this->dotModel = mat4(1.0);
		this->gunModel = mat4(1.0);

		LoadGun();
		LoadTexture();
		LoadShader();
	}
	// 更新变换矩阵、摄像头位置等数据
	void Update(float deltaTime,  bool isShoot) {
		if (isShoot)
			gunRecoil = 10.0f;
		else
			gunRecoil = 0.0f;

		view = camera->GetViewMatrix();
		projection = perspective(radians(camera->GetZoom()), windowSize.x / windowSize.y, 0.1f, 500.0f);
		dotModel = mat4(1.0);
		dotModel[3] = vec4(camera->GetPosition(), 1.0);
		dotModel = translate(dotModel, camera->GetFront());
		dotModel = scale(dotModel, vec3(0.005));
		vec3 gunPos = (camera->GetFront() * 0.25f) + (camera->GetRight() * 0.2f) + (camera->GetUp() * -0.125f) + camera->GetPosition();
		gunModel = mat4(1.0);
		gunModel[0] = vec4(camera->GetRight(), 0.0);
		gunModel[1] = vec4(camera->GetUp(), 0.0);
		gunModel[2] = vec4(-camera->GetFront(), 0.0);
		gunModel[3] = vec4(gunPos, 1.0);
		gunModel = rotate(gunModel, radians(gunRecoil), vec3(1.0, 0.0, 0.0));
		gunModel = scale(gunModel, vec3(0.225));
		gunModel = translate(gunModel, vec3(-0.225, 0.0, -0.225));
		gunModel = rotate(gunModel, radians(-170.0f), vec3(0.0, 1.0, 0.0));
	}
	// 渲染场景
	void Render() {
		dotShader->Bind();
		dotShader->SetMat4("projection", projection);
		dotShader->SetMat4("view", view);
		dotShader->SetMat4("model", dotModel);

		glBindVertexArray(dot->GetVAO());
		glDrawElements(GL_TRIANGLES, static_cast<GLuint>(dot->GetIndices().size()), GL_UNSIGNED_INT, 0);

		gunShader->Bind();
		gunShader->SetMat4("projection", projection);
		gunShader->SetMat4("view", view);
		gunShader->SetMat4("model", gunModel);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap->GetId());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap->GetId());

		glBindVertexArray(gun->GetVAO());
		glDrawElements(GL_TRIANGLES, static_cast<GLuint>(gun->GetIndices().size()), GL_UNSIGNED_INT, 0);
		
		glBindVertexArray(0);
		gunShader->Unbind();
	}
private:
	// 加载枪模型
	void LoadGun() {
		gun = new Model("res/model/gun.obj");
		dot = new Model("res/model/dot.obj");
	}
	// 加载纹理
	void LoadTexture() {
		diffuseMap = new Texture("res/texture/gun-diffuse-map.jpg");
		specularMap = new Texture("res/texture/gun-specular-map.jpg");
	}
	// 加载着色器
	void LoadShader() {
		gunShader = new Shader("res/shader/gun.vert", "res/shader/gun.frag");
		gunShader->Bind();
		gunShader->SetInt("material.diffuse", 0);
		gunShader->SetInt("material.specular", 1);
		gunShader->SetFloat("material.shininess", 64.0);
		gunShader->SetVec3("light.position", vec3(0.0, 400.0, 150.0));
		gunShader->SetVec3("light.ambient", vec3(0.2));
		gunShader->SetVec3("light.diffuse", vec3(0.65));
		gunShader->SetVec3("light.specular", vec3(1.0));
		gunShader->SetVec3("viewPos", camera->GetPosition());
		gunShader->Unbind();

		dotShader = new Shader("res/shader/ball.vert", "res/shader/ball.frag");
		dotShader->Bind();
		dotShader->SetVec3("color", vec3(1.0, 0.0, 0.0));
		dotShader->Unbind();
	}
};

#endif // !PLAYER_H
