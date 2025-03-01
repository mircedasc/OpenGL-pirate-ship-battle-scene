#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;
GLint lightOffLoc;

// camera
gps::Camera myCamera(
    glm::vec3(-1.1f, 0.9f, 2.75f),
    glm::vec3(-1.4f, 1.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
);

GLfloat cameraSpeed = 0.03f;
GLboolean pressedKeys[1024];

// models
gps::Model3D teapot;
gps::Model3D raindrop;
gps::Model3D water;
gps::Model3D glass;
gps::Model3D bottle;
GLfloat angle;
gps::SkyBox mySkyBox;

struct Raindrop {
    glm::vec3 position;   
    glm::vec3 velocity;   
};

std::vector<Raindrop> raindrops;

// shaders
gps::Shader myBasicShader;
gps::Shader skyboxShader;
gps::Shader thunderShader;
gps::Shader light2;

// Error checking
GLenum glCheckError_(const char* file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM: error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE: error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION: error = "INVALID_OPERATION"; break;
        case GL_OUT_OF_MEMORY: error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

// Mouse state
float lastX = 960.0f, lastY = 540.0f;
bool firstMouse = true;
bool lightOn = true;
bool lKeyPressed = true;

bool light1off = false;

// Callback functions
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
            if (key == GLFW_KEY_L && !lKeyPressed) {
                lightOn = !lightOn;  // Toggle the light
                lKeyPressed = true; // Mark the L key as processed
            }
            if (key == GLFW_KEY_O) {
                light1off = !light1off;
            }
            if (key == GLFW_KEY_Z) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            if (key == GLFW_KEY_X) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            if (key == GLFW_KEY_C) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            }
        }
        if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
            if (key == GLFW_KEY_L) {
                lKeyPressed = false; // Reset the flag when L key is released
            }
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    static float sensitivity = 0.1f;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos; // Reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    xOffset *= sensitivity;
    yOffset *= sensitivity;

    myCamera.rotate(yOffset, xOffset);
}

void processMovement() {
    if (pressedKeys[GLFW_KEY_W])
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
    if (pressedKeys[GLFW_KEY_S])
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
    if (pressedKeys[GLFW_KEY_A])
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
    if (pressedKeys[GLFW_KEY_D])
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
    glViewport(0, 0, width, height);
}

void initOpenGLWindow() {
    myWindow.Create(1920, 1080, "OpenGL Project Core");
}

void setWindowCallbacks() {
    glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initOpenGLState() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void initModels() {
    teapot.LoadModel("models/teapot/test5.obj");
    raindrop.LoadModel("models/teapot/raindrop.obj");
    water.LoadModel("models/teapot/water.obj");
    glass.LoadModel("models/teapot/glass.obj");
    bottle.LoadModel("models/teapot/bottle.obj");

}

void initShaders() {
    myBasicShader.loadShader("shaders/basic.vert", "shaders/basic.frag");
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    thunderShader.loadShader("shaders/thunderShader.vert", "shaders/thunderShader.frag");
    light2.loadShader("shaders/light2.vert", "shaders/light2.frag");
    skyboxShader.useShaderProgram();
}

glm::vec3 pointLightPosition = glm::vec3(0.0f, 0.0f, 0.0f); // Example position
glm::vec3 pointLightColor = glm::vec3(1.0f, 1.0f, 1.0f);      // White light
float constantAtt = 1.0f;  // Constant attenuation factor
float linearAtt = 7.1f;   // Linear attenuation factor
float quadraticAtt = 14.2f; // Quadratic attenuation factor

void initUniforms() {
    myBasicShader.useShaderProgram();

    model = glm::mat4(1.0f);
    float scaleFactor = 0.05f; // Adjust the scale factor as needed 
    model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));

    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 20.0f);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


        lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
        lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

        lightColor = glm::vec3(0.3f, 0.6f, 0.6f);
        lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

        lightOffLoc = glGetUniformLocation(myBasicShader.shaderProgram, "light1off");

        // Point Light Uniforms
        GLint pointLightPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLightPosition");
        glUniform3fv(pointLightPosLoc, 1, glm::value_ptr(pointLightPosition));

        GLint pointLightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLightColor");
        glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor));

        GLint constantAttLoc = glGetUniformLocation(myBasicShader.shaderProgram, "constantAtt");
        glUniform1f(constantAttLoc, constantAtt);

        GLint linearAttLoc = glGetUniformLocation(myBasicShader.shaderProgram, "linearAtt");
        glUniform1f(linearAttLoc, linearAtt);

        GLint quadraticAttLoc = glGetUniformLocation(myBasicShader.shaderProgram, "quadraticAtt");
        glUniform1f(quadraticAttLoc, quadraticAtt);
        

}

