/*
 #include "Menu.h"

void Menu::Initialize() {
    state.nextScene = -1;
    //state.player = new Entity();
}

void Menu::Update(float deltaTime)
{
    if (enterGame) {
        state.nextScene = 1;
    }
}

void Menu::Render(ShaderProgram* program) {
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    
    if (state.enterGame == false) {
        Util::DrawText(program, fontTextureID, "George the Platformer", 0.5, -0.1, glm::vec3(0, 0, 0));
        Util::DrawText(program, fontTextureID, "Press Enter to Start", 0.5, -0.1, glm::vec3(0, -2, 0));
    }
} */

#include "Menu.h"
void Menu::Initialize() {
    state.nextScene = -1;
    state.player = new Entity();
}

void Menu::Update(float deltaTime){}
/*
void Menu::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    
    if (state.player->position.x >= 13) {
        state.nextScene = 1;
    }
}
*/

void Menu::Render(ShaderProgram *program) {
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    
    if (state.nextScene == -1){
        Util::DrawText(program, fontTextureID, "Platformer George", 0.5, -0.1, glm::vec3(1.5, -3, 0));
        Util::DrawText(program, fontTextureID, "Press Enter to Start", 0.4, -0.1, glm::vec3(1.75, -5, 0));
    }
}
