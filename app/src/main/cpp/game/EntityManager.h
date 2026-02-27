#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H
#include "Bot.h"
#include "WeaponSystem.h"
#include "Map.h"

class Particle : public GameObject {
public:
    Vec3 velocity;
    float life;
    void spawn(Vec3 p, Vec3 v, Vec3 c, float l) {
        pos = p; velocity = v; color = c; life = l;
        isActive = true; alpha = 1.0f; scaleV = Vec3(0.3f,0.3f,0.3f);
    }
    void update(float dt) override {
        if(!isActive) return;
        pos = pos + (velocity * dt);
        life -= dt;
        alpha = life; 
        if(life <= 0.0f) isActive = false;
    }
};

struct KillFeed { bool active; float timer; float alpha; };

class EntityManager {
public:
    std::vector<Bot> bots;
    std::vector<Particle> particles;
    int botsAlive;
    bool eventKill;
    bool bossModeTriggered;
    bool eventBossKill; 
    KillFeed killFeed;

    EntityManager() {
        bots.resize(40);
        particles.resize(100);
        reset();
    }

    void reset() {
        botsAlive = 0; eventKill = false; eventBossKill = false;
        bossModeTriggered = false;
        killFeed.active = false; killFeed.timer = 0.0f; killFeed.alpha = 0.0f;
        for(auto& b : bots) b.isActive = false;
        for(auto& p : particles) p.isActive = false;
    }

    void spawnEffect(Vec3 p, Vec3 color) {
        for(auto& part : particles) {
            if(!part.isActive) {
                float vx = (rand()%20 - 10) * 0.2f;
                float vy = (rand()%10) * 0.3f + 1.0f;
                float vz = (rand()%20 - 10) * 0.2f;
                part.spawn(p, Vec3(vx, vy, vz), color, 0.8f);
                return;
            }
        }
    }
    
    void spawnBurst(Vec3 p, Vec3 color, int count) {
        for(int i=0; i<count; i++) spawnEffect(p, color);
    }

    void spawnBots(int count) { 
        if(count > 40) count = 40;
        for(int i=0; i<count; i++) {
            for(auto& b : bots) {
                if(!b.isActive) {
                    b.reset(); b.isActive = true;
                    int a=0; float x,z; 
                    do { x=(rand()%100)-50.0f; z=(rand()%100)-50.0f; a++; } while(abs(x)<5 && abs(z)<5 && a<10);
                    b.pos = Vec3(x,0,z); botsAlive++; break;
                }
            }
        }
    }
    
    bool consumeKillEvent() { if(eventKill){ eventKill=false; return true; } return false; }
    
    void triggerKillFeed() {
        killFeed.active = true;
        killFeed.timer = 2.0f;
        killFeed.alpha = 1.0f;
    }

    void update(float dt, Map* map, Player* player, WeaponSystem* ws) {
        if (killFeed.active) {
            killFeed.timer = fmax(0.0f, killFeed.timer - dt);
            if (killFeed.timer < 0.5f) killFeed.alpha = killFeed.timer / 0.5f; 
            
            // Safety Clamps
            if (killFeed.alpha < 0.0f) killFeed.alpha = 0.0f;
            if (killFeed.alpha > 1.0f) killFeed.alpha = 1.0f;
            
            if (killFeed.timer <= 0.0f) { killFeed.active = false; killFeed.alpha = 0.0f; }
        }

        int currentAlive = 0;
        for(auto& b : bots) {
            if(!b.isActive) continue;
            if(!b.isDead) {
                currentAlive++;
                b.updateAI(dt, map, player, bots);
                
                if(b.state == BotState::ATTACK && b.fireTimer <= 0.0f && b.target) {
                     Vec3 dir = b.target->pos - b.pos; 
                     if(dir.length() > 0.01f) {
                         dir.normalize();
                         ws->fire(b.pos, dir, false, WeaponType::PISTOL); 
                         b.fireTimer = b.isBoss ? 0.5f : (1.0f + (rand()%100)/100.0f);
                     }
                }
            }
        }
        botsAlive = currentAlive;
        if(botsAlive < 0) botsAlive = 0;

        if (!bossModeTriggered && botsAlive > 0 && botsAlive <= 3) {
            bossModeTriggered = true;
            for(auto& b : bots) if(b.isActive && !b.isDead) b.activateBossMode();
        }

        for(auto& bullet : ws->bullets) {
            if(!bullet.isActive) continue;

            if(bullet.isPlayerBullet) {
                for(auto& b : bots) {
                    if(b.isActive && !b.isDead && checkCircleCollision(bullet.pos, 0.5f, b.pos, b.isBoss ? 1.5f : 1.0f)) {
                        float dmg = 20.0f;
                        if(bullet.type == WeaponType::BEAM) dmg = 50.0f;
                        if(bullet.type == WeaponType::SHOTGUN) dmg = 10.0f;
                        if(player->ultActive) dmg *= 2.0f;
                        
                        b.takeDamage(dmg);
                        bullet.isActive = false;
                        
                        if(b.isDead) { 
                            eventKill = true; 
                            triggerKillFeed();
                            if(b.isBoss) {
                                eventBossKill = true;
                                spawnBurst(b.pos, Vec3(0.5f, 0, 0), 20);
                            } else {
                                spawnEffect(b.pos, Vec3(1,0,0));
                            }
                        } 
                    }
                }
            } else {
                if(!player->isDead && checkCircleCollision(bullet.pos, 0.5f, player->pos, 1.0f)) {
                    player->takeDamage(5.0f);
                    bullet.isActive = false;
                    spawnEffect(player->pos, Vec3(1,0,0));
                }
            }
        }
        
        for(auto& p : particles) p.update(dt);
    }

    void draw(Shader* s, Mat4& vp) {
        for(auto& b : bots) b.draw(s, vp);
        for(auto& p : particles) p.draw(s, vp);
    }
};
#endif

