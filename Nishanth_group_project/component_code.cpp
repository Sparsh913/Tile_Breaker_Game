#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "fssimplewindow.h"
#include "yssimplesound.h"
#include <algorithm>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int TILE_ROWS = 5;
const int TILE_COLUMNS = 10;
const int TILE_WIDTH = 60;
const int TILE_HEIGHT = 20;
const float BL_PI = 3.14159265f;


class Paddle {
public:
    int x, y;
    int width, height;
    int speed;

    Paddle(int x, int y, int width, int height, int speed)
        : x(x), y(y), width(width), height(height), speed(speed) {}

    void MoveLeft() {
        if (x > 0) {
            x -= speed;
        }
    }

    void MoveRight() {
        if (x + width < SCREEN_WIDTH) {
            x += speed;
        }
    }

    void Render() {
        glColor3f(0.0f, 1.0f, 0.0f);  // Green paddle
        glBegin(GL_QUADS);
        glVertex2i(x, y);
        glVertex2i(x + width, y);
        glVertex2i(x + width, y + height);
        glVertex2i(x, y + height);
        glEnd();
    }
};

class Ball {
public:
    float x, y;
    float radius;
    float speedX, speedY;
    YsSoundPlayer::SoundData wallSound;

    Ball(float x, float y, float radius, float speedX, float speedY)
        : x(x), y(y), radius(radius), speedX(speedX), speedY(speedY) {}

    void LoadSounds(const std::string &wallSoundFile) {
        if (YSOK != wallSound.LoadWav(wallSoundFile.c_str())) {
            std::cerr << "Error: Could not load " << wallSoundFile << std::endl;
        }
    }

    void Move() {
        x += speedX;
        y += speedY;

        // Check for collisions with screen boundaries
        CheckScreenCollision();
    }

    void CheckScreenCollision() {
        if (x - radius <= 0 || x + radius >= SCREEN_WIDTH) {
            speedX = -speedX;  // Bounce horizontally
        }
        if (y - radius <= 0) {
            speedY = -speedY;  // Bounce vertically
        }
    }

    bool CheckPaddleCollision(const Paddle &paddle) {
        if (y + radius >= paddle.y && y - radius <= paddle.y + paddle.height &&
            x >= paddle.x && x <= paddle.x + paddle.width) {
            speedY = -fabs(speedY);  // Reverse Y direction
            return true;  // Collision occurred
        }
        return false;  // No collision
    }

    void Render() {
        glColor3f(1.0f, 1.0f, 0.0f);  // Yellow ball
        glBegin(GL_TRIANGLE_FAN);
        for (int i = 0; i < 100; i++) {
            float angle = 2.0f * BL_PI * i / 100.0f;
            glVertex2f(x + radius * cos(angle), y + radius * sin(angle));
        }
        glEnd();
    }

    bool IsOutOfBounds() const {
        return y - radius > SCREEN_HEIGHT;
    }
};


class Tile {
public:
    int x, y;
    int width, height;
    bool destroyed;

    Tile(int x, int y, int width, int height)
        : x(x), y(y), width(width), height(height), destroyed(false) {}

    void Render() {
        if (!destroyed) {
            // Fill the tile
            glColor3f(1.0f, 0.0f, 0.0f);  // Red fill color
            glBegin(GL_QUADS);
            glVertex2i(x, y);
            glVertex2i(x + width, y);
            glVertex2i(x + width, y + height);
            glVertex2i(x, y + height);
            glEnd();

            // Draw the border
            glColor3f(0.0f, 0.0f, 0.0f);  // Black border
            glBegin(GL_LINE_LOOP);
            glVertex2i(x, y);
            glVertex2i(x + width, y);
            glVertex2i(x + width, y + height);
            glVertex2i(x, y + height);
            glEnd();
        }
    }
};


class Particle {
public:
    float x, y;
    float dx, dy;
    float life;

    Particle(float x, float y, float dx, float dy, float life)
        : x(x), y(y), dx(dx), dy(dy), life(life) {}

    void Update() {
        x += dx;
        y += dy;
        life -= 0.02f;  // Fade out particles
    }

    void Render() {
        if (life > 0) {
            glColor4f(1.0f, 1.0f, 0.0f, life);  // Yellow fading particles
            glBegin(GL_POINTS);
            glVertex2f(x, y);
            glEnd();
        }
    }
};

class ParticleSystem {
public:
    std::vector<Particle> particles;

    void Explode(int x, int y) {
        for (int i = 0; i < 50; i++) {
            float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * BL_PI;
            float speed = static_cast<float>(rand()) / RAND_MAX * 4.0f + 1.0f;
            particles.emplace_back(x, y, cos(angle) * speed, sin(angle) * speed, 1.0f);
        }
    }

    void Update() {
        for (auto &particle : particles) {
            particle.Update();
        }

        // Remove dead particles
        particles.erase(std::remove_if(particles.begin(), particles.end(),
                                       [](Particle &p) { return p.life <= 0; }),
                        particles.end());
    }

    void Render() {
        for (auto &particle : particles) {
            particle.Render();
        }
    }
};
