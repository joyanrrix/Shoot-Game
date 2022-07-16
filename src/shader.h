#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
using namespace glm;
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

class Shader {
private:
	GLuint program;				// 与纹理类似，每个着色器也有唯一的单元与之匹配
public:
	Shader(string vertexPath, string fragmentPath) {
		string vertexCode;
		string fragmentCode;
		ifstream vShaderFile(vertexPath);
		ifstream fShaderFile(fragmentPath);
		stringstream vShaderStream, fShaderStream;

		// 读取文件中的着色器
		string line;
		while (!vShaderFile.eof()) {
			getline(vShaderFile, line);
			vShaderStream << line + '\n';
		}
		while (!fShaderFile.eof()) {
			getline(fShaderFile, line);
			fShaderStream << line + '\n';
		}

		// 把流数据转化为string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		// 顶点着色器
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vShaderCode, NULL);
		glCompileShader(vertexShader);
		checkCompileErrors(vertexShader, "VERTEX");
		// 片段着色器
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);
		checkCompileErrors(fragmentShader, "FRAGMENT");
		// 着色器程序
		program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);
		checkCompileErrors(program, "PROGRAM");
		// 着色器连接程序后删除
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	Shader(Shader* shader) {
		program = shader->GetProgram();
	}

	~Shader() {
		glDeleteProgram(program);
	}

	GLuint GetProgram() {
		return program;
	}
	// 绑定着色器
	void Bind() {
		glUseProgram(program);
	}
	// 解绑着色器
	void Unbind() {
		glUseProgram(0);
	}

	void SetInt(const char* name, int val) {
		glUniform1i(GetLocation(name), val);
	}

	void SetFloat(const char* name, float val) {
		glUniform1f(GetLocation(name), val);
	}

	void SetVec3(const char* name, vec3 val) {
		glUniform3fv(GetLocation(name), 1, &val[0]);
	}

	void SetMat3(const char* name, mat3 val) {
		glUniformMatrix3fv(GetLocation(name), 1, GL_FALSE, &val[0][0]);
	}

	void SetMat4(const char* name, mat4 val) {
		glUniformMatrix4fv(GetLocation(name), 1, GL_FALSE, &val[0][0]);
	}
private:
	// 检查编译着色器过程中有没有出错
	void checkCompileErrors(GLuint shader, string type) {
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << endl;
			}
		}
	}
	// 获取着色器内部变量的地址
	GLuint GetLocation(const char* name) {
		GLint location = glGetUniformLocation(program, name);

		if (location == -1) {
			cout << "Uniform '" << name << "' not defined" << endl;
		}

		return location;
	}
};

#endif // !SHADER_H
