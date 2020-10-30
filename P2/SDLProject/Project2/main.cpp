#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, lpaddleMatrix, rpaddleMatrix, ballMatrix, projectionMatrix;

glm::vec3 left_position = glm::vec3(-4.75, 0, 0);
glm::vec3 ball_position = glm::vec3(0, 0, 0);
glm::vec3 right_position = glm::vec3(4.75, 0, 0);

glm::vec3 left_movement = glm::vec3(0, 0, 0);
glm::vec3 right_movement = glm::vec3(0, 0, 0);
glm::vec3 ball_movement = glm::vec3(0, 0, 0);

float player_speed = 1.0f;

Mix_Music *music;
Mix_Chunk *bounce;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("dooblydoo.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    
    bounce = Mix_LoadWAV("bounce.wav");
    
    viewMatrix = glm::mat4(1.0f);
    lpaddleMatrix = glm::mat4(1.0f);
    rpaddleMatrix = glm::mat4(1.0f);
    ballMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 0.79f, 0.78f, 1.0f);
    
    glUseProgram(program.programID);
    
    glClearColor(0.57f, 0.65f, 0.81f, 1.0f);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
}

bool gameBegin = false;
bool gameEnd = false;
bool freeze = false;
    
void ProcessInput() {
    
    left_movement = glm::vec3(0);
    right_movement= glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;
                
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                    // Move the player left
                    break;
                case SDLK_RIGHT:
                    // Move the player right
                    break;
                case SDLK_SPACE:
                    if (gameEnd == true){
                        freeze = false;
                        ball_position = glm::vec3(0, 0, 0);
                        gameEnd = false;
                        gameBegin = false;
                    }
                    if (gameBegin == false){
                        gameEnd = false;
                        gameBegin = true;
                        ball_movement.x = -3.0;
                        ball_movement.y = -3.0;
                    }
                break;
            }
            break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (!freeze){
        if (keys[SDL_SCANCODE_W] && left_position.y < 2.75) {
            left_movement.y = 2.2f;
        }
        else if (keys[SDL_SCANCODE_S] && left_position.y > -2.75) {
            left_movement.y = -2.2f;
        }
        
        if (keys[SDL_SCANCODE_UP] && right_position.y < 2.75) {
            right_movement.y = 2.2f;
        }
        else if (keys[SDL_SCANCODE_DOWN] && right_position.y > -2.75) {
            right_movement.y = -2.2f;
        }
        
        //if (glm::length(left_movement) > 1.0f) {
        //    left_movement = glm::normalize(left_movement);
        //}
        //if (glm::length(right_movement) > 1.0f) {
        //    right_movement = glm::normalize(right_movement);
        //}
    }
}

float lastTicks = 0.0f; //amount of time that has gone by since we last initialized sdl

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    //if ball touches walls
    if (ball_position.x >= 4.74){
        gameEnd = true;
        freeze = true;
        ball_movement.x = 0;
        ball_movement.y =0;
    }
    if (ball_position.x <= -4.75){
        gameEnd = true;
        freeze = true;
        ball_movement.x = 0;
        ball_movement.y =0;
    }
    
    bool touched = false;
    bool move_left = true;
    bool move_right = false;
    
    //case for collision w/ left paddle
    float Leftxdist = fabs(left_position.x - ball_position.x) - ((0.4) / 2.0f);
    float Leftydist = fabs(left_position.y - ball_position.y) - ((1.8) / 2.0f);
    if (Leftxdist < 0 && Leftydist < 0){ //Colliding!
        move_left = false;
        move_right = true;
        ball_movement.x = 3.0;
        touched = true;
    }
    
    //case for collison w/ right paddle
    float Rightxdist = fabs(right_position.x - ball_position.x) - ((0.4) / 2.0f);
    float Rightydist = fabs(right_position.y - ball_position.y) - ((1.8) / 2.0f);
    if (Rightxdist < 0 && Rightydist < 0){// Colliding!
        move_left = true;
        move_right = false;
        ball_movement.x = -3.0;
        touched = true;
    }
        
    // Add (direction * units per second * elapsed time)
    left_position += left_movement * player_speed * deltaTime;
    lpaddleMatrix = glm::mat4(1.0f);
    lpaddleMatrix = glm::translate(lpaddleMatrix, left_position);
    
    right_position += right_movement * player_speed * deltaTime;
    rpaddleMatrix = glm::mat4(1.0f);
    rpaddleMatrix = glm::translate(rpaddleMatrix, right_position);
    
    //check for ball hitting bottom --> bounce up
    if (ball_position.y >= 3.5){
        ball_movement.y = -3.0;
        touched = true;
    }
    //check for ball hitting up --> bounce down
    if (ball_position.y <= -3.5){
        ball_movement.y = 3.0;
        touched = true;
    }
    
    if (touched == true){
        Mix_PlayChannel(-1, bounce, 0);
    }
    
    //ball
    ball_position += ball_movement * player_speed * deltaTime;
    ballMatrix = glm::mat4(1.0f);
    ballMatrix = glm::translate(ballMatrix, ball_position);
}

void Draw(float* vertices){
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    //glBindTexture(GL_TEXTURE_2D, blockTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    float paddle[] = { -0.2, -0.9, 0.2, -0.9, 0.2, 0.9, -0.2, -0.9, 0.2, 0.9, -0.2, 0.9 };
    float ball[] = { -0.2, -0.2, 0.2, -0.2, 0.2, 0.2, -0.2, -0.2, 0.2, 0.2, -0.2, 0.2 };

    program.SetModelMatrix(lpaddleMatrix);
    Draw(paddle);
    
    program.SetModelMatrix(rpaddleMatrix);
    Draw(paddle);
        
    program.SetModelMatrix(ballMatrix);
    Draw(ball);
    
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
