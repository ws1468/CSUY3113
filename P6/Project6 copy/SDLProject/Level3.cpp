#include "Level3.h"
#define LEVEL3_WIDTH 30
#define LEVEL3_HEIGHT 20

#define LEVEL3_ENEMY_COUNT 9

unsigned int level3_data[] =
{
     0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    10,  6,  7,  8,  9,  6,  6,  8,  8, 15, 10,  8,  6,  6,  7,  8,  9,  6,  6,  8,  8,  6,  7, 15, 78, 78, 10,  6,  6, 79,
    10,  8,  6,  7,  9,  6,  6,  6,  6, 15, 10,  6,  6,  8,  6,  7,  9,  6,  6,  6,  6,  6,  9, 41, 41, 41, 41,  6,  6, 79,
    10,  6,  6, 50, 53,  9,  7,  8,  9, 15, 10,  8,  6,  8,  6,  7,  9,  6,  6, 54, 53,  6,  9,  6,  6,  7,  8,  6,  6, 15,
    10,  6,  6, 15, 10, 41, 41, 41, 41, 45, 10,  8,  6,  8,  6,  7,  9,  6,  6,  5,  0,  6,  9,  6,  6,  7,  8,  6,  6, 15,
    10,  6,  6, 41, 41, 41, 41, 41, 41, 15, 10,  9,  6,  8,  6,  7,  9,  6,  6, 15, 40, 41, 41, 41, 41, 41, 41, 41, 41, 15,
    10,  6,  6,  6,  8,  9,  7,  8,  9, 41, 41,  6,  6,  8,  6,  7,  9,  6,  6, 40, 41, 41, 41, 41, 41, 41, 41, 15, 78, 15,
    10,  6,  9,  6,  6,  6,  8,  6,  6,  8,  8,  8,  6,  8,  6,  7,  9,  6,  6,  6,  6,  6,  9,  6,  6,  7,  8, 15, 78, 15,
    10,  6,  9,  6,  6,  6,  8,  6,  6,  8,  6,  9,  6,  8,  6,  7,  9,  6,  6,  6,  6,  6,  9,  6,  6,  7,  8, 15, 78, 15,
    10,  6,  6, 50, 41, 41, 41, 41, 41, 41, 41, 55,  6,  8, 54, 41, 41, 41, 41, 41, 41, 41, 55,  6,  6,  7,  8, 15, 78, 15,
    10,  6,  6, 40, 41, 41, 41, 41, 41, 41, 44, 45,  6,  8, 15, 10, 41, 41, 41, 41, 41, 15, 10,  6,  6,  7,  8, 15, 78, 15,
    10,  6,  9,  6,  6,  6,  8,  6,  6,  8,  7,  6,  6,  8, 15, 10,  9,  6,  6,  6,  6, 15, 10,  6,  6,  7,  8, 41, 41, 15,
    10,  6,  9,  6,  6,  6,  8,  6,  6,  8,  7,  8,  6,  8, 15, 10,  9,  6,  7,  6,  6, 15, 10,  6,  6,  7,  8,  6,  6, 15,
    10,  6,  6, 54, 41, 41, 41, 41, 55,  6,  8,  9,  6,  8, 15, 10,  9,  6, 10,  6,  6, 15, 10,  6,  6,  7,  8,  6,  6, 15,
    10,  6,  6, 15, 78, 78, 78, 78, 10,  6,  6,  6,  6,  8, 15, 10, 41, 41, 10,  6,  6, 15, 10,  6,  6, 54, 53,  6,  6, 15,
    10,  6,  6, 15, 10, 41, 41, 15, 10,  6,  6,  6,  6,  8, 15, 78, 78, 78, 10,  6,  6, 15, 10,  6,  6, 15, 10,  6,  6, 15,
    10,  6,  6, 15, 10,  9,  7, 41, 41,  6,  6,  6,  6,  8, 41, 41, 41, 41, 41,  6,  6, 41, 41,  6,  6, 15, 10,  6,  6, 15,
    10,  6,  6, 41, 41,  9,  7,  8,  9,  6,  6,  6,  6,  8,  6,  7,  9,  6,  6,  6,  6,  6,  9,  6,  6, 15, 10,  6,  6, 15,
    10,  6,  6,  6, 7,  9,  7,  8,  9,  6,  6,  6,  6,  8,  6,  7,  9,  6,  6,  6,  6,  6,  9,  6,  6, 15, 10,  6,  6, 15,
    40, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 45
};

void Level3::Initialize() {
    
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("Dungeon_Tileset.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 10, 10);

    // Move over all of the player and enemy code from initialization.
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1, -18, 0);
    //state.player->position = glm::vec3(27, -2, 0);
    
    state.player->movement = glm::vec3(0);
    //state.player->acceleration = glm::vec3(0, -9.81f,0);
    state.player->speed = 2.5f;
    
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
    
    //state.player->height = 0.8f; // b/c player's png has extra space
    //state.player->width = .7f;
    //state.player->jumpPower = 6.0f;
       
    state.enemies = new Entity[LEVEL3_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("flame.png");
    
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].speed = 2;
        state.enemies[i].animFrames = 3;
        state.enemies[i].animIndex = 0;
        state.enemies[i].animTime = 0;
        state.enemies[i].animCols = 3;
        state.enemies[i].animRows = 4;
        state.enemies[i].animIndices = new int[3] {9, 10, 11};
        
        state.enemies->height = 0.8f;
        state.enemies->width = .6f;
    }
    
    for (int i = 0; i< 6; i++){
        state.enemies[i].aiType = YPATROL;
        state.enemies[i].aiState = WALKING;
    }
    for (int i = 6; i < LEVEL3_ENEMY_COUNT; i++){
        state.enemies[i].aiType = XPATROL;
        state.enemies[i].aiState = WALKING;
    }
    state.enemies[0].position = glm::vec3(11, -18, 0);
    //state.enemies[0].isActive = false;
    state.enemies[1].position = glm::vec3(14, -1, 0);
    state.enemies[2].position = glm::vec3(15, -3, 0);
    state.enemies[3].position = glm::vec3(16, -5, 0);
    //state.enemies[5].position = glm::vec3(17, -7, 0);
    state.enemies[4].position = glm::vec3(7, -8, 0);
    state.enemies[4].speed = 0.5;
    state.enemies[5].position = glm::vec3(9, -14, 0);
    
    state.enemies[6].position = glm::vec3(1, -6, 0);
    state.enemies[7].position = glm::vec3(23, -10, 0);
    state.enemies[7].speed = 0.5;
    state.enemies[8].position = glm::vec3(25, -11, 0);
    state.enemies[8].speed = 0.5;

}

void Level3::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
    
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
    }
    
    //if (state.player->position.x >= 12) {
    //    state.nextScene = 1;
    //}
    if (state.player->position.x >= 29 && state.player->position.y >= -2 ) {
        state.nextScene = 4;
        gameSuccess = true;
        //gameEnd = true;
    }
}

void Level3::Render(ShaderProgram *program) {
    state.map->Render(program);
    
    state.player->Render(program);
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++){
        state.enemies[i].Render(program);
    }
}
