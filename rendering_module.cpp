#include "rendering_module.h"
#include <cmath>

const float BL_PI = 3.14159265f;

void RenderBackground() {
    glColor3f(0.1f, 0.1f, 0.3f); // Dark blue background
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(SCREEN_WIDTH, 0);
    glVertex2i(SCREEN_WIDTH, SCREEN_HEIGHT);
    glVertex2i(0, SCREEN_HEIGHT);
    glEnd();
}

void Paddle::Render() {
    glColor3f(0.0f, 1.0f, 0.0f); // Green paddle
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x + width, y);
    glVertex2i(x + width, y + height);
    glVertex2i(x, y + height);
    glEnd();
}

void Ball::Render() {
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow ball
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < 100; i++) {
        float angle = 2.0f * BL_PI * i / 100.0f;
        glVertex2f(x + radius * cos(angle), y + radius * sin(angle));
    }
    glEnd();
}

void Tile::Render() {
    if (!destroyed) {
        if (isSpecial) {
            glColor3f(0.0f, 1.0f, 1.0f); // Cyan fill color for special tiles
        } else {
            glColor3f(1.0f, 0.0f, 0.0f); // Red fill color for regular tiles
        }
        glBegin(GL_QUADS);
        glVertex2i(x, y);
        glVertex2i(x + width, y);
        glVertex2i(x + width, y + height);
        glVertex2i(x, y + height);
        glEnd();

        // Draw the border
        glColor3f(0.0f, 0.0f, 0.0f); // Black border
        glBegin(GL_LINE_LOOP);
        glVertex2i(x, y);
        glVertex2i(x + width, y);
        glVertex2i(x + width, y + height);
        glVertex2i(x, y + height);
        glEnd();
    }
}

void Particle::Render() {
    if (life > 0) {
        glColor4f(1.0f, 1.0f, 0.0f, life); // Yellow fading particles
        glBegin(GL_POINTS);
        glVertex2f(x, y);
        glEnd();
    }
}

void ParticleSystem::Render() {
    for (auto &particle : particles) {
        particle.Render();
    }
}

void renderGame(const Paddle &paddle, const Ball &ball, const std::vector<Tile> &tiles, const ParticleSystem &particleSystem) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderBackground();
    paddle.Render();
    ball.Render();
    for (const auto &tile : tiles) {
        tile.Render();
    }
    particleSystem.Render();
}