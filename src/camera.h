#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// 摄像机默认参数
const float YAW = -90.0f;			// 初始偏航角
const float PITCH = 0.0f;			// 初始俯仰角
const float SPEED = 30.0f;			// 摄像头移动速度
const float HEIGHT = 10.0f;			// 摄像头高度
const float SENSITIVITY = 0.05f;     // 视角移动速度
const float ZOOM = 45.0f;			// 视角范围
const float JUMPTIME = 0.1f;		// 跳跃持续时间
const float GRAVITY = 9.8f;			// 重力加速度
const float JUMPSTRENGTH = 60.0f;	// 跳跃加速度

class Camera {
private:
	GLFWwindow* window;

	vec3 position;				// 摄像头位置
	vec3 front;					// 指向摄像头前方的单位向量
	vec3 right;					// 指向摄像头右方的单位向量
	vec3 up;					// 指向摄像头上方的单位向量
	vec3 worldUp;				// 指向世界的上方的单位向量，不会改变

	// 跳跃
	float jumpTimer;			// 跳跃持续时间
	float gravity;				// 重力加速度
	bool isJump;				// 用于判断当前是不是处于跳跃状态

	// 欧拉角
	float yaw;					// 偏航角
	float pitch;				// 俯仰角

	// 摄像头选项
	float movementSpeed;		// 摄像头移动速度
	float mouseSensitivity;		// 视角移动速度
	float zoom;					// 视角的大小，一般45.0f比较真实

	// 鼠标位置
	double mouseX;
	double mouseY;
	bool firstMouse;			// 鼠标是否第一次进入窗口
public:
	Camera(GLFWwindow* window) {
		this->window = window;

		movementSpeed = SPEED;
		mouseSensitivity = SENSITIVITY;
		zoom = ZOOM;
		firstMouse = true;

		jumpTimer = 0;
		isJump = false;
		gravity = -GRAVITY;

		position = vec3(0.0f, HEIGHT, 70.0f);
		worldUp = vec3(0.0f, 1.0f, 0.0f);
		yaw = YAW;
		pitch = PITCH;

		UpdateCamera();
	}	
	// 更新摄像头的数据和控制键盘、鼠标输入
	void Update(float deltaTime) {
		MouseMovement();
		KeyboardInput(deltaTime);
	}

	mat4 GetViewMatrix() {
		return lookAt(position, position + front, up);
	}

	vec3 GetPosition() {
		return position;
	}

	vec3 GetFront() {
		return front;
	}

	vec3 GetRight() {
		return right;
	}

	vec3 GetUp() {
		return up;
	}

	float GetZoom() {
		return zoom;
	}
private:
	// 鼠标输入
	void MouseMovement() {
		double newMouseX, newMouseY;

		glfwGetCursorPos(window, &newMouseX, &newMouseY);

		if (firstMouse) {
			mouseX = newMouseX;
			mouseY = newMouseY;
			firstMouse = false;
		}

		yaw += ((newMouseX - mouseX) * mouseSensitivity);
		pitch += ((mouseY - newMouseY) * mouseSensitivity);

		mouseX = newMouseX;
		mouseY = newMouseY;

		// 保证俯仰角在-90°和90°之间
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		// 更新Front，Right，Up
		UpdateCamera();
	}
	// 键盘输入
	void KeyboardInput(float deltaTime) {
		float velocity = movementSpeed * deltaTime;
		vec3 forward = normalize(cross(worldUp, right));
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			position += forward * velocity;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			position -= forward * velocity;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			position -= right * velocity;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			position += right * velocity;

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !isJump) {
			jumpTimer = JUMPTIME;
			isJump = true;
		}
		if (jumpTimer > 0) {
			gravity += (JUMPSTRENGTH * (jumpTimer / JUMPTIME)) * deltaTime;
			jumpTimer -= deltaTime;
		}
		gravity -= GRAVITY * deltaTime;
		position.y += gravity * deltaTime * 10;

		if (position.y < HEIGHT) {
			position.y = HEIGHT;
			gravity = 0;
			isJump = false;
		}

		CheckCollision();
	}
	// 碰撞检查
	void CheckCollision() {
		if (position.x > 90.0f)
			position.x = 90.0f;
		if (position.x < -90.0f)
			position.x = -90.0f;
		if (position.z > 75.0f)
			position.z = 75.0f;
		if (position.z < -35.0f)
			position.z = -35.0f;
	}
	// 更新摄像头的相关向量
	void UpdateCamera() {
		vec3 front;
		front.x = cos(radians(yaw)) * cos(radians(pitch));
		front.y = sin(radians(pitch));
		front.z = sin(radians(yaw)) * cos(radians(pitch));
		this->front = normalize(front);

		this->right = normalize(cross(this->front, this->worldUp));
		this->up = normalize(cross(this->right, this->front));
	}
};

#endif // !CAMERA_H
