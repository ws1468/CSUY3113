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
#include "Effects.h"
#include "Entity.h"
#include "Map.h"
#include "Scene.h"
#include "Menu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "Ending.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene *currentScene;
Scene *sceneList[5];

Effects *effects;

//bool gameSuccess = false;
//bool gameFail = false;
//bool gameEnd = false;
//bool missingKeys = false;

void SwitchToScene(Scene *scene) {
    currentScene = scene;
    currentScene->Initialize();
}

Mix_Music *music;
Mix_Chunk *death;
Mix_Chunk *success;
Mix_Chunk *nextlvl;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Project 6", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    //using effects_textured
    program.Load("shaders/vertex_lit.glsl", "shaders/fragment_lit.glsl");
    //program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("Dark-Things.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    
    nextlvl = Mix_LoadWAV("nextlvl.wav");
    Mix_VolumeChunk(nextlvl, MIX_MAX_VOLUME / 4);
    death = Mix_LoadWAV("gameover.wav");
    //Mix_VolumeChunk(death, MIX_MAX_VOLUME);
    success = Mix_LoadWAV("success.wav");
    //Mix_VolumeChunk(success, MIX_MAX_VOLUME / 2);
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    sceneList[0] = new Menu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    sceneList[4] = new Ending();
    SwitchToScene(sceneList[0]);
    
    effects = new Effects(projectionMatrix, viewMatrix);
    
    //effects->Start(FADEIN, 0.5f);
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
                    case SDLK_LEFT:
                        // Move the player left
                        break;
                    case SDLK_RIGHT:
                        // Move the player right
                        break;
                    case SDLK_RETURN:
                        if (currentScene == sceneList[0] || currentScene->gameEnd || currentScene == sceneList[4]){
                            for (int i = 1; i < 5; i++){
                                sceneList[i]->gameFail = false;
                                sceneList[i]->gameSuccess = false;
                                sceneList[i]->gameEnd = false;
                            }
                            Mix_PlayChannel(-1, nextlvl, 0);
                            currentScene->state.nextScene = 1;
                        }
                        //currentScene->gameFail = false;
                        //currentScene->gameSuccess = false;
                        //currentScene->gameEnd = false;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    //if (currentScene != sceneList[0] && !currentScene->gameEnd){
    if ( (currentScene != sceneList[0]) && (!currentScene->gameSuccess) && (!currentScene->gameFail) && (currentScene != sceneList[4])){
        if (keys[SDL_SCANCODE_LEFT]) {
            currentScene->state.player->movement.x = -1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animLeft;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            currentScene->state.player->movement.x = 1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animRight;
        }
        
        if (keys[SDL_SCANCODE_DOWN]) {
            currentScene->state.player->movement.y = -1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animDown;
        }
        else if (keys[SDL_SCANCODE_UP]) {
            currentScene->state.player->movement.y = 1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animUp;
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
    
        program.SetLightPosition(currentScene->state.player->position);
        
        effects->Update(FIXED_TIMESTEP);
        
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
    
    viewMatrix = glm::mat4(1.0f);
    /*
    if (currentScene->state.player->position.x > 5) {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
    } else {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
    }*/
    viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, -currentScene->state.player->position.y, 0));    
    
    viewMatrix = glm::translate(viewMatrix, effects->viewOffset);
    
    if (currentScene->state.player->isActive == false) {
    //if (currentScene->state.player->killPlayer){
    //    currentScene->state.player->isActive = false;
        Mix_PlayChannel(-1, death, 0);
        currentScene->gameFail = true;
        currentScene->gameEnd = true;
        currentScene->state.player->killPlayer = false;
    } else if (currentScene->state.player->collidedLeft || currentScene->state.player->collidedRight || currentScene->state.player->collidedTop || currentScene->state.player->collidedBottom) {
        if (currentScene->state.player->lastCollided == ENEMY) {
            Mix_PlayChannel(-1, death, 0);
            currentScene->state.player->isActive = false;
            currentScene->gameFail = true;
            currentScene->gameEnd = true;
        }
        /* else if (currentScene->state.player->lastCollided == ITEM){
            if (currentScene->state.player->lastCollidedWith->itemType == KEY){
                currentScene->state.keysAcq += 1;
                currentScene->state.player->lastCollidedWith->isActive = false;
            } else if (currentScene->state.player->lastCollidedWith->itemType == DOOR) {
                if (currentScene->state.keysAcq == currentScene->state.keysReq) {
                    currentScene->state.doorUnlocked = true;
                } else {
                    missingKeys = true;
                }
            }
        }
        */
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
        
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);
    
    currentScene->Render(&program);
    
    GLuint fontTextureID = Util::LoadTexture("Pastiche_8x8.png");
    glm::vec3 endText;
     
    endText = glm::vec3(currentScene->state.player->position.x -2.75, currentScene->state.player->position.y, 0);
    
    if (currentScene->gameEnd == true){
        if (currentScene->gameSuccess == true){
            Util::DrawText(&program, fontTextureID, "Exit Found!", 0.4f, -0.1f, endText);
            //Mix_PlayChannel(-1, success, 0);
            currentScene->gameSuccess = false;
            //currentScene->gameEnd = true;
        } else if (currentScene->gameFail == true){
            Util::DrawText(&program, fontTextureID, "Game Over, Press Enter", 0.4f, -0.1f, endText);
            //Mix_PlayChannel(-1, death, 0);
            currentScene->gameFail = false;
            //currentScene->gameEnd = true;
        }
    }
    
    effects->Render();
    //effects last
    
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    Mix_FreeMusic(music);
    Mix_FreeChunk(death);
    Mix_FreeChunk(success);
    Mix_FreeChunk(nextlvl);
    
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        
        if (currentScene->state.nextScene >= 0)
            SwitchToScene(sceneList[currentScene->state.nextScene]);
        Render();
    }
    
    Shutdown();
    return 0;
}
