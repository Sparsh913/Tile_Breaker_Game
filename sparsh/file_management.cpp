#include "fssimplewindow.h"
#include "yssimplesound.h"
#include <string>
#include <fstream>
#include <cmath>
#include <vector>  
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <chrono>
#include "ysglfontdata.h"


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

// int displayMainMenu() {
//     while (true) {
//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//         glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
//         glColor3f(1.0f, 1.0f, 1.0f); // White text

//         // Render menu options
//         renderText(300, 200, "1. New Game");
//         renderText(300, 250, "2. Instructions");
//         renderText(300, 300, "3. Leaderboard");
//         renderText(300, 350, "ESC to exit");

//         FsSwapBuffers();
//         FsPollDevice();

//         int lb, mb, rb, mx, my;
//         auto mouseEvent = FsGetMouseEvent(lb, mb, rb, mx, my);

//         int newGameTop = 200, newGameBottom = 230;
//         int instructionsTop = 250, instructionsBottom = 280;
//         int leaderboardTop = 300, leaderboardBottom = 330;
//         int exitTop = 350, exitBottom = 380;

//         if (mouseEvent == FSMOUSEEVENT_LBUTTONDOWN)
//         {
//             if (mx >= 300 && mx <= 600)
//             {
//                 if (my >= newGameTop && my <= newGameBottom)
//                 {
//                     return 0; // New Game
//                 }
//                 else if (my >= instructionsTop && my <= instructionsBottom)
//                 {
//                     return 1; // Instructions
//                 }
//                 else if (my >= leaderboardTop && my <= leaderboardBottom)
//                 {
//                     return 2; // Leaderboard
//                 }
//                 else if (my >= exitTop && my <= exitBottom)
//                 {
//                     return -1; // Exit
//                 }
//             }

//         }

//         int key = FsInkey();
//         if (key == FSKEY_ESC) {
//             return -1; // Exit
//         }  

//         FsSleep(10); // Prevent CPU overuse
//     }
// }

int displayMainMenu() {
    while (true) {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
        glColor3f(1.0f, 1.0f, 1.0f);          // White text

        // Render menu options
        renderText(300, 200, "1. New Game");
        renderText(300, 250, "2. Instructions");
        renderText(300, 300, "3. Leaderboard");
        renderText(300, 350, "ESC to exit");

        FsSwapBuffers();
        FsPollDevice();

        // Retrieve mouse input
        int lb, mb, rb, mx, my;
        auto mouseEvent = FsGetMouseEvent(lb, mb, rb, mx, my);

        // Flip `my` to match OpenGL's coordinate system
        my = SCREEN_HEIGHT - my;

        // Debugging: Print mouse events and coordinates
        std::cout << "Mouse Event: " << mouseEvent
                  << ", mx: " << mx << ", my: " << my
                  << ", lb: " << lb << ", rb: " << rb << std::endl;

        // Define button boundaries
        int buttonX1 = 300, buttonX2 = 560;  // Horizontal range for all options
        int newGameY1 = 390, newGameY2 = 420;       // Y-range for "New Game"
        int instructionsY1 = 345, instructionsY2 = 380; // Y-range for "Instructions"
        int leaderboardY1 = 300, leaderboardY2 = 335;   // Y-range for "Leaderboard"
        int exitY1 = 255 - 24, exitY2 = 290;                 // Y-range for "ESC to Exit"

        // Check for mouse click within button areas
        if (mouseEvent == FSMOUSEEVENT_LBUTTONDOWN) {
            if (mx >= buttonX1 && mx <= buttonX2) {
                if (my >= newGameY1 && my <= newGameY2) {
                    std::cout << "New Game selected!" << std::endl;
                    return 0; // New Game
                } else if (my >= instructionsY1 && my <= instructionsY2) {
                    std::cout << "Instructions selected!" << std::endl;
                    return 1; // Instructions
                } else if (my >= leaderboardY1 && my <= leaderboardY2) {
                    std::cout << "Leaderboard selected!" << std::endl;
                    return 2; // Leaderboard
                } else if (my >= exitY1 && my <= exitY2) {
                    std::cout << "Exit selected!" << std::endl;
                    return -1; // Exit
                }
            }
        }

        // Allow ESC key to exit
        int key = FsInkey();
        if (key == FSKEY_ESC) {
            std::cout << "ESC pressed. Exiting..." << std::endl;
            return -1; // Exit
        }

        FsSleep(10); // Prevent CPU overuse
    }
}


// Function to display the Pause Menu and return the selected state
int displayPauseMenu() {
    while (true) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
        glColor3f(1.0f, 1.0f, 1.0f); // White text

        // Render menu options
        renderText(300, 200, "1. Resume Game");
        renderText(300, 250, "2. Exit Game");
        renderText(300, 300, "3. Start New Game");

        FsSwapBuffers();
        FsPollDevice();

        // Check for user input
        int key = FsInkey();
        if (key == FSKEY_1) {
            return 1; // Resume Game
        }
        else if (key == FSKEY_2) {
            return 0; // Exit Game
        }
        else if (key == FSKEY_3) {
            return 2; // Start New Game
        }

        FsSleep(10); // Prevent CPU overuse
    }
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

void displayInstructions() {
    while (true) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.2f, 0.8f, 1.0f); // Blue background
        glColor3f(1.0f, 1.0f, 1.0f);

        renderText(200, 200, "Instructions:");
        renderText(200, 250, "1. Use arrow keys to navigate.");
        renderText(200, 300, "2. Press Space to select.");
        renderText(200, 350, "Press ESC to return to the Main Menu.");

        FsSwapBuffers();
        FsPollDevice();

        // Check for user input
        int key = FsInkey();
        if (key == FSKEY_ESC) {
            return; // Exit the function when ESC is pressed
        }

        FsSleep(10); // Prevent CPU overuse
    }
}

// Function to display the Leaderboard screen
void displayLeaderboard() {
    while (true) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.6f, 0.1f, 1.0f); // Green background
        glColor3f(1.0f, 1.0f, 1.0f);

        renderText(200, 150, "Leaderboard:");

        // Read leaderboard data from file
        auto leaderboard = readLeaderboardFromFile("leaderboard.csv");

        // Display leaderboard
        int y = 200; // Initial y-position
        for (const auto& entry : leaderboard) {
            renderText(200, y, entry.c_str());
            y += 50; // Move down for the next entry
        }

        renderText(200, y, "Press ESC to return to the Main Menu.");

        FsSwapBuffers();
        FsPollDevice();

        // Check for user input
        int key = FsInkey();
        if (key == FSKEY_ESC) {
            return; // Exit the function when ESC is pressed
        }

        FsSleep(10); // Prevent CPU overuse
    }
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
