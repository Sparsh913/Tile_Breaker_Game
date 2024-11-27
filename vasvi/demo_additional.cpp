#include <iostream>
#include <ctime>
#include <string>
#include "ysglfontdata.h" // For rendering text
#include "fssimplewindow.h" // For graphical window handling

// Global Variables
// int score = 0; // Default score when game starts
int lives = 3; // Default lives when game starts
int level = 1; // Default value for level

// check lives and return them
int num_lives(bool fell) {
    if(fell == true) {
        lives -=1;
    }
    return lives;
}

//Level up and render
void level_up() {
    glRasterPos2i(200, 300);
    YsGlDrawFontBitmap20x28(("Level " + std::to_string(level) + " Completed!").c_str());
    FsSwapBuffers();
    FsSleep(2000);

    level += 1;

    glRasterPos2i(200, 300);
    YsGlDrawFontBitmap20x28(("Level " + std::to_string(level) + " Starts Now!").c_str());
    FsSwapBuffers();
    FsSleep(2000);
}

// Function to apply power-ups
float apply_power_up(float &ball_speedx,float &ball_speedy,int &paddle_width) {
    int powerUps[] = {0,1,2,3};
    int randomIndex = rand() % 4;

    if (powerUps[randomIndex] == 0) {
        lives += 1;
    }
    else if (powerUps[randomIndex] == 1) {
        paddle_width *= 1.2;
    }
    else if (powerUps[randomIndex] == 2) {
        ball_speedx *= 1.2;
        ball_speedy *= 1.2;
    }
    else if (powerUps[randomIndex] == 4) {
        ball_speedx *= 0.8;
        ball_speedy *= 0.8;
    }
    return lives, ball_speedx,ball_speedy,paddle_width;
}

// void update_score_level(int total_tiles) {
//     // Updating the score
//     score += 1;
//     if (score == total_tiles) {
//         level_up();
//         glRasterPos2i(200, 300);
//         YsGlDrawFontBitmap20x28(("Level " + std::to_string(level - 1) + " completed!").c_str());
//         glRasterPos2i(200, 260);
//         YsGlDrawFontBitmap20x28(("Level " + std::to_string(level) + " starts now!").c_str());
//     }
// }

// Function to display the number of lives on the screen
void display_lives_on_screen() {
    glRasterPos2i(10, 460);
    YsGlDrawFontBitmap16x20(("Lives: " + std::to_string(lives)).c_str());
}


