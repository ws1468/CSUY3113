#include "Ending.h"
#define ENDING_WIDTH 12
#define ENDING_HEIGHT 8

#define ENDING_ENEMY_COUNT 1

unsigned int ending_data[] =
{
     0,  1,  1,  1,  1, 66, 67,  1,  1,  1,  1,  5,
    10,  8,  6,  6,  8,  6,  9,  9,  7,  6,  6, 15,
    10,  7,  8,  6,  6,  6,  6,  6,  6,  6,  6, 15,
    10,  6,  6,  6,  9,  8,  6,  6,  8,  6,  6, 15,
    10,  6,  6,  6,  8,  6,  6,  6,  9,  8,  6, 15,
    10,  9,  7,  6,  7,  7,  9,  8,  6,  7,  6, 15,
    10,  6,  8,  6,  6,  6,  6,  6,  6,  6,  6, 15,
    40, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 45,
};

void Ending::Initialize() {
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("Dungeon_Tileset.png");
    state.map = new Map(ENDING_WIDTH, ENDING_HEIGHT, ending_data, mapTextureID, 1.0f, 10, 10);

    // Move over all of the player and enemy code from initialization.
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(5, -3, 0);
    state.player->movement = glm::vec3(0);
    //state.player->acceleration = glm::vec3(0, -9.81f,0);
    state.player->speed = 2.0f;

    state.player->textureID = Util::LoadTexture("hero.png");
    state.player->animRight = new int[3] {24, 25, 26};
    state.player->animLeft = new int[3] {12, 13, 14};
    state.player->animUp = new int[3] {36, 37, 38};
    state.player->animDown = new int[3] {0, 1, 2};

    state.player->animIndices = state.player->animDown;
    state.player->animFrames = 3;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 12;
    state.player->animRows = 8;
    
    //state.player->height = 0.8f; //b/c player's png has extra space
    //state.player->width = .7f;
       
    //state.player->jumpPower = 6.0f;
       
    state.enemies = new Entity[ENDING_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("flame.png");
    
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(4, -2.25, 0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = XPATROL;
    state.enemies[0].aiState = IDLE;
    state.enemies[0].isActive = false;
}

void Ending::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, ENDING_ENEMY_COUNT, state.map);
}

void Ending::Render(ShaderProgram *program) {
    state.map->Render(program);
    GLuint fontTextureID = Util::LoadTexture("Pastiche_8x8.png");

    if (state.nextScene == -1){
        Util::DrawText(program, fontTextureID, "Congratulations!", 0.5, -0.1, glm::vec3(2.25, -2, 0));
        Util::DrawText(program, fontTextureID, "Exit found! You Won!", 0.3, -0.1, glm::vec3(3, -4.5, 0));
    }

    state.player->Render(program);
}
