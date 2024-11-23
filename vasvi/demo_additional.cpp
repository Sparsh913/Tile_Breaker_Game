#include <iostream>
#include <ctime>
#include <string>
#include "ysglfontdata.h" // For rendering text
#include "fssimplewindow.h" // For graphical window handling

// Global Variables
int score = 0; // Default score when game starts
int lives = 2; // Default lives when game starts
int level = 1; // Default value for level
bool level_up_flag = false; // Level up check
float ball_speed = 1.0f; // Initial ball speed (from another file)
float paddle_width = 100.0f; // Initial paddle width (from another file)
bool power_up_active = false; // Check for power-up activity
time_t power_up_timer = 0; // Timer to keep track of power-up duration

// Function to update the score and level
int level_up();
void end_game();
void reset_power_up_effects();

void update_score_level(int total_tiles) {
    // Updating the score
    score += 1;
    if (score == total_tiles) {
        level = level_up();
        glRasterPos2i(200, 300);
        YsGlDrawFontBitmap20x28(("Level " + std::to_string(level - 1) + " completed!").c_str());
        glRasterPos2i(200, 260);
        YsGlDrawFontBitmap20x28(("Level " + std::to_string(level) + " starts now!").c_str());
    }
}

// Function to update the level
int level_up() {
    // Increase the ball speed
    ball_speed += 0.5f; // Speed increment can be adjusted
    // Decrease the paddle width
    paddle_width -= 2.0f; // Paddle width decrement can be adjusted
    level += 1;
    level_up_flag = true; // Mark level up
    return level;
}

// Function to check if the player lost a life
void check_ball_status(bool ball_fell) {
    if (ball_fell) {
        lives -= 1; // Reducing one life
        if (lives <= 0) {
            end_game();
        }
    }
}

// Function to apply power-ups
void apply_power_up(const std::string &power_up_type) {
    if (power_up_type == "extra_life") {
        lives += 1;
        ball_speed += 0.1f; // Increase ball speed slightly with each extra life
    } else if (power_up_type == "increase_paddle_width") {
        paddle_width += 20.0f; // Temporary increase
        power_up_active = true;
        power_up_timer = std::time(0) + 5; // Power-up lasts for 5 seconds
    } else if (power_up_type == "decrease_ball_speed") {
        ball_speed -= 0.3f; // Temporary decrease
        power_up_active = true;
        power_up_timer = std::time(0) + 5;
    } else if (power_up_type == "increase_ball_speed") {
        ball_speed += 0.3f; // Temporary increase
        power_up_active = true;
        power_up_timer = std::time(0) + 5;
    }
}

// Function to check if the power-up duration has expired
void check_power_up_timer() {
    if (power_up_active && std::time(0) > power_up_timer) {
        // Reset values after power-up expires
        reset_power_up_effects();
    }
}

// Function to reset power-up effects
void reset_power_up_effects() {
    if (power_up_active) {
        // Assuming we only have one active power-up at a time
        paddle_width = 100.0f;
        ball_speed = 1.0f;
        power_up_active = false;
    }
}

// Function to display the number of lives on the screen
void display_lives_on_screen() {
    glRasterPos2i(10, 460);
    YsGlDrawFontBitmap16x20(("Lives: " + std::to_string(lives)).c_str());
}

// Function to handle end of the game
void end_game() {
    std::cout << "Game Over!" << std::endl;
    exit(0); // End the game
}

// Function to debug and test components
void debug_and_test_components() {
    // Test Lives Management
    std::cout << "Testing Lives Management...\n";
    check_ball_status(true); // Simulate ball fall
    std::cout << "Lives after ball fall: " << lives << ", Ball speed: " << ball_speed << "\n";
    check_ball_status(true); // Simulate another ball fall
    std::cout << "Lives after another ball fall: " << lives << ", Ball speed: " << ball_speed << "\n";

    // Test Score and Level Management
    std::cout << "Testing Score and Level Management...\n";
    for (int i = 0; i < 50; ++i) { // Simulate hitting all tiles
        update_score_level(50);
    }
    std::cout << "Score: " << score << ", Level: " << level << "\n";

    // Test Power-Up Application
    std::cout << "Testing Power-Up Application...\n";
    apply_power_up("extra_life");
    std::cout << "Lives after extra life power-up: " << lives << ", Ball speed: " << ball_speed << "\n";
    apply_power_up("increase_paddle_width");
    std::cout << "Paddle width after increase: " << paddle_width << "\n";
    check_power_up_timer();
    std::cout << "Paddle width after power-up duration: " << paddle_width << "\n";

    // Test Level-Up Mechanism
    std::cout << "Testing Level-Up Mechanism...\n";
    update_score_level(50); // Trigger level-up
    std::cout << "Level after level-up: " << level << ", Ball speed: " << ball_speed << ", Paddle width: " << paddle_width << "\n";
}

// Main function
int main() {
    FsOpenWindow(0, 0, 800, 600, 1); // Create a window

    int total_tiles = 50; // Example total number of tiles
    bool game_running = true;

    debug_and_test_components(); // Call debugging and testing function

    while (game_running) {
        FsPollDevice();
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        display_lives_on_screen();
        check_power_up_timer();

        // Game state handling
        if (score < total_tiles) { // Condition for tile hit
            update_score_level(total_tiles);
        }

        if (lives > 0 && ball_speed > 0.0f) { // Condition for ball falling
            check_ball_status(false); // Assuming ball is not falling in this iteration
        } else {
            check_ball_status(true); // Ball fell, decrement life
        }

        if (rand() % 100 < 5) { // Random condition to simulate power-up obtained (5% chance per frame)
            apply_power_up("extra_life"); // Example power-up
        }

        FsSwapBuffers();
        FsSleep(10); // Control frame rate
    }

    return 0;
}
