#ifndef BOT_H
#define BOT_H
#include "Entity.h"

enum class BotState { IDLE, ROAM, CHASE, ATTACK, FLEE };

class Bot : public Entity {
public:
    BotState state;
    float stateTimer;
    Entity* target;
    Vec3 moveTarget;
    bool isBoss;
    float animTimer;

    Bot() { type = EntityType::BOT; reset(); }

    void reset() {
        maxHp = 100.0f; hp = maxHp;
        speed = 7.0f;
        isDead = false;
        state = BotState::ROAM;
        stateTimer = 0.0f;
        target = NULL;
        isActive = false;
        isBoss = false; 
        animTimer = 0.0f;
        
        float r = (rand()%10)/10.0f;
        float g = (rand()%10)/10.0f;
        float b = (rand()%10)/10.0f;
        color = Vec3(r, g, b);
        scaleV = Vec3(1, 1, 1);
    }
    
    void activateBossMode() {
        if (!isBoss) {
            isBoss = true;
            maxHp *= 2.0f;
            hp = maxHp; 
            speed *= 1.5f;
            color = Vec3(0.5f, 0, 0); 
            scaleV = Vec3(1.3f, 1.3f, 1.3f); 
            fireRate *= 0.7f; 
        }
    }

    void updateAI(float dt, Map* map, Entity* player, std::vector<Bot>& otherBots) {
        if(isDead) return;
        
        stateTimer = fmax(0.0f, stateTimer - dt);
        fireTimer = fmax(0.0f, fireTimer - dt);
        
        animTimer += dt * 10.0f;
        float bounce = sin(animTimer) * 0.05f;
        scaleV.y = (isBoss ? 1.3f : 1.0f) + bounce;
        scaleV.x = (isBoss ? 1.3f : 1.0f) - (bounce * 0.5f);
        scaleV.z = scaleV.x;

        float minDist = 25.0f; 
        target = NULL;

        if (!player->isDead) {
            float d = (player->pos - pos).length();
            if (d < minDist) { minDist = d; target = player; }
        }

        for(auto& b : otherBots) {
            if(&b != this && !b.isDead && b.isActive) {
                float d = (b.pos - pos).length();
                if (d < minDist) { minDist = d; target = &b; }
            }
        }

        if (hp < (maxHp * 0.3f)) state = BotState::FLEE;
        else if (target != NULL) {
            if (minDist < 8.0f) state = BotState::ATTACK;
            else state = BotState::CHASE;
        } else {
             if (stateTimer <= 0.0f) state = BotState::ROAM;
        }

        Vec3 dir(0,0,0);
        switch(state) {
            case BotState::ROAM:
                if (stateTimer <= 0.0f) {
                    stateTimer = 3.0f;
                    float angle = (rand()%360) * 0.017f;
                    moveTarget = pos + Vec3(cos(angle)*10, 0, sin(angle)*10);
                }
                dir = moveTarget - pos;
                break;
            case BotState::CHASE:
                if(target) dir = target->pos - pos;
                break;
            case BotState::FLEE:
                if(target) dir = pos - target->pos; 
                break;
            case BotState::ATTACK:
                if(target) dir = target->pos - pos;
                break;
        }

        if (state != BotState::ATTACK && dir.length() > 0.1f) {
            dir.normalize();
            Vec3 nextPos = pos + (dir * speed * dt);
            if (!map->checkCollision(nextPos, isBoss ? 1.5f : 1.0f)) {
                pos = nextPos;
            } else {
                stateTimer = 0.0f; 
            }
        }
    }
};
#endif

