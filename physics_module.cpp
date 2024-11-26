#include "physics_module.h"
#include <cmath>
#include <algorithm>

void Paddle::MoveLeft() {
    if (x > 0) {
        x -= speed;
    }
}

void Paddle::MoveRight() {
    if (x + width < SCREEN_WIDTH) {
        x += speed;
    }
}

void Ball::Move() {
    x += speedX;
    y += speedY;
    CheckScreenCollision();
}

void Ball::CheckScreenCollision() {
    if (x - radius <= 0 || x + radius >= SCREEN_WIDTH) {
        speedX = -speedX; // Bounce horizontally
        soundPlayer.PlayOneShot(wallSound);
    }
    if (y - radius <= 0) {
        speedY = -speedY; // Bounce vertically
        soundPlayer.PlayOneShot(wallSound);
    }
}

bool Ball::CheckPaddleCollision(const Paddle &paddle) {
    if (y + radius >= paddle.y && y - radius <= paddle.y + paddle.height &&
        x >= paddle.x && x <= paddle.x + paddle.width) {
        speedY = -fabs(speedY); // Reverse Y direction
        return true; // Collision occurred
    }
    return false; // No collision
}

bool Ball::IsOutOfBounds() const {
    return y - radius > SCREEN_HEIGHT;
}

void ParticleSystem::Explode(int x, int y) {
    for (int i = 0; i < 50; i++) {
        float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * BL_PI;
        float speed = static_cast<float>(rand()) / RAND_MAX * 4.0f + 1.0f;
        particles.emplace_back(x, y, cos(angle) * speed, sin(angle) * speed, 1.0f);
    }
}

void ParticleSystem::Update() {
    for (auto &particle : particles) {
        particle.Update();
    }
    // Remove dead particles
    particles.erase(std::remove_if(particles.begin(), particles.end(),
                                   [](Particle &p) { return p.life <= 0; }),
                    particles.end());
}

void Particle::Update() {
    x += dx;
    y += dy;
    life -= 0.02f; // Fade out particles
}

void initializeTiles(std::vector<Tile> &tiles, int screenWidth, int screenHeight) {
    int totalTileWidth = TILE_COLUMNS * TILE_WIDTH;
    int totalTileHeight = TILE_ROWS * TILE_HEIGHT;
    int startX = (screenWidth - totalTileWidth) / 2; // Center horizontally
    int startY = 50; // Fixed vertical position

    for (int row = 0; row < TILE_ROWS; row++) {
        for (int col = 0; col < TILE_COLUMNS; col++) {
            tiles.emplace_back(startX + col * TILE_WIDTH, startY + row * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT);
        }
    }
}

void updateGameState(int key, Paddle &paddle, Ball &ball, std::vector<Tile> &tiles,
                     ParticleSystem &particleSystem, YsSoundPlayer &soundPlayer,
                     YsSoundPlayer::SoundData &paddleSound) {
    if (key == FSKEY_LEFT) {
        paddle.MoveLeft();
    }
    if (key == FSKEY_RIGHT) {
        paddle.MoveRight();
    }

    ball.Move();

    // Check collision with paddle
    if (ball.CheckPaddleCollision(paddle)) {
        soundPlayer.PlayOneShot(paddleSound);
    }

    // Check collision with tiles
    for (auto &tile : tiles) {
        if (!tile.destroyed && ball.x + ball.radius >= tile.x && ball.x - ball.radius <= tile.x + tile.width &&
            ball.y + ball.radius >= tile.y && ball.y - ball.radius <= tile.y + tile.height) {
            tile.destroyed = true;
            ball.speedY = -ball.speedY;
            particleSystem.Explode(tile.x + tile.width / 2, tile.y + tile.height / 2);
            soundPlayer.PlayOneShot(paddleSound);
        }
    }

    particleSystem.Update();
}