void renderTeapot(gps::Shader shader) {
    shader.useShaderProgram();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    teapot.Draw(shader);
}

void renderTransparent(gps::Shader shader) {
    shader.useShaderProgram();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    water.Draw(shader);
    glass.Draw(shader);
    bottle.Draw(shader);
    glDisable(GL_BLEND);
}

void initRain(int numRaindrops) {
    for (int i = 0; i < numRaindrops; i++) {
        // Spawn raindrops randomly within a larger area
        float x = static_cast<float>(rand()) / RAND_MAX * 60.0f - 30.0f; 
        float y = static_cast<float>(rand()) / RAND_MAX * 50.0f + 20;  
        float z = static_cast<float>(rand()) / RAND_MAX * 60.0f - 30.0f; 

        glm::vec3 position = glm::vec3(x, y, z);
        glm::vec3 velocity = glm::vec3(0.0f, -0.3f, 0.0f); // Falling speed

        raindrops.push_back({ position, velocity });
    }
}


void updateRain() {
    for (auto& drop : raindrops) {
        // Update position based on velocity
        drop.position += drop.velocity;

        // If the raindrop falls below ground level, reset it to a random height
        if (drop.position.y < -1.0f) {
            drop.position.y = static_cast<float>(rand()) / RAND_MAX * 50.0f + 20; // Reset height
            drop.position.x = static_cast<float>(rand()) / RAND_MAX * 60.0f - 30.0f; // Random x
            drop.position.z = static_cast<float>(rand()) / RAND_MAX * 60.0f - 30.0f; // Random z
        }
    }
}


void renderRain(gps::Shader shader) {
    shader.useShaderProgram();

    for (const auto& drop : raindrops) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, drop.position); // Move to raindrop's position
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Render the raindrop
        raindrop.Draw(shader);
    }
}

bool isFlashing = false;
float flashTimer = 2.0f;
float flashDuration = 0.5f; // Duration of the lightning flash
glm::vec3 defaultLightColor(0.1f, 0.3f, 0.3f);
glm::vec3 thunderLightColor(10.0f, 10.0f, 10.0f); // Bright white light for thunder

void updateThunderEffect(float deltaTime) {
    if (isFlashing) {
        flashTimer -= deltaTime;
        if (flashTimer <= 0.0f) {
            // End the flash
            isFlashing = false;
            lightColor = defaultLightColor;
        }
    }
    else {
        // Random chance to start a flash
        if (rand() % 100 == 0) { // 1 in 100 chance per frame
            isFlashing = true;
            flashTimer = flashDuration;
            lightColor = thunderLightColor;
        }
    }
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
}

void renderThunderFlash() {
    if (isFlashing) {
        glDisable(GL_DEPTH_TEST); // Ensure the quad covers the entire screen
        glUseProgram(thunderShader.shaderProgram); // Use a simple shader for the quad
        glDrawArrays(GL_TRIANGLES, 0, 6); // Render a full-screen quad
        glEnable(GL_DEPTH_TEST);
    }
}



void initSkyBox() {
    std::vector<const GLchar*> faces;
    faces.push_back("skybox/emeraldfog_rt.tga");
    faces.push_back("skybox/emeraldfog_lf.tga");
    faces.push_back("skybox/emeraldfog_up.tga");
    faces.push_back("skybox/emeraldfog_dn.tga");
    faces.push_back("skybox/emeraldfog_bk.tga");
    faces.push_back("skybox/emeraldfog_ft.tga");
    mySkyBox.Load(faces);
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    if (!lightOn) {
        lightColor = glm::vec3(0.0f, 0.0f, 0.0f); 
    }
    else {
        lightColor = glm::vec3(0.1f, 0.3f, 0.3f); 
    }
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    glUniform1i(lightOffLoc, light1off);

    mySkyBox.Draw(skyboxShader, view, projection);

    // Update point light position in the shader
    GLint pointLightPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLightPosition");
    glUniform3fv(pointLightPosLoc, 1, glm::value_ptr(pointLightPosition));

    renderTeapot(myBasicShader);

    renderTransparent(myBasicShader);

    renderRain(myBasicShader);
    
}

void cleanup() {
    myWindow.Delete();
}

int main(int argc, const char* argv[]) {
    try {
        initOpenGLWindow();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initModels();
    initShaders();
    initUniforms();
    setWindowCallbacks();
    initSkyBox();
    initRain(30000);

     while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
        updateRain();
        updateThunderEffect(1.0f);
        renderThunderFlash();
        renderScene();
        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());
    }

    cleanup();
    return EXIT_SUCCESS;
}
