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

int displayPauseMenu() {
    while (true) {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
        glColor3f(1.0f, 1.0f, 1.0f);          // White text

        // Render menu options
        renderText(300, 200, "1. Resume Game");
        renderText(300, 250, "2. Exit Game");
        renderText(300, 300, "3. Start New Game");

        FsSwapBuffers();
        FsPollDevice();

        // Retrieve mouse input
        int lb, mb, rb, mx, my;
        auto mouseEvent = FsGetMouseEvent(lb, mb, rb, mx, my);

        // Flip `my` to match OpenGL's coordinate system
        my = SCREEN_HEIGHT - my;

        // Define button boundaries
        int buttonX1 = 300, buttonX2 = 560;  // Horizontal range for all options
        int resumeY1 = 390, resumeY2 = 420;       // Y-range for "Resume Game"
        int exitY1 = 345, exitY2 = 380; // Y-range for "Exit Game"
        int newGameY1 = 300, newGameY2 = 335;   // Y-range for "Start New Game"

        // Check for mouse click within button areas
        if (mouseEvent == FSMOUSEEVENT_LBUTTONDOWN) {
            if (mx >= buttonX1 && mx <= buttonX2) {
                if (my >= resumeY1 && my <= resumeY2) {
                    // std::cout << "Resume Game selected!" << std::endl;
                    return 1; // Resume Game
                } else if (my >= exitY1 && my <= exitY2) {
                    // std::cout << "Exit Game selected!" << std::endl;
                    return 0; // Exit Game
                } else if (my >= newGameY1 && my <= newGameY2) {
                    // std::cout << "Start New Game selected!" << std::endl;
                    return 2; // Start New Game
                }
            }
        }

        // Allow number-based input for user convenience
        int key = FsInkey();
        if (key == FSKEY_1) {
            return 1; // Resume Game
        } else if (key == FSKEY_2) {
            return 0; // Exit Game
        } else if (key == FSKEY_3) {
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
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
        glColor3f(1.0f, 1.0f, 1.0f);         // White text

        // Centered text rendering
        renderText(40, 100, "Instructions:");
        renderText(40, 150, "Green Tiles - Extra Life");
        renderText(40, 200, "Yellow Tiles - Increase Paddle Size");
        renderText(40, 250, "Blue Tiles - Increase Ball Speed");
        renderText(40, 300, "Orange Tiles - Decrease Ball Speed");
        renderText(40, 350, "Magenta Tiles - Decrease Paddle Size");
        renderText(40, 400, "Use left and right Arrows to move the paddle.");
        renderText(40, 500, "Press ESC to return to the Main Menu.");


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

void displayLeaderboard() {
    while (true) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
        glColor3f(1.0f, 1.0f, 1.0f);          // White text

        renderText(100, 50, "Leaderboard:");

        // Read leaderboard data from file
        std::vector<std::pair<std::string, int>> leaderboard;

        std::ifstream file("leaderboard.csv");
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                size_t separatorPos = line.find(',');
                if (separatorPos != std::string::npos) {
                    std::string name = line.substr(0, separatorPos);
                    int score = std::stoi(line.substr(separatorPos + 1));
                    leaderboard.emplace_back(name, score);
                }
            }
            file.close();
        }

        // Sort the leaderboard in descending order by score
        std::sort(leaderboard.begin(), leaderboard.end(), [](const auto &a, const auto &b) {
            return a.second > b.second; // Compare scores
        });

        // Display leaderboard
        renderText(130, 100, "Rank");
        renderText(330, 100, "Name");
        renderText(530, 100, "Score");

        int y = 150; // Start position for rows
        int rank = 1;
        for (const auto &entry : leaderboard) {
            renderText(130, y, std::to_string(rank).c_str()); // Rank
            renderText(330, y, entry.first.c_str());          // Name
            renderText(530, y, std::to_string(entry.second).c_str()); // Score
            y += 50;
            rank++;
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