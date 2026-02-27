#ifndef PLAYER_H
#define PLAYER_H
#include "Entity.h"

class Player : public Entity {
public:
    bool dashActive;
    float dashTimer, dashCooldown;
    bool ultActive;
    float ultTimer, ultCooldown;
    
    GameObject aura;
    float animTimer;

    float baseSpeed, baseDmg, baseMaxHp, baseDashCd;
    float currentSpeed; 

    Player() {
        type = EntityType::PLAYER;
        baseMaxHp = GameConfig::PLAYER_BASE_HP; 
        baseSpeed = GameConfig::PLAYER_BASE_SPEED;
        baseDmg = GameConfig::PLAYER_BASE_DMG;
        baseDashCd = 2.0f;
        reset();
    }

    void reset() {
        isDead = false; pos = Vec3(0,0,0); isActive = true;
        dashActive = false; dashTimer = 0; dashCooldown = 0;
        ultActive = false; ultTimer = 0; ultCooldown = 0;
        
        weaponType = WeaponType::PISTOL; 
        skinId = 0; animTimer = 0;
        scaleV = Vec3(1,1,1);
        
        applySkin(); 
    }

    void applySkin() {
        speed = baseSpeed;
        weaponDmg = baseDmg;
        maxHp = baseMaxHp;
        baseDashCd = 2.0f;

        switch(skinId) {
            case 0: color = Vec3(1, 0.2f, 0.2f); weaponDmg *= 1.25f; break; 
            case 1: color = Vec3(0.5f, 0, 0.5f); speed *= 1.2f; break; 
            case 2: color = Vec3(0.2f, 0.2f, 1); baseDashCd = 1.0f; break; 
            case 3: color = Vec3(1, 0.8f, 0); maxHp += 50.0f; break; 
        }
        
        hp = maxHp; 
        currentSpeed = speed; 
        aura.isActive = false; 
    }

    void setInput(float x, float y) {
        if(isDead) return;
        if(x > 1.0f) x = 1.0f; if(x < -1.0f) x = -1.0f; 
        if(y > 1.0f) y = 1.0f; if(y < -1.0f) y = -1.0f;
        moveDir = Vec3(x, 0, y);
    }

    void triggerDash() {
        if(dashCooldown <= 0.0f && !isDead) {
            dashActive = true;
            dashTimer = GameConfig::DASH_DURATION;
            dashCooldown = baseDashCd; 
            scaleV = Vec3(0.7f, 0.7f, 1.4f); 
        }
    }

    void triggerUlt() {
        if(ultCooldown <= 0.0f && !isDead) {
            ultActive = true;
            ultTimer = GameConfig::ULT_DURATION;
            ultCooldown = GameConfig::ULT_COOLDOWN;
            hp = fmin(hp + 30.0f, maxHp); 
            aura.isActive = true;
            aura.color = Vec3(0, 1, 1); 
            scaleV = Vec3(1.5f, 1.5f, 1.5f); 
        }
    }

    void update(float dt) override {
        if(isDead) return;

        if(dashActive) {
            currentSpeed = GameConfig::PLAYER_DASH_SPEED;
            dashTimer = fmax(0.0f, dashTimer - dt);
            scaleV = Vec3(0.7f, 0.7f, 1.4f); 
            
            if(dashTimer <= 0.0f) { 
                dashActive = false; 
                currentSpeed = speed; 
            }
        } else {
            currentSpeed = speed; 
            dashCooldown = fmax(0.0f, dashCooldown - dt);
        }

        if(!dashActive) {
            if (moveDir.length() > 0.1f) {
                animTimer += dt * 15.0f;
                float bounce = sin(animTimer) * 0.1f;
                float lerp = (10.0f * dt > 1.0f) ? 1.0f : 10.0f * dt;
                
                scaleV.y += ((1.0f + bounce) - scaleV.y) * lerp;
                scaleV.x += ((1.0f - bounce*0.5f) - scaleV.x) * lerp;
                scaleV.z += ((1.0f - bounce*0.5f) - scaleV.z) * lerp;
            } else {
                animTimer = 0;
                float restore = (5.0f * dt > 1.0f) ? 1.0f : 5.0f * dt;
                scaleV.x += (1.0f - scaleV.x) * restore;
                scaleV.y += (1.0f - scaleV.y) * restore;
                scaleV.z += (1.0f - scaleV.z) * restore;
            }
        }

        if(scaleV.x < 0.1f) scaleV.x = 0.1f;
        if(scaleV.y < 0.1f) scaleV.y = 0.1f;
        if(scaleV.z < 0.1f) scaleV.z = 0.1f;

        if(ultActive) {
            ultTimer = fmax(0.0f, ultTimer - dt);
            if(ultTimer <= 0.0f) { 
                ultActive = false; 
                aura.isActive = false; 
            }
        } else {
            ultCooldown = fmax(0.0f, ultCooldown - dt);
        }
        
        if(aura.isActive) {
            aura.pos = pos;
            aura.scaleV = Vec3(1.8f, 0.1f, 1.8f); 
            aura.alpha = 0.4f + sin(ultTimer * 8.0f) * 0.3f; 
        }
        
        fireTimer = fmax(0.0f, fireTimer - dt);
    }
    
    void drawAura(Shader* s, Mat4& vp) {
        if(aura.isActive) aura.draw(s, vp);
    }
};
#endif

