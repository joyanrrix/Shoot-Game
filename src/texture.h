#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <stb_image/stb_image.h>
#include <iostream>
using namespace std;

class Texture {
private:
	GLuint id;
public:
	Texture(const char* path) {
		glGenTextures(1, &id);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
		if (data) {
			int format;
			if (nrComponents == 1 || nrComponents == 2)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			stbi_image_free(data);
		}
		else {
			cout << "Texture failed to load at path: " << path << endl;
			stbi_image_free(data);
		}
	}

	GLuint GetId() {
		return id;
	}
};

#endif
