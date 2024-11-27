#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "fssimplewindow.h"
#include "yssimplesound.h"
#include <algorithm>
#include "rendering_module_main.cpp"
#include "demo_additional.cpp"
#include "file_management.cpp"

//TODO: Implement the following functions in the respective files
// TODO: Nishanth - Split rendering_module_main.cpp into rendering_module.cpp and physics_module.cpp

// const int SCREEN_WIDTH = 800;
// const int SCREEN_HEIGHT = 600;

int main(){
    FsOpenWindow(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
    int game_state = displayMainMenu(); // Tariq's code will handle instructions and leaderboard
    if (game_state == 0) {
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

        int lives = num_lives(false); // to be defined in vasvi's code; the ball has not fallen yet output initial number of lives

        
        for (int row = 0; row < TILE_ROWS; row++) {
            for (int col = 0; col < TILE_COLUMNS; col++) {
                tiles.emplace_back(startX + col * TILE_WIDTH, startY + row * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT);
            }
        }
        // check whether all the tiles are destroyed or not

        while (true) {
            FsPollDevice();
            int key = FsInkey();
            if (key == FSKEY_ESC) {
                // break;
                int mid_state = displayPauseMenu(); // to be defined in tariq's code
                // exit game
                if (mid_state == 0) {
                    break;
                }
                // resume game
                else if (mid_state == 1) {
                    continue;
                }
                // start new game
                else if (mid_state == 2) {
                    // break;
                    ResetGame(ball, paddle, tiles);
                    // score = 0;
                    lives = num_lives(true); // to be defined in vasvi's code; the ball has fallen output updated number of lives
                }
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

            bool specialTileHit = false;
            int score = 0;
            // Check collision with tiles
            for (auto &tile : tiles) {
                if (!tile.destroyed && ball.x + ball.radius >= tile.x && ball.x - ball.radius <= tile.x + tile.width &&
                    ball.y + ball.radius >= tile.y && ball.y - ball.radius <= tile.y + tile.height) {
                    tile.destroyed = true;
                    ball.speedY = -ball.speedY;
                    particleSystem.Explode(tile.x + tile.width / 2, tile.y + tile.height / 2);
                    soundPlayer.PlayOneShot(paddleSound);  // Play tile collision sound
                    score ++;
                    if (tile.IsSpecial()) {
                        specialTileHit = true;
                        lives, ball.speedX,ball.speedY,paddle.width = apply_power_up(ball.speedX,ball.speedY,paddle.width);
                    }
                }
            }

            // Check if all tiles are destroyed
            if (AreAllTilesDestroyed(tiles, ball, paddle)) {
                // std::cout << "All tiles are destroyed! You win!" << std::endl;
                // break;
                bool all_destroyed = true;
                level_up(); // to be defined in vasvi's code
            }

            // Check if ball is out of bounds
            if (ball.IsOutOfBounds()) {
                // std::cout << "Game Over!" << std::endl;
                bool fell = true;
                lives = num_lives(fell); // to be defined in vasvi's code
                if (lives == 0) {
                    std::cout << "Game Over!" << std::endl;
                    break;
                }
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
    }

        return 0;

}