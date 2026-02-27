#ifndef MAP_H
#define MAP_H
#include "../core/MathUtils.h"
#include "GameObject.h"
#include <vector>

class Map {
public:
    int grid[GameConfig::MAP_SIZE][GameConfig::MAP_SIZE];
    std::vector<Vec3> walls; 

    Map() {
        generateDerb();
    }

    void generateDerb() {
        walls.clear(); 
        
        for(int i=0; i<GameConfig::MAP_SIZE; i++) 
            for(int j=0; j<GameConfig::MAP_SIZE; j++) grid[i][j] = 0;

        for(int i=0; i<GameConfig::MAP_SIZE; i++) {
            grid[i][0] = 1; grid[i][GameConfig::MAP_SIZE-1] = 1;
            grid[0][i] = 1; grid[GameConfig::MAP_SIZE-1][i] = 1;
        }

        for(int i=0; i<40; i++) {
            int x = rand() % (GameConfig::MAP_SIZE-4) + 2;
            int z = rand() % (GameConfig::MAP_SIZE-4) + 2;
            for(int bx=0; bx<2; bx++) for(int bz=0; bz<2; bz++) grid[x+bx][z+bz] = 1;
        }
        
        float offset = (GameConfig::MAP_SIZE * GameConfig::CELL_SIZE) / 2.0f;
        for(int i=0; i<GameConfig::MAP_SIZE; i++) {
            for(int j=0; j<GameConfig::MAP_SIZE; j++) {
                if(grid[i][j] == 1) {
                    float x = (i * GameConfig::CELL_SIZE) - offset;
                    float z = (j * GameConfig::CELL_SIZE) - offset;
                    walls.push_back(Vec3(x, 0, z));
                }
            }
        }
    }

    // FIX: Using radius in collision
    bool checkCollision(Vec3 pos, float radius) {
        float offset = (GameConfig::MAP_SIZE * GameConfig::CELL_SIZE) / 2.0f;
        int gx = (int)((pos.x + offset) / GameConfig::CELL_SIZE);
        int gz = (int)((pos.z + offset) / GameConfig::CELL_SIZE);

        // Check 3x3 grid around player
        for(int i = gx-1; i <= gx+1; i++) {
            for(int j = gz-1; j <= gz+1; j++) {
                if(i >= 0 && i < GameConfig::MAP_SIZE && j >= 0 && j < GameConfig::MAP_SIZE) {
                    if(grid[i][j] == 1) {
                        float wx = (i * GameConfig::CELL_SIZE) - offset;
                        float wz = (j * GameConfig::CELL_SIZE) - offset;
                        
                        // Wall AABB: Wall is Cell Size (4.0), half size is 2.0
                        float halfWall = GameConfig::CELL_SIZE * 0.5f;
                        
                        // AABB vs AABB Collision (Player approximated as box of 'radius' size)
                        // Player Box: [pos.x - radius, pos.x + radius]
                        // Wall Box:   [wx - halfWall, wx + halfWall]
                        
                        bool xOverlap = (pos.x - radius < wx + halfWall) && (pos.x + radius > wx - halfWall);
                        bool zOverlap = (pos.z - radius < wz + halfWall) && (pos.z + radius > wz - halfWall);
                        
                        if(xOverlap && zOverlap) return true;
                    }
                }
            }
        }
        return false;
    }
};
#endif

