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
#include "Entity.h"
#include <vector>

#define PLATFORM_COUNT 19
#define ENEMY_COUNT 3

struct GameState {
    Entity *player;
    Entity *platforms;
    Entity *enemies;
};

GameState state;

SDL_Window* displayWindow;

bool gameIsRunning = true;
bool gameSuccess = false;
bool gameFail = false;
bool gameEnd = false;
bool instructions = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint fontTextureID;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
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

void DrawText(ShaderProgram *program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position){
        //shader program, ID of texture, string, the size relative to others, amount of space between chars, where these triangles are
    
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    
    std::vector<float> vertices;
    std::vector<float> texCoords;
    
    //loop for each character in the string
    for(int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        
        // setup the vertices and texCoords
        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
        });
        
        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
        }); //
    } //end of for loop
    
    //after the vertices and texCoords are setup we can draw using familiar code
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Rise of AI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    fontTextureID = LoadTexture("font1.png");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
   
    // Initialize Game Objects
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(-4.5, -1, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -11.81f,0);
    state.player->speed = 1.5f;
    state.player->textureID = LoadTexture("george_0.png");
    
    state.player->animRight = new int[4] {3, 7, 11, 15};
    state.player->animLeft = new int[4] {1, 5, 9, 13};
    state.player->animUp = new int[4] {2, 6, 10, 14};
    state.player->animDown = new int[4] {0, 4, 8, 12};

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    state.player->height = 0.8f; //b/c player's png has extra space
    state.player->width = .7f;
    
    state.player->jumpPower = 8.0f;
    
    //initialize platforms
    state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("platformPack_tile001.png");

    for (int i = 0; i < 11; i++){
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(-5 +i, -3.25f, 0);
    }
    
    int j = 0;
    for (int i = 11; i < 13; i++){
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(-2+j, -1.25f, 0);
        j++;
    }
    
    int k = 0;
    for (int i = 13; i < 16; i++){
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(3+k, -0.25f, 0);
        k++;
    }
    
    state.platforms[16].entityType = PLATFORM;
    state.platforms[16].textureID = platformTextureID;
    state.platforms[16].position = glm::vec3(0, -0.25f, 0);
    
    state.platforms[17].entityType = PLATFORM;
    state.platforms[17].textureID = platformTextureID;
    state.platforms[17].position = glm::vec3(-3, -1.25f, 0);
    
    state.platforms[18].entityType = PLATFORM;
    state.platforms[18].textureID = platformTextureID;
    state.platforms[18].position = glm::vec3(1, -0.25f, 0);
    
    
    for (int i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].Update(0, NULL, NULL, 0);
    }
    
    state.enemies = new Entity[ENEMY_COUNT];
    GLuint bottomTextID = LoadTexture("ctg copy.png");
    GLuint rightTextID = LoadTexture("ctg copy2.png");
    GLuint leftTextID = LoadTexture("ctg.png");
    
    //Bottom
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = bottomTextID;
    state.enemies[0].position = glm::vec3(4.5, -2.25, 0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].aiState = IDLE;
    state.enemies[0].width = .7f;

    
    //Left
    state.enemies[1].entityType = ENEMY;
    state.enemies[1].textureID = leftTextID;
    state.enemies[1].position = glm::vec3(-2, -0.25, 0);
    state.enemies[1].speed = 1;
    state.enemies[1].aiType = PATROL;
    state.enemies[1].aiState = WALKING;
    state.enemies[1].width = .7f;

    
    //Right
    state.enemies[2].entityType = ENEMY;
    state.enemies[2].textureID = rightTextID;
    state.enemies[2].position = glm::vec3(4.5, .75, 0);
    state.enemies[2].speed = 1;
    state.enemies[2].aiType = AGGRESSIVE;
    state.enemies[2].aiState = WALKING;
    state.enemies[2].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[2].width = .7f;


}

void ProcessInput() {
    
    state.player->movement = glm::vec3(0);
    
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
                        if (state.player->collidedBottom){
                            state.player->jump = true;
                        }
                        instructions = false;
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (!gameEnd){
        if (keys[SDL_SCANCODE_LEFT]) {
            state.player->movement.x = -1.0f;
            state.player->animIndices = state.player->animLeft;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            state.player->movement.x = 1.0f;
            state.player->animIndices = state.player->animRight;
        }

        if (glm::length(state.player->movement) > 1.0f) {
            state.player->movement = glm::normalize(state.player->movement);
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
        state.player->Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT);
        
        for (int i=0; i < ENEMY_COUNT; i++){
            state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT);
        }
        
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
    
    state.player->CheckCollisionsX(state.enemies, ENEMY_COUNT);
    state.player->CheckCollisionsY(state.enemies, ENEMY_COUNT);

    if (state.player->lastCollided == ENEMY) {
        if (state.player->collidedLeft || state.player->collidedRight || state.player->collidedTop) {
            state.player->isActive = false;
            gameEnd = true;
            gameFail = true;
        }
        else if (state.player->collidedBottom) {
            (state.player->lastCollidedWith)->isActive = false;
        }
    }
    
    for (int i = 0; i < ENEMY_COUNT; i++){
        state.enemies[i].CheckCollisionsX(state.player, 1);
        state.enemies[i].CheckCollisionsY(state.player, 1);
        
        if (state.enemies[i].lastCollided == PLAYER) {
            if (state.enemies[i].collidedLeft || state.enemies[i].collidedRight || state.enemies[i].collidedBottom) {
                state.enemies[i].lastCollidedWith->isActive = false;
                gameEnd = true;
                gameFail = true;
            }
            else if (state.enemies[i].collidedTop) {
                state.enemies[i].isActive = false;
            }
        }
    }
    
    if (!state.enemies[0].isActive &&
        !state.enemies[1].isActive &&
        !state.enemies[2].isActive) {
        gameEnd = true;
        gameSuccess = true;
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (instructions) {
        DrawText(&program, fontTextureID, "Press Spacebar to jump", 0.5f, -0.25f, glm::vec3(-4.5f, 3.0f, 0));
        DrawText(&program, fontTextureID, "Jump on Enemy's Head to Defeat it", 0.5f, -0.25f, glm::vec3(-4.5f, 2.5f, 0));
    }
    
    for (int i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].Render(&program);
    }
    
    for (int i = 0; i < ENEMY_COUNT; i++){
        state.enemies[i].Render(&program);
    }
    
    state.player->Render(&program);
    
    if (gameSuccess){
        DrawText(&program, fontTextureID, "You Win", 0.5f, -0.25f, glm::vec3(-1.0f, 0, 0));
    }
    else if (gameFail){
        DrawText(&program, fontTextureID, "You Lose", 0.5f, -0.25f, glm::vec3(-1.0f, 0, 0));
    }
    
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
