#include "Level1.h"
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8

#define LEVEL1_ENEMY_COUNT 1

unsigned int level1_data[] =
{
     0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  5, 79, 79,  5,
    10,  6,  7,  8,  9,  6,  6,  6,  8,  8,  5,  6,  6, 15,
    10, 51, 51, 51, 51, 51, 51, 5,  6,  6,   5,  6,  6, 15,
    10, 51, 51, 51, 51, 51, 51, 45,  7,  8,  5,  8,  6, 15,
    10,  9,  8,  6,  6,  7,  8,  9,  8,  6,  5,  6,  6, 15,
    10,  6,  9, 40, 51, 51, 51, 51, 51, 51, 45,  6,  6, 15,
    10,  6,  6,  6,  6,  6,  8,  9,  7,  8,  9,  6,  6, 15,
    40, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 45,
};

void Level1::Initialize() {
    
    state.nextScene = -1;
    //state.keysReq = 1;
    //state.keysAcq = 0;
    //state.doorUnlocked = false;

    GLuint mapTextureID = Util::LoadTexture("Dungeon_Tileset.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 10, 10);

    // Move over all of the player and enemy code from initialization.
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1, -5, 0);
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
       
    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("flame.png");
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(1, -1, 0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = XPATROL;
    state.enemies[0].aiState = IDLE;
    state.enemies[0].isActive = false;
    state.enemies[0].width = .9f;
    
}

void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    }
    //if (state.player->position.x >= 12) {
    //    state.nextScene = 1;
    //}
    if (state.player->position.x >= 11 && state.player->position.y >= 0 ) {
        state.nextScene = 2;
    } else if (state.player->position.x >= 12 && state.player->position.y >= 0){
        state.nextScene = 2;
    }
}

void Level1::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++){
        state.enemies[i].Render(program);
    }
}
