#ifndef RENDERING_MODULE_H
#define RENDERING_MODULE_H

#include <vector>
#include "fssimplewindow.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

class Paddle {
public:
    int x, y;
    int width, height;
    int speed;

    Paddle(int x, int y, int width, int height, int speed);
    void Render();
};

class Ball {
public:
    float x, y;
    float radius;
    float speedX, speedY;

    Ball(float x, float y, float radius, float speedX, float speedY);
    void Render();
};

class Tile {
public:
    int x, y;
    int width, height;
    bool destroyed;
    bool isSpecial;

    Tile(int x, int y, int width, int height, bool isSpecial = false);
    void Render();
};

class Particle {
public:
    float x, y;
    float dx, dy;
    float life;

    Particle(float x, float y, float dx, float dy, float life);
    void Render();
};

class ParticleSystem {
public:
    std::vector<Particle> particles;
    void Render();
};

void RenderBackground();
void renderGame(const Paddle &paddle, const Ball &ball, const std::vector<Tile> &tiles, const ParticleSystem &particleSystem);

#endif // RENDERING_MODULE_H