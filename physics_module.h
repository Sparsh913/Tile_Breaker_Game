#ifndef PHYSICS_MODULE_H
#define PHYSICS_MODULE_H

#include <vector>
#include "yssimplesound.h"

class Paddle {
public:
    void MoveLeft();
    void MoveRight();
};

class Ball {
public:
    void Move();
    void CheckScreenCollision();
    bool CheckPaddleCollision(const Paddle &paddle);
    bool IsOutOfBounds() const;
    void LoadSounds(const std::string &wallSoundFile);
};

class Tile {
public:
    bool IsDestroyed();
    bool IsSpecial();
};

class Particle {
public:
    void Update();
};

class ParticleSystem {
public:
    void Explode(int x, int y);
    void Update();
};

void initializeTiles(std::vector<Tile> &tiles, int screenWidth, int screenHeight);
void updateGameState(int key, Paddle &paddle, Ball &ball, std::vector<Tile> &tiles,
                     ParticleSystem &particleSystem, YsSoundPlayer &soundPlayer,
                     YsSoundPlayer::SoundData &paddleSound);

#endif // PHYSICS_MODULE_H