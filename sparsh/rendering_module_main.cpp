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
YsSoundPlayer soundPlayer;
YsSoundPlayer::SoundData paddleSound;

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
            soundPlayer.PlayOneShot(wallSound);
        }
        if (y - radius <= 0) {
            speedY = -speedY;  // Bounce vertically
            soundPlayer.PlayOneShot(wallSound);
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
    bool isSpecial;  // New member variable to indicate if the tile is special

    Tile(int x, int y, int width, int height, bool isSpecial = false)
        : x(x), y(y), width(width), height(height), destroyed(false), isSpecial(isSpecial) {}

    void Render() {
        if (!destroyed) {
            if (isSpecial) {
                glColor3f(0.0f, 1.0f, 1.0f);  // Cyan fill color for special tiles
            } else {
                glColor3f(1.0f, 0.0f, 0.0f);  // Red fill color for regular tiles
            }
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

    bool IsDestroyed() {
        return destroyed;
    }

    bool IsSpecial() {
        return isSpecial;
    }
};

bool AreAllTilesDestroyed(const std::vector<Tile> &tiles, Ball &ball, Paddle &paddle) {
    for (const auto &tile : tiles) {
        if (!tile.destroyed) {
            return false;
        }
    }
    // Update ball speed and paddle width for the next level
    ball.speedX *= 1.2;  // Increase ball speed by 20%
    ball.speedY *= 1.2;
    paddle.width *= 0.8;  // Decrease paddle width by 20%
    return true;
}


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

void RenderBackground() {
    glColor3f(0.1f, 0.1f, 0.3f);  // Dark blue background
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(SCREEN_WIDTH, 0);
    glVertex2i(SCREEN_WIDTH, SCREEN_HEIGHT);
    glVertex2i(0, SCREEN_HEIGHT);
    glEnd();
}

void ResetGame(Ball &ball, Paddle &paddle, std::vector<Tile> &tiles) {
    // Reset ball position and speed
    ball.x = SCREEN_WIDTH / 2;
    ball.y = SCREEN_HEIGHT / 2;
    ball.speedX = 2;
    ball.speedY = -2;

    // Reset paddle position and width
    paddle.x = SCREEN_WIDTH / 2 - 50;
    paddle.width = 100;

    // Reset tiles
    for (auto &tile : tiles) {
        tile.destroyed = false;
    }
}

int GameLoop() {
    FsOpenWindow(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
    srand(static_cast<unsigned int>(time(nullptr)));

    // Initialize sound files
    soundPlayer.Start();
    if (YSOK != paddleSound.LoadWav("bounce.wav")) {
        std::cerr << "Error: Could not load paddle_hit.wav" << std::endl;
    }

    Paddle paddle(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 50, 100, 10, 10);
    Ball ball(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 8, 2, -2);

    // Load ball's wall collision sound
    ball.LoadSounds("bounce.wav");

    std::vector<Tile> tiles;
    ParticleSystem particleSystem;

    // Initialize tiles
    int totalTileWidth = TILE_COLUMNS * TILE_WIDTH;
    int totalTileHeight = TILE_ROWS * TILE_HEIGHT;

    int startX = (SCREEN_WIDTH - totalTileWidth) / 2;  // Center horizontally
    int startY = 50;  // Fixed vertical position

    for (int row = 0; row < TILE_ROWS; row++) {
        for (int col = 0; col < TILE_COLUMNS; col++) {
            tiles.emplace_back(startX + col * TILE_WIDTH, startY + row * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT);
        }
    }

    while (true) {
        FsPollDevice();
        int key = FsInkey();
        if (key == FSKEY_ESC) {
            break;
        }

        if (key == FSKEY_LEFT) {
            paddle.MoveLeft();
        }
        if (key == FSKEY_RIGHT) {
            paddle.MoveRight();
        }

        ball.Move();

        // Check collision with paddle
        if (ball.CheckPaddleCollision(paddle)) {
            soundPlayer.PlayOneShot(paddleSound);  // Play paddle collision sound
        }

        // Check collision with tiles
        for (auto &tile : tiles) {
            if (!tile.destroyed && ball.x + ball.radius >= tile.x && ball.x - ball.radius <= tile.x + tile.width &&
                ball.y + ball.radius >= tile.y && ball.y - ball.radius <= tile.y + tile.height) {
                tile.destroyed = true;
                ball.speedY = -ball.speedY;
                particleSystem.Explode(tile.x + tile.width / 2, tile.y + tile.height / 2);
                soundPlayer.PlayOneShot(paddleSound);  // Play tile collision sound
            }
        }

        // Check if ball is out of bounds
        if (ball.IsOutOfBounds()) {
            std::cout << "Game Over!" << std::endl;
            break;
        }

        // Update particles
        particleSystem.Update();

        // Render everything
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        RenderBackground();
        paddle.Render();
        ball.Render();
        for (auto &tile : tiles) {
            tile.Render();
        }
        particleSystem.Render();
        FsSwapBuffers();
        FsSleep(10);
    }
    soundPlayer.End();

    return 0;
}
