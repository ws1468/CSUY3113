#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    speed = 0;
    
    modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollision(Entity *other){
    
    if (other == this) return false;
    
    if (isActive == false || other->isActive == false) return false;
    
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);

    if (xdist < 0 && ydist < 0) return true;
    
    return false;
}

void Entity::CheckCollisionsY(Entity *objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity *object = &objects[i];
        
        if (CheckCollision(object)) {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
                lastCollided = object->entityType;
                lastCollidedWith = object;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
                lastCollided = object->entityType;
                lastCollidedWith = object;
            }
        }
    }
}

void Entity::CheckCollisionsX(Entity *objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity *object = &objects[i];
                
        if (CheckCollision(object)) {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
                lastCollided = object->entityType;
                lastCollidedWith = object;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
                lastCollided = object->entityType;
                lastCollidedWith = object;
            }
        }
    }
}

void Entity::CheckCollisionsY(Map *map) {
    // Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);
    
    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    
    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) { position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0){
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
}

void Entity::CheckCollisionsX(Map *map) {
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) { position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }
    
    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) { position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
}

void Entity::switchXWalk(){
    if (walkLeft){
        walkLeft = false;
        walkRight = true;
    } else {
        walkLeft = true;
        walkRight = false;
    }
}

void Entity::switchYWalk(){
    if (walkUp){
        walkUp = false;
        walkDown = true;
    } else {
        walkUp = true;
        walkDown = false;
    }
}

void Entity::AIxPatrol(){
    switch (aiState){
        case WALKING:
            if (walkLeft){
                movement = glm::vec3(-2,0,0);
                if (collidedLeft){
                    switchXWalk();
                }
            }
            if (walkRight){
                movement = glm::vec3(2,0,0);
                if (collidedRight){
                    switchXWalk();
                }
            }
            break;
        case IDLE:
            break;
    }
}

void Entity::AIyPatrol(){
    switch (aiState){
        case WALKING:
            if (walkDown){
                movement = glm::vec3(0,-2,0);
                if (collidedBottom){
                    switchYWalk();
                }
            }
            if (walkUp){
                movement = glm::vec3(0,2,0);
                if (collidedTop){
                    switchYWalk();
                }
            }
            break;
        case IDLE:
            break;
    }
}

void Entity::AI(Entity *player) {
    switch(aiType){
        case XPATROL:
            AIxPatrol();
            break;
        case YPATROL:
            AIyPatrol();
            break;
    }
}

void Entity::Update(float deltaTime, Entity *player, Entity *objects, int objectCount, Map *map) {
    
    if (isActive == false) return;
    
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        } else {
            animIndex = 0;
        }
    }
    
    //if (jump){
    //    jump = false;
    //    velocity.y += jumpPower;
    //}
    
    //movemenr
    velocity.x = movement.x * speed;
    velocity.y = movement.y * speed;
    velocity += acceleration * deltaTime;
            
    position.y += velocity.y * deltaTime;       //move on Y
    CheckCollisionsY(map);
    CheckCollisionsY(objects, objectCount);     //fix if needed
    
    position.x += velocity.x * deltaTime;       //move on x
    CheckCollisionsX(map);
    CheckCollisionsX(objects, objectCount);     //fix if needed
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    
    
    // ----ADDING COLLISION CHECK HERE LATER----
    if (entityType == ENEMY){
        AI(player);

        CheckCollisionsX(player, 1);
        CheckCollisionsY(player, 1);
        //if (collidedLeft || collidedRight || collidedTop || collidedBottom) {
            if (lastCollidedWith == player) {
                lastCollidedWith->isActive = false;
                //killPlayer = true;
            }
            lastCollidedWith = this;
        //}
    }
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram *program) {
    
    if (isActive == false) return;
    
    program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
