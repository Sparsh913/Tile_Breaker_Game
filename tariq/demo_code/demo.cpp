//#include "D:/studies/Sem 3/EngComp/windows-selected/fssimplewindow.h"
//#include "D:/studies/Sem 3/EngComp/windows-selected/yssimplesound.h"
////#include <fssimplewindow.h>
////#include <yssimplesound.h>
//#include <cmath>
//#include <gl/GL.h>
//#include <vector>  
//#include <cstdlib>
//#include <stdio.h>
//#include <string.h>
//#include <cmath>
//#include <chrono>
//#include "ysglfontdata.h"
//
//
//#define _CRT_SECURE_NO_WARNINGS
//
//const int windowWidth = 800;
//const int windowHeight = 600;
//const int frameRate = 16;  // calc ~60FPS
//float spaceshipX = windowWidth / 2; 
//float spaceshipY = windowHeight - 50; 
//const int numAsteroids = 7;  // No of asteroids
//const int demoDuration = 60;  
//
//YsSoundPlayer player;
//YsSoundPlayer::SoundData bgSound, collisionSound;
//
//void initSounds() {
//    player.Start();
//    if (YSOK != bgSound.LoadWav("mushroom-background-music.wav")) {
//        printf("Failed to load background sound!\n");
//    }
//    if (YSOK != collisionSound.LoadWav("eckkech__collision.wav")) {
//        printf("Failed to load collision sound!\n");
//    }
//}
//
//void playBackgroundSound() 
//{
//    player.PlayBackground(bgSound);
//}
//
//
//float calculateDistance(float x1, float y1, float x2, float y2)  // eucl dist
//{
//    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
//}
//
//// star formation. i have made it such that they will be randomly initialized (both position and speed)
//// also made functions to reposition the stars when they get out of the scene
//
//struct Star 
//{
//    float x, y;
//    float speed;
//};
//
//std::vector<Star> stars;
//const int numStars = 100;  // No of stars
//
//// Initialize the stars
//void initStars() 
//{
//    for (int i = 0; i < numStars; ++i) 
//    {
//        Star star;
//        star.x = rand() % windowWidth;
//        star.y = rand() % windowHeight;
//        star.speed = 1 + (rand() % 3);  // Speed: 1 to 3
//        stars.push_back(star);
//    }
//}
//


//#include "D:/studies/Sem 3/EngComp/windows-selected/fssimplewindow.h"
//#include "D:/studies/Sem 3/EngComp/windows-selected/yssimplesound.h"
#include <fssimplewindow.h>
#include <yssimplesound.h>
#include <string>
#include <fstream>
#include <cmath>
#include <gl/GL.h>
#include <vector>  
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <chrono>
#include "ysglfontdata.h"



// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Current menu state
enum MenuState {
    MAIN_MENU,
    NEW_GAME,
    INSTRUCTIONS,
    LEADERBOARD
};

MenuState currentState = MAIN_MENU;

// Function to render text using YSGLFontData
void renderText(int x, int y, const char* text) {
    glRasterPos2i(x, y);
    YsGlDrawFontBitmap16x24(text); // Using 16x24 font size
}

// Function to read the leaderboard from a file
std::vector<std::string> readLeaderboardFromFile(const std::string& filename) {
    std::vector<std::string> leaderboard;
    std::ifstream file(filename);

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            leaderboard.push_back(line);
        }
        file.close();
    }
    else {
        leaderboard.push_back("Error: Unable to open leaderboard file!");
    }

    return leaderboard;
}

// Function to display the Main Menu
void displayMainMenu() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
    glColor3f(1.0f, 1.0f, 1.0f); // White text

    renderText(300, 200, "1. New Game");
    renderText(300, 250, "2. Instructions");
    renderText(300, 300, "3. Leaderboard");
    renderText(300, 350, "Press ESC to exit.");

    FsSwapBuffers();
}

// Function to display the New Game screen
void displayNewGame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
    glColor3f(1.0f, 1.0f, 1.0f);

    renderText(300, 300, "Welcome to the New Game!");
    renderText(300, 350, "Press ESC to return to the Main Menu.");

    FsSwapBuffers();
}

// Function to display the Instructions screen
void displayInstructions() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.2f, 0.8f, 1.0f); // Blue background
    glColor3f(1.0f, 1.0f, 1.0f);

    renderText(200, 200, "Instructions:");
    renderText(200, 250, "1. Use arrow keys to navigate.");
    renderText(200, 300, "2. Press Space to select.");
    renderText(200, 350, "Press ESC to return to the Main Menu.");

    FsSwapBuffers();
}

// Function to display the Leaderboard screen
void displayLeaderboard() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.6f, 0.1f, 1.0f); // Green background
    glColor3f(1.0f, 1.0f, 1.0f);

    renderText(200, 150, "Leaderboard:");

    // Read leaderboard data from file
    auto leaderboard = readLeaderboardFromFile("leaderboard.txt");

    // Display leaderboard
    int y = 200; // Initial y-position
    for (const auto& entry : leaderboard) {
        renderText(200, y, entry.c_str());
        y += 50; // Move down for the next entry
    }

    renderText(200, y, "Press ESC to return to the Main Menu.");

    FsSwapBuffers();
}


// Function to handle the "Resume" button
void displayResume() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
    glColor3f(1.0f, 1.0f, 1.0f);

    renderText(300, 300, "Resuming your game...");
    renderText(300, 350, "Press ESC to return to the Main Menu.");

    FsSwapBuffers();
    FsSleep(2000); // Display the message for 2 seconds
    currentState = NEW_GAME; // Return to the New Game screen
}

// Function to handle the "Better Luck Next Time" button
void displayBetterLuckNextTime() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.0f, 0.0f, 1.0f); // Dark red background
    glColor3f(1.0f, 1.0f, 1.0f);

    renderText(300, 300, "Better Luck Next Time!");
    renderText(300, 350, "Press ESC to return to the Main Menu.");

    FsSwapBuffers();
    FsSleep(3000); // Display the message for 3 seconds
    currentState = MAIN_MENU; // Return to the Main Menu
}


// Main function
int main() {
    FsOpenWindow(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, "Game Menu");

    bool terminate = false;

    while (!terminate) {
        FsPollDevice();
        auto key = FsInkey();

        if (key == FSKEY_ESC) { // Exit or return to Main Menu
            if (currentState == MAIN_MENU) {
                terminate = true;
            }
            else {
                currentState = MAIN_MENU;
            }
        }
        else if (currentState == MAIN_MENU) {
            if (key == FSKEY_1) { // New Game
                currentState = NEW_GAME;
            }
            else if (key == FSKEY_2) { // Instructions
                currentState = INSTRUCTIONS;
            }
            else if (key == FSKEY_3) { // Leaderboard
                currentState = LEADERBOARD;
            }
        }

        // Render the appropriate screen based on the current state
        switch (currentState) {
        case MAIN_MENU:
            displayMainMenu();
            break;
        case NEW_GAME:
            displayNewGame();
            break;
        case INSTRUCTIONS:
            displayInstructions();
            break;
        case LEADERBOARD:
            displayLeaderboard();
            break;
        }

        FsSleep(10); // Reduce CPU usage
    }

    return 0;
}