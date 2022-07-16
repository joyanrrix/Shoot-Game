#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "world.h"

void OpenWindow();
void PrepareOpenGL();

GLFWwindow* window;
vec2 windowSize;

int main() {
    // 帧速率
    const double TARGET_FRAME = 0.016667;                   // 1/60秒，实现60帧
    const double FRAME_ALPHA = 0.25;                        // 计算系数
    double currentFrame;
    double deltaTime;
    double lastFrame;
    double frameTime = 0.0;
    double renderAccum = 0.0;
    double smoothFrameTime = TARGET_FRAME;

    srand(time(0));

    GLuint gameModel = 1;
    cout << "------------请选择游戏模式：1、休闲模式，2、挑战模式（输入模式前的序号即可）------------\n";
    cin >> gameModel;
    cout << "\n";
        
    OpenWindow();
    PrepareOpenGL();

    World world(window, windowSize);

    currentFrame = glfwGetTime();
    lastFrame = currentFrame;

    world.SetGameModel(gameModel);
    float gameTime = 0;

    while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE)) {
        // 求帧速率
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        renderAccum += deltaTime;

        if (renderAccum >= TARGET_FRAME) {
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderAccum -= TARGET_FRAME;

            world.Update(deltaTime);
            if (world.IsOver())
                break;
            world.Render();
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    cout << "----------------------------您的得分为：" << world.GetScore() << " ----------------------------" << endl;
    return 0;
}

void OpenWindow() {
    const char* TITLE = "Shoot Game";
    int WIDTH = 1960;
    int HEIGHT = 1080;

    // 初始化GLFW
    if (!glfwInit()) {
        cout << "Could not initialize GLFW" << endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_REFRESH_RATE, 60);

    window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glGetError();

    windowSize = vec2(WIDTH, HEIGHT);
}

void PrepareOpenGL() {
    // 打开深度测试和混合
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 把背景设置为天蓝色
    glClearColor(0.529f, 0.808f, 0.922f, 0.0f);
}