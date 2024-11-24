//#include "D:/studies/Sem 3/EngComp/windows-selected/fssimplewindow.h"
//#include "D:/studies/Sem 3/EngComp/windows-selected/yssimplesound.h"
#include <fssimplewindow.h>
#include <yssimplesound.h>
#include <cmath>
#include <gl/GL.h>
#include <vector>  
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <chrono>


#define _CRT_SECURE_NO_WARNINGS

const int windowWidth = 800;
const int windowHeight = 600;
const int frameRate = 16;  // calc ~60FPS
float spaceshipX = windowWidth / 2; 
float spaceshipY = windowHeight - 50; 
const int numAsteroids = 7;  // No of asteroids
const int demoDuration = 60;  

YsSoundPlayer player;
YsSoundPlayer::SoundData bgSound, collisionSound;

void initSounds() {
    player.Start();
    if (YSOK != bgSound.LoadWav("mushroom-background-music.wav")) {
        printf("Failed to load background sound!\n");
    }
    if (YSOK != collisionSound.LoadWav("eckkech__collision.wav")) {
        printf("Failed to load collision sound!\n");
    }
}

void playBackgroundSound() 
{
    player.PlayBackground(bgSound);
}


float calculateDistance(float x1, float y1, float x2, float y2)  // eucl dist
{
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

// star formation. i have made it such that they will be randomly initialized (both position and speed)
// also made functions to reposition the stars when they get out of the scene

struct Star 
{
    float x, y;
    float speed;
};

std::vector<Star> stars;
const int numStars = 100;  // No of stars

// Initialize the stars
void initStars() 
{
    for (int i = 0; i < numStars; ++i) 
    {
        Star star;
        star.x = rand() % windowWidth;
        star.y = rand() % windowHeight;
        star.speed = 1 + (rand() % 3);  // Speed: 1 to 3
        stars.push_back(star);
    }
}

void updateStars() 
{
    for (auto& star : stars) 
    {
        star.x -= star.speed;
        if (star.x < 0) 
        {
            star.x = windowWidth;  
            star.y = rand() % windowHeight; 
        }
    }
}

void drawStars() 
{
    glColor3f(1.0f, 1.0f, 1.0f);  // White 
    glBegin(GL_POINTS);
    for (const auto& star : stars) 
    {
        glVertex2f(star.x, star.y);
    }
    glEnd();
}


//void drawSpaceship() 
//{
//    glColor3f(0.0f, 1.0f, 0.0f);  // green
//
//    glBegin(GL_POLYGON);
//    glVertex2f(spaceshipX - 15, spaceshipY);     
//    glVertex2f(spaceshipX + 15, spaceshipY);     
//    glVertex2f(spaceshipX, spaceshipY - 30);     
//    glEnd();
//}


void drawSpaceship() 
{
    // Main body
    glColor3f(0.0f, 1.0f, 0.0f);  // Green body
    glBegin(GL_POLYGON);
    glVertex2f(spaceshipX - 15, spaceshipY);       
    glVertex2f(spaceshipX + 15, spaceshipY);       
    glVertex2f(spaceshipX + 10, spaceshipY - 30);  
    glVertex2f(spaceshipX - 10, spaceshipY - 30);  
    glEnd();

    // Cockpit
    glColor3f(0.0f, 0.0f, 1.0f);  // Blue cockpit
    glBegin(GL_TRIANGLES);
    glVertex2f(spaceshipX - 5, spaceshipY - 30);   
    glVertex2f(spaceshipX + 5, spaceshipY - 30);   
    glVertex2f(spaceshipX, spaceshipY - 45);       
    glEnd();

    // Left wing
    glColor3f(1.0f, 0.0f, 0.0f);  // Red left wing
    glBegin(GL_POLYGON);
    glVertex2f(spaceshipX - 15, spaceshipY);       
    glVertex2f(spaceshipX - 25, spaceshipY + 10);  
    glVertex2f(spaceshipX - 15, spaceshipY - 10);  
    glEnd();

    // Right wing
    glColor3f(1.0f, 0.0f, 0.0f);  // Red right wing
    glBegin(GL_POLYGON);
    glVertex2f(spaceshipX + 15, spaceshipY);       
    glVertex2f(spaceshipX + 25, spaceshipY + 10);  
    glVertex2f(spaceshipX + 15, spaceshipY - 10);  
    glEnd();

    // Flames 
    glColor3f(1.0f, 0.5f, 0.0f);  // Orange flames
    glBegin(GL_TRIANGLES);
    glVertex2f(spaceshipX - 5, spaceshipY);        
    glVertex2f(spaceshipX + 5, spaceshipY);        
    glVertex2f(spaceshipX, spaceshipY + 15);       
    glEnd();
}

struct Asteroid 
{
    float x, y;
    float speed;
    float size;  // Radius 
};

std::vector<Asteroid> asteroids;

void initAsteroids() {
    for (int i = 0; i < numAsteroids; ++i) 
    {
        Asteroid asteroid;
        asteroid.x = rand() % windowWidth;
        asteroid.y = rand() % windowHeight / 2;  // upper half
        asteroid.speed = 1 + (rand() % 2);  // Speed 1 - 2
        asteroid.size = 10 + (rand() % 20); // Size 10 - 30
        asteroids.push_back(asteroid);
    }
}

void updateAsteroids() 
{
    for (auto& asteroid : asteroids) 
    {
        asteroid.y += asteroid.speed;
        if (asteroid.y > windowHeight) 
        {
            asteroid.y = 0;  // Reset 
            asteroid.x = rand() % windowWidth;  
        }
    }
}

void drawAsteroids() 
{
    glColor3f(0.5f, 0.5f, 0.5f);  // Gray 
    for (const auto& asteroid : asteroids) 
    {
        glBegin(GL_TRIANGLE_FAN);
        for (int i = 0; i < 360; i += 20) 
        {
            float angle = i * 3.14159 / 180.0f;
            glVertex2f(asteroid.x + cos(angle) * asteroid.size,
                asteroid.y + sin(angle) * asteroid.size);
        }
        glEnd();
    }
}

void playCollisionSound() 
{
    player.PlayOneShot(collisionSound);
}

void checkCollision() 
{
    for (auto& asteroid : asteroids) 
    {
        float distance = calculateDistance(spaceshipX, spaceshipY, asteroid.x, asteroid.y);

        if (distance < asteroid.size + 15) 
        {
            asteroid.speed = -asteroid.speed;
            asteroid.x += (rand() % 10 - 5);
            playCollisionSound();
        }
    }
}


int main() {
    FsOpenWindow(0, 0, windowWidth, windowHeight, 1);

    // Initialize elements (stars, asteroids, etc.)
    initStars();
    initAsteroids();
    initSounds();
    playBackgroundSound();

    // Start timer
    auto startTime = std::chrono::steady_clock::now();

    while (true) {
        FsPollDevice();

        // Calculate elapsed time
        auto currentTime = std::chrono::steady_clock::now();
        int elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();

        // Check if the elapsed time exceeds the demo duration
        if (elapsedSeconds >= demoDuration || FSKEY_ESC == FsInkey()) {
            break;  // Exit loop if time limit reached or ESC pressed
        }

        // Update elements and check for collisions
        updateStars();
        updateAsteroids();
        checkCollision();

        // Clear, draw, and refresh
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        drawStars();
        drawSpaceship();
        drawAsteroids();

        FsSwapBuffers();
        FsSleep(frameRate);
    }

    player.End();  // Stop sound player
    return 0;
}