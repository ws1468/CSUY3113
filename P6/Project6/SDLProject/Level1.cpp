#include "Level1.h"
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8

#define LEVEL1_ENEMY_COUNT 1
#define LEVEL1_ITEM_COUNT 3

unsigned int level1_data[] =
{
     0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 79, 79,  5,
    10,  6,  7,  8,  9,  6,  6,  6,  8,  8,  8,  6,  6, 15,
    10,  8,  6,  7,  6,  6,  9,  6,  6,  6,  6,  6,  6, 15,
    10,  7,  9,  6,  6,  7,  8,  9,  7,  8,  9,  8,  6, 15,
    10,  9,  8,  7,  6,  8,  6,  6,  6,  7,  6,  6,  6, 15,
    10,  6,  9,  6,  6,  6,  6,  6,  6,  6,  6,  8,  6, 15,
    10,  6,  6,  6,  6,  6,  8,  9,  7,  8,  9,  6,  6, 15,
    40, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 45,
};

void Level1::Initialize() {
    
    state.nextScene = -1;
    state.keysReq = 1;
    state.keysAcq = 0;
    state.doorUnlocked = false;

    GLuint mapTextureID = Util::LoadTexture("Dungeon_Tileset.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 10, 10);

    // Move over all of the player and enemy code from initialization.
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(11, -6, 0);
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
       
    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("flame.png");
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(4, -2.25, 0);
    state.enemies[0].speed = 1;
    state.enemies[0].aiType = XPATROL;
    state.enemies[0].aiState = IDLE;
    state.enemies[0].isActive = false;
    
    
    state.items = new Entity[LEVEL1_ITEM_COUNT];
    GLuint keysTextureID = Util::LoadTexture("key.png");
    state.items[0].entityType = ITEM;
    state.items[0].itemType = KEY;
    state.items[0].textureID = keysTextureID;
    state.items[0].position = glm::vec3(3, -3, 0);
    //state.items[0].isActive = false;
    
    GLuint doorTextureID = Util::LoadTexture("Dungeon_Tileset.png");
    for (int i = 1; i < LEVEL1_ITEM_COUNT; i++) {
        state.items[i].entityType = ITEM;
        state.items[i].itemType = DOOR;
        state.items[i].textureID = doorTextureID;
        state.items[i].animFrames = 1;
        state.items[i].animIndex = 0;
        state.items[i].animTime = 0;
        state.items[i].animCols = 10;
        state.items[i].animRows = 10;
    }
    state.items[1].position = glm::vec3(11, 0, 0);
    state.items[1].animIndices = new int[1]{66};
    //state.items[1].animIndices = state.items[1].animDoorC;
    //state.items[1].isActive = true;

    state.items[2].position = glm::vec3(12, 0, 0);
    //state.items[2].animIndices = state.items[2].animDoorC;
    state.items[2].animIndices = new int[1]{67};
    //state.items[2].isActive = true;
}

void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.items, LEVEL1_ITEM_COUNT, state.map);
    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.items, LEVEL1_ITEM_COUNT, state.map);
    }
    
    for (int i = 0; i < LEVEL1_ITEM_COUNT; i++) {
        state.items[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.items, LEVEL1_ITEM_COUNT, state.map);
    }
    //if (state.player->position.x >= 12) {
    //    state.nextScene = 1;
    //}
    if (state.player->position.x >= 12) {
    //    state.nextScene = 1;
    //}
    /*
    if (state.player->lastCollided == ITEM) {
        if (state.player->lastCollidedWith->itemType == DOOR){
            if (state.doorUnlocked == true){
                state.nextScene = 2;
            }
        }
    }
     */
}

void Level1::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++){
        state.enemies[i].Render(program);
    }
    for (int i = 0; i < LEVEL1_ITEM_COUNT; i++){
        state.items[i].Render(program);
    }
}
