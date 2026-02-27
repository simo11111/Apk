#ifndef ENTITY_H
#define ENTITY_H
#include "GameObject.h"

enum class EntityType { PLAYER, BOT };

class Entity : public GameObject {
public:
    EntityType type;
    float hp, maxHp;
    float speed;
    bool isDead;
    int kills;
    int skinId;
    
    float fireTimer;
    float fireRate;
    float weaponDmg;
    WeaponType weaponType;

    Entity() { 
        type = EntityType::BOT;
        isDead = false; 
        kills = 0; 
        fireTimer = 0.0f;
        fireRate = 0.5f; 
        weaponDmg = 10.0f;
        weaponType = WeaponType::PISTOL;
        hp = 100.0f; maxHp = 100.0f;
        speed = 8.0f;
        skinId = 0;
    }

    virtual void takeDamage(float amount) {
        if(isDead) return;
        hp -= amount;
        if(hp <= 0.0f) {
            hp = 0.0f;
            isDead = true;
            color = Vec3(0.2f, 0.2f, 0.2f); 
            scaleV.y = 0.2f; 
        }
    }
};
#endif

