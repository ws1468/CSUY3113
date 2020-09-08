#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, starMatrix, starrMatrix, starrrMatrix, cloudMatrix, clouddMatrix, starivMatrix, projectionMatrix;

float star_rotate = 0;
float star_rotatee = 0;
float cloud_right = -3.75;
float cloud_left = 3.75;

float star_x = -3.75;
float star_y = 3;
float star_scale = 1.0;

GLuint starTextureID;
GLuint cloudTextureID;
GLuint starrTextureID;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
    std::cout << "Unable to load image. Make sure the path is correct\n"; assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
        return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("ws1468 Project One - Starry Sky", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    starMatrix = glm::mat4(1.0f);
    starrMatrix = glm::mat4(1.0f);
    starrrMatrix = glm::mat4(1.0f);
    cloudMatrix = glm::mat4(1.0f);
    clouddMatrix = glm::mat4(1.0f);
    starivMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f); //the background color;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    starTextureID = LoadTexture("starrr.png");
    cloudTextureID = LoadTexture("cloud.png");
    starrTextureID = LoadTexture("starrrr.png");
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float lastTicks = 0.0f; //amount of time that has gone by since we last initialized sdl

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    cloud_right += .9f * deltaTime;
    cloud_left -= .5f * deltaTime;
    star_rotate += 90.f * deltaTime;
    star_rotatee += 80.f * deltaTime;
    
    star_x += 2.0f * deltaTime;
    star_y -= .5f * deltaTime;
    star_scale += .2 * deltaTime;
    
    starivMatrix = glm::mat4(1.0f);
    starivMatrix = glm::translate(starivMatrix, glm::vec3(star_x, star_y, 0.0f));
    starivMatrix = glm::rotate(starivMatrix,glm::radians(star_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    starivMatrix = glm::scale(starivMatrix, glm::vec3(star_scale, star_scale, 1.0f));

    
    starMatrix = glm::mat4(1.0f);
    starMatrix = glm::translate(starMatrix, glm::vec3(-3.0f, 3.0f, 0.0f));
    starMatrix = glm::rotate(starMatrix,glm::radians(star_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    
    starrMatrix = glm::mat4(1.0f);
    starrMatrix = glm::translate(starrMatrix, glm::vec3(0.0f, 1.0f, 0.0f));
    starrMatrix = glm::rotate(starrMatrix,glm::radians(star_rotatee), glm::vec3(0.2f, 0.1f, 1.0f));
    
    starrrMatrix = glm::mat4(1.0f);
    starrrMatrix = glm::translate(starrrMatrix, glm::vec3(2.0f, 2.0f, 0.0f));
    starrrMatrix = glm::rotate(starrrMatrix, glm::radians(star_rotate), glm::vec3(0.0f, 0.1f, 1.0f));
    
    cloudMatrix = glm::mat4(1.0f);
    cloudMatrix = glm::translate(cloudMatrix, glm::vec3(cloud_right, 1.0f, 0.0f));
    
    clouddMatrix = glm::mat4(1.0f);
    clouddMatrix = glm::translate(clouddMatrix, glm::vec3(cloud_left, -1.0f, 0.0f));
    
    
}

void Render() {
    float vertices[] = { -3.0, -3.0, 3.0, -3.0, 3.0, 3.0, -3.0, -3.0, 3.0, 3.0, -3.0, 3.0 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    program.SetModelMatrix(starivMatrix);
    glBindTexture(GL_TEXTURE_2D, starrTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetModelMatrix(starMatrix);
    glBindTexture(GL_TEXTURE_2D, starTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetModelMatrix(starrMatrix);
    glBindTexture(GL_TEXTURE_2D, starTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetModelMatrix(starrrMatrix);
    glBindTexture(GL_TEXTURE_2D, starTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetModelMatrix(cloudMatrix);
    glBindTexture(GL_TEXTURE_2D, cloudTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetModelMatrix(clouddMatrix);
    glBindTexture(GL_TEXTURE_2D, cloudTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
