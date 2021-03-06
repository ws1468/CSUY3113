#include "Level2.h"
#define LEVEL2_WIDTH 14
#define LEVEL2_HEIGHT 16

#define LEVEL2_ENEMY_COUNT 2
#define LEVEL2_ITEM_COUNT 4

unsigned int level2_data[] =
{
     0,  1,  1,  1,  1, 79, 79,  1,  1,  1,  1,  1,  1,  5,
    10,  6,  7,  8,  9,  6,  6,  8,  8,  6,  7,  8,  6, 15,
    10,  8,  6,  7,  9,  6,  6,  6,  6,  6,  9,  6,  6, 15,
    10,  6,  6,  6,  8,  9,  7,  8,  9,  6,  7,  8,  6, 15,
    10,  6,  6,  6,  8,  9,  7,  8,  9,  6,  6,  8,  6, 15,
    10,  6,  6,  6,  8,  9,  7,  8,  9,  6,  7,  9,  6, 15,
    10,  6,  6,  6,  8,  9,  7,  8,  9,  6,  6,  6,  6, 15,
    10,  6,  9,  6,  6,  6,  8,  6,  6,  8,  8,  8,  6, 15,
    10,  6,  9,  6,  6,  6,  8,  6,  6,  8,  6,  9,  6, 15,
    10,  7,  9,  7,  8,  9,  7,  8,  9,  8,  9,  9,  6, 15,
    10,  9,  8,  9,  6,  8,  6,  6,  6,  7,  8,  9,  6, 15,
    10,  6,  9,  6,  6,  6,  8,  6,  6,  8,  7,  6,  6, 15,
    10,  6,  9,  6,  6,  6,  8,  6,  6,  8,  7,  8,  6, 15,
    10,  6,  6,  6,  8,  9,  7,  8,  9,  6,  8,  9,  6, 15,
    10,  6,  6,  6,  8,  9,  7,  8,  9,  6,  6,  6,  6, 15,
    40, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 45,
};

void Level2::Initialize() {
    
    state.nextScene = -1;
    state.keysReq = 1;
    state.keysAcq = 0;
    state.doorUnlocked = false;

    GLuint mapTextureID = Util::LoadTexture("Dungeon_Tileset.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 10, 10);

    // Move over all of the player and enemy code from initialization.
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(6, -14, 0);
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
    
    //state.player->height = 0.8f; // b/c player's png has extra space
    //state.player->width = .7f;
    //state.player->jumpPower = 6.0f;
       
    state.enemies = new Entity[LEVEL2_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("flame.png");
    
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = enemyTextureID;
        //state.enemies[i].speed = 2;
        state.enemies[i].aiType = XPATROL;
        state.enemies[i].aiState = WALKING;

        state.enemies[i].animFrames = 3;
        state.enemies[i].animIndex = 0;
        state.enemies[i].animTime = 0;
        state.enemies[i].animCols = 3;
        state.enemies[i].animRows = 4;
        state.enemies[i].animIndices = new int[3] {9, 10, 11};
        
        state.enemies->height = 0.9f;
        state.enemies->width = .8f;
    }
    state.enemies[0].position = glm::vec3(1, -9, 0);
    //state.enemies[0].isActive = false;
    state.enemies[1].position = glm::vec3(12, -4, 0);
    //state.enemies[1].isActive = false;
    
    
    state.items = new Entity[LEVEL2_ITEM_COUNT];
    GLuint keysTextureID = Util::LoadTexture("key.png");
    for (int i = 0; i < 2; i++){
        state.items[i].itemType = KEY;
        state.items[i].textureID = keysTextureID;
    }
    state.items[0].position = glm::vec3(1, -4, 0);
    //state.items[0].isActive = false;
    state.items[1].position = glm::vec3(8, -9, 0);

    GLuint doorTextureID = Util::LoadTexture("Dungeon_Tileset.png");
    for (int i = 2; i < LEVEL2_ITEM_COUNT; i++) {
        state.items[i].entityType = ITEM;
        state.items[i].itemType = DOOR;
        state.items[i].textureID = doorTextureID;
        state.items[i].animFrames = 1;
        state.items[i].animIndex = 0;
        state.items[i].animTime = 0;
        state.items[i].animCols = 10;
        state.items[i].animRows = 10;
    }
    state.items[2].position = glm::vec3(5, 0, 0);
    state.items[2].animIndices = new int[1]{66};
    //state.items[1].isActive = false;

    state.items[3].position = glm::vec3(6, 0, 0);
    state.items[3].animIndices = new int[1]{67};
    //state.items[2].isActive = false;
    
}

void Level2::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.items, LEVEL2_ITEM_COUNT, state.map);
    
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.items, LEVEL2_ITEM_COUNT, state.map);
    }
    
    for (int i = 0; i < LEVEL2_ITEM_COUNT; i++) {
        state.items[i].Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.items, LEVEL2_ITEM_COUNT, state.map);
    }
    
    //if (state.player->position.x >= 12) {
    //    state.nextScene = 1;
    //}
}

void Level2::Render(ShaderProgram *program) {
    state.map->Render(program);
    
    state.player->Render(program);
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++){
        state.enemies[i].Render(program);
    }
    
    for (int i = 0; i < LEVEL2_ITEM_COUNT; i++){
        state.items[i].Render(program);
    }
}
