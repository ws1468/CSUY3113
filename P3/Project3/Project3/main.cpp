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

#include <vector>

#include "Entity.h"

#define PLATFORM_COUNT 32

struct GameState {
    Entity *player;
    Entity *platforms;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool gameWin = false;
bool gameFail = false;

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
    displayWindow = SDL_CreateWindow("Lunar Lander : Patience is Key", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
    state.player->position = glm::vec3(2.25, 3.25, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -.01f,0);
    state.player->speed = 1.5f;
    state.player->textureID = LoadTexture("playerShip1_red.png");
    
    state.player->height = 1.0f;
    state.player->width = 1.0f;
        
    //initialize platforms
    state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("platformPack_tile016.png");
    
    //bottom left of goal
    for (int i=0; i < 6; i++){
        int j = -1;
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3((i * j), -3.25, 0);
    }
    //left wall
    int k = 0;
    for (int i = 6; i < 13; i++){
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(-4.5, -2.25f + k, 0);
        k++;
    }
    //right wall
    k = 0;
    for (int i = 13; i < 20; i++){
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(4.5, -2.25f + k, 0);
        k++;
    }
    //bottom left of goal
    k = 0;
    for (int i = 20; i < 23; i++){
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3((3 + k), -3.25f, 0);
        k++;
    }
    //obstacle left top
    state.platforms[23].textureID = platformTextureID;
    state.platforms[23].position = glm::vec3(-3.5, 2.0f, 0);
    
    state.platforms[24].textureID = platformTextureID;
    state.platforms[24].position = glm::vec3(-2.5, 2.0f, 0);
    
    //obstacle right middle
    state.platforms[25].textureID = platformTextureID;
    state.platforms[25].position = glm::vec3(3.5, 0.5f, 0);
    
    state.platforms[26].textureID = platformTextureID;
    state.platforms[26].position = glm::vec3(2.5, 0.5f, 0);
    
    state.platforms[27].textureID = platformTextureID;
    state.platforms[27].position = glm::vec3(1.5, 0.5f, 0);
    
    state.platforms[28].textureID = platformTextureID;
    state.platforms[28].position = glm::vec3(0.5, 0.5f, 0);
    
    //obstacle middle middle
    state.platforms[29].textureID = platformTextureID;
    state.platforms[29].position = glm::vec3(-2, -.75f, 0);

    //landing platforms
    GLuint landingTextureID = LoadTexture("platformPack_tile019.png");
    state.platforms[30].textureID = landingTextureID;
    state.platforms[30].position = glm::vec3(1, -3.25f, 0);
    
    state.platforms[31].textureID = landingTextureID;
    state.platforms[31].position = glm::vec3(2, -3.25f, 0);
    
    for (int i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].Update(0, NULL, 0);
    }
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
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        if ( !gameWin && !gameFail && !state.player->collidedBottom)
        state.player->acceleration.x += -.25f;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        if (!gameWin && !gameFail && !state.player->collidedBottom)
        state.player->acceleration.x += 0.25f;
    }

    if (glm::length(state.player->movement) > 1.0f) {
        state.player->acceleration = glm::normalize(state.player->acceleration);
        state.player->movement = glm::normalize(state.player->movement);
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
        state.player->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);
    
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
    
    if (state.player->collidedWith == &state.platforms[30] || state.player->collidedWith == &state.platforms[31]){
        gameWin = true;
    }
    
    else {
        if (state.player->collidedLeft || state.player->collidedRight || state.player->collidedBottom){
            gameFail = true;
        }
    }
    
    if (gameWin || gameFail){
        state.player->acceleration = glm::vec3(0);
        state.player->movement = glm::vec3(0);
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (int i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].Render(&program);
    }
    state.player->Render(&program);
    
    if (gameWin){
        DrawText(&program, fontTextureID, "Mission Successful", 0.5f, -0.25f, glm::vec3(-2.0f, 0, 0));
    }
    else if (gameFail){
        DrawText(&program, fontTextureID, "Mission Failed", 0.5f, -0.25f, glm::vec3(-2.0f, 0, 0));
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

