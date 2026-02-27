#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H
#include "game/Player.h"
#include "game/EntityManager.h"
#include "game/WeaponSystem.h"
#include "game/Map.h"

class GameEngine {
    Shader* shader;
    Player* player;
    WeaponSystem* weapons;
    EntityManager* entities;
    Map* map;
    
    Mat4 projMat, viewMat;
    float screenW, screenH;
    int gameState;
    
    float zoneRadius;
    float cameraShake;
    float slowMoTimer;
    
    float lastDt; // Store for input synchronization

public:
    GameEngine() {
        shader = new Shader(); 
        player = new Player(); 
        weapons = new WeaponSystem(); 
        entities = new EntityManager(); 
        map = new Map();
        lastDt = 0.016f;
        reset();
    }
    
    ~GameEngine() { delete shader; delete player; delete weapons; delete entities; delete map; }

    void reset() {
        if(player) player->reset();
        if(entities) entities->reset();
        if(map) map->generateDerb();
        if(entities) entities->spawnBots(30);
        gameState = 0;
        zoneRadius = GameConfig::ZONE_START_RADIUS;
        cameraShake = 0.0f;
        slowMoTimer = 0.0f;
    }

    void init() { 
        shader->init(); 
        glEnable(GL_DEPTH_TEST); 
        glEnable(GL_BLEND); 
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f); 
    }
    
    void resize(int w, int h) {
        if (h == 0) h = 1; 
        screenW=(float)w; screenH=(float)h; glViewport(0,0,w,h);
        float aspect=screenW/screenH; Mat4::perspective(projMat, 1.0f, aspect, 1.0f, 100.0f);
    }

    void input(float jx, float jy, bool fire, bool dash, bool ult) {
        if(gameState != 0) return;
        
        if(jx>1.0f) jx=1.0f; if(jx<-1.0f) jx=-1.0f;
        if(jy>1.0f) jy=1.0f; if(jy<-1.0f) jy=-1.0f;
        
        Vec3 moveDir(jx, 0, jy);
        if(moveDir.length() > 0.01f) {
            // FIX: Use lastDt for sync
            Vec3 nextPos = player->pos + (moveDir * player->currentSpeed * lastDt);
            if (!map->checkCollision(nextPos, 1.0f)) {
                player->pos = nextPos;
                player->setInput(jx, jy);
            }
        }
        
        if(dash) { player->triggerDash(); cameraShake = 0.3f; }
        if(ult) { player->triggerUlt(); cameraShake = 0.5f; }
        
        if(fire && player->fireTimer <= 0.0f) {
            Vec3 dir = moveDir; 
            if(dir.length() < 0.1f) dir = Vec3(0,0,-1); else dir.normalize();
            
            WeaponType wType = WeaponType::PISTOL; 
            if (player->ultActive) wType = WeaponType::BEAM;
            
            weapons->fire(player->pos, dir, true, wType);
            player->fireTimer = (wType==WeaponType::BEAM) ? 0.1f : 0.3f;
            cameraShake = 0.1f;
        }
    }

    void step(float dt) {
        if(dt > 0.1f) dt = 0.1f; 
        if(dt < 0.001f) dt = 0.001f; 

        float realDt = dt;
        lastDt = dt; // Store for Input Sync

        if (slowMoTimer > 0.0f) {
            dt *= 0.2f; 
            slowMoTimer = fmax(0.0f, slowMoTimer - realDt);
        }

        if (gameState == 0) {
            if(zoneRadius > GameConfig::ZONE_MIN_RADIUS) zoneRadius -= GameConfig::ZONE_SHRINK_SPEED * dt;
            if(player->pos.length() > zoneRadius) player->takeDamage(GameConfig::ZONE_DMG * dt);

            player->update(dt);
            weapons->update(dt);
            entities->update(dt, map, player, weapons);

            if (player->isDead) gameState = 2;
            else if (entities->botsAlive == 0) gameState = 1;
            
            if (entities->eventKill) slowMoTimer = 0.2f;
            if (entities->eventBossKill) {
                slowMoTimer = 1.0f; 
                cameraShake = 1.0f;
                entities->eventBossKill = false; 
            }
        }

        if (cameraShake > 0.0f) cameraShake = fmax(0.0f, cameraShake - realDt);
        
        float shakeX = ((rand()%200 - 100)/5000.0f) * (cameraShake * 10.0f);
        float shakeZ = ((rand()%200 - 100)/5000.0f) * (cameraShake * 10.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader->use();

        Vec3 camPos = player->pos + Vec3(shakeX, 25, 18 + shakeZ); 
        Mat4::lookAt(viewMat, camPos, player->pos, Vec3(0,1,0));
        Mat4 vp; Mat4::multiply(vp, projMat, viewMat);
        
        GameObject wall; wall.scaleV = Vec3(4, 4, 4); wall.color = Vec3(0.4f, 0.4f, 0.5f); wall.isActive = true;
        for(auto& w : map->walls) { 
            float distSq = (w.x - player->pos.x)*(w.x - player->pos.x) + (w.z - player->pos.z)*(w.z - player->pos.z);
            if(distSq < GameConfig::WALL_CULL_SQ) { 
                wall.pos=w; wall.draw(shader, vp); 
            } 
        }

        player->drawAura(shader, vp);
        player->draw(shader, vp);
        entities->draw(shader, vp);
        weapons->draw(shader, vp);
    }
    
    bool consumeKillEvent() { return entities->consumeKillEvent(); }
    bool consumeShootEvent() { return weapons->consumeShootEvent(); }
    int getBotsAlive() { return entities->botsAlive; }
    int getHP() { return (int)player->hp; }
    int getState() { return gameState; }
    float getUltProgress() { 
        if (player->ultCooldown <= 0.0f) return 1.0f;
        float prog = 1.0f - (player->ultCooldown / GameConfig::ULT_COOLDOWN); 
        if(prog < 0.0f) prog = 0.0f; if(prog > 1.0f) prog = 1.0f; 
        return prog;
    }
    bool isUltActive() { return player->ultActive; }
    bool hasKillFeed() { return entities->killFeed.active; }
    float getKillFeedAlpha() { return entities->killFeed.alpha; }
};
#endif

