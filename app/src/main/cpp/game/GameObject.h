#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H
#include "../core/MathUtils.h"
#include "../core/Shader.h"

namespace GameConfig {
    constexpr int MAP_SIZE = 50;
    constexpr float CELL_SIZE = 4.0f;
    constexpr float WALL_CULL_SQ = 2500.0f;

    constexpr float PLAYER_BASE_HP = 100.0f;
    constexpr float PLAYER_BASE_SPEED = 8.0f;
    constexpr float PLAYER_DASH_SPEED = 25.0f;
    constexpr float PLAYER_BASE_DMG = 15.0f;
    
    constexpr float DASH_DURATION = 0.2f;
    constexpr float ULT_DURATION = 5.0f;
    constexpr float ULT_COOLDOWN = 20.0f;

    constexpr float BULLET_SPEED_STD = 30.0f;
    constexpr float BULLET_SPEED_BEAM = 60.0f;
    constexpr float SHOTGUN_SPREAD = 0.15f;
    
    constexpr float ZONE_START_RADIUS = 100.0f;
    constexpr float ZONE_MIN_RADIUS = 15.0f;
    constexpr float ZONE_SHRINK_SPEED = 1.0f;
    constexpr float ZONE_DMG = 5.0f;
}

enum class WeaponType { 
    PISTOL, 
    SHOTGUN, 
    BEAM 
};

static const float CUBE[] = { 
    -0.5f,-0.5f,0.5f, 0.5f,-0.5f,0.5f, 0.5f,0.5f,0.5f, 0.5f,0.5f,0.5f, -0.5f,0.5f,0.5f, -0.5f,-0.5f,0.5f,
    -0.5f,-0.5f,-0.5f, -0.5f,0.5f,-0.5f, 0.5f,0.5f,-0.5f, 0.5f,0.5f,-0.5f, 0.5f,-0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
    -0.5f,0.5f,-0.5f, -0.5f,0.5f,0.5f, 0.5f,0.5f,0.5f, 0.5f,0.5f,0.5f, 0.5f,0.5f,-0.5f, -0.5f,0.5f,-0.5f,
    -0.5f,-0.5f,-0.5f, 0.5f,-0.5f,-0.5f, 0.5f,-0.5f,0.5f, 0.5f,-0.5f,0.5f, -0.5f,-0.5f,0.5f, -0.5f,-0.5f,-0.5f,
    0.5f,-0.5f,-0.5f, 0.5f,0.5f,-0.5f, 0.5f,0.5f,0.5f, 0.5f,0.5f,0.5f, 0.5f,-0.5f,0.5f, 0.5f,-0.5f,-0.5f,
    -0.5f,-0.5f,-0.5f, -0.5f,-0.5f,0.5f, -0.5f,0.5f,0.5f, -0.5f,0.5f,0.5f, -0.5f,0.5f,-0.5f, -0.5f,-0.5f,-0.5f
};

class GameObject {
public:
    Vec3 pos, scaleV, color;
    float alpha;
    bool isActive;

    GameObject() : isActive(false), scaleV(1,1,1), color(1,1,1), alpha(1.0f) {}
    virtual ~GameObject() {}
    
    virtual void update(float dt) {} 

    void draw(Shader* s, Mat4& vp) {
        if (!isActive) return;
        Mat4 model; // Identity
        Mat4::translate(model, pos.x, pos.y, pos.z);
        Mat4::scale(model, scaleV.x, scaleV.y, scaleV.z);
        
        Mat4 mvp;
        Mat4::multiply(mvp, vp, model); // mvp = vp * model
        
        glUniformMatrix4fv(s->mvpHandle, 1, GL_FALSE, mvp.m);
        glUniform4f(s->colorHandle, color.x, color.y, color.z, 1.0f);
        s->setAlpha(alpha);
        
        glVertexAttribPointer(s->posHandle, 3, GL_FLOAT, GL_FALSE, 0, CUBE);
        glEnableVertexAttribArray(s->posHandle);
        glDrawArrays(GL_TRIANGLES, 0, 36); 
    }
};
#endif

