#ifndef WEAPON_SYSTEM_H
#define WEAPON_SYSTEM_H
#include <vector>
#include "GameObject.h"

class Bullet : public GameObject {
public:
    Vec3 velocity;
    float life;
    bool isPlayerBullet;
    WeaponType type;

    void spawn(Vec3 p, Vec3 dir, bool isP, WeaponType wType) {
        pos = p;
        life = 1.5f; isActive = true; isPlayerBullet = isP; type = wType;
        
        if(isP) color = (wType == WeaponType::BEAM) ? Vec3(0,1,1) : Vec3(1,1,0); 
        else color = Vec3(1,0,0);
        
        float speed = GameConfig::BULLET_SPEED_STD;
        if(wType == WeaponType::BEAM) { 
            scaleV = Vec3(0.5f, 0.5f, 3.0f); 
            speed = GameConfig::BULLET_SPEED_BEAM;
        } else { 
            scaleV = Vec3(0.2f, 0.2f, 0.2f);
        }
        velocity = dir * speed;
    }

    void update(float dt) override {
        if (!isActive) return;
        pos = pos + (velocity * dt);
        life -= dt; // Not clamping life is fine, checked next line
        if (life <= 0.0f) isActive = false;
    }
};

class WeaponSystem {
public:
    std::vector<Bullet> bullets;
    bool eventShoot;

    WeaponSystem() {
        bullets.resize(100); 
        eventShoot = false;
    }

    void update(float dt) { for(auto& b : bullets) b.update(dt); }

    void fire(Vec3 origin, Vec3 dir, bool isPlayer, WeaponType wType) {
        if(wType == WeaponType::SHOTGUN) {
            spawnBullet(origin, dir, isPlayer, wType); 
            
            float spread = GameConfig::SHOTGUN_SPREAD;
            Vec3 left = Vec3(dir.x * 0.9f + dir.z * spread, 0, dir.z * 0.9f - dir.x * spread);
            Vec3 right = Vec3(dir.x * 0.9f - dir.z * spread, 0, dir.z * 0.9f + dir.x * spread);
            
            if(left.length()>0.001f) left.normalize(); 
            if(right.length()>0.001f) right.normalize();
            
            spawnBullet(origin, left, isPlayer, wType);
            spawnBullet(origin, right, isPlayer, wType);
        } else {
            spawnBullet(origin, dir, isPlayer, wType);
        }
        
        if(isPlayer) eventShoot = true;
    }

    void spawnBullet(Vec3 p, Vec3 d, bool isP, WeaponType t) {
        for(auto& b : bullets) {
            if(!b.isActive) { b.spawn(p, d, isP, t); return; }
        }
    }
    
    bool consumeShootEvent() { if(eventShoot){ eventShoot=false; return true; } return false; }
    void draw(Shader* s, Mat4& vp) { for(auto& b : bullets) b.draw(s, vp); }
};
#endif

