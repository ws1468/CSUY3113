#include "Level2.h"
#define LEVEL2_WIDTH 16
#define LEVEL2_HEIGHT 8

#define LEVEL2_ENEMY_COUNT 2

unsigned int level2_data[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1,
    3, 1, 1, 1, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 2,
    3, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 2, 2, 2, 2, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

void Level2::Initialize() {
    
    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 4, 1);

    // Move over all of the player and enemy code from initialization.
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1, -3.5, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f,0);
    state.player->speed = 2.0f;
    state.player->textureID = Util::LoadTexture("george_0.png");
    
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
    
    state.player->jumpPower = 6.0f;
    
    state.enemies = new Entity[LEVEL2_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");
    
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++){
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].acceleration = glm::vec3(0, -9.81f, 0);
        state.enemies[i].speed = 1;
        state.enemies[i].aiType = AGGRESSIVE;
        state.enemies[i].aiState = WALKING;
    }
    
    //state.enemies[0].entityType = ENEMY;
    //state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(6, -6, 0);
   // state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    //state.enemies[0].speed = 1;
    //state.enemies[0].aiType = WAITANDGO;
    //state.enemies[0].aiState = IDLE;
    
    //state.enemies[1].entityType = ENEMY;
    //state.enemies[1].textureID = enemyTextureID;
    state.enemies[1].position = glm::vec3(11, -5, 0);
    //state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    //state.enemies[1].speed = 1;
    //state.enemies[1].aiType = WAITANDGO;
    //state.enemies[1].aiState = IDLE;
}

void Level2::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++)
    {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    }
    
    if (state.player->position.x >= 15) {
        state.nextScene = 3;
    }
}

void Level2::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    state.enemies[0].Render(program);
    state.enemies[1].Render(program);
}


