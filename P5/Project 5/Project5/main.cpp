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

#include <SDL_mixer.h>

#include "Util.h"
#include "Entity.h"
#include "Map.h"
#include "Scene.h"
#include "Menu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"


SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene *currentScene;
Scene *sceneList[4];

int lives = 3;
bool death = false;

int gameWinsound = 0;
int gameFailsound = 0;
//bool gameSuccess = false;
//bool gameFail = false;
bool gameEnd = false;

void SwitchToScene(Scene *scene) {
    currentScene = scene;
    currentScene->Initialize();
}

Mix_Music *music;
Mix_Chunk *bounce;
Mix_Chunk *dies;
Mix_Chunk *gameWin;
Mix_Chunk *gameLose;

void Initialize() {
    //SDL_Init(SDL_INIT_VIDEO);
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("George the Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("dooblydoo.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 8);
    
    bounce = Mix_LoadWAV("bounce.wav");
    dies = Mix_LoadWAV("gameover.wav");
    Mix_VolumeChunk(dies, MIX_MAX_VOLUME / 2);
    gameWin = Mix_LoadWAV("success.wav");
    Mix_VolumeChunk(gameWin, MIX_MAX_VOLUME / 2);
    gameLose = Mix_LoadWAV("failure.wav");
    Mix_VolumeChunk(gameLose, MIX_MAX_VOLUME / 2);
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.50f, 0.90f, 1.0f, 0.70f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    sceneList[0] = new Menu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    SwitchToScene(sceneList[0]);
}

void ProcessInput() {
    
    currentScene->state.player->movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        if (currentScene->state.player->collidedBottom){
                            currentScene->state.player->jump = true;
                        }
                        break;
                        
                    case SDLK_RETURN:
                        if (currentScene == sceneList[0]){
                            currentScene->state.nextScene = 1;
                            Mix_PlayChannel(-1, bounce, 0);
                        }
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (!currentScene->gameEnd){
        if (keys[SDL_SCANCODE_LEFT]) {
            currentScene->state.player->movement.x = -1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animLeft;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            currentScene->state.player->movement.x = 1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animRight;
        }
        if (glm::length(currentScene->state.player->movement) > 1.0f) {
            currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
        }
    }
}

#define FIXED_TIMESTEP 0.0166666f  //60 frames per sec
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    deltaTime += accumulator; //if not enough time has passed
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP) { //if enough time has passed
    // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        currentScene->Update(FIXED_TIMESTEP);
        
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
    
    viewMatrix = glm::mat4(1.0f);
    
    //screen scrolling
    if (currentScene->state.player->position.x > 5) {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
    } else {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
    }
    
    /*
    if (currentScene->state.player->position.x > 5 && (currentScene->state.player->position.y > 3.75 || currentScene->state.player->position.y < -3.75)) {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, -currentScene->state.player->position.y, 0));
    }
    else if (currentScene->state.player->position.y > 3.75 || currentScene->state.player->position.y < -3.75 ) {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, -currentScene->state.player->position.y, 0));
    }
    else if (currentScene->state.player->position.x > 5) {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
    } else {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
    }*/
    
    if (currentScene->state.player->killPlayer){
        death = true;
        currentScene->state.player->killPlayer = false;
    } else if (currentScene->state.player->lastCollided == ENEMY) {
        if (currentScene->state.player->collidedLeft || currentScene->state.player->collidedRight || currentScene->state.player->collidedTop || currentScene->state.player->collidedBottom) {
            death = true;
        }
        currentScene->state.player->lastCollided = PLATFORM;
    } else if (currentScene->state.player->position.y < - 9) {
        death = true;
    }
    
    if (death == true && lives > 0){
        lives -= 1;
        death = false;
        if (lives != 0){
            Mix_PlayChannel(-1, dies, 0);
            currentScene->state.player->position = glm::vec3(1, -3.5, 0);
        }
    } else if (lives == 0){
        death = false;
        //Mix_PlayChannel(-1, gameLose, 1);
        currentScene->state.player->isActive = false;
        currentScene->gameEnd = true;
        currentScene->gameFail = true;
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetViewMatrix(viewMatrix);

    currentScene->Render(&program);
        
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    
    glm::vec3 endText, livesText;
    
    if (currentScene->state.player->position.x > 5) {
        endText = glm::vec3(currentScene->state.player->position.x -2, -2, 0);
        livesText = glm::vec3(currentScene->state.player->position.x -4, -.5, 0);
    } else {
        endText = glm::vec3(3, -3, 0);
        livesText = glm::vec3(1, -.5, 0);
    }
    
    /*
    if (-currentScene->state.player->position.x > 5 && (currentScene->state.player->position.y > 3.75 || currentScene->state.player->position.y < -3.75)) {
        endText = glm::vec3(currentScene->state.player->position.x, currentScene->state.player->position.y, 0);
        livesText = glm::vec3(currentScene->state.player->position.x - 4, currentScene->state.player->position.y -.75, 0);
    }
    else if (currentScene->state.player->position.y > 3.75 || currentScene->state.player->position.y < -3.75 ) {
        endText = glm::vec3(0, currentScene->state.player->position.y -.75, 0);
        livesText = glm::vec3(1, currentScene->state.player->position.y -.75, 0);
    }
    else if (currentScene->state.player->position.x > 5) {
        endText = glm::vec3(currentScene->state.player->position.x, 0, 0);
        livesText = glm::vec3(currentScene->state.player->position.x - 4, -.25, 0);
    } else {
        endText = glm::vec3(.5, .5, 0);
        livesText = glm::vec3(1, -.25, 0);
    } */
        
    if (currentScene->gameSuccess){
        //Mix_PlayChannel(-1, gameWin, 0);
        if (gameWinsound <=2){
            gameWinsound += 1;
        }
        Util::DrawText(&program, fontTextureID, "You Win", 0.5f, -0.25f, endText);
    }
    else if (currentScene->gameFail){
        //Mix_PlayChannel(-1, gameLose, 1);
        if (gameFailsound <= 2){
            gameFailsound += 1;
        }
        Util::DrawText(&program, fontTextureID, "You Lose", 0.5f, -0.25f, endText);
    }
    
    if (gameWinsound == 1){
        Mix_PlayChannel(-1, gameWin, 0);
    } else if (gameFailsound == 1){
        Mix_PlayChannel(-1, gameLose, 0);
    }
    
    if (currentScene != sceneList[0]) Util::DrawText(&program, fontTextureID, "Lives " + std::to_string(lives), 0.5, -0.1, livesText);
    
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    Mix_FreeMusic(music);
    Mix_FreeChunk(bounce);
    Mix_FreeChunk(dies);
    Mix_FreeChunk(gameWin);
    Mix_FreeChunk(gameLose);
    
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        if (currentScene->state.nextScene >= 0) SwitchToScene(sceneList[currentScene->state.nextScene]);
        Render();
    }
    
    Shutdown();
    return 0;
}

